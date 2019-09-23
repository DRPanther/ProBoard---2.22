#define Use_MsgBase

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <time.h>
#include <io.h>
#include <tswin.hpp>

#include "fossil.hpp"
#include "desqview.hpp"
#include "proboard.hpp"



#define STAT_LINE_Y 20
#define LAST_CALLER_Y 16

#define EVENT_LABEL 0x5F
#define EVENT_DATA  0x5E

#define TITLE_COLOR 0x71
#define LABEL_COLOR 0x70

static void
fos_sendbreak(int port)
{
   fos_break(port,TRUE);

   unsigned long ticks = clockticks();
   while(clockdiff(ticks)<2) {}

   fos_break(port,FALSE);
}

struct modem_config : _ModemConfig {};

class modem
   {
      int  port;
      long baud;
      int  delay;
   public:
      modem(int p,long b);
     ~modem()
         {
            fos_deinit(port);
         }
      void set_delay(int d)
         {
            delay = d;
         }
      void cmd(char *s);
      bool check_response(String& s);
  };

modem::modem(int p,long b)
{
   byte baudcode = 0;

   port = p;
   baud = b;

   fos_init(port);

   switch(baud)
   {
      case   300L: baudcode = 0x43; break;
      case  1200L: baudcode = 0x83; break;
      case  2400L: baudcode = 0xA3; break;
      case  4800L: baudcode = 0xC3; break;
      case  9600L: baudcode = 0xE3; break;
      case 19200L: baudcode = 0x03; break;
      case 38400L: baudcode = 0x23; break;
   }

   fos_setbps(port,baudcode);
}

void
modem::cmd(char *s)
{
   for(;*s;s++)
   {
      char c = *s;

      switch(c)
      {
         case '~': ::delay(500);
                   break;
         case '`':
         case '':
         case 'v': fos_setdtr(port,0);
                   break;
         case '^': fos_setdtr(port,1);
                   break;
         case '$': fos_sendbreak(port);
                   break;
         case '|': c = '\r';
         default : fos_sendnw(port,c);
                   if(delay)
                     ::delay(delay*100);
      }
   }
}

bool
modem::check_response(String& s)
{
   static String resp;

   if(!fos_byteready(port))
   {
      return FALSE;
   }

   byte c = fos_getch(port);

   if(c == '\r')
   {
      s = resp;
      resp.clear();

      return (s[0]) ? TRUE:FALSE;
   }

   if(c != '\n') resp << char(c);

   return FALSE;
}

const int STATUS_WAITING_FOR_CALL = 0;
const int STATUS_INITIALIZING     = 1;
const int STATUS_WAITING_FOR_OK   = 2;


bool
do_modem(modem_response *response)
{
     tsw_fillscreen( '�', 0x19 );

     Window dlg_win( 3, 
                     2, 
                     76, 
                     22, 
                     0x70,
                     SHADOW,
                     CHISEL_BORDER,
                     0x7F,
                     NULL,
                     0x78 );

     Window bar_win( 1, 
                     tsw_vsize, 
                     tsw_hsize,
                     tsw_vsize,
                     0x4F,
                     NOBORDER|NOSAVE,
                     NULL );

     Window stat_win( 6, 
                      19, 
                      73, 
                      21, 
                      0x0E,                  // Inner color
                      NOSAVE,
                      RECESS_BORDER,
                      0x08,                  // Bright color
                      NULL,
                      0x0F,                  // Dark color
                      0x70 );                // Outer color

     Window lc_win( 6,
                    15,
                    73,
                    17,
                    0x1F,
                    NOSAVE,
                    RECESS_BORDER,
                    0x08,
                    NULL,
                    0x0F,
                    0x70 );


     Window resp_win( 6,
                      8,
                      28,
                      13,
                      0x30,
                      NOSAVE,
                      RECESS_BORDER,
                      0x08,
                      NULL,
                      0x0F,
                      0x70 );

     Window event_win( 30,
                       8,
                       73,
                       13,
                       EVENT_LABEL,
                       NOSAVE,
                       RECESS_BORDER,
                       0x08,
                       NULL,
                       0x0F,
                       0x70 );

     dlg_win.ShadowColor( 0x10 );
   
     dlg_win.open();
     resp_win.open();
     lc_win.open();
     stat_win.open();
     event_win.open();
     bar_win.open();


     if ( registered )
     {
          tsw_centerline( 3, 
                          form( "ProBoard v%s (Node %d - S/N=%s)",
                                VERSION,
                                (int) node_number,
                                software_serial_number() ),
                          TITLE_COLOR );
     }
     else
     {
          tsw_centerline( 3, 
                          form( "ProBoard v%s [Node %d - UNREGISTERED for %d day%s]",
                                VERSION,
                                (int) node_number,
                                pastFirstUse,
                                ( pastFirstUse == 1 )
                                   ? ""
                                   : "s" ),
                          TITLE_COLOR );
     }

     tsw_centerline( 4, 
                     "Copyright (c) 1990-1999  TeleGrafix Communications, Inc.",
                     TITLE_COLOR );

     tsw_centerline( 5, 
                     "All Rights Reserved",
                     TITLE_COLOR );

     tsw_maputs( 4,
                 6,
                 0x7F,
                 "�" );

     tsw_maputs( 5,
                 6,
                 0x78,
                 "컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�" );

     tsw_centerline( 14, "Last Caller" );
     tsw_centerline( 18, "Current Status" );

     modem_config *modem_cfg = new modem_config;
     CLEAR_OBJECT(*modem_cfg);

     File f;

     if(!f.open(FileName(syspath,form("MODEM%03d.PB",node_number))))
          if(!f.open(FileName(syspath,"MODEM.PB")))
          {
               LOG("Unable to open MODEM.PB");
               delete modem_cfg;
               return FALSE;
          }


     f.read(modem_cfg,sizeof(modem_config));
     f.close();

     modem_cfg->port--;

     if(response->baud)    modem_cfg->maxBps = response->baud;
     if(response->port>=0) modem_cfg->port = response->port;

     tsw_maputs( 8,
                 7,
                 LABEL_COLOR,
                 form( "Modem COM%d/%ld bps",
                       modem_cfg->port + 1,
                       modem_cfg->maxBps ) );

     tsw_maputs( 43,
                 7,
                 LABEL_COLOR,
                 "System Statistics" );

     response->port = modem_cfg->port;

     modem modem(modem_cfg->port,(modem_cfg->flags & MODEM_LOCKED) ? 38400L : modem_cfg->maxBps);

     modem.set_delay(modem_cfg->commandDelay);

     tsw_cursoroff();

     stat_win.clear();
     tsw_centerline(STAT_LINE_Y,"Initializing modem",0xE);

     event_win.attrib( EVENT_LABEL );
     event_win << "\n      # Calls: ";
     
     event_win.attrib( EVENT_DATA );
     event_win << form( "%-6ld    ", totalcalls );

     event_win.attrib( EVENT_LABEL );
     event_win << "# Users: ";

     event_win.attrib( EVENT_DATA );
     event_win << form( "%d\n", num_users );

     event_win.attrib( EVENT_LABEL );
     event_win << "# Hudson Msgs: ";
     
     event_win.attrib( EVENT_DATA );
     event_win << form( "%-5ld\n", msgbase.totalMsgs() );

     event_win.attrib( EVENT_LABEL );

     if ( lastcaller.name  &&
          *lastcaller.name )
     {
          tsw_centerline( LAST_CALLER_Y,
                          form( "%s (%02d:%02d to %02d:%02d)",
                                lastcaller.name,
                                lastcaller.timeIn[ 0 ],
                                lastcaller.timeIn[ 1 ],
                                lastcaller.timeOut[ 0 ],
                                lastcaller.timeOut[ 1 ] ),
                                0x1E );
     }
     else
     {
          tsw_centerline( LAST_CALLER_Y,
                          form( "(%02d:%02d to %02d:%02d)",
                                lastcaller.timeIn[ 0 ],
                                lastcaller.timeIn[ 1 ],
                                lastcaller.timeOut[ 0 ],
                                lastcaller.timeOut[ 1 ] ),
                                0x1E );
     }


     time_t start_time = time(NULL);

     time_t init_sent = 0;
     int init_tries = 0;
     bool initialized = FALSE;
     bool blanked = FALSE;

     bool init1_ok;
     bool init2_ok;
     bool init3_ok;

     bool init1_sent = FALSE;
     bool init2_sent = FALSE;
     bool init3_sent = FALSE;

     time_t blank_start = time(NULL);

     word *blank_buf = new word[tsw_vsize*tsw_hsize];

     for(;;)
     {
          String s;
          Date date(TODAY);
          Time time(NOW);

          if(!blanked)
          {
               bar_win.direct( 57,
                               1,
                               0x4E,
                               form( "%02d:%02d:%02d",
                               time[ 0 ],
                               time[ 1 ],
                               time[ 2 ] ) );

               bar_win.direct( 66,
                               1,
                               0x4E,
                               form( "- %02d %s %d ",
                               date[ 0 ],
                               months_short[ date[ 1 ] ],
                               date[ 2 ]  +  1900 ) );      // Y2K OKAY!

               event_win.setPos(1,1);

               event_win.attrib( EVENT_LABEL );
               event_win << "   Next Event: ";
               
               event_win.attrib( EVENT_DATA );

               if ( nextevent.enabled )
               {
                    event_win << form( "%s %02d:%02d (%d min)  ",days_short[nextevent.nextday()],nextevent.start[0],nextevent.start[1],nextevent.minutesleft());
               }
               else
               {
                    event_win << "-NONE-";
               }

               event_win.attrib( EVENT_LABEL );
               event_win << "\n\n\n    Time idle: ";
               
               event_win.attrib( EVENT_DATA );
               event_win << form( "%ld min", (::time(NULL)-start_time)/60);
          }


          if(init_sent && (::time(NULL)-init_sent)>5 && !initialized)
          {
               if(!blanked) stat_win.clear();

               if(init_tries>3)
               {
                    if(!blanked) tsw_centerline(STAT_LINE_Y,"Unable to initialize modem",0xE);
                    response->port = -1;
                    break;
               }

               if(!blanked) tsw_centerline(STAT_LINE_Y,form("Unable to initialize modem. Retrying (Try #%d)",init_tries+1),0x8E);

               initialized = FALSE;
               init1_sent  = FALSE;
               init2_sent  = FALSE;
               init3_sent  = FALSE;
               init_sent   = 0;
          }

          if(!initialized)
          {
               if(!init1_sent)
               {
                    if(modem_cfg->cmdInit1[0])
                    {
                         modem.cmd(modem_cfg->cmdInit1);

                         init1_ok   = FALSE;
                    }

                    init1_sent = TRUE;
                    init_sent  = ::time(NULL);
               }

               if(!init2_sent && init1_ok)
               {
                    if(modem_cfg->cmdInit2[0])
                    {
                         modem.cmd(modem_cfg->cmdInit2);

                         init2_ok   = FALSE;
                    }
                    else
                    {
                         init2_ok = TRUE;
                    }

                    init2_sent = TRUE;
                    init_sent  = ::time(NULL);
               }

               if(!init3_sent && init2_sent && init2_ok)
               {
                    if(modem_cfg->cmdInit3[0])
                    {
                         modem.cmd(modem_cfg->cmdInit3);

                         init3_ok   = FALSE;
                    }
                    else
                    {
                         init3_ok    = TRUE;
                    }

                    init3_sent = TRUE;
                    init_sent  = ::time(NULL);

                    init_tries++;
               }
          }

          if(!initialized && init1_ok && init2_ok && init3_ok)
          {
               initialized = TRUE;

               stat_win.clear();
               tsw_centerline(STAT_LINE_Y,"Waiting for a call",0xE);

               bar_win.direct( 3,  1, 0x4E, "Alt-L"        );
               bar_win.direct( 8,  1, 0x4F, "=Local"       );
                                            
               bar_win.direct( 16, 1, 0x4E, "Alt-J"        );
               bar_win.direct( 21, 1, 0x4F, "=Jump to DOS" );
                                            
               bar_win.direct( 35, 1, 0x4E, "Esc"          );
               bar_win.direct( 38, 1, 0x4F, "=Exit"        );


               //----------------------------------------------
               // This line of code is used to show the current
               // screen saver timeout (debuging only)
               //----------------------------------------------

               //bar_win.direct( 50, 1, form( "%ld", (long) modem_cfg->blankTime ) );
          }


          if(init_sent && (::time(NULL)-init_sent) > 300)
          {
               initialized = FALSE;
               init1_sent  = FALSE;
               init2_sent  = FALSE;
               init3_sent  = FALSE;
               init_sent   = 0;
               init_tries  = 0;
          }


          if(modem.check_response(s))
          {
               s << '|';

               if(blanked)
               {
                    tsw_puttext(blank_buf,1,1,tsw_hsize,tsw_vsize);
                    blanked = FALSE;
                    blank_start = ::time(NULL);
               }

               if(init1_sent && !init1_ok && s==modem_cfg->cmdInit1)
                    continue;
               if(init2_sent && !init2_ok && s==modem_cfg->cmdInit2)
                    continue;
               if(init3_sent && !init3_ok && s==modem_cfg->cmdInit3)
                    continue;

               s.delLast();
               resp_win << form("%-17.17s",(char *)s) << '\n';
               s << '|';

               if(strstr(s,modem_cfg->msgOk) && !initialized)
               {
                    if(init1_sent && !init1_ok)
                         init1_ok = TRUE;
                    if(init2_sent && !init2_ok)
                         init2_ok = TRUE;
                    if(init3_sent && !init3_ok)
                         init3_ok = TRUE;
               }


               if(initialized)
               {
                    String connect = s;
                    long baud = 0;

                    if(modem_cfg->msgRing[0] && strstr(connect,modem_cfg->msgRing) && (modem_cfg->flags & MODEM_MANUAL_ANSWER))
                    {
                         modem.cmd(modem_cfg->cmdAnswer);
                    }

                    if(!baud && modem_cfg->userConBps1 && strstr(connect,modem_cfg->msgConUser1) && modem_cfg->msgConUser1[0]) baud = modem_cfg->userConBps1;
                    if(!baud && modem_cfg->userConBps2 && strstr(connect,modem_cfg->msgConUser2) && modem_cfg->msgConUser2[0]) baud = modem_cfg->userConBps2;
                    if(!baud && modem_cfg->userConBps3 && strstr(connect,modem_cfg->msgConUser3) && modem_cfg->msgConUser3[0]) baud = modem_cfg->userConBps3;
                    if(!baud && modem_cfg->userConBps4 && strstr(connect,modem_cfg->msgConUser4) && modem_cfg->msgConUser4[0]) baud = modem_cfg->userConBps4;
                    if(!baud && modem_cfg->userConBps5 && strstr(connect,modem_cfg->msgConUser5) && modem_cfg->msgConUser5[0]) baud = modem_cfg->userConBps5;
                    if(!baud && modem_cfg->userConBps6 && strstr(connect,modem_cfg->msgConUser6) && modem_cfg->msgConUser6[0]) baud = modem_cfg->userConBps6;

                    if(!baud && strstr(connect,modem_cfg->msgCon115200)&& modem_cfg->msgCon115200[0])baud = 115200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon64000) && modem_cfg->msgCon64000[0]) baud = 64000L;
                    if(!baud && strstr(connect,modem_cfg->msgCon57600) && modem_cfg->msgCon57600[0]) baud = 57600L;
                    if(!baud && strstr(connect,modem_cfg->msgCon38400) && modem_cfg->msgCon38400[0]) baud = 38400L;
                    if(!baud && strstr(connect,modem_cfg->msgCon36000) && modem_cfg->msgCon36000[0]) baud = 36000L;
                    if(!baud && strstr(connect,modem_cfg->msgCon33600) && modem_cfg->msgCon33600[0]) baud = 33600L;
                    if(!baud && strstr(connect,modem_cfg->msgCon31200) && modem_cfg->msgCon31200[0]) baud = 31200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon28800) && modem_cfg->msgCon28800[0]) baud = 28800L;
                    if(!baud && strstr(connect,modem_cfg->msgCon26400) && modem_cfg->msgCon26400[0]) baud = 26400L;
                    if(!baud && strstr(connect,modem_cfg->msgCon24000) && modem_cfg->msgCon24000[0]) baud = 24000L;
                    if(!baud && strstr(connect,modem_cfg->msgCon21600) && modem_cfg->msgCon21600[0]) baud = 21600L;
                    if(!baud && strstr(connect,modem_cfg->msgCon19200) && modem_cfg->msgCon19200[0]) baud = 19200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon16800) && modem_cfg->msgCon16800[0]) baud = 16800L;
                    if(!baud && strstr(connect,modem_cfg->msgCon14400) && modem_cfg->msgCon14400[0]) baud = 14400L;
                    if(!baud && strstr(connect,modem_cfg->msgCon12000) && modem_cfg->msgCon12000[0]) baud = 12000L;
                    if(!baud && strstr(connect,modem_cfg->msgCon9600 ) && modem_cfg->msgCon9600[0] ) baud = 9600L;
                    if(!baud && strstr(connect,modem_cfg->msgCon7200 ) && modem_cfg->msgCon7200[0] ) baud = 7200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon4800 ) && modem_cfg->msgCon4800[0] ) baud = 4800L;
                    if(!baud && strstr(connect,modem_cfg->msgCon2400 ) && modem_cfg->msgCon2400[0] ) baud = 2400L;
                    if(!baud && strstr(connect,modem_cfg->msgCon1200 ) && modem_cfg->msgCon1200[0] ) baud = 1200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon1275 ) && modem_cfg->msgCon1275[0] ) baud = 1200L;
                    if(!baud && strstr(connect,modem_cfg->msgCon300  ) && modem_cfg->msgCon300[0]  ) baud = 300L;

                    if(baud)
                    {
                         stat_win.clear();
                         tsw_centerline(STAT_LINE_Y,form("Incoming call at %ld bps",baud),0xE);
                         response->baud = baud;
                         break;
                    }

                    if(modem_cfg->msgConExternal[0] && strstr(connect,modem_cfg->msgConExternal))
                    {
                         SCREEN.clear();
                         modem.modem::~modem();
                         exit(modem_cfg->externalErrorLevel);
                    }
               }
          }

          if ( modem_cfg->blankTime  && 
               ! blanked             && 
               ( ::time( NULL )  -  blank_start )  >  modem_cfg->blankTime )
          {
               tsw_gettext(blank_buf,1,1,tsw_hsize,tsw_vsize);
               tsw_fillscreen(' ',0);
               blanked = TRUE;
          }

          if(   nextevent.minutesleft()==0 && nextevent.start[0]==time[0]
              && nextevent.start[1]==time[1] && time[2]<=1
              && nextevent.node == node_number)
          {
               modem.modem::~modem();
               SCREEN.clear();
               
               
               if(nextevent.type)
               {
                    String s = "*=*Q*N*C /C ";
                    s << nextevent.command;
                    shell(s);
                    exit(0);
               }
               else
               {
                    exit(nextevent.errorlevel);
               }
          }

          if(KB.hit())
          {
               if(blanked)
               {
                    tsw_puttext(blank_buf,1,1,tsw_hsize,tsw_vsize);
                    blanked = FALSE;
                    blank_start = ::time(NULL);
               }

               KEY k=KB.get();

               if(k==KEY_ALTL)
               {
                    stat_win.clear();
                    tsw_centerline(STAT_LINE_Y,"Local login",0xE);
                    modem.cmd(modem_cfg->cmdOffHook);
                    response->baud = 0;
                    sleep(1);
                    break;
               }

               if(k==KEY_ESC)
               {
                    stat_win.clear();
                    tsw_centerline(STAT_LINE_Y,"Exit",0xE);
                    modem.cmd(modem_cfg->cmdDown);
                    sleep(1);
                    modem.modem::~modem();
                    SCREEN.clear();
                    exit(99);
               }

               if(k==KEY_ALTJ)
               {
                    tsw_gettext(blank_buf,1,1,tsw_hsize,tsw_vsize);
                    tsw_cursoron();
                    SCREEN.clear();
                    modem.cmd(modem_cfg->cmdOffHook);
                    shell("*X*C*N*Q");
                    tsw_cursoroff();
                    tsw_puttext(blank_buf,1,1,tsw_hsize,tsw_vsize);

                    initialized = FALSE;
                    init1_sent  = FALSE;
                    init2_sent  = FALSE;
                    init3_sent  = FALSE;
                    init_sent   = 0;
                    init_tries  = 0;
               }
          }

          if(!access(form("%sDOWN.%d",syspath,node_number),0))
          {
               stat_win.clear();
               tsw_centerline(STAT_LINE_Y,"Going down on external request",0xE);
               LOG("Going down on external request");

               modem.cmd(modem_cfg->cmdDown);
               sleep(1);
               modem.modem::~modem();
               SCREEN.clear();

               File f;
               f.open(form("%sISDOWN.%d",syspath,node_number),fmode_create|fmode_write);
               f.close();

               exit(100);
          }

          static int timeslice_count = 0;

          if(!(++timeslice_count % 20))
               DV_timeslice();
     }


     tsw_cursoron();

     delete modem_cfg;
     delete [] blank_buf;

     bar_win.close();
     resp_win.close();
     lc_win.close();
     stat_win.close();
     event_win.close();
     dlg_win.close();

     SCREEN.clear();

     return (response->port<0) ? FALSE:TRUE;
}

