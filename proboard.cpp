#define Use_MsgBase
#define Use_LinkedList
#define Use_Handlers

#define INCLUDE_REGIS

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <direct.h>
#include <ctype.h>
#include <tswin.hpp>
#include "proboard.hpp"
//#include "timebomb.h"
#include <dos.h>

long Message::lastIndex = 0;

static int port = PORT_VALUE;


bool do_modem(modem_response *);

// unsigned _stack = 12000;

extern unsigned _stklen = 13000;

#if __OVERLAY__
    unsigned _ovrbuffer = 0x800;
#endif

char *months_short[]      = { "???","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char *days_short[]        = { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
char *days_full[]         = { "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
char *date_formats[]      = { "MM/DD/YY",   "YY/MM/DD",   "DD/MM/YY",   NULL };
char *date_formats_long[] = { "MM/DD/YYYY", "YYYY/MM/DD", "DD/MM/YYYY", NULL };

bool chatflag       = FALSE;
bool noyell_flag    = FALSE;
bool password_ok    = FALSE;
bool desqview       = FALSE;
bool stopped        = FALSE;
bool use_ems        = TRUE;
bool use_xms        = FALSE;
bool quiet          = FALSE;
bool echo_entered   = FALSE;
bool net_entered    = FALSE;

bool registered     = FALSE;
word max_node_count = 2;

bool menuchanged    = TRUE;
bool updatemenu     = TRUE;
bool reliable       = TRUE;
bool ignore_carrier = FALSE;
bool fatal          = FALSE;
bool no_hangup      = FALSE;
bool rip_detected   = FALSE;
bool sysop_next     = FALSE;

int  num_yells      = 0;
long totalcalls     = 0;
int  num_users      = 0;
int  time_limit     = 0;
int  time_removed   = 0;
int  node_number    = 1;
int  num_limits     = 0;
int  returnvalue    = 0;
int  display_mode   = 1;
int  event_due      = 10080;
byte org_videomode  = 0;
word org_numlines   = 0;
word min_level      = 0;
int  rip_screenlines= 0;
int  pastFirstUse   = 31;
bool newFilesChecked = FALSE;

int  download_limit = 0;
int  download_delay = 0;
word free_download  = 0;
word upload_needed  = 0;


String S_PRESS_ENTER_TO_CONTINUE;

byte current_color;

fossilio io;

BinLog lastcaller;

char user_firstname[36] = "";
char mypath[64]         = "";
char syspath[64]        = "";
char curmenu[9]         = "";
char page_reason[81]    = "";

User          user;
Config    far cfg;
usertimer     timer;
limits        limit[50];
KeyData       regis;
event         nextevent;

MarkedMsgList markedmsglist;

command_stack comstack;

LinkedList<PEXhandle> sysopkey_handlers;
LinkedList<PEXhandle> hangup_handlers;

Date login_date;
Time login_time;

int user_recnr = -1;

FileName FN_BINLOG_PB;
FileName FN_TIMELOG_PRO;
FileName FN_ONLINE_PRO;
FileName FN_MSGAREAS_PRO;
FileName FN_PVTFILES_PB;

MsgBaseList msgbase;

MsgBase *squishMsgBase;
MsgBase *hudsonMsgBase;
MsgBase *jamMsgBase;

char *EMPTY_STRING="";

extern "C" void cleanup();

extern void (*_new_handler)();

void nomemory();

static void _near init();
void readconfig();



#ifdef SOFTWARE_EXPIRE

//---------------------  
// Expire on 06/01/2000
//---------------------  

int BetaTooOld( void );

#endif



void
main(int argc,char *argv[])
{
   File     f;
   long     baud=0;
   int      i;
   FileName fn;

   bool standalone = FALSE;


/*#ifdef SOFTWARE_EXPIRE

     if ( BetaTooOld() )
     {
          printf( "\nThis software expired on %02d/%02d/%04d.  Contact TeleGrafix Communications at:\n\n",
                  EXPIRE_MONTH,
                  EXPIRE_DAY,
                  EXPIRE_YEAR );

          printf( "   http://www.telegrafix.com/products/proboard\n\n" );
          
          printf( "to obtain the latest version.\7\n" );
          printf( "\n" );

          sleep( 5 );

          exit( 1 );
     }

#endif */


   org_videomode = getvideomode();
   org_numlines  = tsw_vsize;

   for(i=1;i<argc;i++)
      if(argv[i][0]=='-' || argv[i][0]=='/')
         switch(toupper(argv[i][1]))
         {
            case 'B':
               baud = atol(&argv[i][2]);
               break;

            case 'P':
               port = atoi(&argv[i][2])-1;
               break;

            case 'N': node_number = atoi(&argv[i][2]);
               break;

            case 'Q':
               quiet       = TRUE;
               tsw_shutup  = TRUE;
               break;

            case 'E':
               set43();
               break;

            case 'V':
               {
                  int mode = atoi(&argv[i][2]);
                  if(mode>=3 && mode<=255)
                     setvideomode(mode);
               }
               break;

            case 'X':
               use_ems = FALSE;
               break;

            case 'I':
               ignore_carrier = TRUE;
               break;

            case 'S':
               standalone = TRUE;
               break;

            case 'A':
               reliable = TRUE;
               break;

            case 'L':
               min_level = unsigned(atol(&argv[i][2]));
               break;

            case 'T':
               event_due = atoi(&argv[i][2]);
               break;

            default :
               printf("\nUnknown option '-%c'\n",toupper(argv[i][1]));
               sleep(5);
         }

   if(_osmajor<3 || (_osmajor==3 && _osminor<30))
   {
      puts("\nDOS v3.30 or higher needed!");
      exit(1);
   }

#if __OVERLAY__
   if(use_ems)
      _OvrInitEms(0, 0, 0);

   if(use_xms)
      _OvrInitExt(0 ,0   );
#endif

   syspath[0]='\0';

   if(getenv("PROBOARD"))
   {
      strcpy(syspath,getenv("PROBOARD"));
      append_backspace(syspath);
   }

   if(!syspath[0])
   {
      fn(argv[0]);
      fn.stripName();

      strcpy(syspath,fn);
      append_backspace(syspath);
   }

   strupr(syspath);

   getcwd(mypath,59);
   append_backspace(mypath);

   check_exe(argv[0]);

   readconfig();

   if(!ReadLanguage(cfg.defaultLanguage))
   {
      fatalerror("Error reading default language files");
   }

   readconfig();

   FN_BINLOG_PB   (syspath,    "BINLOG.PB");
   FN_TIMELOG_PRO (syspath,    "TIMELOG.PRO" );
   FN_ONLINE_PRO  (syspath,    "ONLINE.PRO"  );
   FN_MSGAREAS_PRO(syspath,    "MESSAGES.PB" );
   FN_PVTFILES_PB (syspath,    "PVTFILES.PB ");

   _new_handler=nomemory;

   if(cfg.egamode) set43();

   if(tsw_videocard==EGA) tsw_vsize=*((byte _far *)0x484)+1;

   tsw_hsize = *((byte far *)0x44A);

   init();

   run_sdkfile("/I INIT");
   run_sdkfile("/I INIT_1");
   run_sdkfile("/I INIT_2");
   run_sdkfile("/I INIT_3");
   run_sdkfile("/I INIT_4");
   run_sdkfile("/I INIT_5");
   run_sdkfile("/I INIT_6");
   run_sdkfile("/I INIT_7");
   run_sdkfile("/I INIT_8");
   run_sdkfile("/I INIT_9");

   msgbase.add( squishMsgBase = new SquishMsgBase );
   msgbase.add( hudsonMsgBase = new HudsonMsgBase );
   msgbase.add( jamMsgBase    = new JamMsgBase    );

   if(standalone)
   {
      modem_response response;
      CLEAR_OBJECT(response);

      response.baud = baud;
      response.port = port;

      if(do_modem(&response))
      {
         baud = response.baud;
         port = response.port;
      }
      else
      {
         fatalerror("Modem initialization failed");
      }

      if(!baud) port = -1;
   }
   else
   {
      if(port<0 && baud)
         port = 0;
   }

   SCREEN.clear();
   SCREEN.attrib(15);

   SCREEN.change(1,1,tsw_hsize,tsw_vsize-1);

   update_user_window();

   if(baud || cfg.loglocal) LOG("-----------------------------------------------------");

   if(port<0)
      io.init(0,0);
   else
      io.init(port,baud);

   if(baud)
      LOG("Incoming call at %ld bps",baud);
   else
      LOG("Local login");

   atexit(cleanup);

   timer.start(20);

   update_display();

   if(nextevent.minutesleft()<=5 || (event_due<=5 && event_due>=0))
   {
      if(showansascrip("EVENTDUE") == ANS_NOFILE)
         io << '\n' << S_EVENT_HAS_TO_RUN << "\n\xFF";

      LOG("Event has to run");

      sleep(2);
      exit_proboard();
   }

   show_intro();

   if(login()) exit_proboard();

   LOG(1,"Calling from %s - Level %u",user.city,user.level);

   {
      user_online uo;
      uo.write();
   }

   for(i=0;i<strlen(user.name) && user.name[i]!=' ';i++)
      user_firstname[i] = user.name[i];

   user_firstname[i]=0;

   limit[0].level        = user.level;
   limit[0].timelimit    = 20;
   limit[0].daily_klimit = 0;

   fn(syspath,"LIMITS.PRO");

   if(!f.open(fn))
      file_error("LIMITS.PRO");
   else
   {
      num_limits = f.read(limit,50*sizeof(limits))/sizeof(limits);

      if(!num_limits)
         num_limits++;

      f.close();
   }

   adjust_limits();

   if(nextevent.minutesleft() < event_due)
      event_due = nextevent.minutesleft();

   if(event_due<=5 && event_due>=0)
   {
      LOG("Event has to run");
      io << '\n' << S_EVENT_HAS_TO_RUN << "\n\xFF";
      sleep(2);
      exit_proboard();
   }

   if(event_due>=0 && timer.left()>event_due-5)
   {
      LOG("Time limit adjusted from %d to %d minutes" , timer.left() , event_due-5);
      time_removed = timer.left() - event_due + 5;
      timer.changeleft(event_due-5);

      io << "\n\n" << S_UPCOMING_EVENT << "\n\n";
      io << S_PRESS_ENTER_TO_CONTINUE << '\n';
   }

   showansascrip("WELCOME");
   for(i=1;i<10;i++)
   {
      run_sdkfile(form("/I WELCOME%d",i));
      showansascrip(form("WELCOME%d",i));
   }

   run_sdkfile(form("/I SEC%u",user.level));
   showansascrip(form("SEC%u",user.level));

   switch(cfg.mailcheck)
   {
      case 2:
      {
         /*
         if(rip_mode)
         {
            rip_query("$COFF$");
            rip_query("$PHASER$");

            show_rip("_CHKMAIL");
         }
         else
         */
         {
            io << "\n" << S_CHECK_MAIL_PROMPT;
         }

         //if(rip_mode) io.show_remote = FALSE;
         bool resp = io.ask();
         //if(rip_mode) io.show_remote = TRUE;

         rip_query("$CON$");

         if(!resp) break;

         /*if(rip_mode) rip_reset();
                 else */io << '\n';
      }

      case 1:
         mailcheck();
         break;
   }

   rip_query("$CON$");

   io << '\n';

   showansascrip("NEWS");

   if(user.birthDate[0]==login_date[0] && user.birthDate[1]==login_date[1])
   {
      run_sdkfile("/I BIRTHDAY");
      showansascrip("BIRTHDAY");
   }

   menu_processor();
}



#ifdef SOFTWARE_EXPIRE

int BetaTooOld( void )
{
     struct dosdate_t d;


     _dos_getdate( & d );


     if ( 
          d.year  >= EXPIRE_YEAR   and
          d.month >= EXPIRE_MONTH  and
          d.day   >  EXPIRE_DAY 
        )
     {
          //---------------------
          // Software has expired
          //---------------------

          return( TRUE );
     }
     else
     {
          //---------------------------------------
          // Re-establish the proper starting port
          // number for ProBoard's COM port support
          //---------------------------------------

          port = -1;

          return( FALSE );
     }
}



#endif    // SOFTWARE_EXPIRE



void
readconfig()
{
   File f;
   FileName fn(syspath,"CONFIG.PRO");

   if(!f.open(fn))
   {
      file_error(fn , f.error());
   }
   else
   {
      if(f.len()!=sizeof(cfg) || f.read(&cfg,sizeof(cfg))!=sizeof(cfg))
         fatalerror(form("File <%s> is invalid." , (char *)fn));
   }

   f.close();

   if(memcmp(cfg.dateStampSignature,"ecwqnoiwef",10))
   {
      memcpy(cfg.dateStampSignature,"ecwqnoiwef",10);
      cfg.dateStamp.today();
      cfg.dateStampCrc = crc32(cfg.dateStampSignature,13);

      f.open(fn,fmode_create);
      f.write(&cfg,sizeof(cfg));
      f.close();
   }

   if(cfg.dateStampCrc != crc32(cfg.dateStampSignature,13))
   {
      pastFirstUse = 61;
   }
   else
   {
      pastFirstUse = Date(TODAY) - cfg.dateStamp;
   }

   if(!cfg.defaultLanguage[0])
      strcpy(cfg.defaultLanguage,"ENGLISH");

   if(pb_curlang.menuPath[0])
      strcpy(cfg.mnupath , pb_curlang.menuPath);

   if(pb_curlang.txtPath[0])
      strcpy(cfg.txtpath , pb_curlang.txtPath);

   append_backspace(cfg.txtpath);
   append_backspace(cfg.RIPpath);
   append_backspace(cfg.iconspath);
   append_backspace(cfg.mnupath);
   append_backspace(cfg.msgpath);
   append_backspace(cfg.uploadpath);
   append_backspace(cfg.pexpath);
   append_backspace(cfg.pvtuploadpath);
   append_backspace(cfg.nodelistdir);

   if(cfg.pwdchar == '\0') cfg.pwdchar = '#';

   if(cfg.numnodes>1) cfg.multiline = TRUE;

   if(cfg.IObuffersize < 1 || cfg.IObuffersize > 9999)
      cfg.IObuffersize = 1;

   User::setDir(cfg.msgpath);
}

static void near
init()
{
     //----------------------------  
     // JDR: REGISTRATION REFERENCE
     //----------------------------  
     
     if ( registered  ||
          max_node_count != 2 )
     {
          //-------------------------------------
          // Someone tampered with the .EXE file,
          // trying to hardcode these values
          //-------------------------------------

          exit_proboard();
     }


     CHECK_REG();


#ifdef KEY_NEEDED

     if ( ! registered )
     {
          LOG( "This version does not work in unregistered mode" );
      
          exit_proboard();
     }

#endif


     //-------------------------------------  
     // Check the node count of this license
     //-------------------------------------  

     if ( registered )
     {
          if ( max_node_count != 0 )
          {
               //-----------------------------------------------------------  
               // We have a distinct node count.  Are we within our license?
               //-----------------------------------------------------------  

               if ( node_number > max_node_count )
               {
                    //-----------------  
                    // License exceeded
                    //-----------------  

                    fatalerror( form( "Node license exceeded.  Tried to load node %d (%d allowed)",
                                      (int) node_number,
                                      (int) max_node_count ) );

                    exit_proboard();
               }

          }
     }
     else
     {
          //--------------------------------------------------  
          // Unregistered version -- only allow two nodes max!
          //--------------------------------------------------  

          if ( node_number > MAX_UNREG_NODES )
          {
               fatalerror( form( "Unregistered versions cannot start more than %d nodes", MAX_UNREG_NODES ) );

               exit_proboard();
          }
     }


     if ( tsw_OS == OS_DESQVIEW ) 
     {
          desqview = TRUE;
     }


     timelog tl;


     tl.read();

     totalcalls = tl.totalcalls;


     File f;


     if ( f.open( FN_BINLOG_PB ) )
     {
          if ( f.len()  >=  sizeof( BinLog ) )
          {
               f.seek( f.len() - sizeof( BinLog ) );

               f.read( & lastcaller,  sizeof( lastcaller ) );
          }


          f.close();
     }


     event e;

     memset( & nextevent,
             0,
             sizeof( nextevent ) );


     if ( f.open( FileName( syspath, "EVENTS.PRO" ) ) )
     {
          for(;;)
          {
               if ( f.read( & e,  sizeof( e ) )  !=  sizeof( e ) )
               {
                    break;
               }

               if ( e.minutesleft()  <=  nextevent.minutesleft() )
               {
                    nextevent = e;
               }
          }
     }


     num_users  =  int( User::numUsers() );

  // user.gentops();
}




