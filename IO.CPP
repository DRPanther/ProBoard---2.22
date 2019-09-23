#define Use_MsgBase

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <tswin.hpp>
#include <time.h>
#include <io.h>
#include <dos.h>
#include "desqview.hpp"
#include "proboard.hpp"
#include "fossil.hpp"

static String align_string(char *s , int width , int alignment);

static void near
nocarrier()
{
   if(ignore_carrier) return;

   dword ticks = clockticks();

   for(;clockdiff(ticks)<10;)   // Check for 1/2 second
   {
      msleep(100);

      if(fos_carrier(io.port)) return;
   }

   LOG("Carrier lost!");

   exit_proboard();
}

static byte conversion[256];

void
fossilio::init(int p,long b)
{
   byte baudcode;

   for(int i=0;i<256;i++) conversion[i]=i;

   conversion[(byte)'Ú']='+';
   conversion[(byte)'Ã']='+';
   conversion[(byte)'À']='+';
   conversion[(byte)'Â']='+';
   conversion[(byte)'Å']='+';
   conversion[(byte)'Á']='+';
   conversion[(byte)'¿']='+';
   conversion[(byte)'´']='+';
   conversion[(byte)'Ù']='+';
   conversion[(byte)'É']='+';
   conversion[(byte)'Ë']='+';
   conversion[(byte)'»']='+';
   conversion[(byte)'Ì']='+';
   conversion[(byte)'Î']='+';
   conversion[(byte)'¹']='+';
   conversion[(byte)'È']='+';
   conversion[(byte)'Ê']='+';
   conversion[(byte)'¼']='+';
   conversion[(byte)'Ö']='+';
   conversion[(byte)'Ò']='+';
   conversion[(byte)'·']='+';
   conversion[(byte)'Ç']='+';
   conversion[(byte)'×']='+';
   conversion[(byte)'¶']='+';
   conversion[(byte)'Ó']='+';
   conversion[(byte)'Ð']='+';
   conversion[(byte)'½']='+';
   conversion[(byte)'Õ']='+';
   conversion[(byte)'Ñ']='+';
   conversion[(byte)'¸']='+';
   conversion[(byte)'Æ']='+';
   conversion[(byte)'Ø']='+';
   conversion[(byte)'µ']='+';
   conversion[(byte)'Ô']='+';
   conversion[(byte)'Ï']='+';
   conversion[(byte)'¾']='+';
   conversion[(byte)'Í']='-';
   conversion[(byte)'Ä']='-';
   conversion[(byte)'³']='|';
   conversion[(byte)'º']='|';
   conversion[(byte)'Û']='#';
   conversion[(byte)'û']='*';
   conversion[(byte)'']='^';
   conversion[(byte)'']='v';
   conversion[(byte)'°']='#';
   conversion[(byte)'±']='#';
   conversion[(byte)'²']='#';

   port=p;
   baud=b;
   stopenabled=0;
   show_local  = TRUE;
   show_remote = TRUE;
   more_func = NULLFUNC;

   inputbuffer.clear();

   if(baud)
   {
      switch(baud)
      {
         case   300L: baudcode=0x43; break;
         case  1200L: baudcode=0x83; break;
         case  2400L: baudcode=0xA3; break;
         case  4800L: baudcode=0xC3; break;
         case  7200L:
         case  9600L: baudcode=0xE3; break;
         case 12000L:
         case 14400L:
         case 16800L:
         case 19200L: baudcode=0x03; break;
         default    : baudcode=0x23; break;
      }

      fos_init(port);
      fos_setbps(port,baudcode);
      fos_flowctl(port,2);
      sleep(1);
   }

   if(outputBuffer == NULL)
      outputBuffer = new byte[cfg.IObuffersize];

   bufferPtr = 0;

   SCREEN.disableCursor();
}

void
fossilio::clearinput()
{
   fos_purgeinput(port);
}

fossilio::~fossilio()
{
   cooldown();

   if(outputBuffer != NULL)
      delete [] outputBuffer;
}

void fossilio::cooldown()
{
   if(baud && baud != 65535L)
   {
      fos_purgeoutput(port);
      fos_deinit(port);
      sleep(1);
   }
}

void
fossilio::heatup()
{
   if(baud)
   {
      fos_init(port);
      sleep(1);
   }

   bufferPtr = 0;
}

byte
fossilio::readkey()
{
   if(KB.hit())
   {
      KEY key=KB.get();
      if(key>256)
      {
         sysopkey(key);
         return 0;
      }
      else
      {
         external=0;
         return (byte)key;
      }
   }

   static dword prevticks = 0;

   if(clockdiff(prevticks)>90)
   {
      static int inchatreq=0;

      prevticks = clockticks();

      SCRAMBLE();

      timer.check();

      if(!access(form("%sDOWN.%d",syspath,node_number),0))
      {
         io << S_SYSTEM_GOING_DOWN;

         LOG("Going down on external request");

         File f;
         f.open(form("%sISDOWN.%d",syspath,node_number),fmode_create|fmode_write);
         f.close();

         exit_proboard(100);
      }

      if(!inchatreq && cfg.numnodes>1 && !chatflag)
      {
         user_online uo;
         int cht=uo.check_chat();

         if(cht)
         {
            inchatreq=1;
            uo.read(cht);
            io << "\n\n" << S_USER_REQUESTS_CHAT(uo.name,form("%d",cht));

            if(ask(1))
            {
               uo.setstatus(UO_CHATTING);

               io << "\n\n" << S_MULTILINE_CHAT_STARTED;

               multilinechat(cht);

               io << "\n\n" << S_MULTILINE_CHAT_ENDED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

               uo.setstatus(UO_BROWSING);
            }
            else
            {
               uo.setstatus(UO_BUSY);
            }

            inchatreq=0;
         }
      }
   }

   if(!baud) return 0;

   FOS_STATUS status = fos_status(port);

   if(!status_carrier(status)) nocarrier();

   if(status_byteready(status))
   {
      external = 1;
      return fos_getch(port);
   }

   return 0;
}

void
fossilio::flush()
{
   SCREEN.placeCursor();

   if(bufferPtr)
   {
      for(;;)
      {
         FOS_STATUS s = fos_status(port);

         if(!(s & FSTAT_CARRIER)) nocarrier();

         word x = fos_sendblock(port,outputBuffer,bufferPtr);

         if(x >= bufferPtr) break;

         bufferPtr -= x;

         memmove( outputBuffer , &outputBuffer[x] , bufferPtr);
      }

      bufferPtr = 0;
   }
}

fossilio&
fossilio::operator<<(char c)
{
   if(c=='\f' && !cls_mode && user_recnr>=0 && !SCREEN.inEscape()) return *this;

   if(c=='\f' && !SCREEN.inEscape())
   {
      color(COLOR_WHITE);
      linecounter(0);
   }

   if(c=='\n' && !SCREEN.inEscape()) (*this) << '\r';

   if(no_ibm) c=conversion[(byte)c];

   if(byte(c) == byte(255))
   {
      flush();

      return *this;
   }

   if(baud && show_remote)
   {
      if(cfg.IObuffersize == 1)
      {
         for(;;)
         {
            FOS_STATUS s = fos_status(port);

            if(!(s & FSTAT_CARRIER)) nocarrier();

            if(s & FSTAT_BUFEMPTY) break;
         }

         fos_send(port,c);
      }
      else
      {
         outputBuffer[bufferPtr++] = c;

         if(bufferPtr == cfg.IObuffersize) flush();
      }
   }

   if(show_local) SCREEN << c;

   return *this;
}

fossilio&
fossilio::operator<<(char *s)
{
   send(s);

   return *this;
}

byte
fossilio::send(char *str,char *hotkeys)
{
   if(hotkeys)
      strupr(hotkeys);

   for(;*str;str++)
   {
      byte c=0;

      if(SCREEN.inEscape())
      {
         (*this) << (*str);

         continue;
      }

      if(stopenabled || hotkeys)
         c = toupper(readkey());

      if(hotkeys && c)
         if(strchr(hotkeys,c))
         {
            return c;
         }

      if(c=='S')
      {
         stopped = TRUE;
         flush();

         return 1;
      }

      if(c=='P')
         wait("P\r");

      c = *str;

      if(c=='\t')
      {
         wait('\r');

         continue;
      }

      if(c == 26)
      {
         fullcolor(*(++str));
         continue;
      }

      if(c<8 && c>0)
      {
         color(c);

         continue;
      }

      if(c == 24 || c == 25)
      {
         int l = int(*(str+1));
         char *fname = new char[l+1];
         strncpy(fname,str+2,l);
         fname[l] = '\0';

         if(c == 24)
            run_sdkfile(fname);
         else
            showansasc(fname);

         str += l+1;

         delete [] fname;

         continue;
      }

      if(c<0x18 && c>0x10 && c!=0x16)
      {
         color((c-0x10)|0x80);
      }
      else
      {
         (*this) << c;
      }
   }

 return 0;
}

#pragma warn -def

const int ALIGN_LEFT   = 0,
          ALIGN_RIGHT  = 1,
          ALIGN_CENTER = 2;

byte
fossilio::sendfile(char *fn,char *hotkeys)
{
bool fflag  = FALSE,
     kflag  = FALSE,
     moreok = TRUE,
     stopok = TRUE,
     avt_compr = FALSE;

int alignment = ALIGN_LEFT;

String str;

if(rip_mode && (!show_local || !show_remote))
{
   moreok = FALSE;
   stopok = FALSE;
}

static char *on_string = "ON " , *off_string = "OFF";

int fieldwidth;
byte avt_compr_char = '\0';

File f(fn,fmode_read,1024);
if(!f.opened()) return 2;

if(hotkeys) strupr(hotkeys);

linecounter(0);
for(;;)
   {
   byte c=toupper(readkey());
   if(hotkeys && c) if(strchr(hotkeys,c))
   {
      return c;
   }
   if(c=='S' && stopok)
	 {
	 stopped=1;
    flush();
	 return 1;
	 }
   if(c=='P') wait("\rP");

   int x=f.readByte();
   if(x<0) break;

   byte input=(byte)x;

   if(avt_compr)
     {
      if(fflag || kflag)
        {
         if(!avt_compr_char)
           {
            avt_compr_char = input;
           }
          else
           {
            if(avt_compr_char=='@' || avt_compr_char=='#' || avt_compr_char=='%')
              {
               fieldwidth += input;

               switch(avt_compr_char)
                 {
                  case '@': alignment = ALIGN_LEFT;   break;
                  case '#': alignment = ALIGN_RIGHT;  break;
                  case '%': alignment = ALIGN_CENTER; break;
                 }
              }
            avt_compr = FALSE;
           }
        }
       else
        {
         avt_compr = FALSE;

         (*this) << char(25) << input;
        }

      continue;
     }

   if(SCREEN.inEscape())
     {
      (*this) << input;
      continue;
     }

   if(input == 0x19)
     {
      avt_compr = TRUE;
      avt_compr_char = '\0';
      continue;
     }

   if((fflag || kflag) && (input=='@' || input=='#' || input=='%'))
     {
      fieldwidth++;

      switch(input)
         {
          case '@': alignment = ALIGN_LEFT;   break;
          case '#': alignment = ALIGN_RIGHT;  break;
          case '%': alignment = ALIGN_CENTER; break;
         }

      continue;
     }

   if(fflag)
      {
      switch(input)
        {
        case 'A': str = user.name;                                 break;
        case 'B': str = user.city;                                 break;
        case 'C': str = user.passWord;                             break;
        case 'D': str = user.dataPhone;                            break;
        case 'E': str = user.voicePhone;                           break;
        case 'F': str = user.lastDate.format("D MMM CCYY");        break;
        case 'G': str = user.lastTime.format("HH:MM:SS");          break;
        case 'H':
        case 'I':
        case 'J':
        case 'K': {
                    char s[33];
                    user.aFlags.flagstostr(s);
                    str = s;
                  } break;
        case 'L':  str = form("%d",user.credit);       break;
        case 'M':  str = form("%lu",user.msgsPosted);   break;
        case 'N':  str = form("%d",user.highMsgRead);  break;
        case 'O':  str = form("%u",user.level);        break;
        case 'P':  str = form("%lu",user.timesCalled);  break;
        case 'Q':  str = form("%lu",user.numUploads);   break;
        case 'R':  str = form("%lu",user.kbUploaded);   break;
        case 'S':  str = form("%lu",user.numDownloads); break;
        case 'T':  str = form("%lu",user.kbDownloaded); break;
        case 'U':  str = form("%ld",user.timeUsed);     break;
        case 'V':  str = form("%d",user.screenLength);  break;
        case 'W':  str = user_firstname;                           break;
        case 'X':  str = (ansi_mode?  on_string:off_string);       break;
        case 'Y':  str = (pause_mode? on_string:off_string);       break;
        case 'Z':  str = (cls_mode?   on_string:off_string);       break;
        case '0':  str = (fsed_mode?  on_string:off_string);       break;
        case '1':  str = user.alias;                               break;
        case '2':  str = (stack_mode? off_string:on_string);       break;
        case '3':  str = (no_ibm?     off_string:on_string);       break;
        case '4':  str = user.state;                               break;
        case '5':  str = user.birthDate.format("D MMM CCYY");      break;
        case '6':  str = user.expDate.format("D MMM CCYY");        break;
        case '7':  str = form("%d",int(user.expDate - Date(TODAY)));   break;
        case '8':  str = (avatar?     on_string:off_string);       break;
        case '9':  str = (avtplus?    on_string:off_string);       break;
        case '>':  str = user.forwardTo;                           break;
        case '<':  str = date_formats_long[user.dateFormat];       break;
        case '[':  str = form("%ld",long(download_limit)-user.kbToday);    break;
        case '!': if(timer.online()<download_delay) str = form("%d",download_delay-timer.online());
                                               else str = "0";
                  break;
        case '$':  str = user.address1;                            break;
        case '\'': str = user.address2;                            break;
        case '&':  str = user.address3;                            break;
        case '`': switch(user.sex)
                   {
                     case  1: str = S_MALE;    break;
                     case  2: str = S_FEMALE;  break;
                     default: str = S_UNKNOWN; break;
                   }
                   break;

        case '~':  str = form("%d",download_delay);    break;
        case ':':  str = user.firstDate.format("D MMM CCYY");        break;
        case '\"': str = user.country; break;
        case '*':  str = form("%d",user.fileGroup); break;
        case '+':  str = form("%d",user.msgGroup); break;
        case '(':  {
                     AreaGroup group;
                     File f(FileName(syspath,"FGROUPS.PB"));
                     f.seek(long(user.fileGroup-1) * sizeof(AreaGroup));
                     if(f.read(&group,sizeof(AreaGroup)) != sizeof(AreaGroup))
                        str = "";
                     else
                        str = group.name;
                   }
                   break;
        case ')':  {
                     AreaGroup group;
                     File f(FileName(syspath,"MGROUPS.PB"));
                     f.seek(long(user.msgGroup-1) * sizeof(AreaGroup));
                     if(f.read(&group,sizeof(AreaGroup)) != sizeof(AreaGroup))
                        str = "";
                     else
                        str = group.name;
                   }
                   break;
        case '=' : str = user.faxPhone;  break;
        case '\\': str = user.language; break;
        case '^' : break;

        
        //------------  
        // New in v2.2
        //------------  

        case ';':

            if ( cfg.hidePassword )
            {
               str = String( cfg.pwdchar, strlen( user.passWord ) );
            }
            else
            {
               str = user.passWord;
            }

            break;
        }

      (*this) << align_string(str, fieldwidth , alignment);

      fflag=0;
      continue;
      }

   if(kflag)
      {
      Date dd(TODAY);
      Time tt(NOW);

      str.clear();

      switch(input)
         {
         case 'A': str = form("%ld",totalcalls); break;
         case 'B': str = lastcaller.name; break;
         case 'C': str = form("%ld",msgbase.totalMsgs()); break;
         case 'D': str = form("%d",0/*minfo.low*/);   break;
         case 'E': str = form("%d",0/*minfo.high*/);  break;
         case 'F': str = form("%d",num_yells);   break;
         case 'G': str = days_full[dd.weekDay()]; break;
         case 'H': str = form("%d",num_users);   break;
         case 'I': str = tt.format("HH:MM");      break;
         case 'J': str = dd.format("DD MMM CCYY");break;
         case 'K': str = form("%d",timer.online());   break;
         case 'L': str = "0"; break;
         case 'M': str = form("%ld",user.timeUsed);   break;
         case 'N': str = "0"; break;
         case 'O': str = form("%d",timer.left());   break;
         case 'P': str = VERSION;        break;
         case 'Q': str = form("%d",time_limit);   break;
         case 'R': str = form("%ld",io.baud);   break;
         case 'S': str = days_short[dd.weekDay()];   break;
         case 'T': str = form("%d",download_limit);   break;
         case 'U': str = form("%d",nextevent.minutesleft()); break;
         case 'V': str = form("%02d:%02d",nextevent.start[0],nextevent.start[1]); break;
         case 'W': str = form("%d",node_number);   break;
         case 'X': LOG("Hangup from text file");
                   io.flush();
                   sleep(2);
                   exit_proboard();
         case 'Y': {
                   MsgArea ma;
                   if(ma.read(user.msgArea))
                     str = ma.name;
                   else
                     str = "* None *";
                   } break;
         case 'Z': {
                   FileArea fa;
                   if(fa.read(user.fileArea))
                     str = fa.name;
                   else
                     str = "* None *";
                   } break;
         case '0': {
                    MsgArea ma;
                    ma.read(user.msgArea);
                    str = form("%ld",ma.numMsgs());
                   }
                   break;
         case '1': str = form("%d",user.msgArea); break;
         case '2': str = form("%d",user.fileArea); break;
         }

      (*this) << align_string(str, fieldwidth , alignment);

      kflag=0;
      continue;
      }
   fflag=kflag=fieldwidth=0;
   if(input<27)
    {
    switch(input)
       {
       case  1: wait('\r');
                break;
       case  2: stopok = FALSE;
                break;
       case  3: stopok = TRUE ;
                break;
       case  4: moreok = TRUE ;
                break;
       case  5: moreok = FALSE;
                break;
       case  6: fflag = 1;
                fieldwidth = 2;
                break;
       case 10: (*this) << input;
                if(moreok)
                  if(linecounter())
                  {
                     flush();
                     return 1;
                  }
                break;
       case 11: kflag=1;
                fieldwidth=2;
                break;
       //case 12: cls();
       //         break;
       case 17: break;                         // XON/XOFF
       case 19: break;                         // XON/XOFF
       case 23: sleep(1);
                break;
       case 24: break;
       case 26: break;                         // EOF
       default: kflag = fflag = FALSE;
                (*this) << input;
                break;
       }
    }
   else
    (*this) << input;
   }

   flush();

   return 0;
}

#pragma warn .def

byte
fossilio::wait()
{
 SCREEN.placeCursor();

 timer.check();
 timer.clearinactivity();

 flush();

 byte c;

 if(comstack.pollnext()) return comstack.getnext();

 redo:

 c = inputbuffer.getnext();

 if(c)
   {
    external = 0;
    return c;
   }

 while(!(c=readkey()))
   {
    static unsigned long ticks = clockticks();

    if(clockdiff(ticks)>90)      // Update info-window every 5 seconds.
      {
       update_display();
       ticks = clockticks();
      }

    if(inputbuffer.pollnext()) goto redo;
    timer.checkinactivity();
    DV_timeslice();
   }

 return c;
}

void
fossilio::wait(char c)
{
   while(toupper(wait())!=toupper(c)) {}
}

byte
fossilio::wait(char *s)
{
   strupr(s);

   for(;;)
   {
      char k = toupper(wait());

      if(strchr(s,k)) return k;
   }
}

void
fossilio::drawfield(int max)
{
   int i;

   if(ansi_mode || avatar)
   {
      fullcolor((SCREEN.attrib()) & (0xF) | (cfg.promptColor<<4));

      if(!avatar)
      {
         for(i=0;i<max;i++) (*this) << ' ';
         (*this) << form("\x1b[%dD",max);
      }
      else
      {
         (*this) << char(25) << ' ' << char(max);

         if(avtplus)
         {
            (*this) << char(22) << char(25) << char(2)
                     << char(22) << char(5)  << char(max);
         }
         else
            for(i=0;i<max;i++) (*this) << '\b';
      }
   }
}

void
fossilio::read(char *s,int max,byte mode)
{
 char k;
 int i=0;

 if(!(mode&READMODE_NOFIELD))
   drawfield(max);


 i=0;
 while((k=wait())!=13)
    {
     if(k==27 || k==10 || k==9) continue;
     if(k==8)
       {
        if(i>0)
          {
           i--;
           (*this) << "\b \b";
          }
       }
      else if(i<max && (isprint(k) || (k & 0x80)))
              {
               switch(mode & 7)
                 {
                  case READMODE_ALL    : break;
                  case READMODE_DIGITS : if(!isdigit(k)) continue;
                                         break;
                  case READMODE_UPFIRST: if(s[i-1]=='.' ||  s[i-1]==' ' ||  s[i-1]=='-' || i==0)
                                                k=toupper(k);
                                           else k=tolower(k);
                                         break;
                  case READMODE_UPALL  : k=toupper(k);
                 }
               s[i++]=k;
               if(mode &  READMODE_PWD) (*this) << cfg.pwdchar;
                                   else (*this) << k;
              }
    }

   io << '\r';
   s[i]='\0';

   if(avatar)
      (*this) << char(22) << char(1) << char(SCREEN.attrib()&0x0F);
   else
      if(ansi_mode) (*this) << "\x1b[40m";

   flush();
}

int
fossilio::read(int& i,int len)
{
   long l;
   int ret = read(l,(len<5)?len:5);

   if(ret >= 0) i = int(l);

   return ret;
}

int
fossilio::read(long& i,int len)
{
   char s[10];

   read(s,(len<10)?len:10,READMODE_DIGITS);
   if(!s[0]) return -1;

   i = atol(s);

   return 0;
}


void
fossilio::hangup()
{
   if(baud) fos_setdtr(port,0);
}

int
fossilio::ask(int def)
{
 int ret;
 String keys;

 if(def)
 {
   io  << S_ASK_YES_NO;
   keys = K_ASK_YES_NO;
 }
 else
 {
   io  << S_ASK_NO_YES;
   keys = K_ASK_NO_YES;
 }

 keys << '\r';

 char k = wait(keys);

 if(k == keys[0])
   ret = TRUE;
 if(k == keys[1])
   ret = FALSE;
 if(k == keys[2])
   ret = def;

 if(ret)
   (*this) << S_YES;
 else
   (*this) << S_NO;

 (*this) << char(0xFF);

 return ret;
}

void
fossilio::color(byte c)
{
   current_color=c;

   int blink=(int)(c & 0x80);
   c &= 0x7;

   if(avatar)
      {
      static char colors[] = { 0,4,2,6,1,5,3,7 };

      (*this) << char(22) << char(1);
      (*this) << char(colors[c]|8);
      if(blink) (*this) << char(22) << char(2);
      return;
      }

   if(!ansi_mode) return;

   String s("\x1b[0;1;3");

   s << (char)(c+'0');
   if(blink) s << ";5";
   s << 'm';

   for(int i=0;s[i];i++) (*this) << s[i];
}

void
fossilio::cls()
{
   (*this) << '\f' << char(0xFF);
}


void
fossilio::enablestop()
{
 stopenabled = 1;
 stopped = 0;
}

void
fossilio::disablestop()
{
 stopenabled = stopped = 0;
}

static
String
align_string(char *s , int width , int alignment)
{
    String ret_s;
    int i,j = width-strlen(s);

    if(strlen(s) >= width) return String(s);

    switch(alignment)
    {
        case ALIGN_LEFT   : for(i=0;i<width && s[i];i++) ret_s << s[i];
                            for(   ;i<width;i++) ret_s << ' ';
                            break;
        case ALIGN_CENTER : j = (width-strlen(s))/2;
        case ALIGN_RIGHT  : for(i=0 ; i<j ; i++) ret_s << ' ';
                            for(    ; i<width && s[i-j] ; i++) ret_s << s[i-j];
                            for(    ; i<width           ; i++) ret_s << ' ';
    }

    return ret_s;
}
