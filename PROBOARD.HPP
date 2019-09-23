#ifndef PROBOARD_HPP
#define PROBOARD_HPP

#include <tslib.hpp>
#include <time.h>

#define VERSION            "2.22"
#define TEARVERSION        "2.22"
#define PID_STRING         "ProBoard 2.22"
#define PID_STRING_HUDSON  PID_STRING " H"
#define PID_STRING_SQUISH  PID_STRING " S"
#define PID_STRING_JAM     PID_STRING " J"
#define PORT_VALUE   10

#define MAX_UNREG_NODES 255
#define MAX_UNREG_USERS 100000

#define VER_NUM  222

const word VER_WORD = 0x0220;
const word VER_BETA = 0xFFFF;   // 0000-EFFF = beta
                                // F000-FEEE = gamma
                                // FFFF      = release


//--------------------------------------------  
// Color of the status bar on the main console
//--------------------------------------------  

#define BAR_COLOR 0x4E


#include <pb_lib.hpp>

#ifndef MSGAPI
  class MSG;
#endif

#ifdef Use_MsgBase
  #include "msgbase.hpp"
#else
  class Message;
  class MsgBase;
  class MsgBaseList;
  class SquishMsgBase;
  class HudsonMsgBase;
  class JamMsgBase;
#endif

extern String S_PRESS_ENTER_TO_CONTINUE;

const int MAX_PEX_HANDLERS = 100;

const int BUFSIZE_FAST = 2048;
const int BUFSIZE_SLOW = 512;

#ifdef RELEASE
 inline void SCRAMBLE()
 {
  *((dword _far *)12) = *((dword _far *)0x46C);
 }
#else
 #define SCRAMBLE() ;
#endif

/*
struct User : _User
  {
   int Read(int rec);          // Read record from recnr 'rec'
   int Write(int rec);         // Write record to recnr 'rec' (-1 = add)
   int Find(char *username);   // Find user with name 'username'
   int FindAlias(char *alias); // Find user with alias 'alias'
   void list(char *);          // Generate user list
   void gentops();             // Generate top-statistics
   int FindFirstMatch(char *str,bool fuzzy);
   int FindNextMatch (char *str,bool fuzzy);
  };
*/

struct User : _User
   {
      void list(char *,bool aliases=FALSE);          // Generate user list
      //void gentops();             // Generate top-statistics
   };

struct Config           : _Config         {};
struct LanguageString   : _LanguageString {};
struct AreaGroup        : _AreaGroup      {};

struct FileArea : _FileArea
  {
   static File f;
   static int lastAreaNum;
   static FileArea *lastArea;
   static int numAreas;

   static void open();
 public:
   int  areaNum;

   bool read(int);

   static void close();
   static int  highAreaNum();
  };

struct timelog : _TimeLog
  {
   void read();
   void update();
  };

struct dl_today
  {
  int files;
  int kb;
  };

struct BinLog
  {
    Date date;         // 3
    Time timeIn;       // 3
    Time timeOut;      // 3
    char name[36];     // 36
    char city[26];     // 26
    char country[26];  // 26
    long baud;         // 4
    word node;         // 2
    long kbDown;       // 4
    long kbUp;         // 4
    word yells;        // 2
    word level;        // 2
    dword uflags;      // 4
    char alias[36];    // 36

    byte extra[45];

    BinLog();

    bool append();
  };

struct user_online
  {
  char name[36];
  char city[16];
  unsigned baud;
  int status;
  byte extra[8];

  void read(int = 0);
  void write();
  void clear();
  int chatreq(int);     // 0->Chat Granted   1->Timeout   2->Not Avail
  int check_chat();     // 0->No  (>0)->node
  void setstatus(int stat,int node=0);
  int getstatus(int node=0);
  };

#define UO_OFFLINE  -2
#define UO_BROWSING  0
#define UO_SHELLING -3
#define UO_BUSY     -4
#define UO_CHATTING -1

struct limits
  {
   unsigned      level;
   int           timelimit;
   int           daily_klimit;
   int           pre_download;
   char          id[6];
   unsigned      free;              // Free upload in Kb.
   unsigned char factor;            // Percentage upload required
   unsigned      max_download;      // Max download for this level
   unsigned      fallto;            // Fall to level x when max. reached
   int           msgfactor;         // # Kbytes granted per message written
   int           sessionlimit;      // Time limit per session
   byte extra[3];
  };

struct topentry
  {
  char name[36];
  dword n;
  };

struct aka
  {
  int zone;
  int net;
  int node;
  int point;

  int read(int n);
  };


class usertimer
  {
  time_t started;
  time_t suspend_start;
  int suspendedtime;
  bool suspended;
  int fixedtime;
  time_t inactivity_start;
 public:
  usertimer();
  void start(int minutes);
  void increase();
  void decrease();
  void changeleft(int);
  void suspend();
  void restart();
  int timesuspended() { return suspendedtime; }
  int left();
  int online();
  int used();
  void check();
  void clearinactivity();
  void checkinactivity();
  };

#define READMODE_ALL       0
#define READMODE_UPFIRST   1
#define READMODE_UPALL     2
#define READMODE_DIGITS    3
#define READMODE_PWD      64
#define READMODE_NOFIELD 128

class command_stack
  {
  int sp;
  char buf[100];
 public:
  command_stack() { sp=0; }
  void parse(char *);
  void clear() { sp=0; }
  char getnext();
  char pollnext();
  };

struct fossilio
  {
  int port;      // comport (0=COM1)
  long baud;     // baud rate
  int external;  // 1 if key received from com-port
  int stopenabled;
  bool show_local;
  bool show_remote;
  command_stack inputbuffer;
  int (*more_func)();

  byte *outputBuffer;
  int  bufferPtr;

  void init(int port,long baud);
  fossilio() { baud = 65535L; outputBuffer = NULL; }
  ~fossilio();
  void flush();
  byte readkey();
  byte wait();
  byte wait(char *);
  void wait(char);
  fossilio& operator<<(char);
  fossilio& operator<<(char*);
  byte send(char *str,char *hotkeys=NULL);
  byte sendfile(char *filename,char *hotkeys=NULL);
  void drawfield(int len);
  void read(char *s,int max,byte mode=READMODE_ALL);
  int  read(int& i,int max=5);
  int  read(long&, int max=7);
  void readpass(char *s);
  void hangup();
  int ask(int def=1);
  void cooldown();
  void heatup();
  void color(byte);
  void fullcolor(byte);
  void move(int x,int y);
  void clreol();
  void cls();
  void enablestop();
  void disablestop();
  void clearinput();
  };


struct node_entry
   {
   byte type;
   int zone,region,net,node;
   int cost;
   char name[50];
   char city[50];

   int read(char *);
   };

struct KeyData
   {
      Date createDate;
      Time createTime;
      char createdBy[36];
      char sysopName[36];
      char bbsName[36];
      bool alpha;
      bool beta;
      bool gamma;
      bool distSite;
      bool regSite;
      bool commercial;
      Date expirDate;
      word expirVersion;
      byte extra[2];
   };

#define NODE_ZONE 1
#define NODE_REGION 2
#define NODE_HOST 3
#define NODE_HUB 4

struct event
  {
  int type;             // Errorlevel or DOS command
  int errorlevel;       // Errorlevel to use (>5)
  char command[80];     // DOS Command to execute
  Time start;           // Time of the event
  int duration;         // Duration of the event (min).
  byte empty;
  byte days;            // Bits 0-6 -> days of the week
  byte node;            // Node number to execute the event on
  byte enabled;         // Enabled?

  int minutesleft();    // Minutes until this event
  int nextday();        // Day of this event
  };

struct pvt_file
  {
  Date date;
  char fname[13];
  char username[36];
  char from[36];
  };


struct modem_response
  {
   long baud;
   int  port;
  };

struct protocol
  {
   char name[50];          // Name of protocol
   char key;               // Hotkey for Protocol
   char flags;             // Protocol behavior
   char logfile[60];       // Logfile from protocol
   char ctlfile[60];       // Control file (redirection file)
   char dlcom[126];        // Download command line
   unsigned efficiency;    // Throughput efficiency
   char ulcom[128];        // Upload command line
   char dlstring[80];      // String to place in download control file
   char ulstring[80];      // String to place in upload control file
   char dlkeyword[40];     // Download keyword to search for in logfile
   char ulkeyword[40];     // Upload keyword to search for in logfile
   int wordnr;             // File name word nr after keyword (1-..)

   bool read(int n);
  };

#define PROT_BATCH   1
#define PROT_ENABLED 2
#define PROT_BOTH    4
#define PROT_BIM     8
#define PROT_LOCAL  16

extern byte current_color;

extern bool chatflag;
extern bool noyell_flag;
extern int  num_yells;

extern bool stopped;

extern char user_firstname[];
extern char mypath[];
extern char syspath[];
extern char curmenu[];
extern char page_reason[];

extern bool menuchanged;
extern bool updatemenu;

extern long totalcalls;
extern bool password_ok;
extern int  num_users;
extern int  time_limit;
extern int  event_due;
extern int  time_removed;
extern int  download_limit;
extern int  download_delay;
extern word free_download;
extern word upload_needed;
extern int  node_number;
extern bool desqview;
extern int  num_limits;
extern int  returnvalue;
extern bool quiet;
extern bool net_entered,echo_entered;
extern bool registered;
extern word max_node_count;
extern int  display_mode;
extern bool ega_43;
extern bool use_ems;
extern byte org_videomode;
extern word org_numlines;
extern bool ignore_carrier;
extern bool reliable;
extern bool fatal;
extern bool no_hangup;
extern word min_level;
extern bool rip_detected;
extern bool sysop_next;
extern int  rip_screenlines;
extern int  pastFirstUse;
extern bool newFilesChecked;

extern User          user;
extern int           user_recnr;
extern Config    far cfg;
extern limits        limit[];
extern command_stack comstack;
extern KeyData       regis;
extern fossilio      io;
extern event         nextevent;

extern LanguageFile    pb_curlang;
extern LanguageString *pb_lang;
extern int            pb_num_lang;

extern BinLog        lastcaller;

#ifdef Use_TagList
   extern LinkedList<FilesIdx> taglist;
#endif

struct PEXhandle
  {
   void *func;
   unsigned ds;
  };

//extern PEXhandle pex_handlers[];

extern "C" int  CallPEX(PEXhandle * ...);
//extern "C" void add_fhandles();

#ifdef Use_Handlers
   extern LinkedList<PEXhandle> sysopkey_handlers;
   extern LinkedList<PEXhandle> hangup_handlers;
#endif

/*
#define PEX_IO_INIT            (&pex_handlers[0])
#define PEX_IO_STATUS          (&pex_handlers[1])
#define PEX_IO_HANGUP          (&pex_handlers[2])
#define PEX_IO_READBYTE        (&pex_handlers[3])
#define PEX_IO_SENDBYTE        (&pex_handlers[4])
#define PEX_IO_SENDBLOCK       (&pex_handlers[5])
#define PEX_SYSOPKEY           (&pex_handlers[6])
#define PEX_HANGUP             (&pex_handlers[7])
*/

/*
extern
    File f_users,
         f_usersxi,
         f_usersidx,
         f_userspb;
*/

extern char *noregmsg;

extern Date login_date;
extern Time login_time;

void chat();
void LOG(char * ...);
void LOG(int level, char * ...);
void fatalerror(char *s);
void file_error(int fn , int errno = 0);      // <0 -> Return, >0 -> Exit
void file_error  (char *fn , int errno = 0);
void file_warning(char *fn , int errno = 0);
int login();
void menu_processor();

void shell              (char * = NULL),    combined_clear     (char * = NULL),
     showansasc         (char * = NULL),    clearmarked        (char * = NULL),
     pagesysop          (char * = NULL),    qscan_msg          (char * = NULL),
     writemsg           (char * = NULL),    scan_msg           (char * = NULL),
     readmsg            (char * = NULL),    usage_graph        (char *       ),
     city_change        (char * = NULL),    days_graph         (char *       ),
     change_state       (char * = NULL),    weeks_graph        (char *       ),
     password_change    (char * = NULL),    version_info       (char * = NULL),
     lines_change       (char * = NULL),    raw_dir            (char * = NULL),
     ansi_toggle        (char * = NULL),    list_files         (char * = NULL),
     cls_toggle         (char * = NULL),    new_files          (char * = NULL),
     pause_toggle       (char * = NULL),    keyword_search     (char * = NULL),
     set_filearea       (char * = NULL),    filename_search    (char * = NULL),
     fsed_toggle        (char * = NULL),    download           (char * = NULL),
     hotkey_toggle      (char * = NULL),    upload             (char * = NULL),
     user_list          (char * = NULL),    view_named_file    (char * = NULL),
     questionnaire      (char * = NULL),    view_file          (char * = NULL),
     mailcheck          (char * = NULL),    view_file_wait     (char * = NULL),
     list_archive       (char * = NULL),    lastcallers        (char * = NULL),
     time_stat          (char * = NULL),    logentry           (char * = NULL),
     combined_select    (char * = NULL),    logoff             (char * = NULL),
     change_access      (char * = NULL),    view_gif           (char * = NULL),
     tops               (char * = NULL),    ibm_toggle         (char * = NULL),
     show_users_online  (char * = NULL),    phone_change       (char * = NULL),
     multichat          (char * = NULL),    dataphone_change   (char * = NULL),
     set_msgarea        (char * = NULL),    handle_change      (char * = NULL),
     view_ans_wait      (char * = NULL),    usereditor         (char * = NULL),
     bulletin           (char * = NULL),    toggle_avatar      (char * = NULL),
     toggle_avtplus     (char * = NULL),    show_graph         (char *       ),
     ripfont_change     (char * = NULL),    select_language    (char * = NULL),
     rip_toggle         (char * = NULL),    edit_taglist       (char * = NULL),
     change_dateformat  (char * = NULL),    change_faxphone    (char * = NULL),
     change_address     (char * = NULL),    change_country     (char * = NULL),
     change_default_protocol(char * = NULL),
     select_msggroup    (char * = NULL),    select_filegroup   (char * = NULL);

int  run_sdkfile        (char * = NULL);
char showansascrip      (char *);
extern void (*menufunctions[])(char *);

void  parse_date(Date& d,char *s,int format);
void  multilinechat(int);
char *strstrl(char *,char *);
int   check_access(unsigned,AccessFlags);        // 0 -> Denied
int   check_access(unsigned,AccessFlags,AccessFlags);        // 0 -> Denied
int   linecounter(int = -1);
void  sysopkey(word);
void  replymsg(Message&);
int   matchstring(char *s,char *d);          // 1->ok, 0->not ok
int   matchstring(char *s,char *d,int l);    // 1->ok, 0->not ok
int   matchfile(char *search,char *file);
char *findmatch(char *,char *);
void  strip_path(char *);
void  strip_fname(char *);
void  msged();
char  showansasc(char *file,char *hotkeys);
bool  show_rip( char *file , bool reset = FALSE);

void  rip_send( char *s);
void  rip_query( char *cmd);
void  rip_reset();
void  rip_textmode();
void  rip_nomore();
void  rip_kill_mouse_fields();
void  rip_bar(int,int,int,int);
void  rip_color(int);
void  rip_fill_color(int,int);
void  rip_send_icons();
void  rip_button(int x1,int y1,int x2,int y2,int hotkey,bool def,char *icon,char *text,char *hostcmd);
void  rip_start_dialog(char *prompt,int max);
void  rip_end_dialog();
void  rip_start_yesno_dialog( char *prompt , bool);
void  rip_clear_status();
void  rip_show_more();
void  rip_show_more_tag();
void  rip_show_enter();

#define rip_save_textwindow()       rip_query("$STW$")
#define rip_restore_textwindow()    rip_query("$RTW$")
#define rip_save_mouse_fields()     rip_query("$SMF$")
#define rip_restore_mouse_fields()  rip_query("$RMF$")
#define rip_paste_clipboard()       rip_query("$PCB$")
#define rip_doorway_on()            rip_query("$DWAYON$")
#define rip_doorway_off()           rip_query("$DWAYOFF$")
#define rip_cursor_on()             rip_query("$CON$")
#define rip_cursor_off()            rip_query("$COFF$")

String meganum(word);

void  readconfig();
int   parse_data(char *s,String *param);     // ret=number of params
void  create_arealist(String param[],int numpara,BitArray& arealist,bool msgareas = FALSE);
void  adjust_limits();
void  deltrailspaces(char *);
int   wordwrap(char *,String&,int max);
char  playsong(char *name,char *keys,int length);
void  toggle_userinfo();
void  toggle_help();
void  toggle_macrohelp();
void  edit_user();
void  screen_image();
void  update_user_window();
void  update_display();
long  post_message(char *from,char *to,char *subj,int area,int pvt);
long  post_netmail_message(char *from,char *to,char *subj,int area,int zone,int net,int node,int point,bool crash,bool attach,bool kill);
void  write_msgtmp(char *s);
void  edit_message();
void  write_msginf( char *from , char *to , char *subj , long num , char *area , char *pvt);
String replace_stringvars(char *s);
void  show_intro();
void  check_exe(char *);
int   fuzzy_search(char *,char *,int);
int intimewindow(Time t1,Time t2);
char *dos_error_message(int errno);
bool select_protocol(protocol& p , char protocol_key = '\0' , bool seldef =  FALSE);
String fullcolor_string(byte color);
void remove_tag(char *name , int area);
bool is_tagged(char *name , int area);
void write_taglist();
void read_taglist();
void InputDate( Date &d,
                byte  iDateFormat );

void rip_text_window( int x0, int y0 , int x1 , int y1 , int font );

extern "C"
  {
   void     set43();
   void     set25();
   void     setvideomode(unsigned);
   unsigned getvideomode();
  }

void exit_proboard(int level = 0);

extern char *months_short[];
extern char *months_full [];
extern char *days_short  [];
extern char *days_full   [];
extern char *date_formats[];
extern char *date_formats_long[];

extern usertimer timer;

extern FileName FN_BINLOG_PB,
                FN_TIMELOG_PRO,
                FN_ONLINE_PRO,
                FN_MSGAREAS_PRO,
                FN_PVTFILES_PB;

const int ERR_BINLOG_PB    = 6;
const int ERR_TIMELOG_PRO  = 7;
const int ERR_ONLINE_PRO   = 9;
const int ERR_AKA_PRO      = 10;
const int ERR_USERS_BBS    = 11;
const int ERR_USERSXI_BBS  = 12;
const int ERR_USERSIDX_BBS = 13;
const int ERR_USERSPB_BBS  = 14;
const int ERR_LASTREAD_BBS = 15;

extern char *EMPTY_STRING;

#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_WHITE   7
#define COLOR_BLINK   0x80

#define ERRLVL_FATALERR 1

#define READMSG_ALL 1
#define READMSG_FWD 1
#define READMSG_REV 2
#define READMSG_SEL 3
#define READMSG_SEARCH 4
#define READMSG_NEW 5
#define READMSG_INDIV 6
#define READMSG_MARKED 7

#define ANS_OK     0    // showansasc retval if ok
#define ANS_STOP   1    // showansasc retval if file stopped
#define ANS_NOFILE 2    // showansasc retval if no file

#define ansi_mode  (user.uFlags & UFLAG_ANSI)
#define cls_mode   (user.uFlags & UFLAG_CLEAR)
#define pause_mode (user.uFlags & UFLAG_PAUSE)
#define fsed_mode  (user.uFlags & UFLAG_FSED)
#define rip_mode   (rip_detected && !(user.uFlags & UFLAG_NORIP))
#define stack_mode (!(user.uFlags & UFLAG_HOTKEYS))
#define no_ibm     (user.uFlags & UFLAG_NOIBM)
#define avatar     (user.uFlags & UFLAG_AVATAR)
#define avtplus    (user.uFlags & UFLAG_AVTPLUS)

struct DownloadFile
   {
      int    area;
      long   size;
      bool   free;
      bool   copy;
      String name;
   };

#include "prompts.hpp"

bool ReadLanguage( char *langname );

char wait_language_hotkeys(char *hk, bool enter = TRUE);
int  language_string_length( char *str );



#include "reg.hpp"


#endif

