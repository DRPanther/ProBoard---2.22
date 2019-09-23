#define Use_MsgBase

#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <tswin.hpp>
#include "proboard.hpp"

extern "C" int swapshell(char *command);

static void write_dorinfo(int node,bool write_handle,bool write_avatar);
static void write_exitinfo1();
static void read_exitinfo1();
static void write_exitinfo2();
static void read_exitinfo2();
static void write_doorsys();

unsigned shell_windowed   = 0;
unsigned shell_updateline = 0;
unsigned shell_swap       = 0;

char shell_swapname[13];

static bool use_underscore;

inline void
test_ems()
{
 char far *ptr = (char far *) MK_FP(*((word *)MK_FP(0,0x67*4+2)),10);

 if(strncmp(ptr,"EMMXXXX0",8)) use_ems = 0;
}


static void
set_environment()
{
 static char _far *alloc_ptr = NULL;

 int envlen = 1;       // Start with 1 (extra zero byte at end of table)

 for(int i=0;environ[i];i++) envlen += strlen(environ[i])+1;

 if(alloc_ptr != NULL) free(alloc_ptr);

 char far *eptr = (char far *)malloc(envlen+16);

 alloc_ptr = eptr;

 if(eptr == NULL) return;

 eptr = (char far *)normalize_ptr(eptr);          // Normalize pointer
 eptr = (char far *)MK_FP(FP_SEG(eptr)+1,0);      // Make zero-offset pointer

 char _far *p = eptr;

 for(i=0;environ[i];i++)
   {
    memcpy(p,environ[i],strlen(environ[i])+1);
    p += strlen(environ[i]) + 1;
   }

 *p = 0;

 *((word _far *)MK_FP(_psp,0x2C)) = FP_SEG(eptr);
}

void
shell(char *data)
{
 bool hotfossil      = FALSE;
 bool log            = TRUE;
 bool message        = TRUE;
 bool old_exitinfo   = FALSE;
 bool do_door_sys    = FALSE;
 bool suspend        = FALSE;
 bool sysop          = FALSE;
 bool no_io          = FALSE;
 bool doscmd         = FALSE;
 bool write_handle   = FALSE;
 bool write_avatar   = TRUE;

#ifdef DEBUG_EFL
 LOG("EFLDEBUG: [SP=%04X] shell() entry",_SP);
#endif

 use_underscore      = TRUE;

 int  dorinfo_node = node_number;

 test_ems();
 shell_swap = cfg.doswap;

 String commandline;

 shell_windowed   = 0;
 shell_updateline = 0;

 bool flag = FALSE;

 for(;*data;data++)
    {
     if(*data=='*' && !flag)
       {
        flag = TRUE;
        continue;
       }

     if(flag)
       {                                       // free: J,K,U
       switch(toupper(*data))
         {
         case '*': commandline << '*';                            break;
         case '#': commandline << node_number;                    break;
         case 'A': write_handle = TRUE;                           break;
         case 'B': commandline << io.baud;                        break;
         case 'C': commandline << getenv("COMSPEC");              break;
         case 'D': do_door_sys = TRUE;                            break;
         case 'E': old_exitinfo = TRUE;                           break;
         case 'F': commandline << user_firstname;                 break;
         case 'G': commandline << ((ansi_mode)?1:0);              break;
         case 'H': hotfossil = TRUE;                              break;
         case 'I': commandline << cfg.inactivity_time;            break;
    //   case 'J': break;
    //   case 'K': break;
         case 'L': {
                    String s(user.name);
                    for(int i=0;i<s.len();i++) if(s[i]==' ') break;
                    commandline << (char *)(&s[i+1]);
                   }                                              break;
         case 'M': commandline << mypath;                         break;
         case 'N': log = FALSE;                                   break;
         case 'O': {
                    FileArea fa;
                    if(!fa.read(user.fileArea))
                      {
                       user.fileArea = 1;
                       fa.read(1);
                      }
                    commandline << fa.filepath;
                   } break;
         case 'P': commandline << (io.port+1);                    break;
         case 'Q': message = FALSE;                               break;
         case 'R': commandline << user_recnr;                     break;
         case 'S': commandline << syspath;                        break;
         case 'T': commandline << timer.left();                   break;
    //   case 'U':
         case 'V': write_avatar = FALSE;                          break;
         case 'W': shell_windowed=1;                              break;
         case 'X': shell_swap=1;                                  break;
         case 'Y': shell_swap=0;                                  break;
         case 'Z': doscmd = TRUE;                                 break;
         case '0': dorinfo_node = 1;                              break;
         case '1': shell_updateline=1;                            break;
         case '2': shell_updateline=tsw_vsize;                    break;
         case '!': suspend = TRUE;                                break;
         case '\\': sysop = TRUE;                                 break;
         case '=': no_io = TRUE;                                  break;
         case '_': use_underscore = FALSE;                        break;
         }

        flag = FALSE;
       }
      else
       {
        if(*data!=' ' || commandline.len()) commandline << *data;
       }
    }

 if(doscmd)
   {
    commandline.insert(0," /C ");
    commandline.insert(0,getenv("COMSPEC"));
   }

 if(log) LOG(1,"DOS SHELL: %s",(char *)commandline);

 if(sysop) io << '\n' << S_SYSOP_SHELLING << char(255);

 if(message) io << "\n\f" << S_LOADING << "\n\xFF";

 user_online uo;
 uo.setstatus(UO_SHELLING);

 if(user_recnr >= 0) user.write(FALSE);

 write_taglist();

 FileArea::close();

 msgbase.close();

 /*
 f_users   .close();
 f_usersxi .close();
 f_usersidx.close();
 f_userspb .close();
 */

 display_mode = 1;

 word *screenbuf=new word[tsw_hsize*tsw_vsize];
 tsw_gettext(screenbuf,1,1,tsw_hsize,tsw_vsize);

 if(!hotfossil && !no_io) io.cooldown();

 sprintf(shell_swapname,"PB$$$%03.3d.SWP",node_number);

 byte videomode = getvideomode();

 unsigned old_vsize = tsw_vsize;
 unsigned old_hsize = tsw_hsize;

 write_dorinfo(dorinfo_node,write_handle,write_avatar);

 if(old_exitinfo)
   write_exitinfo1();
 else
   write_exitinfo2();

 if(do_door_sys) write_doorsys();

 if(suspend) timer.suspend();

 set_environment();

#ifdef DEBUG_EFL
 LOG("EFLDEBUG: [SP=%04X] Before swapshell()",_SP);
#endif

 returnvalue=swapshell(commandline);

#ifdef DEBUG_EFL
 LOG("EFLDEBUG: [SP=%04X] After  swapshell()",_SP);
#endif

 if(suspend) timer.restart();

 timer.clearinactivity();

 if(old_exitinfo)
   read_exitinfo1();
 else
   read_exitinfo2();

 if(tsw_videocard==EGA) tsw_vsize = *((byte _far *)0x484)+1;

 tsw_hsize = *((byte far *)0x44A);

 if((old_vsize != tsw_vsize) || (old_hsize != tsw_hsize))
   {
    setvideomode(videomode);

    if((videomode == 3) && (old_vsize>25)) set43();

    if(tsw_videocard==EGA) tsw_vsize=*((byte _far *)0x484)+1;

    tsw_hsize = *((byte far *)0x44A);
   }

 SCREEN.moveCursor(0,0);

 if(!hotfossil && !no_io) io.heatup();

 read_taglist();

 msgbase.open();

 uo.setstatus(UO_BROWSING);

 tsw_puttext(screenbuf,1,1,tsw_hsize,tsw_vsize);

 timer.clearinactivity();

 if(rip_mode) io << "\r\r[2!";

 if(sysop) io << S_SYSOP_RETURNED_FROM_DOS << char(255);

 unlink(form("%sDORINFO%d.DEF",mypath,dorinfo_node));
 unlink(form("%sDOOR.SYS",mypath));
 unlink(form("%sEXITINFO.BBS",mypath));

 delete [] screenbuf;

 updatemenu = TRUE;
}

static void
write_dorinfo(int node,bool write_handle,bool write_avt)
{
 int i;

 File f(form("%sDORINFO%d.DEF",mypath,node),fmode_create | fmode_text);

 String sysopname (cfg.sysopname);
 String username  (user.name);
 String usercity  (user.city);
 String boardname (cfg.bbsname);

 if(write_handle) if(user.alias[0]) username = user.alias;

 sysopname.upperCase();
 username.upperCase();
 usercity.upperCase();
 boardname.upperCase();

 f << boardname << '\n';

 for(i=0;i<sysopname.len();i++)
   if(sysopname[i]==' ')
     {
      sysopname[i++] = '\0';
      break;
     }

 f << sysopname << '\n';

 if(sysopname[i])
   for(int j=0;j<strlen(&sysopname[i]);j++)
     f << char((sysopname[i+j]==' ' && use_underscore)? '_' : sysopname[i+j]);

 f << form("\nCOM%d\n%ld BAUD,N,8,1\n0\n",(io.baud)?(io.port+1):0,io.baud);

 for(i=0;i<username.len();i++)
   if(username[i]==' ')
     {
      username[i++] = '\0';
      break;
     }

 f << username << '\n';

 if(username[i])
   for(int j=0;j<strlen(&username[i]);j++)
     f << char((username[i+j]==' ' && use_underscore)? '_' : username[i+j]);

 f << form("\n%s\n",(char *)usercity);

 int graphics = (ansi_mode)?1:0;
 if(write_avt && (avatar || avtplus)) graphics += 2;

 f << form("%d\n%u\n%d\n",graphics,user.level,timer.left());
}

static void
write_doorsys()
{
 File f(form("%sDOOR.SYS",mypath),fmode_create | fmode_text);

 f << form("COM%d:\n",(io.baud)?(io.port+1):0);
 f << form("%ld\n8\n",io.baud);
 f << form("%d\n",node_number);
 f << form("%ld\nY\nN\nN\nN\n",io.baud);
 f << user.name << '\n';
 f << user.city << '\n';
 f << user.voicePhone << '\n';
 f << user.dataPhone << '\n';
 f << '\n';
 f << form("%u\n",user.level);
 f << form("%lu\n",user.timesCalled);
 f << user.lastDate.format("MM-DD-YY") << '\n';        // Y2K BUG!
 f << form("%ld\n",60L*timer.left());        
 f << form("%d\n",timer.left());
 if(ansi_mode)
   f << "GR\n";
 else
   f << "NG\n";
 f << form("%d\nN\n\n\n",user.screenLength);
 f << user.expDate.format("MM-DD-YY") << '\n';         // Y2K BUG!
 f << form("%d\n",user_recnr);
 f << (user.defaultProtocol ? form("%c\n",user.defaultProtocol) : "\n");
 f << form("%ld\n",user.numUploads);
 f << form("%ld\n",user.numDownloads);
 f << form("%ld\n",user.kbToday);
 f << form("%d\n",download_limit);
 f << user.birthDate.format("MM-DD-YY") << '\n';       // Y2K BUG!
 f << cfg.msgpath << '\n';
 f << cfg.msgpath << '\n';
 f << cfg.sysopname << '\n';
 f << user.alias << '\n';
 f << nextevent.start.format("HH:MM") << '\n';         
 f << "N\nN\nY\n7\n0\n";
 f << user.lastDate.format("MM-DD-YY") << '\n';        // Y2K BUG!
 f << login_time.format("HH:MM") << '\n';
 f << user.lastTime.format("HH:MM") << '\n';
 f << "32768\n";
 f << "0\n";
 f << form("%ld\n",user.kbUploaded);
 f << form("%ld\n",user.kbDownloaded);
 f << user.comment << "\n0\n";
 f << form("%lu\n",user.msgsPosted);
}

static long
swapbits(long x)
{
 long y = 0;

 for(int i=0;i<32;i++) if(TESTBIT(x,long(i))) SETBIT(y,long(31-i));

 return y;
}

struct exitinfo2
  {
   unsigned BaudRate;

   long     CallCount;             // Sysinfo record
   char     LastCaller[36];
   byte     Extra1[128];

   char     StartDate[9];          // Timelog record
   int      BusyPerHour[24];
   int      BusyPerDay[7];

   RA2_UsersBBS users_bbs;      //*// User record

   byte     EventStatus;           // Event record
   char     EventRunTime[6];
   byte     EventErrorLevel;
   byte     EventDays;
   byte     EventForced;
   char     EventLastTimeRun[9];

   bool     NetMailEntered;
   bool     EchoMailEntered;
   char     LoginTime[6];
   char     LoginDate[9];
   int      TmLimit;
   long     LoginSec;
//   long     dummy;              // ???
   int      UserRecNum;
   int      ReadThru;
   int      PageTimes;
   int      DownLimit;
   char     TimeOfCreation[6];
   /*
   char     LogonPassword[16];  //*
   */
   dword    LogonPasswordCRC;   //*
   byte     WantChat;
   int      DeductedTime;
   char     MenuStack[50][9];
   byte     MenuPointer;

   RA2_UsersXiBBS usersxi_bbs;  //*// UsersXI record

   bool     ErrorFreeConnect;
   bool     SysopNext;

   byte     Extra4[209];

   char     PageReason[81];
   byte     StatusLine;
   char     LastCostMenu[9];
   word     MenuCostPerMin;
   bool     DoesAVT;
   bool     RIPmode;

   byte     extra5[86];
  };

struct exitinfo1
  {
   unsigned BaudRate;
   long     CallCount;
   char     LastCaller[36];
   byte     Extra1[128];
   char     StartDate[9];
   int      BusyPerHour[24];
   int      BusyPerDay[7];

   RA1_UsersBBS users_bbs;

   byte     EventStatus;
   char     EventRunTime[6];
   byte     EventErrorLevel;
   byte     EventDays;
   byte     EventForced;
   char     EventLastTimeRun[9];
   byte     NetMailEntered;
   byte     EchoMailEntered;
   char     LoginTime[6];
   char     LoginDate[9];
   int      TmLimit;
   long     LoginSec;
   long     dummy;
   int      UserRecNum;
   int      ReadThru;
   int      PageTimes;
   int      DownLimit;
   char     TimeOfCreation[6];
   char     LogonPassword[16];
   byte     WantChat;
   int      DeductedTime;
   char     MenuStack[50][9];
   byte     MenuPointer;

   RA1_UsersXiBBS usersxi_bbs;

   byte     ErrorFreeConnect;
   byte     SysopNext;
   byte     Extra4[309];
};



static int time_adj;



static 
void write_exitinfo2()
{
     struct exitinfo2 *ei  =  new exitinfo2;


     memset( ei,
             0,
             sizeof( exitinfo2 ) );


     RA2_UsersPbBBS upb;


     time_adj       = timer.used();
     user.timeUsed += time_adj;


     user.toRa( ei->users_bbs,
                ei->usersxi_bbs,
                upb );

     ei->BaudRate  = io.baud;
     ei->CallCount = totalcalls;


     strcpy( ei->LastCaller, lastcaller.name );
     c2pas(  ei->LastCaller );

     strcpy( ei->StartDate, "01-01-80" );
     c2pas(  ei->StartDate );


     ei->TmLimit     =  timer.left();
     ei->DownLimit   =  int( download_limit - user.kbToday );
     ei->UserRecNum  =  user_recnr;
     ei->LoginSec    =  3600L * login_time[ 0 ]  +  
                        60L   * login_time[ 1 ]  +  
                                login_time[ 0 ];


     Date now(NOW);


     sprintf( ei->TimeOfCreation,
              "%02d:%02d",
              now[ 0 ],
              now[ 1 ] );

     c2pas( ei->TimeOfCreation );


     sprintf( ei->LoginTime,
              "%02d:%02d",
              login_time[ 0 ],
              login_time[ 1 ] );

     c2pas( ei->LoginTime );


     sprintf( ei->LoginDate,
              "%02d-%02d-%02d",
              login_date[ 0 ],
              login_date[ 1 ],
              login_date[ 2 ]  %  100 );               // Y2K FIXED!
           // login_date[ 2 ] );                       // Y2K BUG!

     c2pas( ei->LoginDate );


     ei->PageTimes = num_yells;


     sprintf( ei->EventRunTime,
              "%02d:%02d",
              nextevent.start[ 0 ],
              nextevent.start[ 1 ] );

     c2pas( ei->EventRunTime );


     if ( nextevent.enabled ) 
     {
          ei->EventStatus = 1;
     }
     else 
     {
          ei->EventStatus = 2;
     }


     ei->EventErrorLevel = nextevent.errorlevel;


     if ( nextevent.enabled ) 
     {
          ei->EventDays = nextevent.days;
     }
     else 
     {
          ei->EventDays = 0;
     }


     strcpy( ei->EventLastTimeRun, "01-01-80" );
     c2pas(  ei->EventLastTimeRun );


     ei->NetMailEntered   =  ( net_entered  )  ? TRUE : FALSE;
     ei->EchoMailEntered  =  ( echo_entered )  ? TRUE : FALSE;

     ei->RIPmode  =  ! ! rip_mode;
     ei->DoesAVT  =  ! ! avatar;


     File f( form( "%sEXITINFO.BBS", mypath ),
                   fmode_create );

     f.write( ei,  sizeof( exitinfo2 ) );


     delete ei;
}

static void
write_exitinfo1()
{
 struct exitinfo1 *ei = new exitinfo1;

 memset(ei,0,sizeof(exitinfo1));

 ei->BaudRate  = io.baud;
 ei->CallCount = totalcalls;

 strcpy(ei->LastCaller,lastcaller.name);
 c2pas(ei->LastCaller);
 strcpy(ei->StartDate,"01-01-80");
 c2pas(ei->StartDate);

 ei->TmLimit    = timer.left();
 ei->LoginSec   = 3600L*login_time[0]+60L*login_time[1]+login_time[0];
 ei->DownLimit  = int(download_limit-user.kbToday);
 ei->UserRecNum = user_recnr;

 Date now(NOW);
 sprintf(ei->TimeOfCreation,"%02d:%02d",now[0],now[1]);
 c2pas(ei->TimeOfCreation);
 sprintf(ei->LoginTime,"%02d:%02d",login_time[0],login_time[1]);
 c2pas(ei->LoginTime);
 sprintf(ei->LoginDate,"%02d-%02d-%02d",login_date[0],login_date[1],login_date[2]);
 c2pas(ei->LoginDate);

 ei->PageTimes=num_yells;

 sprintf(ei->EventRunTime,"%02d:%02d",nextevent.start[0],nextevent.start[1]);
 c2pas(ei->EventRunTime);

 if(nextevent.enabled) ei->EventStatus = 1;
                  else ei->EventStatus = 2;

 ei->EventErrorLevel = nextevent.errorlevel;

 if(nextevent.enabled) ei->EventDays = nextevent.days;
                  else ei->EventDays = 0;

 strcpy(ei->EventLastTimeRun,"01-01-80");
 c2pas(ei->EventLastTimeRun);

 ei->NetMailEntered  = (net_entered)  ? TRUE : FALSE;
 ei->EchoMailEntered = (echo_entered) ? TRUE : FALSE;

 strcpy(ei->users_bbs.Name,user.name);
 strcpy(ei->users_bbs.Location,user.city);
 strcpy(ei->users_bbs.Password,user.passWord);
 strncpy(ei->users_bbs.DataPhone,user.dataPhone,12);
         ei->users_bbs.DataPhone[12]='\0';
 strncpy(ei->users_bbs.VoicePhone,user.voicePhone,12);
         ei->users_bbs.VoicePhone[12]='\0';
 strcpy(ei->usersxi_bbs.Handle,user.alias);
 strcpy(ei->usersxi_bbs.Comment,user.comment);
 strcpy(ei->usersxi_bbs.ForwardFor,user.forwardTo);

 sprintf(ei->usersxi_bbs.BirthDate,"%02d-%02d-%02d",user.birthDate[1],user.birthDate[0],user.birthDate[2]);
 sprintf(ei->usersxi_bbs.FirstDate,"%02d-%02d-%02d",user.firstDate[1],user.firstDate[0],user.firstDate[2]);
 sprintf(ei->usersxi_bbs.SubDate  ,"%02d-%02d-%02d",user.expDate[1],user.expDate[0],user.expDate[2]);
 sprintf(ei->users_bbs  .LastDate ,"%02d-%02d-%02d",user.lastDate[1],user.lastDate[0],user.lastDate[2]);
 sprintf(ei->users_bbs  .LastTime ,"%02d:%02d"     ,user.lastTime[0],user.lastTime[1]);

 ei->users_bbs.Security     = user.level        ;
 ei->users_bbs.NoCalls      = user.timesCalled  ;
 ei->users_bbs.Elapsed      = user.timeUsed     ;
 ei->users_bbs.Downloads    = user.numDownloads ;
 ei->users_bbs.DownloadsK   = user.kbDownloaded  ;
 ei->users_bbs.Uploads      = user.numUploads   ;
 ei->users_bbs.UploadsK     = user.kbUploaded    ;
 ei->users_bbs.MsgsPosted   = user.msgsPosted   ;
 ei->users_bbs.TodayK       = user.kbToday       ;
 ei->users_bbs.ScreenLength = user.screenLength  ;
 ei->users_bbs.Credit       = user.credit       ;
 ei->users_bbs.Pending      = user.pending      ;
 ei->users_bbs.LastRead     = user.highMsgRead ;

 for(int z=0;z<25;z++) ei->usersxi_bbs.CombinedInfo[z] = ((byte *)(&user.combinedBoards))[z];

 ei->users_bbs.Flags       = swapbits(*((long *)&user.aFlags));

 if(user.uFlags & UFLAG_DELETED) ei->users_bbs.Attribute |= RA1_UFLAG_DELETED;
 if(user.uFlags & UFLAG_CLEAR)   ei->users_bbs.Attribute |= RA1_UFLAG_CLRSCR;
 if(user.uFlags & UFLAG_PAUSE)   ei->users_bbs.Attribute |= RA1_UFLAG_MORE;
 if(user.uFlags & UFLAG_ANSI)    ei->users_bbs.Attribute |= RA1_UFLAG_ANSI;
 if(user.uFlags & UFLAG_NOKILL)  ei->users_bbs.Attribute |= RA1_UFLAG_NOKILL;
 if(user.uFlags & UFLAG_IGNORE)  ei->users_bbs.Attribute |= RA1_UFLAG_IGNORE;
 if(user.uFlags & UFLAG_FSED)    ei->users_bbs.Attribute |= RA1_UFLAG_FSE;
// if(uflags & UFLAG_QUIET)   ei->users_bbs.Attribute |= RA_UFLAG_QUIET;

 if(user.uFlags & UFLAG_HIDDEN)  ei->users_bbs.Attribute2 |= RA1_UFLAG2_HIDDEN;
 if(user.uFlags & UFLAG_AVATAR)  ei->users_bbs.Attribute2 |= RA1_UFLAG2_AVATAR;

 if(user.uFlags & UFLAG_HOTKEYS) ei->users_bbs.Attribute2 |= RA1_UFLAG2_HOTKEYS;

 ei->usersxi_bbs.MsgArea  = user.msgArea & 0xFF;
 ei->usersxi_bbs.FileArea = user.fileArea & 0xFF;

 c2pas(ei->users_bbs.Name);
 c2pas(ei->users_bbs.Location);
 c2pas(ei->users_bbs.Password);
 c2pas(ei->users_bbs.DataPhone);
 c2pas(ei->users_bbs.VoicePhone);
 c2pas(ei->users_bbs.LastTime);
 c2pas(ei->users_bbs.LastDate);
 c2pas(ei->usersxi_bbs.Handle);
 c2pas(ei->usersxi_bbs.Comment);
 c2pas(ei->usersxi_bbs.BirthDate);
 c2pas(ei->usersxi_bbs.SubDate);
 c2pas(ei->usersxi_bbs.FirstDate);
 c2pas(ei->usersxi_bbs.ForwardFor);

 File f(form("%sEXITINFO.BBS",mypath),fmode_create);
 f.write(ei,sizeof(exitinfo1));
 delete ei;
}



static void
read_exitinfo2()
{
 struct exitinfo2 *ei = new exitinfo2;

 RA2_UsersPbBBS upb;

 user.toRa(ei->users_bbs,ei->usersxi_bbs,upb);

 memset(ei,0,sizeof(exitinfo2));

 File f(form("%sEXITINFO.BBS",mypath));
 if(f.read(ei,sizeof(exitinfo2)) != sizeof(exitinfo2))
   {
    delete ei;
    return;
   }
 f.close();

 num_yells = ei->PageTimes;

 unsigned oldlevel = user.level;

 user.fromRa(ei->users_bbs,ei->usersxi_bbs,upb);

 user.timeUsed -= time_adj;

 timer.changeleft(int(time_limit-timer.used()-user.timeUsed-time_removed));

 net_entered = ei->NetMailEntered;
 net_entered = ei->EchoMailEntered;

 if(user.level != oldlevel) adjust_limits();

 delete ei;
}

static void
read_exitinfo1()
{
 struct exitinfo1 *ei = new exitinfo1;

 memset(ei,0,sizeof(exitinfo1));

 File f(form("%sEXITINFO.BBS",mypath));
 if(f.read(ei,sizeof(exitinfo1)) != sizeof(exitinfo1))
   {
    delete ei;
    return;
   }
 f.close();

 num_yells = ei->PageTimes;

 unsigned oldlevel = user.level;

 user.level = ei->users_bbs.Security;

 net_entered = ei->NetMailEntered;
 net_entered = ei->EchoMailEntered;

 if(user.level != oldlevel) adjust_limits();

 delete ei;
}
