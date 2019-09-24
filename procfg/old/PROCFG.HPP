
/* ------------------------------------------------------------
 * Filename ............... ProCfg.Hpp
 *
 * General Purpose ........ main include file
 * ------------------------------------------------------------
 * First date ............. long time ago ;-)
 *
 * First in version ....... 0.00 alpha 1 ?
 *
 * Written by ............. P. Leybaert & A. Schellinck
 * ------------------------------------------------------------
 * Revisions:
 * ----------
 *
 *   Date   | By |                  Purpose                   |
 * ---------+----+--------------------------------------------+
 * 08/05/99 | JR | Updated to v2.2                            |
 *          |    |                                            |
 *          |    |                                            |
 */


/*--] Include files [------------------------------------------------------*/

#include <tslib.hpp>
#include <tswin.hpp>
#include <pb_lib.hpp>

/*--] Defines [------------------------------------------------------------*/

//----------------------------------  
// Color of status bar at the bottom
//----------------------------------  

#define BAR_COLOR 0x4E                  // Generic color
#define BAR_HIGH  0x4E                  // Status bar highlight color
#define BAR_NORM  0x4F                  // Status bar normal color


#define PROT_BATCH                     1
#define PROT_ENABLED                   2
#define PROT_BOTH                      4
#define PROT_BIM                       8
#define PROT_LOCAL                    16

#define MAX_AKA                        50
#define MAX_FILEAREA                   10000
#define MAX_MSGAREA                    10000
#define MAX_PVTFILE                    100
#define MAX_LIMITS                     50
#define MAX_PROTOCOL                   20
#define MAX_EVENT                      30

/*--] version control [----------------------------------------------------*/

#define VERSION                        "2.22"

/*--] Structure definitions [----------------------------------------------*/

struct filearea     : _FileArea      {};
struct Config       : _Config        {};
struct pbconfig     : _ModemConfig   {};

/*--] Structures [-------------------------------] aka [-------------------*/

struct aka
{
   int zone;
   int net;
   int node;
   int point;
};

/*--] Structures [-------------------------------] protocol [--------------*/

struct protocol
{
   char name[50];                      // Name of protocol
   char key;                           // Hotkey for Protocol
   char flags;                         // Protocol behavior
   char logfile[60];                   // Logfile from protocol
   char ctlfile[60];                   // Control file (redirection file)
   char dlcom[126];                    // Download command line
   unsigned eff;
   char ulcom[128];                    // Upload command line
   char dlstring[80];                  // String to place in download control file
   char ulstring[80];                  // String to place in upload control file
   char dlkeyword[40];                 // Download keyword to search for in logfile
   char ulkeyword[40];                 // Upload keyword to search for in logfile
   int wordnr;                         // File name word nr after keyword (1-..)
};

/*--] Structures [-------------------------------] limits [----------------*/

struct limits
{
   unsigned level;
   int timelimit;
   int daily_klimit;
   int pre_download;
   char id[6];
   unsigned      free;                 // Free upload in Kb.
   unsigned char factor;               // Percentage upload required
   unsigned      max_download;         // Max download for this level
   int           fallto;               // Fall to level x when max. reached
   int           msgfactor;            // # Kbytes granted per message written
   byte extra[5];
};

/*--] Structures [-------------------------------] topentry [--------------*/

struct topentry
{
   char name[36];
   unsigned n;
};


/*--] Structures [-------------------------------] event [-----------------*/

struct event
{
   int type;             // Errorlevel or DOS command
   int errorlevel;       // Errorlevel to use (>5)
   char command[80];     // DOS Command to execute
   Time start;           // Start time for event
   int duration;         // Duration (min)
   byte empty;
   byte days;            // Bits 0-6 -> days of the week
   byte node;            // Node number to execute the event on
   byte enabled;         // Enabled?
};

/*--] Structures [-------------------------------] pvt_files [-------------*/

struct pvt_file
{
   Date date;
   char fname[13];
   char username[36];
   char from[36];
};

/*--] Structures [-------------------------------] User [------------------*/

struct User : _User
{
};

/*--] Prototypes [---------------------------------------------------------*/

int   ask_save          (                                               );
int   ask_confirm       (  char *title                                  );
KEY   aka_select        (  int, Window&, int, int ,char, void*          );
void  aka_select_func   (  int i,int x,int y                            );
void  append_backspace  (  char *                                       );

void  editPvtFile       (                                               );
void  editSysParam      (                                               );
void  editTemplate      (  char *sysPath                                );
void  editUser          (  char *syspath, int, char *[]                 );

void  edit_akas         (                                               );
KEY   edit_color        (  int, Window&, int, int ,char, void*          );
void  edit_events       (                                               );
void  edit_fgroups      (                                               );
void  edit_mgroups      (                                               );
void  edit_language     (  char *                                       );
void  edit_limits       (                                               );
void  edit_macros       (                                               );
void  edit_menu         (  char *                                       );
void  edit_areas        (  int fa                                       );
void  edit_pb           (                                               );
void  edit_protocol     (                                               );
void  edit_stats        (                                               );
KEY   edit_timeframe    (  int, Window&, int, int ,char, void*          );
KEY   edit_nodes        (  int, Window& ,int ,int ,char, void*          );
void  EditListColors    (                                               );
KEY   flag_select       (  int, Window&, int, int ,char, void*          );
KEY   flagonoff_select  (  int, Window&, int, int ,char, void*          );
void  menu_editor       (  int, char *[]                                );
String select_language   (  bool edit                                    );
void  show_flags        (                                               );
void  showAbout         (                                               );

void ShowBusy(char *msg);
void HideBusy();


/*--] Global variables [---------------------------------------------------*/

extern protocol      *p;
extern Config        far cfg;
extern pbconfig      far pbcfg;
extern limits        *limit;
extern aka           *akas;
extern event         *events;
//extern pvt_file      *pvtfiles;
extern int           num_proto;
extern int           num_msgareas;
extern int           num_limits;
extern int           num_aka;
extern int           num_events;
//extern int           num_pvtfiles;
extern KEY           select_hotkeys[];
extern char          *loglevels[];
extern char          *months_short[];
extern char          *days_short[];
extern char          *yesnoask[];
extern char          syspath[];

void KeyItem( Window &w,
              int     x,
              int     y,
              ATTR    iKeyColor,
              char   *pKey,
              ATTR    iDescColor,
              char   *pDesc );

/*---------------------------] END OF THE CODE [---------------------------*/

