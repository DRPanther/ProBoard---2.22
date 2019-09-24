
/* ------------------------------------------------------------
 * Filename ............... EditUser.Cpp
 *
 * General Purpose ........ Edit user records
 * ------------------------------------------------------------
 * First date ............. 6 november 1993
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

#include <stdlib.h>
#include <string.h>
#include <TsWin.Hpp>

#define Uses_PbUsersIdx
#define Uses_PbUsersPb
#define Uses_PbUsersBbs
#include "ThProCfg.Hpp"
#include "Template.Hpp"

/*--] External variables [-------------------------------------------------*/

ChrT        usrFind[80];
LngP        userList      = NULL;
UShrtT      userListCount = 0;
UShrtT      NumUsers      = 0;

/*--] Global variables [---------------------------------------------------*/

LCL PbUsersBbsP usrbbs;
LCL userFilter  filter;

/*--] Code [-------------------------------------] Linked List functions [-*/

/*
 * Routine   : clearUserList()
 * Purpose   : create a linked list with all user records
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

void clearUserList()
{
   for(ShrtT cnt = 0; cnt < NumUsers; cnt++)
      userList[cnt] = LngT(cnt);
}

/*
 * Routine   : fillUserList()
 * Purpose   : fill the linked list with filtered users
 * ------------------------------------------------------------------------
 * Parameters: filter record
 * Return    : number of users in the linked list
 *
 */

ShrtT fillUserList(userFilter& filter)
{
   ShrtT ret_val = 0;

   if(userList != NULL)
      delete userList;

   userList = new LngT[NumUsers];
   memset(userList, 0, NumUsers * SIZ(LngT));

   if(    (filter.name[0]     == 0           )
       && (filter.location[0] == 0           )
       && (filter.flagsOn     == 0           )
       && (filter.flagsOff    == 0           )
       && (filter.lowerLevel  == 0           )
       && (filter.higherLevel == WrdT(65535L))
     )
   {
      clearUserList();
      ret_val = NumUsers;
   }
   else
   {
      Window w;

      PbFlags flagsOn  = "";
      PbFlags flagsOff = "ABCDEFGHIJKMLNOPQRSTUVWXYZ123456";

      w.open(11, 10, 70, 12, setFBColor(hWhite, nCyan), SHADOW);
      w.title(" Searching (Press <Esc> to cancel)", setFBColor(hYellow, nCyan));
      w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, 0, NumUsers - 1, TRUE);
      tsw_cursoroff();

      if(filter.flagsOn != 0)
      {
         for(ShrtT cnt = 1; cnt <= 32; cnt++)
         {
            if(filter.flagsOn.get(ShrtT(cnt)) == TRUE)
               flagsOn.set(ShrtT(33 - cnt));
         }
      }

      if(filter.flagsOff != 0)
      {
         for(ShrtT cnt = 1; cnt <= 32; cnt++)
         {
            if(filter.flagsOff.get(ShrtT(cnt)) == TRUE)
               flagsOff.reset(ShrtT(33 - cnt));
         }
      }

      for(ShrtT cnt = 0; cnt < NumUsers; cnt++)
      {
         w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, cnt, NumUsers - 1, FALSE);

         if(KB.hit() && KB.get() == KEY_ESC)
         {
            clearUserList();
            break;
         }

         if(usrbbs->read(cnt) == TRUE)
         {
            FlgT addRecord = TRUE;

            if((addRecord == TRUE) && (filter.name[0] != 0))
            {
               pas2c(usrbbs->Name);
               String name(usrbbs->Name);
               if(name.fuzzySearch(filter.name, cfg.fuzzyRate) < cfg.fuzzyRate)
                  addRecord = FALSE;
            }

            if((addRecord == TRUE) && (filter.location[0] != 0))
            {
               pas2c(usrbbs->Location);
               String city(usrbbs->Location);
               if(city.fuzzySearch(filter.location, cfg.fuzzyRate) < cfg.fuzzyRate)
                  addRecord = FALSE;
            }

            if((addRecord == TRUE) && (filter.flagsOn != 0))
            {
               if((usrbbs->Flags & flagsOn) != flagsOn)
                  addRecord = FALSE;
            }

            if((addRecord == TRUE) && (filter.flagsOff != 0))
            {
               if(usrbbs->Flags != (usrbbs->Flags & flagsOff))
                  addRecord = FALSE;
            }

            if((addRecord == TRUE) && (filter.lowerLevel != 0))
            {
               if(usrbbs->Security < filter.lowerLevel)
                  addRecord = FALSE;
            }

            if((addRecord == TRUE) && (filter.higherLevel != WrdT(65535L)))
            {
               if(usrbbs->Security > filter.higherLevel)
                  addRecord = FALSE;
            }

            if(addRecord == TRUE)
               userList[ret_val++] = cnt;
         }
      }
   }

   userListCount = ret_val;
   return(ret_val);
}

/*
 * Routine   : getDiskRecNo()
 * Purpose   : return the disk record number for a record in the linked list
 * ------------------------------------------------------------------------
 * Parameters: linked list record number
 * Return    : disk record number or -1 if not in there's a problem
 *
 */

ShrtT getDiskRecNo(ShrtT recno)
{
   ShrtT ret_val = -1;

   if(recno < userListCount)
      ret_val = ShrtT(userList[recno]);

   return(ret_val);
}

/*
 * Routine   : getListRecNo()
 * Purpose   : return the list record number for a record number on disk
 * ------------------------------------------------------------------------
 * Parameters: disk record number
 * Return    : list record number or -1 if not in the list
 *
 */

ShrtT getListRecNo(ShrtT recno)
{
   ShrtT ret_val = -1;

   for(ShrtT cnt = 0; cnt < userListCount; cnt++)
   {
      if(ShrtT(userList[cnt]) == recno)
         ret_val = cnt;
   }

   return(ret_val);
}

/*
 * Routine   : filterUser();
 * Purpose   : filter user records
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : new list length
 *
 */

ShrtT filterUser()
{
   ShrtT ret_val = userListCount;

   userFilterT filter;

   if(askYesNo("Enable filter?") == TRUE)
   {
      Field frm[]=
      {
         /* TYPE           dataPtr                 choices     hookFunc       flags       len      width    x     y    */
         /* ----------------------------------------------------------------------------------------------------------- */
         {  FRM_STRING     ,&filter.name           ,0          ,0             ,0          ,35      ,36      ,19   ,1   },
         {  FRM_STRING     ,&filter.location       ,0          ,0             ,0          ,25      ,26      ,19   ,2   },
         {  FRM_FUNCTION   ,&filter.flagsOn        ,0          ,flag_select   ,0          ,32      ,33      ,19   ,3   },
         {  FRM_FUNCTION   ,&filter.flagsOff       ,0          ,flag_select   ,0          ,32      ,33      ,19   ,4   },
         {  FRM_UNSIGNED   ,&filter.lowerLevel     ,0          ,0             ,0          ,5       ,6       ,19   ,5   },
         {  FRM_UNSIGNED   ,&filter.higherLevel    ,0          ,0             ,0          ,5       ,6       ,19   ,6   }
      };

      FormWindow w(11,  6, 69, 13, setFBColor(hWhite, nRed), SHADOW);
      w.define(frm, 6, setFBColor(hYellow, nRed));
      w.open();
      w.title(" Filter ", setFBColor(hYellow, nRed));

      w <<  "\n"
            " Name ..........:\n"
            " Location ......:\n"
            " Flags On ......:\n"
            " Flags Off .....:\n"
            " Minimum level .:\n"
            " Maximum level .:";

      w.process();

      if(askYesNo("Use this filter?") == TRUE)
         ret_val = fillUserList(filter);
   }
   else
   {
      userFilterT empty;
      ret_val = fillUserList(empty);
   }

   return(ret_val);
}

/*--] Code [-------------------------------------] Search functions [------*/

/*
 * Routine   : searchUser()
 * Purpose   : search the whole userbase for a selected user
 * ------------------------------------------------------------------------
 * Parameters: record number to start on (-1 for 1st search)
 * Return    : record number or -1 if not found
 *
 */

ShrtT searchUser(ShrtT startRec)
{
   Window w;

   ShrtT max_rec = userListCount;

   if(max_rec < 2)
      return(-1);

   if(startRec == -1)
   {
      w.open(10,  7, 60,  9, setFBColor(hWhite, nCyan), SHADOW);
      CLEAR_OBJECT(usrFind);

      w << " Search for : ";

      tsw_cursoron();
      w.attrib(setFBColor(hYellow, nCyan));
      if(w.scan(usrFind, 35, 32, SCAN_UPPER) == SF_ESC)
         return(-1);

      w.close();
   }

   tsw_cursoroff();

   if(strlen(usrFind) == 0)
      return(-1);


   if(startRec + 1 >= max_rec)
      return(-1);

   w.open(11, 10, 70, 12, setFBColor(hWhite, nCyan), SHADOW);
   w.title(" Searching (Press <Esc> to cancel)", setFBColor(hYellow, nCyan));

   DwdT nameCrc = raCRC(usrFind);
   ShrtT found = -1;

   if(startRec == -1)                  // only for the 1st search...
   {
      // search the index file
      w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, 0, NumUsers-1, TRUE);
      PbUsersIdxP usridx = new PbUsersIdxT(ChrP(cfg.msgpath));
      if(usridx->open(fmode_read) == TRUE)
      {
         for(ShrtT cnt = startRec + 1; cnt < max_rec && found == -1; cnt++)
         {
            w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, cnt, NumUsers - 1, FALSE);

            if(KB.hit() && KB.get() == KEY_ESC)
               break;

            usridx->read(getDiskRecNo(cnt));
            if(usridx->nameCrc == nameCrc)
               found = cnt;
            if(usridx->handleCrc == nameCrc)
               found = cnt;
         }
      }

      delete(usridx);
   }

   if(found == -1)
   {
      // search the userbase file
      w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, 0, NumUsers-1, TRUE);
      PbUsersPbP usrpb = new PbUsersPbT(ChrP(cfg.msgpath));
      if(usrpb->open(fmode_read) == TRUE)
      {
         for(ShrtT cnt = startRec + 1; cnt < max_rec && found == -1; cnt++)
         {
            w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, cnt, NumUsers - 1, FALSE);

            if(KB.hit() && KB.get() == KEY_ESC)
               break;

            usrpb->read(getDiskRecNo(cnt));

            String name(usrpb->name);
            if(name.fuzzySearch(usrFind, cfg.fuzzyRate) >= cfg.fuzzyRate)
               found = cnt;
         }
      }

      delete usrpb;
   }

   return(found);
}

/*--] Code [-------------------------------------] User pick functions [---*/

/*
 * Routine   : listUser()
 * Purpose   : list the specified user
 * ------------------------------------------------------------------------
 * Parameters: user number, X and Y coordinates
 * Return    : None
 *
 */

void listUser(IntT recno, IntT x, IntT y)
{
   ShrtT recNum = getDiskRecNo(recno);
   if(recNum != -1)
   {
      if(usrbbs->read(recNum) == TRUE)
      {
         pas2c(usrbbs->Name);
         pas2c(usrbbs->Location);

         tsw_maputs(x, y, setFBColor(hWhite, nBlue), form(" %c ³ %5d º %5u ³ %-30.30s ³ %-20.20s",
                                                            ((usrbbs->uFlags & UFLAG_DELETED) ? 'þ' : ' '),
                                                            recNum + 1 , usrbbs->Security, usrbbs->Name, usrbbs->Location
                                                         )
                   );
      }
      else
         tsw_beep();
   }
}

/*
 * Routine   : pickUser
 * Purpose   : Let the user pick another userrecord
 * ------------------------------------------------------------------------
 * Parameters: current record number
 * Return    : selected record number or -1 to cancel
 *
 */

ShrtT pickUser(ShrtT curRecNo)
{
   ShrtT ret_val = -1;

   LCL ChrP pickHelp[] =
   {
      " Page Up ............: Go to previous page",
      " Page Down ..........: Go to next page",
      " Home ...............: Go to first record",
      " End ................: Go to last record",
      " Escape .............: End",
      "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
      " 1..9 ...............: Start typing record number to go to",
      " A..Z ...............: Start typing record number to search",
      " ALT-F ..............: Filter user records",
      " ALT-G ..............: Go to another record",
      " ALT-S ..............: Search record by user's name",
      " ALT-N ..............: Search next record",
      NULL
   };

   usrbbs = new PbUsersBbsT(ChrP(cfg.msgpath));

   if(usrbbs->open(fmode_shared) == FALSE)
   {
      tsw_beep();
   }
   else
   {
      KEY hotkeys[]=
      {
         '1','2','3','4','5','6','7','8','9',   // GOTO
         'A','B','C','D','E','F','G','H','I',   // SEARCH
         'J','K','L','M','N','O','P','Q','R',
         'S','T','U','V','W','X','Y','Z',
         KEY_ALTF,                              // FILTER
         KEY_ALTG,                              // GOTO
         KEY_ALTN,                              // SEARCH NEXT
         KEY_ALTS,                              // SEARCH
         KEY_F1  ,                              // HELP
         NULL
      };

      SelectWindow sw;
      sw.open( 2, 2, 79, tsw_vsize - 2, setFBColor(hYellow, nBlue), SHADOW);
      sw.title(" Select User ", setFBColor(hWhite, nBlue));

      sw.define((userList == NULL) ? NumUsers : userListCount, setFBColor(nBlack, nWhite), listUser, 0, NULL, hotkeys);

      sw.direct( 4,0            ,'Ñ'); sw.direct(12,0            ,'Ë');
      sw.direct(20,0            ,'Ñ'); sw.direct(53,0            ,'Ñ');

      sw.direct( 4,tsw_vsize - 5,'Ï'); sw.direct(12,tsw_vsize - 5,'Ê');
      sw.direct(20,tsw_vsize - 5,'Ï'); sw.direct(53,tsw_vsize - 5,'Ï');

      tsw_cursoroff();

      sw.show(curRecNo);

      FlgT pickLoop = TRUE;
      while(pickLoop)
      {
         tsw_showfooter("Press \001F1\x1 to see the helpscreen", BAR_NORM, BAR_HIGH );

         ShrtT choice = sw.process();
         switch(choice)
         {
            case SL_ESC:
            {
               pickLoop = FALSE;
            }
            break;

            case SL_HOTKEY:
            {
               switch(sw.hotkey)
               {
                  case KEY_F1    :
                  {
                     showHelp(pickHelp, "Pick a User Help" );
                  }
                  break;

                  case KEY_ALTF:
                  {
                     ShrtT curLen = userListCount;
                     ShrtT newLen = filterUser();
                     if(curLen != newLen)
                     {
                        sw.redefine(newLen);
                        sw.show(0);
                     }
                  }
                  break;

                  case '1': case '2': case '3':
                  case '4': case '5': case '6':
                  case '7': case '8': case '9':
                  {
                     KB.push((KEY)(char)sw.hotkey);
                  }                                   // NO BREAK!!!

                  case KEY_ALTG:
                  {
                     IntT userNo = 1;

                     Window w(10,  5, 32, 7, setFBColor(hWhite, nCyan));
                     w.open();
                     w << " Goto user: ";

                     tsw_cursoron();
                     w.attrib(setFBColor(hYellow, nCyan));

                     if(w.scan(userNo, 5, 6) == SF_ESC)
                        userNo = -1;

                     tsw_cursoroff();

                     if((userNo <= NumUsers) && (userNo > 0) )
                     {

                        choice = getListRecNo(userNo - 1);
                        if(choice != -1)
                           sw.show(choice);
                     }
                  }
                  break;

                  case KEY_ALTN:
                  {
                     ShrtT userNo = searchUser(sw.current);
                     if(userNo != -1)
                     {
                        sw.show(userNo);
                     }
                     else
                        tsw_beep();
                  }
                  break;


                  case 'A': case 'B': case 'C': case 'D':
                  case 'E': case 'F': case 'G': case 'H':
                  case 'I': case 'J': case 'K': case 'L':
                  case 'M': case 'N': case 'O': case 'P':
                  case 'Q': case 'R': case 'S': case 'T':
                  case 'U': case 'V': case 'W': case 'X':
                  case 'Y': case 'Z':
                  {
                     KB.push((KEY)(char)sw.hotkey);
                  }

                  case KEY_ALTS:
                  {
                     ShrtT userNo = searchUser(-1);
                     if(userNo != -1)
                     {
                        sw.show(userNo);
                     }
                     else
                        tsw_beep();
                  }
                  break;
               }
            }
            break;

            default:
            {
               ret_val = choice;
               pickLoop = FALSE;
            }
            break;
         }
      }
   }

   delete(usrbbs);

   return(ret_val);
}

/*--] Code [-------------------------------------] ]-----------------------*/

/*
 * Routine   : helpFunction()
 * Purpose   : show online help for each field
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void helpFunction(IntT helpNo)
{
   // ----------------------------------------------------------------------
   // Philippe, het is de bedoeling dat hier uitleg per veld in de
   // userfiche komt. Omdat jouw engels veel beter is, denk ik dat jij dat
   // best doet. Hou aub wel mijn source een beetje in ere he...
   // ----------------------------------------------------------------------

   ChrP txt[]=
   {
      "",
      "",
      "Press [ENTER] to edit/view password",
      "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
      "", "",
   };

   if(strlen(txt[helpNo]) != 0)
      tsw_showfooter(txt[helpNo], BAR_COLOR);
   else
      tsw_showfooter("Press \001F1\x1 to see help screen", BAR_NORM, BAR_HIGH);
}

/*
 * Routine   : editUser()
 * Purpose   : main edit function
 * ------------------------------------------------------------------------
 * Parameters: command line parameters
 * Return    : None
 *
 */

void editUser(char *sysPath, int argc, char *argv[])
{
   User cmpuser;
   User::setDir(cfg.msgpath);
   NumUsers = ShrtT(User::numUsers());

   fillUserList(filter);
   CLEAR_OBJECT(usrFind);

   LCL ChrP editHelp[] =
   {
      "",
      "              5EPage Up: 5BGo to previous record",
      "            5EPage Down: 5BGo to next record",
      "         5ECTRL-Page Up: 5BGo to first record",
      "       5ECTRL-Page Down: 5BGo to last record",
      "               5EEscape: 5BEnd",
      "50ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
      "                5EALT-A: 5BAdd a record",
//    "                5EALT-C: 5BGlobal changes on all records (or FilterList)",
      "                5EALT-D: 5B(Un)Delete current user",
      "                5EALT-F: 5BShow flag descriptions",
      "                5EALT-L: 5BList user records",
      "                5EALT-N: 5BSearch next record",
      "                5EALT-P: 5B(Un)Hide password",
//    "                5EALT-Q: 5BShow questionnaire answers",
      "                5EALT-R: 5BRestore user record",
      "                5EALT-S: 5BSearch record by user name",
      "                5EALT-V: 5BValidate user",
//    "                5EALT-W: 5BWrite a message to the current user",
      "                  5EF10: 5BEdit statistics",
      "",
      NULL
   };

   LCL ChrP logLevels[] =
   {
      "Friend" , "Normal" , "Suspicious" , "Extensive" ,
      NULL
   };

   /* ----------------------------------------------------------------------
   LCL ChrP YesNoAsk[] =
   {
      "No" , "Yes" , "Ask" ,
      NULL
   };
   ---------------------------------------------------------------------- */

   LCL ChrP sexes[] =
   {
      "Unknown" , "Male" , "Female" ,
      NULL
   };

   LCL ChrP terminals[] =
   {
      "TTY" , "ANSI" , "Avatar" , "Avt/0+",
      NULL
   };

   LCL ChrP dateFormats[] =
   {
      "MM/DD/YYYY", 
      "YYYY/MM/DD", 
      "DD/MM/YYYY",
      NULL
   };

   User user;

   LCL Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y     */
      /* ------------------------------------------------------------------------------------------------------- */
      { FRM_STRING      , user.name             ,0          ,0          ,0          ,35      ,32      ,14   ,1    },
      { FRM_STRING      , user.alias            ,0          ,0          ,0          ,35      ,32      ,14   ,2    },
      { FRM_FUNCTION    , &user                 ,0          ,enterPwd   ,0          ,15      ,16      ,14   ,3    },
      { FRM_STRING      , user.city             ,0          ,0          ,0          ,25      ,26      ,14   ,4    },
      { FRM_STRING      , user.state            ,0          ,0          ,0          ,25      ,26      ,14   ,5    },
      { FRM_STRING      , user.country          ,0          ,0          ,0          ,25      ,26      ,14   ,6    },
      { FRM_STRING      , user.address1         ,0          ,0          ,0          ,50      ,32      ,14   ,7    },
      { FRM_STRING      , user.address2         ,0          ,0          ,0          ,50      ,32      ,14   ,8    },
      { FRM_STRING      , user.address3         ,0          ,0          ,0          ,50      ,32      ,14   ,9    },
      { FRM_STRING      , user.forwardTo        ,0          ,0          ,0          ,35      ,32      ,14   ,10   },
      { FRM_UNSIGNED    ,&user.level            ,0          ,0          ,0          ,5       ,6       ,14   ,11   },
      { FRM_FUNCTION    ,&user.aFlags           ,0          ,flag_select,0          ,32      ,32      ,14   ,12   },
      { FRM_DATE        ,&user.expDate          ,0          ,0          ,0          ,10      ,10      ,14   ,13   },
      { FRM_UNSIGNED    ,&user.expLevel         ,0          ,0          ,0          ,5       ,6       ,14   ,14   },
      { FRM_FUNCTION    ,&user.expFlagsOn       ,0          ,flag_select,0          ,5       ,6       ,14   ,15   },
      { FRM_FUNCTION    ,&user.expFlagsOff      ,0          ,flag_select,0          ,5       ,6       ,14   ,16   },
      { FRM_STRING      , user.comment          ,0          ,0          ,0          ,79      ,65      ,11   ,18   },
      { FRM_STRING      , user.voicePhone       ,0          ,0          ,FRM_UPPER  ,15      ,16      ,60   ,1    },
      { FRM_STRING      , user.dataPhone        ,0          ,0          ,FRM_UPPER  ,15      ,16      ,60   ,2    },
      { FRM_STRING      , user.faxPhone         ,0          ,0          ,FRM_UPPER  ,15      ,16      ,60   ,3    },
      { FRM_DATE        ,&user.birthDate        ,0          ,0          ,0          ,10      ,10      ,60   ,4    },
      { FRM_CHOICE      ,&user.sex              ,sexes      ,0          ,0          ,7       ,7       ,60   ,5    },
      { FRM_CHOICE      ,&user.dateFormat       ,dateFormats,0          ,0          ,10      ,10      ,60   ,6    },
      { FRM_UNSIGNED    ,&user.screenLength     ,0          ,0          ,0          ,4       ,5       ,60   ,7    },
      { FRM_CHAR        ,&user.defaultProtocol  ,0          ,0          ,FRM_UPPER  ,1       ,1       ,60   ,8    },
      { FRM_STRING      , user.language         ,0          ,0          ,FRM_UPPER  ,8       ,9       ,60   ,9    },
      { FRM_CHOICE      ,&user.logLevel         ,logLevels  ,0          ,0          ,10      ,10      ,60   ,10   },
      { FRM_CHOICE      ,0                      ,terminals  ,0          ,0          ,6       ,6       ,60   ,11   },
      { FRM_LONG        ,&user.credit           ,0          ,0          ,0          ,7       ,8       ,60   ,12   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,11   ,20   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,11   ,21   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,11   ,22   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,26   ,20   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,26   ,21   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,26   ,22   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,42   ,20   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,42   ,21   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,42   ,22   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,57   ,20   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,57   ,21   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,57   ,22   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,73   ,20   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,73   ,21   },
      { FRM_YESNO       ,0                      ,0          ,0          ,0          ,3       ,3       ,73   ,22   }
   };

   FormWindow w( 1,  
                 1, 
                 80, 
                 24, 
                 0x70,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x7F,
                 NULL,
                 0x78 );

   w.open();

   w <<  "  User Name:                                   VoicePhone: \n"
         "      Alias:                                    DataPhone: \n"
         "   Password:                                     FaxPhone: \n"
         "       City:                                   Birth Date: \n"
         "      State:                                          Sex: \n"
         "    Country:                                     Date Fmt: \n"
         "  Address 1:                                   Scr.Height: \n"
         "  Address 2:                                      DefProt: \n"
         "  Address 3:                                     Language: \n"
         "  ForwardTo:                                    Log Level: \n"
         "      Level:                                     Terminal: \n"
         "      Flags:                                   NetmCredit: \n"
         "  Exp. Date:                                    \n"
         " Exp. Level:                                   First Call: \n"
         "  ExFlagsOn:                                   Last  Call: \n"
         " ExFlagsOff:\n"
         "\n"
         " Comment:\n"
         "\n"
         " Deleted:       HotKeys:         NoKill:        InTops:       FreeChat:   \n"
         "    More:           IBM:       IgnoreDL:        Hidden:        LocOnly:   \n"
         "  ClrScr:          FsEd:          Atten:         Guest:            RIP:   ";

   KEY hotkeys[]=
   {
      KEY_PGUP ,                          // next
      KEY_PGDN ,                          // previous
      KEY_CPGUP,                          // first
      KEY_CPGDN,                          // last
      KEY_ALTA ,                          // add
//    KEY_ALTC ,                          // global change
      KEY_ALTD ,                          // delete / undelete
      KEY_ALTF ,                          // show flags description
      KEY_ALTL ,                          // list
      KEY_ALTN ,                          // search next
      KEY_ALTP ,                          // hide password
      KEY_ALTR ,                          // restore record
      KEY_ALTS ,                          // search
      KEY_ALTV ,                          // validate
//    KEY_ALTW ,                          // write message
      KEY_F10  ,                          // edit stat
      KEY_F1   ,                          // help
      NULL
   };

   w.define( frm, 
             44, 
             0x71,
             0x1E,
             hotkeys, 
             helpFunction );

   ShrtT curRecNo = 0;

   if(argc > 0)
      curRecNo = atoi(argv[0]) - 1;

   if(curRecNo < 0)
      curRecNo = 0;

   ShrtT nextRecNo    = curRecNo;
   FlgT  readRequired = TRUE;

   user.newUser();
   tsw_cursoron();

   FlgT editLoop = TRUE;
   while(editLoop)
   {
      FlgT  forcedSave   = FALSE;

      if(curRecNo != nextRecNo)
      {
         curRecNo = nextRecNo;
         readRequired = TRUE;
      }

      if(readRequired == TRUE)
      {
         readRequired = FALSE;

         if(curRecNo > NumUsers)
            curRecNo = 0;

         user.read(getDiskRecNo(curRecNo));
         memcpy(&cmpuser, &user, sizeof(user));
      }

      w.title( form( "Record %d/%d", 
                     getDiskRecNo( curRecNo )  +  1, 
                     NumUsers ),
                     
               0x70 );

      if(user.firstDate.ok())
         w.direct(60, 14, 0x74, user.firstDate.format("DD-MMM-CCYY"));
      else
         w.direct(60, 14, 0x74, "??-???-????");

      if(user.lastDate.ok())
         w.direct(60, 15, 0x74, user.lastDate.format("DD-MMM-CCYY"));
      else
         w.direct(60, 15, 0x74, "??-???-????");

      Date today(TODAY);
      ShrtT age = Date(TODAY).age(user.birthDate);


      if(age > 0)
         w.direct(71, 4, 0x74, form("(%3d)", age));
      else
         w.direct(71, 4, 0x74, "(???)");

      FlgT DELETED    = !!(user.uFlags & UFLAG_DELETED);
      FlgT PAUSE      = !!(user.uFlags & UFLAG_PAUSE);
      FlgT CLEAR      = !!(user.uFlags & UFLAG_CLEAR);
      FlgT HOTKEYS    = !!(user.uFlags & UFLAG_HOTKEYS);
      FlgT NOIBM      =  !(user.uFlags & UFLAG_NOIBM);
      FlgT FSED       = !!(user.uFlags & UFLAG_FSED);
      FlgT NOKILL     = !!(user.uFlags & UFLAG_NOKILL);
      FlgT IGNORE     = !!(user.uFlags & UFLAG_IGNORE);
      FlgT ATTEN      = !!(user.uFlags & UFLAG_ATTEN);
      FlgT INTOPS     =  !(user.uFlags & UFLAG_NOTOPS);
      FlgT HIDDEN     = !!(user.uFlags & UFLAG_HIDDEN);
      //FlgT QUIET      = !!(user.uFlags & UFLAG_QUIET);
      FlgT GUEST      = !!(user.uFlags & UFLAG_GUEST);
      //FlgT PAGEPRI    = !!(user.uFlags & UFLAG_PAGEPRI);
      FlgT LOCALONLY  = !!(user.uFlags & UFLAG_LOCALONLY);
      //FlgT MULTILOGIN = !!(user.uFlags & UFLAG_MULTILOGIN);
      FlgT FREECHAT   = !!(user.uFlags & UFLAG_FREECHAT);
      FlgT RIP        =  !(user.uFlags & UFLAG_NORIP);

      frm[29].value = &DELETED   ;
      frm[30].value = &PAUSE     ;
      frm[31].value = &CLEAR     ;
      frm[32].value = &HOTKEYS   ;
      frm[33].value = &NOIBM     ;
      frm[34].value = &FSED      ;
      frm[35].value = &NOKILL    ;
      frm[36].value = &IGNORE    ;
      frm[37].value = &ATTEN     ;
      frm[38].value = &INTOPS    ;
      frm[39].value = &HIDDEN    ;
      frm[40].value = &GUEST     ;
      frm[41].value = &FREECHAT  ;
      frm[42].value = &LOCALONLY ;
      frm[43].value = &RIP       ;

      frm[12].data  = dateFormats[cfg.europe ? 2 : 0];
      frm[20].data  = dateFormats[cfg.europe ? 2 : 0];

      IntT terminal = 0;

      if(user.uFlags & UFLAG_AVTPLUS)
         terminal = 3;
      else
         if(user.uFlags & UFLAG_AVATAR)
            terminal = 2;
         else
            if(user.uFlags & UFLAG_ANSI)
              terminal = 1;
            else
               terminal = 0;

      frm[27].value = &terminal;

      IntT ret_code = w.process();

      if(user.sex > 2)
         user.sex = 0;  // :-)

      user.uFlags = 0;

      if(DELETED    ) user.uFlags |= UFLAG_DELETED;
      if(PAUSE      ) user.uFlags |= UFLAG_PAUSE;
      if(CLEAR      ) user.uFlags |= UFLAG_CLEAR;
      if(HOTKEYS    ) user.uFlags |= UFLAG_HOTKEYS;
      if(!NOIBM     ) user.uFlags |= UFLAG_NOIBM;
      if(FSED       ) user.uFlags |= UFLAG_FSED;
      if(NOKILL     ) user.uFlags |= UFLAG_NOKILL;
      if(IGNORE     ) user.uFlags |= UFLAG_IGNORE;
      if(ATTEN      ) user.uFlags |= UFLAG_ATTEN;
      if(!INTOPS    ) user.uFlags |= UFLAG_NOTOPS;
      if(HIDDEN     ) user.uFlags |= UFLAG_HIDDEN;
      //if(QUIET      ) user.uFlags |= UFLAG_QUIET;
      if(GUEST      ) user.uFlags |= UFLAG_GUEST;
      //if(PAGEPRI    ) user.uFlags |= UFLAG_PAGEPRI;
      if(LOCALONLY  ) user.uFlags |= UFLAG_LOCALONLY;
      //if(MULTILOGIN ) user.uFlags |= UFLAG_MULTILOGIN;
      if(FREECHAT   ) user.uFlags |= UFLAG_FREECHAT;
      if(!RIP       ) user.uFlags |= UFLAG_NORIP;

      switch(terminal)
      {
         case 1: user.uFlags |= UFLAG_ANSI;                             break;
         case 2: user.uFlags |= UFLAG_ANSI|UFLAG_AVATAR;                break;
         case 3: user.uFlags |= UFLAG_ANSI|UFLAG_AVATAR|UFLAG_AVTPLUS;  break;
      }

      switch(ret_code)
      {
         case ED_HOT:
         {
            switch(w.scanHotKey)
            {
               case KEY_F1    :
               {
                  showHelp( editHelp, "Edit User Help" );
                  continue;
               }

               case KEY_F10   :
               {
                  Field frm[]=
                  {
                     /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y    */
                     /* ------------------------------------------------------------------------------------------------------- */
                     {  FRM_LONG       ,&user.timesCalled      ,0          ,0          ,0          ,5       ,6       ,27   , 2  },
                     {  FRM_LONG       ,&user.numDownloads     ,0          ,0          ,0          ,5       ,6       ,27   , 4  },
                     {  FRM_LONG       ,&user.kbDownloaded     ,0          ,0          ,0          ,5       ,6       ,27   , 5  },
                     {  FRM_LONG       ,&user.numUploads       ,0          ,0          ,0          ,5       ,6       ,27   , 6  },
                     {  FRM_LONG       ,&user.kbUploaded       ,0          ,0          ,0          ,5       ,6       ,27   , 7  },
                     {  FRM_LONG       ,&user.msgsPosted       ,0          ,0          ,0          ,5       ,6       ,27   , 8  },
                     {  FRM_LONG       ,&user.totalTimeUsed    ,0          ,0          ,0          ,6       ,6       ,27   , 9  },
                     {  FRM_LONG       ,&user.timeUsed         ,0          ,0          ,0          ,5       ,6       ,27   ,11  },
                     {  FRM_LONG       ,&user.kbToday          ,0          ,0          ,0          ,5       ,6       ,27   ,12  },

                     {  FRM_INT        ,&user.tbTimeBalance    ,0          ,0          ,0          ,5       ,6       ,53   , 2  },
                     {  FRM_INT        ,&user.tbTimeWithdrawn  ,0          ,0          ,0          ,5       ,6       ,53   , 3  },
                     {  FRM_INT        ,&user.tbTimeDeposited  ,0          ,0          ,0          ,5       ,6       ,53   , 4  },
                     {  FRM_INT        ,&user.tbTimeLoaned     ,0          ,0          ,0          ,5       ,6       ,53   , 5  },
                     {  FRM_DATE       ,&user.tbTimePayback    ,0          ,0          ,0          ,10      ,10      ,53   , 6  },

                     {  FRM_INT        ,&user.tbKbBalance      ,0          ,0          ,0          ,5       ,6       ,53   , 8  },
                     {  FRM_INT        ,&user.tbKbWithdrawn    ,0          ,0          ,0          ,5       ,6       ,53   , 9  },
                     {  FRM_INT        ,&user.tbKbDeposited    ,0          ,0          ,0          ,5       ,6       ,53   ,10  },
                     {  FRM_INT        ,&user.tbKbLoaned       ,0          ,0          ,0          ,5       ,6       ,53   ,11  },
                     {  FRM_DATE       ,&user.tbKbPayback      ,0          ,0          ,0          ,10      ,10      ,53   ,12  },

                     {  FRM_DATE       ,&user.tbLastUsed       ,0          ,0          ,0          ,10      ,10      ,53   ,14  },
                  };

                  if(user.tbTimeLoaned == 0)
                  {
                     user.tbTimePayback.set(0, 0, 0);
                     cmpuser.tbTimePayback.set(0, 0, 0);
                  }

                  if(user.tbKbLoaned == 0)
                  {
                     user.tbKbPayback.set(0, 0, 0);
                     cmpuser.tbKbPayback.set(0, 0, 0);
                  }

                  frm[13].data  = dateFormats[cfg.europe ? 2 : 0];
                  frm[18].data  = dateFormats[cfg.europe ? 2 : 0];
                  frm[19].data  = dateFormats[cfg.europe ? 2 : 0];

                  FormWindow w( 7,  5, 72, 21, setFBColor(hWhite, nCyan), SHADOW);
                  w.define(frm, 20, setFBColor(hYellow, nCyan));
                  w.open();
                  w.title(" Statistics ", setFBColor(hYellow, nCyan));

                  w << "\n"
                       " Times called           :          Time Balance   :\n"
                       "                                        Withdrawn :\n"
                       " # Downloads ever       :               Deposited :\n"
                       " KB Downloaded ever     :               Borrowed  :\n"
                       " # Uploads ever         :               Payback   :\n"
                       " KB Uploaded ever       :\n"
                       " # Messages posted ever :         Kbyte Balance   :\n"
                       " Time online ever (min) :               Withdrawn :\n"
                       "                                        Deposited :\n"
                       " Time used today (min)  :               Borrowed  :\n"
                       " KB Downloaded today    :               Payback   :\n"
                       "\n"
                           "                               Last Used Timebank :";
                  w.process();

                  continue;
               }

               case KEY_ALTD  :
               {
                  if(!(user.uFlags & UFLAG_DELETED))
                     user.uFlags |= UFLAG_DELETED;
                  else
                     user.uFlags &= ~UFLAG_DELETED;

                  forcedSave = TRUE;
               }
               break;

               case KEY_ALTF  :
               {
                  tsw_cursoroff();
                  show_flags();
                  tsw_cursoron();
                  continue;
               }

               case KEY_ALTP  :
               {
                  cfg.hidePassword = (cfg.hidePassword ? FALSE : TRUE);
                  continue;
               }


               case KEY_ALTR  :
               {
                  if(memcmp(&user, &cmpuser, sizeof(user)) != 0)
                  {
                     if(askYesNo("Record changed. Restore old record?") == TRUE)
                        memcpy(&user, &cmpuser, sizeof(user));
                  }

                  continue;
               }

               case KEY_ALTV  :
               {
                  tpl = new TemplateIO(sysPath);
                  if(tpl->open(fmode_rw|fmode_copen) == TRUE)
                  {
                     ShrtT tplNo = pickTemplate();
                     if(tplNo != -1)
                     {
                        tpl->read(tplNo);
                        tsw_showfooter(form("Template: %s", ChrP(tpl->desc)), BAR_COLOR);

                        FlgT validate = TRUE;
                        if(cfg.valConfirm == TRUE)
                           validate = askYesNo("Validate user?");

                        if(validate == TRUE)
                        {
                           if(tpl->read(tplNo) == TRUE)
                           {
                              forcedSave = TRUE;

                              strcpy(user.comment,tpl->comment);

                              if(tpl->secLevel   != 0)      user.level          = tpl->secLevel ;
                              if(tpl->expLevel   != 0)      user.expLevel       = tpl->expLevel ;
                              if(tpl->tbTime     != 0)      user.tbTimeBalance += tpl->tbTime   ;
                              if(tpl->tbKbyte    != 0)      user.tbKbBalance   += tpl->tbTime   ;
                              if(tpl->netCredit  != 0)      user.credit        += tpl->netCredit;
                              if(tpl->logLevel   <= 3)      user.logLevel       = tpl->logLevel ;
                              if(tpl->hidden     == FALSE)  user.uFlags        &= ~UFLAG_HIDDEN ;
                              if(tpl->hidden     == TRUE )  user.uFlags        |=  UFLAG_HIDDEN ;
                              if(tpl->noTops     == FALSE)  user.uFlags        &= ~UFLAG_NOTOPS ;
                              if(tpl->noTops     == TRUE )  user.uFlags        |=  UFLAG_NOTOPS ;
                              if(tpl->atten      == FALSE)  user.uFlags        &= ~UFLAG_ATTEN  ;
                              if(tpl->atten      == TRUE )  user.uFlags        |=  UFLAG_ATTEN  ;
                              if(tpl->noKill     == FALSE)  user.uFlags        &= ~UFLAG_NOKILL ;
                              if(tpl->noKill     == TRUE )  user.uFlags        |=  UFLAG_NOKILL ;
                              if(tpl->ignDld     == FALSE)  user.uFlags        &= ~UFLAG_IGNORE ;
                              if(tpl->ignDld     == TRUE )  user.uFlags        |=  UFLAG_IGNORE ;

                              if(tpl->subscribe != 0)
                              {

                                 if((    (user.expDate[0] == 0)
                                     && (user.expDate[1] == 0)
                                     && (user.expDate[2] == 0)
                                    )
                                    ||
                                    ( user.expDate < Date(TODAY) )
                                   )
                                 {
                                    user.expDate.today();
                                 }

                                 Date expDate(user.expDate[0], user.expDate[1], user.expDate[2]);
                                 expDate += tpl->subscribe;
                                 user.expDate[0] = expDate.day  ();
                                 user.expDate[1] = expDate.month();
                                 user.expDate[2] = expDate.year ();
                              }
                              else
                              {
                                 if(    (user.expDate[0] != 0)
                                     || (user.expDate[1] != 0)
                                     || (user.expDate[2] != 0)
                                   )
                                 {
                                    if(askYesNo("Clear expiration date?") == TRUE)
                                    {
                                       user.expDate[0] = 0;
                                       user.expDate[1] = 0;
                                       user.expDate[2] = 0;
                                    }
                                 }
                              }

                              user.expFlagsOn  = tpl->expFlagsOn;
                              user.expFlagsOff = tpl->expFlagsOff;

                              PbFlags flg(user.aFlags);
                              flg.set  (tpl->valFlagsOn );
                              flg.reset(tpl->valFlagsOff);
                              user.aFlags = flg;

                              /* ----------------------------------------------------------------
                              if(strlen(tpl->valMsg) != 0)
                              {
                                 FlgT writeMsg = TRUE;
                                 if(cfg.valConfirm == TRUE)
                                    writeMsg = askYesNo("Write validation message?");

                                 if(writeMsg == TRUE)
                                 {
                                    FileName fn(ChrP(cfg.valMsgPath));
                                    fn << tpl->valMsg;

                                    String name(user.name);
                                    IntT pos = name.find(" ");
                                    if(pos != -1)
                                       name.del(pos, 0);

                                    String header("Hello ");
                                    header << name << "!";

                                    postFileMsg(   fn,
                                                   cfg.messageBoard,
                                                   ChrP(cfg.sysopname),
                                                   user.name,
                                                   cfg.valSubject,
                                                   TRUE,
                                                   header,
                                                   "",
                                                   ""
                                               );
                                 }
                              }

                              ---------------------------------------------------------------- */

                           }

                        }
                     }
                  }

                  delete tpl;
               }
               break;

               case KEY_ALTW  :
               {
               }
               break;

            }
         }
         break;
      }

      FlgT isChanged = FALSE;
      if((user.name[0] != 0) || (forcedSave == TRUE))
      {
         if(memcmp(&user, &cmpuser, sizeof(user)) != 0)
         {
            isChanged = TRUE;

            ShrtT stat;

            if(forcedSave == FALSE)
               stat = ask_save();
            else
               stat = 1;

            if(stat == 1)
            {
               user.write(FALSE);
               readRequired = TRUE;
            }
         }
      }

      ShrtT max_rec = userListCount;
      switch(ret_code)
      {
         case ED_HOT :
         {
            switch(w.scanHotKey)
            {
               case KEY_PGDN  :
               {
                  if(curRecNo + 1 < max_rec)
                     nextRecNo = curRecNo + 1;
               }
               break;

               case KEY_PGUP  :
               {
                  if(curRecNo > 0)
                     nextRecNo = curRecNo - 1;
               }
               break;

               case KEY_CPGUP :
               {
                  if(curRecNo != 0)
                     nextRecNo = 0;
               }
               break;

               case KEY_CPGDN :
               {
                  if(curRecNo + 1 < max_rec)
                     nextRecNo = max_rec - 1;
               }
               break;

               case KEY_ALTA  :
               {
                  memset(&cmpuser, 0, sizeof(cmpuser));
                  user.newUser();
                  user.firstDate.today();
                  user.write(TRUE);
                  nextRecNo = NumUsers++;

                  userFilter tempFilter;
                  fillUserList(tempFilter);
                  max_rec = userListCount;
               }
               break;

               case KEY_ALTC  :
               {
               }
               break;

               case KEY_ALTF  :
               {
               }
               break;

               case KEY_ALTL  :
               {
                  nextRecNo = pickUser(curRecNo);
                  if(nextRecNo == -1)
                  {
                     tsw_beep();
                     nextRecNo = curRecNo;
                  }
                  else
                     readRequired = TRUE;
               }
               break;

               case KEY_ALTN  :
               {
                  nextRecNo = searchUser(curRecNo);
                  if(nextRecNo == -1)
                  {
                     tsw_beep();
                     nextRecNo = curRecNo;
                  }
               }
               break;

               case KEY_ALTS:
               {
                  nextRecNo = searchUser(-1);
                  if(nextRecNo == -1)
                  {
                     tsw_beep();
                     nextRecNo = curRecNo;
                  }
               }
               break;
            }
         }
         break;

         case ED_ESC :
         {
            if((isChanged == FALSE) || (askYesNo("Exit the user editor?") == TRUE))
               editLoop = FALSE;
         }
         break;
      }
   }

   delete(userList);
   userList = NULL;

   tsw_cursoroff();
}

/*---------------------------] END OF THE CODE [---------------------------*/
