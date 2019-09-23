#define Use_MsgBase

#include <ctype.h>
#include <string.h>
#include "proboard.hpp"

#define SEND_HEADER_LINE()         \
    {                              \
     k = io.send(s,hotkeys);       \
     switch(k)                     \
       {                           \
       case 0:  break;             \
       case 1:  return 'S';        \
       default: return k;          \
       }                           \
     if(/*!rip_mode &&*/ linecounter()) return 'S'; \
     s.clear();                    \
    }

static void
status_button( int num , char *text)
{
   rip_button(590 - num*40,11,620 - num*40,21,0,0,"",text,"");
}

byte
Message::show()
{
char *hotkeys="AN\r";
String s;
char k;

//if(rip_mode) io.show_remote = FALSE;

s << '\r' << S_MSGHEADER_MESSAGENUM(form("%ld",msgNum()),msgArea->name) << " \7";

//s << form("\r\7Message #\6%-5ld\2 [%s] \7",msgNum(),msgArea->name);

if(attr & MSGATTR_PRIVATE) s << S_MSGHEADER_PRIVATE << ' ';

if(msgArea->msgKind == MSG_NET)
  {
   if(attr & MSGATTR_CRASH) s << S_MSGHEADER_CRASH << ' ';
   if(attr & MSGATTR_KILL)  s << S_MSGHEADER_KILL << ' ';
   if(attr & MSGATTR_FILE)  s << S_MSGHEADER_FILE << ' ';
   if(attr & MSGATTR_SENT)  s << S_MSGHEADER_SENT << ' ';
  }


if(prev) s << form("\7[%ld <ÄÄ] ",msgArea->msgNum(prev));
if(next) s << form("\7[ÄÄ> %ld] ",msgArea->msgNum(next));

s << '\n';

SEND_HEADER_LINE();

String org(from);
String rip_org(from);
String dest(to);
String rip_dest(to);

org << "\7";
dest << "\7";

if(msgArea->msgKind == MSG_NET)
  {
   int pt_from = pointNum(0);
   int pt_to   = pointNum(1);

   org << form(" (%d:%d/%d",origZone,origNet,origNode);
   dest << form(" (%d:%d/%d",destZone,destNet,destNode);

   if(pt_from) org  << form(".%d",pointNum(0));
   if(pt_to)   dest << form(".%d",pointNum(1));

   org  << ')';
   dest << ')';

   rip_org << form(" (%d:%d/%d",origZone,origNet,origNode);
   rip_dest << form(" (%d:%d/%d",destZone,destNet,destNode);

   if(pt_from) rip_org  << form(".%d",pointNum(0));
   if(pt_to)   rip_dest << form(".%d",pointNum(1));

   rip_org  << ')';
   rip_dest << ')';
  }

  /*
  if(rip_mode)
  {
   io.show_remote = TRUE;

   String rip_subj;

   for(int i=0; subj[i] ; i++)
   {
      if(strchr("\\|",subj[i])) rip_subj << '\\';

      rip_subj << subj[i];
   }

   rip_send("\r!|Y00000100|1B0000020PI8030E080F080700080107000000\n");
   rip_send(form("!|1U2I0R9Q130000<>%s<>|1U2I159Q1H0000<>%s<>\n",(char *)rip_org,(char *)rip_dest));
   rip_send("!|1B0000020PI8030E080F080700000107000000\n");
   rip_send( form( "!|1UCS0RFA130000<>%d %s %04d<>|1UFK0RH8130000<>%02d:%02d<>\n",
                   postDate[ 0 ],
                   months_short[ postDate[ 1 ] ],
                   postDate[ 2 ] + 1900,          // Y2K FIX!
                   postTime[ 0 ], 
                   postTime[ 1 ] ) );

   if(attr & MSGATTR_RECEIVED)
   {
      rip_send("!|1B0000020PI803000F0F0807000G0107000000\n");
      rip_send("!|1UA015CI1H0000<>Received<>\n");
      rip_send("!|1B0000020PI8030E080F080700000107000000\n");

      if ( recvDate.ok() )
           rip_send( form( "!|1UCS15FA1H0000<>%d %s %04d<>|1UFK15H81H0000<>%02d:%02d<>\n",
                           recvDate[ 0 ],
                           months_short[ recvDate[ 1 ] ],
                           recvDate[ 2 ] + 1900,       // Y2K FIX
                           recvTime[ 0 ],
                           recvTime[ 1 ],
                           recvTime[ 2 ] ) );
   }

   rip_send("!|1B0000020PI8030B080F080700080107000000\n");
   rip_send(form("!|1U2I1LH81X0000<>%s<>\n",(char *)rip_subj));

   rip_send(form("!|1B0000020PHC020E080F080100000107000000|1U0K0B280N0000<>#%ld<>\n",msgNum()));
   rip_send(form("!|1B0000020PHC020E080F080300000107000000|1U2I0B9Q0N0000<>%s<>\n",msgArea->name));

   rip_send("!|1B0000020PVK020E080F080300000107000000|Y02000400\n");

   int button_num = 0;

   if(attr & MSGATTR_PRIVATE)
      status_button(button_num++,"PVT");

   if(msgArea->msgKind == MSG_NET)
   {
      if(attr & MSGATTR_CRASH) status_button(button_num++,"CR");
      if(attr & MSGATTR_KILL)  status_button(button_num++,"KILL");
      if(attr & MSGATTR_FILE)  status_button(button_num++,"FILE");
      if(attr & MSGATTR_SENT)  status_button(button_num++,"SNT");
   }

   if(prev || next) rip_send("!|1B0000020QOY020E000F080500000107000000\n");
   if(prev) rip_send("!|1UA00BB40N1O00<><<>-\n");
   if(next) rip_send("!|1UBE0BCI0N1Q00<>><>+\n");

   rip_send("!|1B0000020QPE020F000F080300020E07000000|Y00000100\n");

   rip_screenlines = 24;

   io.show_remote = FALSE;
  }
  */


  // Y2K FIXED!

  s << S_MSGHEADER_FROM( (char *) org,
                         form( "%2d %s %04d",
                               postDate[ 0 ],
                               months_short[ postDate[ 1 ] ],
                               postDate[ 2 ] + 1900 ),      

                         form( "%02d:%02d",
                               postTime[ 0 ],
                               postTime[ 1 ] )
                       ) << '\n';

  //s << form("\6From: \3%-47.47s   \6Posted:\3 %2d %s %02d\5  %02d:%02d\n",
  //           (char *)org,postDate[0],months_short[postDate[1]],postDate[2],postTime[0],postTime[1]);

  SEND_HEADER_LINE();

  s << S_MSGHEADER_TO((char *)dest) << ' ';

  //s << form("\6To  : \3%-47.47s \6",(char *)dest);

  if(attr & MSGATTR_RECEIVED)
  {
      if(recvDate.ok())
      {

          // Y2K FIX!

         s << S_MSGHEADER_RECEIVED_ON( form( "%2d %s %04d",
                                             recvDate[ 0 ],
                                             months_short[ recvDate[ 1 ] ],
                                             recvDate[ 2 ] + 1900 ),       

                                       form( "%02d:%02d",
                                             recvTime[ 0 ],
                                             recvTime[ 1 ] ) );

         //s << form("Received:\3 %2d %s %02d\5  %02d:%02d",recvDate[0],months_short[recvDate[1]],recvDate[2],recvTime[0],recvTime[1],recvTime[2]);
      }
      else
      {
         s << S_MSGHEADER_RECEIVED;
         //s << "Received";
      }
  }

  s << '\n';

  SEND_HEADER_LINE();

  if(attr & MSGATTR_FILE) s << S_MSGHEADER_FILEATTACHED(subj) << '\n';
                   else   s << S_MSGHEADER_SUBJECT(subj) << '\n';

  SEND_HEADER_LINE();

s << "\n\7";

SEND_HEADER_LINE();

s = "";

   io.show_remote = TRUE;

io << "\7";

int inkludge=0;

String wrap;

int inquote=0;
int index=0;

char line[100];
memset(line,0,100);

bool msgdone = FALSE;

for(long txt_off = 0; !msgdone ;txt_off += 255)
  {
  char r[256];

  int l = msgBase()->readMsgText(*this,r,txt_off,255);

  if(l < 255) msgdone = TRUE;

  for(int j=0;j<l;j++)
    {
    byte x=toupper(io.readkey());
    switch(x)
      {
      case 'S':
      case 'N':
      case '\r': return x;
      case 'P': io.wait("P\r");
      }

    x = r[j];

    if(x == '\t') x = ' ';
    if(x == 1)    inkludge=1;
    if(x == '\n' || x == 0x8D || x <= 8) continue;

    if(x == '\r')
      {
       x='\n';
       if(inkludge)
         {
          inkludge=0;
          continue;
         }
      }

    if(inkludge) continue;

    if(x=='\n')
      {
       inquote=0;
       for(int i=0;i<6 && i<index;i++) if(line[i]=='>') inquote=1;
       if(inquote)
         {
          if(avatar) io << char(22) << char(1) << char(0xB);
            else if(ansi_mode) io << "[36m";
         }
       if(strncmp(line,"SEEN-BY",7))
         {
          io << line << '\n';
          if(linecounter()) return 'S';
         }
       if(inquote)
         {
          if(avatar) io << char(22) << char(1) << char(0xF);
            else if(ansi_mode) io << "[37m";
         }

       memset(line,0,100);
       index=0;
      }
     else
      {
       if(x>=' ' || x==0x1B) line[index++] = x;
      }

    if(index>79)
       {
       wordwrap(line,wrap,79);
       if(strncmp(line,"SEEN-BY",7))
         {
          io << line << '\n';
          if(linecounter()) return 'S';
         }
       memset(line,0,100);
       strcpy(line,wrap);
       index=strlen(line);
       }
    }
  }

if(index)
  if(strncmp(line,"SEEN-BY",7))
    {
     io << line << '\n';
     if(linecounter()) return 'S';
    }

return 0;
}

