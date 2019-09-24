
/* ------------------------------------------------------------
 * Filename ............... EditPvt.Cpp
 *
 * General Purpose ........ Edit private files settings
 * ------------------------------------------------------------
 * First date ............. 19 dec 1993
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

#define Use_LinkedList

#include <TsWin.Hpp>


#define Uses_PbUsersIdx
#define Uses_PbUsersPb
#define Uses_PbUsersBbs
#include "ThProCfg.Hpp"
#include "Template.Hpp"

struct FileEntry
   {
      FileName fname;
      LngT     size;
      Date     date;
      Time     time;
   };

LinkedList<_PrivateFile> far pvtfiles;
LinkedList<FileEntry>    far filelist;

/*--] Code [-------------------------------------] sortedFileList [--------*/

/*
 * Routine   : compare()
 * Purpose   : compare two objects from the linked list
 * ------------------------------------------------------------------------
 * Parameters: pointer to 2 objects
 * Return    : return code from stricmp()
 *
 */

int
CompareFileEntry(FileEntry& e1,FileEntry& e2)
{
   return stricmp(e1.fname, e2.fname);
}

/*--] Code [-------------------------------------] File pick functions [---*/

/*
 * Routine   : listFile()
 * Purpose   : list the specified private file record
 * ------------------------------------------------------------------------
 * Parameters: record number, X and Y coordinates
 * Return    : None
 *
 */

LCL void listFile(IntT recno, IntT x, IntT y)
{
   ChrT c = '+';
   char lp = '(';
   char rp = ')';

   FileName fn(pvtfiles[recno].fname);

   if(strchr(fn,'\\'))
   {
      if(access(fn,0))
         c = '-';
   }
   else
   {
      lp = rp = ' ';

      if(access(FileName(cfg.pvtuploadpath, fn), 0))
         c = '-';
   }


   fn.stripPath();

   String keep_kill;

   if(pvtfiles[recno].attr & PVTFILE_KEEP)
      keep_kill = "KEEP";
   else
      keep_kill = "KILL";

   tsw_maputs(x + 1, y, setFBColor(hYellow, nMagenta), form("[%c] %s  %c%-12s%c  -  %-28.28s [%4s]",
                                                             c,
                                                             ChrP(pvtfiles[recno].date.format("DD MMM YY")),
                                                             lp,(char *)fn,rp,
                                                             pvtfiles[recno].to,
                                                             (char *)keep_kill
                                                        )
             );

}

/*
 * Routine   : listFileEntry()
 * Purpose   : list the specified file from private files directory
 * ------------------------------------------------------------------------
 * Parameters: record number, X and Y coordinates
 * Return    : None
 *
 */

LCL void listFileEntry(IntT recno, IntT x, IntT y)
{
   FileEntry entry = filelist[recno];

   tsw_maputs(x + 1, y, setFBColor(hYellow, nBlue), form("%-12.12s   %s   %s   %6lu",
                                                             (char *)entry.fname,
                                                             (char *)entry.date.format("DD MMM CCYY"),
                                                             (char *)entry.time.format("HH:MM.SS"),
                                                             entry.size
                                                        )
             );

}

/*
 * Routine   : editPvtFile()
 * Purpose   : edit private files
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

void editPvtFile()
{
   KEY hotkeys[] =
   {
      KEY_INS,
      KEY_DEL,
      0
   };

   Field frm[]=
   {
      /* TYPE           dataPtr                 choices     hookFunc    flags       len      width    x     y    */
      /* ------------------------------------------------------------------------------------------------------- */
      { FRM_STRING      ,0                     ,0           ,0         ,FRM_UPPER   ,79      ,50     ,13    ,2    },
      { FRM_STRING      ,0                     ,0           ,0         ,0           ,35      ,36     ,13    ,4    },
      { FRM_STRING      ,0                     ,0           ,0         ,0           ,35      ,36     ,13    ,5    },
      { FRM_YESNO       ,0                     ,0           ,0         ,0           ,3       ,3      ,13    ,7    }
   };

   pvtfiles.kill();

   File f;

   if(f.open(FileName(syspath,"PVTFILES.PB")))
   {
      for(;;)
      {
         _PrivateFile tmp;

         if(f.read(&tmp,sizeof(tmp)) != sizeof(tmp))
            break;

         if(tmp.fname[0])
            pvtfiles.add(tmp);
      }

      f.close();
   }


   SelectWindow sw( pvtfiles.count(), 
                    0x70,
                    listFile, 
                    0, 
                    NULL, 
                    hotkeys );

   sw.open( 3,
            4, 
            77, 
            21,
            0x5F,
            SHADOW | BRACKETS,
            CHISEL_BORDER,
            0x5D,
            NULL,
            0x51 );

   sw.title( "Personal Files", 0x5E );

   for(;;)
   {
      tsw_showfooter( "\x1<Del>\x1 - Delete      \x1<Ins>\x1 - Add      \x1<Enter>\x1 - Edit", BAR_NORM, BAR_HIGH );
      sw.redefine(pvtfiles.count());

      ShrtT choice = sw.process();
      if(choice == SL_ESC)
         break;

      if(choice == SL_HOTKEY)
      {
         switch(sw.hotkey)
         {
            case KEY_INS:
            {
               _PrivateFile tmp;

               CLEAR_OBJECT(tmp);

               choice = pvtfiles.count();
               sw.current = choice;

               tmp.date.today();
               strcpy(tmp.from,cfg.sysopname);

               pvtfiles.add(tmp);
            }
            break;

            case KEY_DEL:
            {
               if(!pvtfiles.count())
               {
                  tsw_beep();
                  continue;
               }

               FileName fn(cfg.pvtuploadpath,pvtfiles[sw.current].fname);
               if(!access(fn,0))
                  if(askYesNo(form("Delete file %s ?",pvtfiles[sw.current].fname)) == TRUE)
                     unlink(fn);

               pvtfiles.get(sw.current);
               pvtfiles.remove();

               continue;
            }
         }
      }

      ShrtT recno = choice;
      KEY pickkeys[]=
      {
         KEY_ALTL ,                          // list
         NULL
      };

      FormWindow w( 7, 
                    8, 
                    74, 
                    17, 
                    0x3F,
                    SHADOW | BRACKETS,
                    CHISEL_BORDER,
                    0x3B,
                    NULL,
                    0x31 );

      w.define( frm, 
                4, 
                0x31,
                0x4E,
                pickkeys, 
                NULL );

      w.open();
      w.title( "Edit Personal File", 0x3E );

      w.attrib( 0x3F );
      w << "\n  FileName:\n"
           "\n From user:"
           "\n   To user:\n"
           "\n   Delete?:";

      bool kill_file = !(pvtfiles[recno].attr & PVTFILE_KEEP);

      frm[0].value = pvtfiles[recno].fname;
      frm[1].value = pvtfiles[recno].from;
      frm[2].value = pvtfiles[recno].to;
      frm[3].value = &kill_file;

      for(;;)
      {
         tsw_showfooter("[\001ALT-L\001] Show file- or userlist", BAR_NORM, BAR_HIGH );

         tsw_cursoron();
         ShrtT choice = w.process();
         if(choice == ED_ESC)
            break;

         if(choice == ED_HOT)
         {
            switch(w.scanHotKey)
            {
               case KEY_ALTL:
               {
                  User user;
                  User::setDir(cfg.msgpath);
                  NumUsers = ShrtT(User::numUsers());

                  userFilter filter;
                  fillUserList(filter);

                  CLEAR_OBJECT(usrFind);

                  switch(w.getField())
                  {

                     case 0:                    // filename
                     {
                        FileName mask(cfg.pvtuploadpath);
                        mask.appendBS();
                        mask << "*.*";

                        filelist.kill();
                        {
                           DirScan find;
                           int status = find.first(mask);
                           while(status == TRUE)
                           {
                              FileEntry entry;
                              entry.fname = find.name();
                              entry.size  = find.size();
                              entry.date  = find.date();
                              entry.time  = find.time();

                              filelist.add(entry);

                              status = find.next();
                           }
                        }

                        if(filelist.count() != 0)
                        {
                           filelist.sort(CompareFileEntry);

                           SelectWindow sw( filelist.count(), 
                                            0x70,
                                            listFileEntry, 
                                            0, 
                                            NULL, 
                                            NULL );

                           sw.open( 13, 
                                    6, 
                                    67, 
                                    19,
                                    0x17,
                                    SHADOW | BRACKETS,
                                    CHISEL_BORDER,
                                    0x19,
                                    NULL,
                                    0x10 );

                           sw.title( ChrP( mask ), 0x1E );

                           tsw_cursoroff();
                           int fileno = sw.process();
                           if(fileno != SL_ESC)
                           {
                              strcpy(pvtfiles[recno].fname, filelist[fileno].fname);
                           }
                        }

                        continue;
                     }

                     case 1:                    // from user
                     {
                        ShrtT userno = pickUser(0);
                        if(userno != -1)
                        {
                           user.read(getDiskRecNo(userno));
                           strcpy(pvtfiles[recno].from, user.name);
                        }

                        continue;
                     }

                     case 2:                    // to user
                     {
                        ShrtT userno = pickUser(0);
                        if(userno != -1)
                        {
                           user.read(getDiskRecNo(userno));
                           strcpy(pvtfiles[recno].to, user.name);
                        }

                        continue;
                     }
                  }
               }
            }
         }
      }

      pvtfiles[choice].attr = 0;

      if(!kill_file)
         pvtfiles[choice].attr |= PVTFILE_KEEP;
   }


   if(f.open(FileName(syspath,"PVTFILES.PB"),fmode_create))
   {
      for(pvtfiles.rewind(); !pvtfiles.eol();pvtfiles++)
      {
         if(f.write(&pvtfiles.get(),sizeof(_PrivateFile)) != sizeof(_PrivateFile))
            break;
      }

      f.close();
   }

   tsw_cursoroff();
}

/*---------------------------] END OF THE CODE [---------------------------*/
