
/* ------------------------------------------------------------
 * Filename ............... EditVTpl.Cpp
 *
 * General Purpose ........ Edit validate templates
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

#include <string.h>

#define Uses_MultiRecIO
#include "ThProCfg.Hpp"
#include "Template.Hpp"

/*-------------------------------------------------------------------------*/

TemplateIOP tpl = NULL;
ChrT tplFind[80];

/*--] Code [-------------------------------------] search functions [------*/

/*
 * Routine   : searchTemplate()
 * Purpose   : search the whole template database for a selected template
 * ------------------------------------------------------------------------
 * Parameters: record number to start on (-1 for 1st search)
 * Return    : record number or -1 if not found
 *
 */

LCL ShrtT searchTemplate(ShrtT startRec)
{
   ShrtT max_rec = tpl->count();
   if(max_rec < 2)
      return(-1);

   Window w;

   if(startRec == -1)
   {
      w.open( 10,  
              7, 
              62,  
              9, 
              0x3F,
              SHADOW,
              CHISEL_BORDER,
              0x3B,
              NULL,
              0x31 );

      CLEAR_OBJECT(tplFind);

      w << " Search for: ";

      w.attrib( 0x3E );

      tsw_cursoron();
      if(w.scan(tplFind, 60, 35, SCAN_UPPER) == SF_ESC)
         return(-1);

      w.close();
   }

   tsw_cursoroff();

   if(strlen(tplFind) == 0)
      return(-1);


   if(startRec + 1 >= max_rec)
      return(-1);

   w.open( 11, 
           10, 
           70, 
           12, 
           0x3F,
           BRACKETS,
           CHISEL_BORDER,
           0x3B,
           NULL,
           0x31 );

   w.title( "Searching (Press <Esc> to cancel)", 0x3E );

   ShrtT found = -1;
   w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, 0, max_rec - 1, TRUE);
   for(ShrtT cnt = startRec + 1; cnt < max_rec && found == -1; cnt++)
   {
      w.gauge(  2,  1, setFBColor(hWhite, nBlack), 54, cnt, max_rec - 1, FALSE);

      if(KB.hit() && KB.get() == KEY_ESC)
         break;

      tpl->read(cnt);

      String desc(tpl->desc);
      if(desc.fuzzySearch(tplFind, cfg.fuzzyRate) >= cfg.fuzzyRate)
         found = cnt;
   }

   return(found);
}

/*--] Code [-------------------------------------] pick functions [--------*/

/*
 * Routine   : listTemplate()
 * Purpose   : list the specified template
 * ------------------------------------------------------------------------
 * Parameters: template number, X and Y coordinates
 * Return    : None
 *
 */

LCL void listTemplate(IntT recno, IntT x, IntT y)
{
   String txt = form("   ³ %5d ³ ............................................................", recno + 1);

   if(tpl->read(recno) == TRUE)
      if(tpl->inUse() == TRUE)
         txt = form(" %c ³ %5d ³ %s", (tpl->deleted ? 'þ' : ' '), recno + 1, tpl->desc);

   if(txt.len() > 72)
      txt.del(72, 0);

   tsw_maputs(x     , y, 0x3F, txt);
}

/*
 * Routine   : pickTemplate
 * Purpose   : Let the user pick another template
 * ------------------------------------------------------------------------
 * Parameters: current record number
 * Return    : selected record number or -1 to cancel
 *
 */

ShrtT pickTemplate(ShrtT curRecNo)
{
   ShrtT ret_val = -1;

   LCL ChrP pickHelp[] =
   {
      "       5EPage Up: 5BGo to previous page",
      "     5EPage Down: 5BGo to next page",
      "          5EHome: 5BGo to first record",
      "           5EEnd: 5BGo to last record",
      "        5EEscape: 5BEnd",
      "50ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
      "          5E1..9: 5BStart typing record number to go to",
      "          5EA..Z: 5BStart typing record number to search",
      "         5EALT-G: 5BGo to another record",
      "         5EALT-S: 5BSearch record by description",
      "         5EALT-N: 5BSearch next record",
      NULL
   };

   KEY hotkeys[]=
   {
      '1','2','3','4','5','6','7','8','9',   // GOTO
      'A','B','C','D','E','F','G','H','I',   // SEARCH
      'J','K','L','M','N','O','P','Q','R',
      'S','T','U','V','W','X','Y','Z',
      KEY_ALTG,                              // GOTO
      KEY_ALTN,                              // SEARCH NEXT
      KEY_ALTS,                              // SEARCH
      KEY_F1  ,                              // SHOW HELP
      NULL
   };

   ShrtT maxRec = tpl->count();

   SelectWindow sw;

   sw.open( 2, 
            3, 
            78, 
            tsw_vsize - 2, 
            0x3F,
            SHADOW | BRACKETS,
            CHISEL_BORDER,
            0x3B,
            NULL,
            0x31 );

   sw.title( "Select Template", 0x3E );

   sw.define( maxRec, 
              0x1E,
              listTemplate, 
              0, 
              NULL, 
              hotkeys );

   sw.direct( 4,0            ,0x3B, 'Â'); sw.direct(12,0            ,0x3B, 'Â');
   sw.direct( 4,tsw_vsize - 5, 0x31, 'Á'); sw.direct(12,tsw_vsize - 5,0x31, 'Á');

   sw.show(curRecNo);

   FlgT pickLoop = TRUE;
   while(pickLoop)
   {
      tsw_showfooter("Press [\001F1\001] to see the help screen", BAR_NORM, BAR_HIGH);

      tsw_cursoroff();
      ShrtT choice = sw.process();
      tsw_cursoron ();

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
                  showHelp( pickHelp, "Pick Template Help" );
               }
               break;

               case '1': case '2': case '3':
               case '4': case '5': case '6':
               case '7': case '8': case '9':
               {
                  KB.push((KEY)(char)sw.hotkey);
               }                                   // NO BREAK!!!

               case KEY_ALTG  :
               {
                  IntT tplNo = 1;

                  Window w(10,  5, 36, 7, setFBColor(hWhite, nCyan));
                  w.open();
                  w << " Goto template: ";

                  tsw_cursoron();
                  w.attrib(setFBColor(hYellow, nCyan));

                  if(w.scan(tplNo, 5, 6) == SF_ESC)
                     tplNo = -1;

                  tsw_cursoroff();

                  if((tplNo <= maxTemplates) && (tplNo >= 1) )
                  {
                     pickLoop = FALSE;
                     ret_val = tplNo - 1;
                     sw.show(ret_val);
                  }
               }
               break;

               case KEY_ALTN:
               {
                  ShrtT tplNo = searchTemplate(sw.current);
                  if(tplNo != -1)
                  {
                     sw.show(tplNo);
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
                  ShrtT tplNo = searchTemplate(-1);
                  if(tplNo != -1)
                  {
                     sw.show(tplNo);
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

   return(ret_val);
}

/*--] Code [-------------------------------------] Edit functions [--------*/

/*
 * Routine   : helpFunction()
 * Purpose   : show the online help for each field
 * ------------------------------------------------------------------------
 * Parameters: field number
 * Return    : None
 *
 */

LCL void helpFunction(IntT helpNo)
{
   ChrP txt[]=
   {
      "Description for this template.",
      "The security level the user will get when validated with this template.",
      "How many days he/she will get the new security level.",
      "The expiration level for this template.",
      "Deposit an amount of time in the user's time bank account.",
      "Deposit an amount of kbytes in the user's time bank account.",
      "The netmail credit a user can use.",

      "Name of the file containing the validation text (for TheUser).",

      "Flags to turn on when validating a user.",
      "Flags to turn off when validating a user.",
      "Flags to turn on when a user's level is expired.",
      "Flags to turn off when a user's level is expired.",
      "Comment to place in a user's record.",
      "The new log level for the user after validation.",
      "",
      "",
      "",
      "",
      "",
      "Template marked for deletion?",
   };

   if(strlen(txt[helpNo]) != 0)
      tsw_showfooter(txt[helpNo], BAR_COLOR);
   else
      tsw_showfooter("Press [\001F1\001] to see the help screen", BAR_NORM, BAR_HIGH);
}

/*
 * Routine   : editTemplate()
 * Purpose   : edit a selected template
 * ------------------------------------------------------------------------
 * Parameters: system path
 * Return    : None
 *
 */

void editTemplate(ChrP sysPath)
{
   LCL ChrP editHelp[] =
   {
      "        5EPage Up: 5BGo to previous record",
      "      5EPage Down: 5BGo to next record",
      "   5ECTRL-Page Up: 5BGo to first record",
      " 5ECTRL-Page Down: 5BGo to last record",
      "         5EEscape: 5BEnd",
      "50ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
      "          5EALT-A: 5BAdd a record",
      "          5EALT-D: 5BDelete / Undelete the current template",
      "          5EALT-F: 5BShow flag description",
      "          5EALT-L: 5BList all records",
      "          5EALT-S: 5BSearch record by description",
      "          5EALT-N: 5BSearch next record",
      NULL
   };

   CLEAR_OBJECT(tplFind);

   LCL ChrP LogLevels[] =
   {
      "Friend" , "Normal" , "Suspicious" , "Extensive" , "Ignore",
      NULL
   };

   LCL ChrP YesNo[] =
   {
      "No", "Yes",
      NULL
   };

   LCL ChrP YesNoIgnore[] =
   {
      "No", "Yes", "Ignore",
      NULL
   };

   tpl = new TemplateIO(sysPath);
   if(tpl->open(fmode_rw|fmode_copen)== FALSE)
   {
      tsw_beep();
      return;
   }

   Field frm[]=
   {
      /* TYPE           dataPtr           choices     hookFunc       flags    len      width    x     y     */
      /* -------------------------------------------------------------------------------------------------- */
      {  FRM_STRING     , tpl->desc       ,0          ,0             ,0       ,60      ,53      ,19   ,2    },    // description
      {  FRM_UNSIGNED   ,&tpl->secLevel   ,0          ,0             ,0       ,5       ,6       ,19   ,3    },    // securiy level
      {  FRM_UNSIGNED   ,&tpl->subscribe  ,0          ,0             ,0       ,5       ,6       ,19   ,5    },    // subscribe
      {  FRM_UNSIGNED   ,&tpl->expLevel   ,0          ,0             ,0       ,5       ,6       ,19   ,6    },    // expiration level
      {  FRM_UNSIGNED   ,&tpl->tbTime     ,0          ,0             ,0       ,5       ,6       ,19   ,8    },    // timebank time
      {  FRM_UNSIGNED   ,&tpl->tbKbyte    ,0          ,0             ,0       ,5       ,6       ,19   ,9    },    // timebank kbyte
      {  FRM_UNSIGNED   ,&tpl->netCredit  ,0          ,0             ,0       ,5       ,6       ,19   ,10   },    // netmail credit
      {  FRM_STRING     , tpl->valMsg     ,0          ,0             ,0       ,8       ,9       ,19   ,11   },    // validate message
      {  FRM_FUNCTION   ,&tpl->valFlagsOn ,0          ,flag_select   ,0       ,26      ,26      ,19   ,13   },    // flags on
      {  FRM_FUNCTION   ,&tpl->valFlagsOff,0          ,flag_select   ,0       ,26      ,26      ,19   ,14   },    // flags off
      {  FRM_FUNCTION   ,&tpl->expFlagsOn ,0          ,flag_select   ,0       ,26      ,26      ,19   ,15   },    // flags on
      {  FRM_FUNCTION   ,&tpl->expFlagsOff,0          ,flag_select   ,0       ,26      ,26      ,19   ,16   },    // flags off
      {  FRM_STRING     , tpl->comment    ,0          ,0             ,0       ,80      ,53      ,19   ,18   },    // comment
      {  FRM_CHOICE     ,&tpl->logLevel   ,LogLevels  ,0             ,0       ,10      ,10      ,57   ,3    },    // loglevels
      {  FRM_CHOICE     ,&tpl->hidden     ,YesNoIgnore,0             ,0       ,6       ,6       ,57   ,5    },    // hidden
      {  FRM_CHOICE     ,&tpl->noTops     ,YesNoIgnore,0             ,0       ,6       ,6       ,57   ,6    },    // in tops
      {  FRM_CHOICE     ,&tpl->atten      ,YesNoIgnore,0             ,0       ,6       ,6       ,57   ,7    },    // attention
      {  FRM_CHOICE     ,&tpl->noKill     ,YesNoIgnore,0             ,0       ,6       ,6       ,57   ,8    },    // no kill
      {  FRM_CHOICE     ,&tpl->ignDld     ,YesNoIgnore,0             ,0       ,6       ,6       ,57   ,9    },    // ignore dl
      {  FRM_CHOICE     ,&tpl->deleted    ,YesNo      ,0             ,0       ,6       ,6       ,57   ,11   },    // template deleted?
   };

   FormWindow w( 2,  
                 3, 
                 77, 
                 23, 
                 0x1F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x19,
                 NULL,
                 0x10 );
   w.open();

   w.attrib( 0x1F );
   w  << "\n"
         "     Description:\n"
         "  Security level:                             Loglevel:\n"
         "\n"
         "       Subscribe:                               Hidden:\n"
         "    Expir. level:                               NoTops:\n"
         "                                             Attention:\n"
         "  Time bank time:                               NoKill:\n"
         "    Time bank KB:                             IgnoreDL:\n"
         "  Netmail credit:\n"
         "    Validate Msg:                              Deleted:\n"
         "\n"
         "  Val. Flags  ON:\n"
         "  Val. Flags OFF:\n"
         "  Exp. Flags  ON:\n"
         "  Exp. Flags OFF:\n"
         "\n"
         "         Comment:";


   KEY hotkeys[]=
   {
      KEY_PGUP ,                          // next
      KEY_PGDN ,                          // previous
      KEY_CPGUP,                          // first
      KEY_CPGDN,                          // last
      KEY_ALTA ,                          // add
      KEY_ALTD ,                          // delete / undelete
      KEY_ALTF ,                          // show flags-info
      KEY_ALTL ,                          // list
      KEY_ALTN ,                          // search next
      KEY_ALTS ,                          // search
      KEY_F1   ,                          // help
      NULL
   };

   w.define( frm, 
             20, 
             0x1B,
             0x4E,
             hotkeys, helpFunction);

   ShrtT maxRec = tpl->count();
   if(maxRec == 0)
   {
      tpl->clear();
      tpl->write(0);
      maxRec++;
   }

   ShrtT curRecNo     = 0;
   ShrtT nextRecNo    = curRecNo;
   FlgT  readRequired = TRUE;

   tpl->clear();

   tsw_cursoron();

   WrdT crcVal;

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
         tpl->read(curRecNo);
         crcVal = crc(tpl, sizeof(template_data));
      }

      w.title( form( "Record %d/%d", curRecNo + 1, maxRec), 0x1E );

      IntT ret_code = w.process();

      switch(ret_code)
      {
         case ED_HOT:
         {
            switch(w.scanHotKey)
            {
               case KEY_F1    :
               {
                  showHelp( editHelp, "Edit Template Help" );
                  continue;
               }

               case KEY_ALTD  :
               {
                  if(tpl->inUse())
                     tpl->deleted = (tpl->deleted ? FALSE : TRUE);

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
            }
         }
         break;
      }


      FlgT isChanged = FALSE;
      if(tpl->inUse())
      {
         if(crcVal != crc(tpl, sizeof(template_data)))
         {
            ShrtT stat;
            isChanged = TRUE;

            if(forcedSave == FALSE)
               stat = ask_save();
            else
               stat = 1;

            if(stat == 1)
            {
               tpl->write(curRecNo);
               readRequired = TRUE;
            }
         }
      }

      switch(ret_code)
      {
         case ED_HOT :
         {
            switch(w.scanHotKey)
            {
               case KEY_PGDN  :
               {
                  if(curRecNo + 1 < maxRec)
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
                  if(curRecNo + 1 < maxRec)
                     nextRecNo = maxRec - 1;
               }
               break;

               case KEY_ALTA  :
               {
                  IntT tplNo = tpl->count() + 1;

                  Window w( 10,  
                            5, 
                            36, 
                            7, 
                            0x3F,
                            SHADOW,
                            CHISEL_BORDER,
                            0x3B,
                            NULL,
                            0x31 );
                  w.open();
                  w << " New template: ";

                  tsw_cursoron();

                  w.attrib( 0x3E );

                  if(w.scan(tplNo, 5, 6) == SF_ESC)
                     tplNo = -1;

                  tsw_cursoroff();

                  if((tplNo <= maxTemplates) && (tplNo > 1) )
                  {
                     if(tplNo > maxRec)
                     {
                        tpl->clear();
                        for(ShrtT cnt = maxRec; cnt < tplNo; cnt++)
                           tpl->write(cnt);
                     }

                     nextRecNo = tplNo - 1;
                  }

                  maxRec = tpl->count();
               }
               break;

               case KEY_ALTL  :
               {
                  nextRecNo = pickTemplate(curRecNo);
                  if(nextRecNo == -1)
                  {
                     nextRecNo = curRecNo;
                     readRequired = TRUE;
                  }

                  if(nextRecNo > maxRec - 1)
                  {
                     tpl->clear();
                     for(ShrtT cnt = maxRec; cnt <= nextRecNo; cnt++)
                        tpl->write(cnt);
                  }

                  maxRec = tpl->count();
               }
               break;

               case KEY_ALTN  :
               {
                  nextRecNo = searchTemplate(curRecNo);
                  if(nextRecNo == -1)
                  {
                     tsw_beep();
                     nextRecNo = curRecNo;
                     readRequired = TRUE;
                  }
               }
               break;

               case KEY_ALTS:
               {
                  nextRecNo = searchTemplate(-1);
                  if(nextRecNo == -1)
                  {
                     tsw_beep();
                     nextRecNo = curRecNo;
                     readRequired = TRUE;
                  }
               }
               break;
            }
         }
         break;

         case ED_ESC :
         {
            if((isChanged == FALSE) || (askYesNo("Exit template editor?") == TRUE))
               editLoop = FALSE;
         }
         break;
      }
   }

   maxRec = tpl->count();

   FlgT found = FALSE;                 // search deleted records
   for(ShrtT cnt = 0; cnt < maxRec && found == FALSE; cnt++)
      if(tpl->read(cnt) == TRUE)
         if(tpl->deleted == TRUE)
            found = TRUE;

   if(found == TRUE)                   // clear deleted records
      if(askYesNo("Clear deleted records?") == TRUE)
         for(ShrtT cnt = 0; cnt < maxRec; cnt++)
            if(tpl->read(cnt) == TRUE)
               if(tpl->deleted == TRUE)
               {
                  tpl->clear();
                  tpl->write(cnt);
               }


   ShrtT lastUsed = -1;                // search last used record
   for(cnt = maxRec - 1; cnt >= 0 && lastUsed == -1; cnt--)
      if(tpl->read(cnt) == TRUE)
         if(tpl->inUse() == TRUE)
            lastUsed = cnt;

   if(lastUsed + 1 != maxRec)
   {
      tpl->pos(lastUsed + 1);          // cut all other records
      tpl->cut();
   }

   delete(tpl);
   tsw_cursoroff();
}

/*---------------------------] END OF THE CODE [---------------------------*/
