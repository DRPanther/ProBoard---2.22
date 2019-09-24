
/* ------------------------------------------------------------
 * Filename ............... EditSys.Cpp
 *
 * General Purpose ........ Edit System Options
 * ------------------------------------------------------------
 * First date ............. 21 dec 1993
 *
 * First in version ....... 2.00
 *
 * Written by ............. Alain Schellinck
 * ------------------------------------------------------------
 * Revisions:
 * ----------
 *
 *   Date   | By |                  Purpose                   |
 * ---------+----+--------------------------------------------+
 *          |    |                                            |
 *          |    |                                            |
 *          |    |                                            |
 */

/*-------------------------------------------------------------------------*/

#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <TsWin.Hpp>

#include "ThProCfg.Hpp"

bool
ask_create(String path)
{
   if(path.len() <= 3)
      return TRUE;

   path.delLast('\\');

   if(ts_DirExists(path))
      return TRUE;

   Window w(16,9,65,18,0xF,SHADOW|CENTERED_X);
   w.open();

   w.centerLine(2,"Directory does not exist:",0xF);
   w.centerLine(4,path,0xE);


   menu_item savemenu[]=
   {
      { 'C', 15,15, "              Create Directory"   },
      { 'N', 14,14, "          Do NOT Create Directory"  },
      {  0 , 0,0, NULL                                }
   };

   PopMenu menu(savemenu,0x70,0xF);
   menu.open(18,15,63,16,0x7,NOBORDER|CENTERED_X);

   int choice = menu.process();

   if(choice == 1)
   {
      if(ts_MakeFullDir(path))
         return TRUE;

      tsw_beep();
      tsw_beep();
      tsw_beep();
      tsw_beep();

      return FALSE;
   }
   else
   {
      return FALSE;
   }
}


/*--] Code [-------------------------------------] path editor [-----------*/

/*
 * Routine   : sysPathHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysPathHelp(IntT field)
{
   ChrP helpText[]=
   {
      "Drive+Directory for ANS/ASC files",
      "Drive+Directory for RIP sequences",
      "Drive+Directory for RIP icons used in RIP screens",
      "Drive+Directory for menu-files (*.MNU)",
      "Drive+Directory for messagebase",
      "Drive+Directory where uploads should be placed",
      "Drive+Directory where private uploads (user-to-user) should be placed",
      "Drive+Directory where raw nodelist is located",
      "Drive+Directory from where .PEX files should be loaded",
      "Full path and filename of fullscreen-editor (eg. *SQUICKED.EXE)",
      "Set to \"Yes\" if you want to use the internal TheEdit/Lite as your editor",
      "Full path and filename + parameters for external chat utility"
   };

   tsw_showfooter(helpText[field], BAR_COLOR);
}

/*
 * Routine   : editPaths()
 * Purpose   : edit the system paths
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editPaths()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_STRING      ,cfg.txtpath            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,2    },
      { FRM_STRING      ,cfg.RIPpath            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,3    },
      { FRM_STRING      ,cfg.iconspath          ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,4    },
      { FRM_STRING      ,cfg.mnupath            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,5    },
      { FRM_STRING      ,cfg.msgpath            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,6    },
      { FRM_STRING      ,cfg.uploadpath         ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,7    },
      { FRM_STRING      ,cfg.pvtuploadpath      ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,8    },
      { FRM_STRING      ,cfg.nodelistdir        ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,9    },
      { FRM_STRING      ,cfg.pexpath            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,10   },
      { FRM_STRING      ,cfg.editorname         ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,12   },
      { FRM_YESNO       ,&cfg.internalfsed      ,0          ,0          ,0          ,3      ,3        ,34   ,13   },
      { FRM_STRING      ,cfg.extChat            ,0          ,0          ,FRM_UPPER  ,60     ,30       ,16   ,15   }
   };

   FormWindow w( 5, 
                 3, 
                 54, 
                 20, 
                 0x3F,
                 BRACKETS | SHADOW,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "Paths", 0x3E );

   w << "\n    Textfiles:\n"
          "    RIP files:\n"
          "    RIP icons:\n"
          "        Menus:\n"
          "  Messagebase:\n"
          "      Uploads:\n"
          "  Pvt Uploads:\n"
          " Nodelist Dir:\n"
          "    PEX files:\n"
          "\n"
          "   Editor Cmd:\n"
          " Use internal fullscreen editor?\n\n"
          "  Extern.Chat:";

   w.define( frm, 
             12, 
             0x31,
             0x4E,
             NULL, 
             sysPathHelp );

   for(;;)
   {
      tsw_cursoron();
      w.process();
      tsw_cursoroff();

      append_backspace(cfg.txtpath      ); if(cfg.txtpath      [0] == '\\') cfg.txtpath      [0] = '\0';
      append_backspace(cfg.RIPpath      ); if(cfg.RIPpath      [0] == '\\') cfg.RIPpath      [0] = '\0';
      append_backspace(cfg.iconspath    ); if(cfg.iconspath    [0] == '\\') cfg.iconspath    [0] = '\0';
      append_backspace(cfg.mnupath      ); if(cfg.mnupath      [0] == '\\') cfg.mnupath      [0] = '\0';
      append_backspace(cfg.msgpath      ); if(cfg.msgpath      [0] == '\\') cfg.msgpath      [0] = '\0';
      append_backspace(cfg.uploadpath   ); if(cfg.uploadpath   [0] == '\\') cfg.uploadpath   [0] = '\0';
      append_backspace(cfg.pvtuploadpath); if(cfg.pvtuploadpath[0] == '\\') cfg.pvtuploadpath[0] = '\0';
      append_backspace(cfg.nodelistdir  ); if(cfg.nodelistdir  [0] == '\\') cfg.nodelistdir  [0] = '\0';
      append_backspace(cfg.pexpath      ); if(cfg.pexpath      [0] == '\\') cfg.pexpath      [0] = '\0';

      if(cfg.txtpath[1] != ':' || !ask_create(cfg.txtpath))
      {
         tsw_beep();

         if(cfg.msgpath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for text files (ANSI files)","\x4EPress any key to continue",NULL);

         w.setField(0);
         continue;
      }
      if(cfg.RIPpath[1] != ':' || !ask_create(cfg.RIPpath))
      {
         tsw_beep();

         if(cfg.RIPpath[1] != ':')
         {
            tsw_msgbox(0x4F,"\x4E WARNING ","\x4FYou have not entered a path for RIP files","\x4FYou will not be able to use RIP","\x4F","\x4EPress any key to continue",NULL);
         }
         else
         {
            w.setField(1);
            continue;
         }
      }
      if(cfg.iconspath[1] != ':' || !ask_create(cfg.iconspath))
      {
         tsw_beep();

         if(cfg.iconspath[1] != ':')
         {
            tsw_msgbox(0x4F,"\x4E WARNING ","\x4FYou have not entered a path for RIP icons","\x4FYou will not be able to use RIP","\x4F","\x4EPress any key to continue",NULL);
         }
         else
         {
            w.setField(2);
            continue;
         }
      }
      if(cfg.mnupath[1] != ':' || !ask_create(cfg.mnupath))
      {
         tsw_beep();

         if(cfg.mnupath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for your menu files","\x4EPress any key to continue",NULL);

         w.setField(3);
         continue;
      }
      if(cfg.msgpath[1] != ':' || !ask_create(cfg.msgpath))
      {
         tsw_beep();

         if(cfg.msgpath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for your user files and Hudson message base","\x4EPress any key to continue",NULL);

         w.setField(4);
         continue;
      }
      if(cfg.uploadpath[1] != ':' || !ask_create(cfg.uploadpath))
      {
         tsw_beep();

         if(cfg.uploadpath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path where uploads should be placed","\x4EPress any key to continue",NULL);

         w.setField(5);
         continue;
      }
      if(cfg.pvtuploadpath[1] != ':' || !ask_create(cfg.pvtuploadpath))
      {
         tsw_beep();

         if(cfg.pvtuploadpath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for personal files","\x4EPress any key to continue",NULL);

         w.setField(6);
         continue;
      }
      if(cfg.nodelistdir[1] != ':' || !ask_create(cfg.nodelistdir))
      {
         tsw_beep();

         if(cfg.nodelistdir[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for your nodelist files","\x4EPress any key to continue",NULL);

         w.setField(7);
         continue;
      }
      if(cfg.pexpath[1] != ':' || !ask_create(cfg.pexpath))
      {
         tsw_beep();

         if(cfg.pexpath[1] != ':')
            tsw_msgbox(0x4F,"\x4E ERROR ","\x4FPlease enter a path for your PEX files","\x4EPress any key to continue",NULL);

         w.setField(8);
         continue;
      }

      break;
   }

   strip_all(cfg.extChat);
}

/*--] Code [-------------------------------------] new user editor [-------*/

/*
 * Routine   : sysUserHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysUserHelp(IntT i)
{
   ChrP txt[]=
   {
      "Level to be given to new users",
      "Flags to be given to new users",
      "Loglevel to be given to new users",
      "Allow users to select ANSI terminal emulation?",
      "Allow users to select AVATAR 0/0+ terminal emulation?",
      "Ask new users for their voice phone number?",
      "Ask new users for their data phone number?",
      "Ask new users for their fax phone number?",
      "Ask new users for their birth date or allow them not to enter it",
      "Ask new users for their state?",
      "Ask new users for their country?",
      "Ask new users for their mailing address?",
      "Ask new users for their sex?",
      "Ask new users for their preferred date format?"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   : editUserParam()
 * Purpose   : edit the new user default parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editUserParam()
{
   LCL ChrP birthChoice[] =
   {
      "No" , "Yes" , "Allow blank" ,
      NULL
   };

   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_UNSIGNED    ,&cfg.newuserlevel      ,0          ,0          ,0          ,5       ,5       ,22   ,2    },
      { FRM_FUNCTION    ,&cfg.newuserflags      ,0          ,flag_select,0          ,26      ,26      ,22   ,3    },
      { FRM_CHOICE      ,&cfg.newuserloglevel   ,loglevels  ,0          ,0          ,10      ,10      ,22   ,5    },
      { FRM_YESNO       ,&cfg.allowansi         ,0          ,0          ,0          ,3       ,3       ,22   ,7    },
      { FRM_YESNO       ,&cfg.allowavatar       ,0          ,0          ,0          ,3       ,3       ,22   ,8    },
      { FRM_YESNO       ,&cfg.askvoicephone     ,0          ,0          ,0          ,3       ,3       ,22   ,9    },
      { FRM_YESNO       ,&cfg.askdataphone      ,0          ,0          ,0          ,3       ,3       ,22   ,10   },
      { FRM_YESNO       ,&cfg.askfaxphone       ,0          ,0          ,0          ,3       ,3       ,22   ,11   },
      { FRM_CHOICE      ,&cfg.askBirthDay       ,birthChoice,0          ,0          ,11      ,11      ,22   ,12   },
      { FRM_YESNO       ,&cfg.askstate          ,0          ,0          ,0          ,3       ,3       ,22   ,13   },
      { FRM_YESNO       ,&cfg.askcountry        ,0          ,0          ,0          ,3       ,3       ,22   ,14   },
      { FRM_YESNO       ,&cfg.askaddress        ,0          ,0          ,0          ,3       ,3       ,22   ,15   },
      { FRM_YESNO       ,&cfg.asksex            ,0          ,0          ,0          ,3       ,3       ,22   ,16   },
      { FRM_YESNO       ,&cfg.askdateformat     ,0          ,0          ,0          ,3       ,3       ,22   ,17   }
   };

   FormWindow w( 4, 
                 3, 
                 61, 
                 22, 
                 0x3F,
                 BRACKETS | SHADOW,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "New Users", 0x3E );

   w << "\n"
        "     New user level:\n"
        "     New user flags:\n"
        "\n"
        "  New user loglevel:\n"
        "\n"
        "         Allow ANSI:\n"
        "       Allow AVATAR:\n"
        "    Ask voice phone:\n"
        "    Ask data  phone:\n"
        "    Ask fax   phone:\n"
        "     Ask birth date:\n"
        "          Ask state:\n"
        "        Ask country:\n"
        "        Ask address:\n"
        "            Ask sex:\n"
        "    Ask date format:";

   w.define( frm, 
             14,
             0x31,
             0x4E,
             NULL, 
             sysUserHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] [-----------------------*/

/*
 * Routine   : sysSecHelp()
 * Purpose   : display the help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysSecHelp(IntT i)
{
   ChrP txt[]=
   {
      "Allow quick sysop login by pressing [Enter] ?",
      "Write a message to user & sysop if someone failed after max. pwd-retries ?",
      "Allow users to login with their handle (alias)",
      "Log local calls in PROBOARD.LOG",
      "Hide sysop activity from Last Callers, User list, Users online ?",
      "Maximum number of times a password may be re-entered",
      "Minimum password length",
      "Message area # where security-related messages are to be stored",
      "Level needed to write \"crashmail\"",
      "Flags needed to write \"crashmail\"",
      "Level needed to be allowed \"file-attach\"",
      "Flags needed to be allowed \"file-attach\"",
      "Are users required to enter a system password",
      "Enter the system password to be entered (if enabled)"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   : editSecurity()
 * Purpose   : edit system security parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editSecurity()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_YESNO       ,&cfg.allowquicklogin   ,0          ,0          ,0          ,3       ,3       ,26   ,2    },
      { FRM_YESNO       ,&cfg.pwdmessages       ,0          ,0          ,0          ,3       ,3       ,26   ,3    },
      { FRM_YESNO       ,&cfg.allowalias        ,0          ,0          ,0          ,3       ,3       ,26   ,4    },
      { FRM_YESNO       ,&cfg.loglocal          ,0          ,0          ,0          ,3       ,3       ,26   ,5    },
      { FRM_YESNO       ,&cfg.discrete          ,0          ,0          ,0          ,3       ,3       ,26   ,6    },
      { FRM_INT         ,&cfg.max_passinput     ,0          ,0          ,0          ,2       ,3       ,26   ,8    },
      { FRM_INT         ,&cfg.min_passlength    ,0          ,0          ,0          ,2       ,3       ,26   ,9    },
      { FRM_INT         ,&cfg.securityboard     ,0          ,0          ,0          ,5       ,6       ,26   ,11   },
      { FRM_UNSIGNED    ,&cfg.crashlevel        ,0          ,0          ,0          ,5       ,6       ,26   ,13   },
      { FRM_FUNCTION    ,&cfg.crashflags        ,0          ,flag_select,0          ,26      ,26      ,26   ,14   },
      { FRM_UNSIGNED    ,&cfg.attachlevel       ,0          ,0          ,0          ,5       ,6       ,26   ,16   },
      { FRM_FUNCTION    ,&cfg.attachflags       ,0          ,flag_select,0          ,26      ,26      ,26   ,17   },
      { FRM_YESNO       ,&cfg.usesystempwd      ,0          ,0          ,0          ,3       ,3       ,53   ,2    },
      { FRM_STRING      ,cfg.systempwd          ,0          ,0          ,0          ,15      ,16      ,53   ,3    }
   };

   if(cfg.crashlevel     == 0) cfg.crashlevel     = WrdT(64000L);
   if(cfg.attachlevel    == 0) cfg.attachlevel    = WrdT(64000L);
   if(cfg.max_passinput  == 0) cfg.max_passinput  = 3;
   if(cfg.min_passlength == 0) cfg.min_passlength = 4;
   if(cfg.securityboard  == 0) cfg.securityboard  = 1;

   FormWindow w( 4,
                 3,
                 76,
                 22, 
                 0x3F,
                 BRACKETS | SHADOW,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "Security", 0x3E );

   w << "\n"
        "      Allow quick login:            Use system pwd:\n"
        " Write pwd-failure msgs:           System password:\n"
        " Allow login with alias:\n"
        "        Log local calls:\n"
        "    Hide sysop activity:\n"
        "\n"
        "  Max. password retries:\n"
        "  Min. password  length:\n"
        "\n"
        "  Security message area:\n"
        "\n"
        "    Level for crashmail:\n"
        "    Flags for crashmail:\n"
        "\n"
        "  Level for file-attach:\n"
        "  Flags for file-attach:";

   w.define( frm, 
             14, 
             0x31, 
             0x4E, 
             NULL,
             sysSecHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] [-----------------------*/

/*
 * Routine   : sysYellHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysYellHelp(IntT i)
{
   ChrP txt[]=
   {
      "Maximum number of times the sysop may be paged per call",
      "Duration of the 'page-bell' when paging the sysop (in seconds)",
      "Paging Hours (Press [Enter] to edit)",
      "Message area # where messages should be posted when the sysop is unavailable"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   : editYell()
 * Purpose   : edit the yell parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editYell()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc          flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------------  */
      { FRM_INT         ,&cfg.max_sysop_pages   ,0          ,0                ,0          ,2       ,3       ,26   ,2    },
      { FRM_INT         ,&cfg.pagebell_length   ,0          ,0                ,0          ,4       ,5       ,26   ,3    },
      { FRM_FUNCTION    ,&cfg.pagingHours       ,0          ,edit_timeframe   ,0          ,20      ,20      ,26   ,4    },
      { FRM_INT         ,&cfg.pageArea          ,0          ,0                ,0          ,5       ,6       ,26   ,5    }
   };

   FormWindow w( 18, 
                 11, 
                 70, 
                 18, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "Yelling", 0x3E );

   w << "\n"
        "       Max. sysop pages:\n"
        "  Pagebell length (sec):\n"
        "           Paging Hours:\n"
        "         Message area #:";

   w.define( frm, 
             4, 
             0x31, 
             0x4E, 
             NULL, 
             sysYellHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] transfer editor [-------*/

/*
 * Routine   : sysTransferHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysTransferHelp(IntT i)
{
   ChrP txt[]=
   {
      "Minimum diskspace required on upload drive for uploads to be allowed",
      "Download Hours (Press [Enter] to edit)",
      "Check for duplicate uploads?",
      "Kill duplicate uploads?",
      "Ignore file extension when checking for duplicate uploads?",
      "Command to execute for upload scanner (@<FILE>@ = file to scan)",
      "<Errorlevel>=Scanner uses errorlevel   <Semaphore>=Scanner uses semaphore file",
      "Name of semaphore file created by upload scanner",
      "Action to perform for failed files",
      "Area to move failed files to (if action is set to \"Move\")",
      "Add the name of the user who uploaded a file to FILES.BBS/FILES.PVT?"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   :
 * Purpose   :
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editTransfer()
{
   char *virScanTypes[]   = { "Errorlevel" , "Semaphore" , NULL };
   char *virScanActions[] = { "None" , "Move" , "Delete" , NULL };

   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_INT         ,&cfg.uploadspace       ,0          ,0          ,0          ,4       ,5       ,26   ,2    },
      { FRM_FUNCTION    ,&cfg.downloadHours     ,0          ,edit_timeframe,0       ,20      ,20      ,26   ,4    },
      { FRM_YESNO       ,&cfg.checkdupes        ,0          ,0          ,0          ,3       ,3       ,26   ,6    },
      { FRM_YESNO       ,&cfg.killdupes         ,0          ,0          ,0          ,3       ,3       ,26   ,7    },
      { FRM_YESNO       ,&cfg.ignore_ext        ,0          ,0          ,0          ,3       ,3       ,26   ,8    },
      { FRM_STRING      , cfg.virScanCommand    ,0          ,0          ,FRM_UPPER  ,60      ,30      ,26   ,10   },
      { FRM_CHOICE      ,&cfg.virScanType       ,virScanTypes,0         ,0          ,10      ,10      ,26   ,11   },
      { FRM_STRING      , cfg.virScanSemaphore  ,0          ,0          ,FRM_UPPER  ,13      ,14      ,26   ,12   },
      { FRM_CHOICE      ,&cfg.virScanFailedAction,virScanActions,0      ,0          ,6       ,6       ,26   ,13   },
      { FRM_INT         ,&cfg.virScanFailedArea ,0          ,0          ,0          ,5       ,6       ,26   ,14   },
      { FRM_YESNO       ,&cfg.addUploaderName   ,0          ,0          ,0          ,3       ,3       ,36   ,16    }
   };

   FormWindow w( 9, 
                 3, 
                 70, 
                 20, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "File Transfer", 0x3E );

   w << "\n"
        "   Minimum Upload space:       Kb\n"
        "\n"
        "         Download hours:\n"
        "\n"
        "     Check dupe uploads:\n"
        "      Kill dupe uploads:\n"
        " Ignore file extensions:\n\n"
        " Upload scanner command:\n"
        "      ÃÄÄÄÄÄÄÄÄÄÄ> type:\n"
        "      ÃÄÄÄÄÄ> semaphore:\n"
        "      ÃÄÄÄÄÄÄÄÄ> action:\n"
        "      ÀÄ> bad file area:\n\n"
        " Add uploader's name to FILES.BBS:";

   w.define( frm, 
             11, 
             0x31, 
             0x4E, 
             NULL, 
             sysTransferHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] General editor [--------*/

/*
 * Routine   : sysGeneralHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysGeneralHelp(IntT i)
{
   ChrP txt[]=
   {
      "Check for mail at logon?",      // mailcheck
      "Allow new users to enter only one word as their name?",                              // allowoneword
      "Europe = \"DD/MM/YY\"  American = \"MM/DD/YY\"  (For use in ProCFG only)",           // europe
      "Use file sharing/locking?",                              // multiline
      "Swap to EMS/Disk when shelling to DOS? (Default)",                              // doswap
      "Use extra buffers to speed up ProBoard?",                              // fastmode
      "Set the Kill/Sent flag for netmail?",                              // killsent
      "Ask confirmation when validating a user?",                              // valConfirm
      "Number of days to log in the binary log file. 0 = forever",   // binlogdays
      "Log local logons to the binary log file (BINLOG.PB) ?",                              // \qnloglocal
      "Outbound serial I/O buffer size in bytes (1 = disable , 32 = recommended)",                              // IObuffersize
      "Default language file to be loaded at startup and when language not found",
      "Maximum number of seconds a user can be inactive",                              // inactivity_time
      "String to use for quoting ('@' is replaced by the user's initials)",                              // quotestring
      "Character to display when typing/showing a user's password",                              // pwdchar
      "Hide password in the user editor?",                              // hidePassword
      "# seconds after which the screen saver goes active",                              // blanktime
      "Fuzzy search rate for the user editor (0-100 %)",                              // fuzzyRate
      "If disabled, ProBoard will not use RIP, even if detected"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}
/*
 * Routine   :
 * Purpose   :
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editGeneral()
{
   char *enab_disab[] = { "Enabled" , "Disabled" , NULL };

   LCL ChrP dateFormats[] =
   {
      "American" , "European" ,
      NULL
   };

   if(cfg.europe)
      cfg.europe=1;

   if(!cfg.quotestring[0])
      strcpy(cfg.quotestring,"@>");

   if(!cfg.pwdchar)
      cfg.pwdchar = '±';

   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_CHOICE      ,&cfg.mailcheck         ,yesnoask   ,0          ,0          ,3       ,3       ,25   ,2    },
      { FRM_YESNO       ,&cfg.allowoneword      ,0          ,0          ,0          ,3       ,3       ,25   ,3    },
      { FRM_CHOICE      ,&cfg.europe            ,dateFormats,0          ,0          ,8       ,8       ,25   ,4    },
      { FRM_YESNO       ,&cfg.multiline         ,0          ,0          ,0          ,3       ,3       ,25   ,5    },
      { FRM_YESNO       ,&cfg.doswap            ,0          ,0          ,0          ,3       ,3       ,25   ,6    },
      { FRM_YESNO       ,&cfg.fastmode          ,0          ,0          ,0          ,3       ,3       ,25   ,7    },
      { FRM_YESNO       ,&cfg.killsent          ,0          ,0          ,0          ,3       ,3       ,25   ,8    },
      { FRM_YESNO       ,&cfg.valConfirm        ,0          ,0          ,0          ,3       ,3       ,25   ,9    },
      { FRM_UNSIGNED    ,&cfg.binlogdays        ,0          ,0          ,0          ,3       ,4       ,25   ,11   },
      { FRM_YESNO       ,&cfg.binloglocal       ,0          ,0          ,0          ,3       ,3       ,25   ,12   },
      { FRM_UNSIGNED    ,&cfg.IObuffersize      ,0          ,0          ,0          ,4       ,5       ,25   ,14   },
      { FRM_STRING      ,&cfg.defaultLanguage   ,0          ,0          ,0          ,8       ,9       ,25   ,16   },
      { FRM_UNSIGNED    ,&cfg.inactivity_time   ,0          ,0          ,0          ,4       ,5       ,55   ,2    },
      { FRM_STRING      ,cfg.quotestring        ,0          ,0          ,0          ,5       ,6       ,55   ,4    },
      { FRM_CHAR        ,&cfg.pwdchar           ,0          ,0          ,0          ,1       ,1       ,55   ,6    },
      { FRM_YESNO       ,&cfg.hidePassword      ,0          ,0          ,0          ,1       ,3       ,55   ,7    },
      { FRM_UNSIGNED    ,&pbcfg.blankTime       ,0          ,0          ,0          ,4       ,5       ,55   ,9    },
      { FRM_UNSIGNED    ,&cfg.fuzzyRate         ,0          ,0          ,0          ,3       ,4       ,55   ,11   },
      { FRM_CHOICE      ,&cfg.disableRIP        ,enab_disab ,0          ,0          ,8       ,8       ,55   ,13   }
   };

   FormWindow w( 10, 
                 4, 
                 76, 
                 22, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "System Options", 0x3E );

   w << "\n"
        "   Check mail at login:             Inactivity limit:\n"
        "  Allow one word names:\n"
        "           Date format:                 Quote string:\n"
        "      Use file sharing:\n"
        "          Swap to disk:             Password display:\n"
        "             Fast mode:                Hide password:\n"
        " Kill/Sent for netmail:\n"
        "      Confirm validate:              Screen blanking:      s\n"
        "\n"
        "     Activity log size:             FuzzySearch rate:   %\n"
        "    Log local activity:\n"
        "                                        RIP Graphics:\n"
        "       I/O Buffer Size:\n\n"
        " Default language file:";

   if(cfg.IObuffersize < 1 || cfg.IObuffersize > 9999)
      cfg.IObuffersize = 1;

   w.attrib(0x3E);

   w.define( frm, 
             19, 
             0x31,
             0x4E,
             NULL, 
             sysGeneralHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();

   cfg.allowansi      = (cfg.allowansi     ) ?1:0;
   cfg.allowoneword   = (cfg.allowoneword  ) ?1:0;
   cfg.allowmsgupload = (cfg.allowmsgupload) ?1:0;
   cfg.europe         = (cfg.europe        ) ?1:0;
   cfg.loglocal       = (cfg.loglocal      ) ?1:0;
   cfg.doswap         = (cfg.doswap        ) ?1:0;
   cfg.fastmode       = (cfg.fastmode      ) ?1:0;
   cfg.killsent       = (cfg.killsent      ) ?1:0;
   cfg.discrete       = (cfg.discrete      ) ?1:0;
   cfg.allowalias     = (cfg.allowalias    ) ?1:0;
   cfg.allowavatar    = (cfg.allowavatar   ) ?1:0;
   cfg.binloglocal    = (cfg.binloglocal   ) ?1:0;
}


/*--] Code [-------------------------------------] display editor [--------*/

/*
 * Routine   : sysDisplayHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysDisplayHelp(IntT i)
{
   ChrP txt[]=
   {
      "Use EGA 43 or VGA 50 line mode ?",
      "Select the background color for input fields (prompts)"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}


KEY
edit_bgcol(int mode,Window& w,int x,int y,char attr,void *data)
{
   //byte& col = *(byte *)data;
   byte col = *(byte *)data;
   char *example  = " Example Prompt     ";

   if(mode)
   {
      CursorState _cursor(FALSE);

      int x2 = x+w.minX+26;
      int y2 = y+w.minY+8;

      if(x2 > 79)
         x2 -= (x2-79);
      if(y2 > 23)
         y2 -= (y2-23);


      Window sw( x2 - 27,
                 y2 - 9,
                 x2,
                 y2,
                 0x7,
                 0,
                 CHISEL_BORDER,
                 0x07,
                 NULL,
                 0x08 );

      sw.open();

      for(;;)
      {
         for(byte c=0 ; c<8 ; c++)
         {
            sw.direct( 1,c+1,"  ");
            sw.direct( 3,c+1, (c<<4)|0xF , example);
            sw.direct(23,c+1,"  ");
         }

         sw.direct(1,col+1,0xF,"<<");
         sw.direct(23,col+1,0xF,">>");

         KEY k = KB.get();

         switch(k)
         {
            case KEY_UP: if(col == 0)
                            col = 8;
                         col--;
                         break;
            case KEY_DN: if(++col > 7)
                             col = 0;
                         break;

            case KEY_RET: *(byte *)data = col;
            case KEY_ESC: return 0;
         }
      }
   }

   w.direct(x,y,attr,"Press [Enter] to change");

   w.direct(x+25,y,(col << 4)|0xF,example);

   return 0;
}


/*
 * Routine   : editDisplay()
 * Purpose   : edit display parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editDisplay()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_YESNO       ,&cfg.egamode           ,0          ,0          ,0          ,4       ,5       ,23   ,2    },
      { FRM_FUNCTION    ,&cfg.promptColor       ,0          ,edit_bgcol ,0          ,20      ,20      ,23   ,4    }
   };

   FormWindow w( 4, 
                 14, 
                 76, 
                 20, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "Display Options", 0x3E );

   w << "\n"
        " Use 43/50 Line Mode:\n"
        "\n"
        "   Prompt Background:";

   w.define( frm, 
             2, 
             0x31, 
             0x4E, 
             NULL, 
             sysDisplayHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] Site info editor [------*/

/*
 * Routine   : sysSiteHelp()
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysSiteHelp(IntT i)
{
   ChrP txt[]=
   {
      "Name of your BBS",
      "Location of your BBS (City)",
      "Primary phone number of your BBS",
      "Name of the sysop",
      "Default origin line to be used in echomail areas where no origin line is given",
      "Number of nodes on your system"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   : editSite()
 * Purpose   : edit site info
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editSite()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_STRING      ,cfg.bbsname            ,0          ,0          ,0          ,35      ,36      ,24   ,2    },
      { FRM_STRING      ,cfg.location           ,0          ,0          ,0          ,35      ,36      ,24   ,3    },
      { FRM_STRING      ,cfg.phone              ,0          ,0          ,0          ,25      ,26      ,24   ,4    },
      { FRM_STRING      ,cfg.sysopname          ,0          ,0          ,0          ,35      ,36      ,24   ,6    },
      { FRM_STRING      ,cfg.originline         ,0          ,0          ,0          ,60      ,47      ,24   ,8    },
      { FRM_INT         ,&cfg.numnodes          ,0          ,0          ,0          ,3       ,4       ,24   ,10   },
   };

   FormWindow w( 3,
                 10,
                 77,
                 22, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "Site Info", 0x3E );

   w << "\n"
        "          System Name:\n"
        "             Location:\n"
        "         Phone Number:\n"
        "\n"
        "           Sysop Name:\n"
        "\n"
        "  Default Origin Line:\n"
        "\n"
        "      Number of nodes:";

   w.define( frm, 
             6, 
             0x31,
             0x4E,
             NULL, 
             sysSiteHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}

/*--] Code [-------------------------------------] QWK editor [------------*/

/*
 * Routine   : sysQwkHelp
 * Purpose   : display help lines
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void sysQwkHelp(IntT i)
{
   ChrP txt[]=
   {
      "File name for QWK packets (BbsId)",
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}

/*
 * Routine   : editQwk()
 * Purpose   : edit the qwk parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

LCL void editQwk()
{
   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------------  */
      { FRM_STRING      ,cfg.QWKid              ,0          ,0          ,FRM_UPPER  ,8       ,9       ,18   ,2    }              ,
   };

   FormWindow w( 5, 
                 8, 
                 54, 
                 12, 
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.open();

   w.title( "QWK Options", 0x3E );

   w << "\n QWK File Name :";

   w.define( frm, 
             1, 
             0x31,
             0x4E, 
             NULL, 
             sysQwkHelp );

   tsw_cursoron();
   w.process();
   tsw_cursoroff();
}


/*--] Code [-------------------------------------] main menu [-------------*/

/*
 * Routine   : editSystemParam()
 * Purpose   : edit system parameters
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

void editSysParam()
{
   menu_item menuitems[]=
   {
      { 'P', 2 ,2, " Paths"           },
      { 'N', 2 ,2, " New Users"       },
      { 'S', 2 ,2, " Security"        },
      { 'Y', 2 ,2, " Yelling"         },
      { 'O', 9 ,9, " System Options"  },
      { 'F', 2 ,2, " File Transfer"   },
      { 'D', 2 ,2, " Display Options" },
      { 'I', 7 ,7, " Site Info"       },
      { 'Q', 2 ,2, " QWK Options"     },
      { 'L', 7 ,7, " File List Format"},
      { 0  , 0 ,0, NULL               }
   };


   PopMenu menu( menuitems, 
                 0x1E,        // setFBColor( nBlack,  nWhite ), 
                 0x3E );      // setFBColor( hYellow, nMagenta ) );

   menu.open( 45, 
              5, 
              66, 
              16, 
              0x30,
              SHADOW | BRACKETS,
              CHISEL_BORDER,
              0x3F,
              NULL,
              0x38 );

   //menu.border(setFBColor(hYellow, nMagenta));
   
   menu.title( "Options", 0x30 );

   for(;;)
   {
      tsw_showfooter( "Choose a System Option", BAR_COLOR);
      int choice = menu.process();

      if(choice<1)
         break;

      switch(choice)
      {
         case 1: editPaths    (); break;
         case 2: editUserParam(); break;
         case 3: editSecurity (); break;
         case 4: editYell     (); break;
         case 5: editGeneral  (); break;
         case 6: editTransfer (); break;
         case 7: editDisplay  (); break;
         case 8: editSite     (); break;
         case 9: editQwk      (); break;
         case 10:EditListColors();break;
      }
   }

}

/*---------------------------] END OF THE CODE [---------------------------*/
