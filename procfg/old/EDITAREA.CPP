#define Use_DynamicArrayFile

#include <stdio.h>
#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"
#include "area.hpp"

static void list_func(int i,int x,int y);

Area *area;

void
edit_areas(int fa)
{
   if(fa)
      area = new FileArea;
   else
      area = new MsgArea;

   int num_areas = area->numAreas();

   KEY hotkeys[]={ ' ','1','2','3','4','5','6','7','8','9',KEY_INS, KEY_DEL , KEY_ALTC, KEY_ALTA, KEY_ALTD, KEY_ALTS, KEY_ALTM, KEY_ALTU, NULL };

   SelectWindow sw;

   sw.open( 1,
            1,
            tsw_hsize,
            tsw_vsize - 3,
            0x70,
            BRACKETS,
            CHISEL_BORDER,
            0x7F,
            NULL,
            0x78 );

   sw.define( num_areas + 1,
              0x1E,
              list_func,
              0,
              NULL,
              hotkeys );

   if(fa)
   {
      sw.title( "Select file area", 0x70);

      sw.direct( 10, 0,             0x7F, 'ֲ' );
      sw.direct( 10, tsw_vsize - 6, 0x78, 'ֱ' );
   }
   else
   {
      sw.title( "Select Message Area", 0x70 );

      sw.direct( 10, 0,           0x7F, 'ֲ' );
      sw.direct( 29, tsw_vsize-6, 0x78, 'ֱ' );
      sw.direct( 10, tsw_vsize-6, 0x78, 'ֱ' );
   }

   int choice = 0;

   bool reselect = TRUE;

   for(;;)
   {
      if(reselect)
      {
         Window footwin;

         footwin.open( 1,
                       tsw_vsize - 2,
                       tsw_hsize,
                       tsw_vsize,
                       BAR_COLOR,
                       NOBORDER );

         KeyItem( footwin,
                  10,
                  1,
                  0x4E,
                  "<Ins>",
                  0x4F,
                  "Insert area" );

         KeyItem( footwin,
                  10,
                  2,
                  0x4E,
                  "<AltS>",
                  0x4F,
                  "Sort marked areas" );

         KeyItem( footwin,
                  10,
                  3,
                  0x4E,
                  "<AltU>",
                  0x4F,
                  "Untag all areas" );


         KeyItem( footwin,
                  40,
                  1,
                  0x4E,
                  "<Del>",
                  0x4F,
                  "Delete area(s)" );

         KeyItem( footwin,
                  40,
                  2,
                  0x4E,
                  "<AltM>",
                  0x4F,
                  "Move area(s)" );

         KeyItem( footwin,
                  40,
                  3,
                  0x4E,
                  "<Space>",
                  0x4F,
                  "Mark range" );


         KeyItem( footwin,
                  66,
                  1,
                  0x4E,
                  "<AltA>",
                  0x4F,
                  "Add area" );

         KeyItem( footwin,
                  66,
                  2,
                  0x4E,
                  "<AltC>",
                  0x4F,
                  "Clear area(s)" );

         KeyItem( footwin,
                  66,
                  3,
                  0x4E,
                  "<Enter>",
                  0x4F,
                  "Edit" );


         choice=sw.process();

         if(choice==SL_ESC) break;

         if(choice==SL_HOTKEY)
         {
            switch(sw.hotkey)
            {
               case ' ':
                  {
                     if(sw.current >= num_areas)
                        continue;

                     if(area->selStart() < 0)
                     {
                        area->selStart(sw.current);
                        area->selEnd  (sw.current);
                     }
                     else
                     {
                        if(sw.current == area->selStart()-1)
                        {
                           area->selStart(sw.current);
                        }
                        else
                        if(sw.current == area->selEnd()+1)
                        {
                           area->selEnd(sw.current);
                        }
                        else
                        if(sw.current < area->selStart() || sw.current > area->selEnd())
                        {
                           area->selStart(sw.current);
                           area->selEnd  (sw.current);
                        }
                     }

                     if(sw.current < num_areas-1)
                        KB.push(KEY_DN);

                     continue;
                  }

               case KEY_ALTU:
                  {
                     area->selStart(-1);
                     area->selEnd(-1);

                     continue;
                  }

               case KEY_INS:
                  {
                     area->insert(sw.current+1);
                     num_areas = area->numAreas();
                     sw.redefine(num_areas+1);

                     continue;
                  }

               case KEY_DEL:
                  {
                     if(area->selStart() >= 0)
                     {
                        if(!ask_confirm("Delete all marked areas?"))
                           continue;

                        for(;;)
                        {
                           area->remove(area->selStart()+1);

                           if(area->selStart() == area->selEnd())
                              break;

                           area->selEnd(area->selEnd()-1);
                        }

                        area->selStart(-1);
                        area->selEnd(-1);
                     }
                     else
                     {
                        if(sw.current >= num_areas)
                        {
                           tsw_beep();
                           continue;
                        }

                        if(ask_confirm("Delete current area?"))
                        {
                           area->remove(sw.current+1);
                        }
                     }

                     num_areas = area->numAreas();
                     sw.redefine(num_areas+1);

                     continue;
                  }

               case KEY_ALTC:
               {
                  if(area->selStart() >= 0)
                  {
                     if(!ask_confirm("Clear all marked areas?"))
                        continue;

                     for(int i=area->selStart();i<=area->selEnd();i++)
                        area->clear(i+1);
                  }
                  else
                  {
                     if(num_areas < 1 || !area->read(sw.current+1))
                     {
                        tsw_beep();
                        continue;
                     }

                     if(ask_confirm("Clear this area?") > 0)
                        area->clear(sw.current+1);
                  }

                  continue;
               }

               case KEY_ALTS:
               {
                  ShowBusy("Sorting Areas");

                  if(area->selStart() >= 0 && (area->selEnd()-area->selStart()))
                  {
                     area->sort(area->selStart(),area->selEnd());
                  }

                  HideBusy();

                  continue;
               }

               case KEY_ALTM:
               {
                  ShowBusy("Moving Areas");

                  if(area->selStart() >= 0)
                  {
                     if(!area->move(area->selStart(),area->selEnd(),sw.current))
                        tsw_beep();
                  }

                  HideBusy();

                  area->selStart(-1);
                  area->selEnd  (-1);

                  continue;
               }


               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9': KB.push((KEY)(char)sw.hotkey);
               case KEY_ALTA:
                  {
                     int newareanum = num_areas+1;

                     Window w( 10,
                               5,
                               40,
                               7,
                               0x3F,
                               SHADOW,
                               CHISEL_BORDER,
                               0x3B,
                               NULL,
                               0x31 );

                     w.open();

                     w << " New area number: ";

                     tsw_cursoron();

                     w.attrib(0x3E);
                     w.scan(newareanum,5,6);
                     tsw_cursoroff();

                     if(newareanum > MAX_MSGAREA || newareanum < 1) continue;

                     if(newareanum > num_areas)
                     {
                        for(int i=num_areas+1;i<=newareanum;i++)
                           area->clear(i);

                        num_areas = newareanum;

                        sw.redefine(num_areas+1);
                     }

                     choice = newareanum - 1;

                     sw.show(choice);
                  }
            }
         }
      }

      if(sw.current == num_areas)
      {
         area->clear(sw.current+1);
         num_areas++;
         sw.redefine(num_areas+1);
      }

      if(!area->read(choice+1))
      {
         tsw_beep();
         choice = -1;
         continue;
      }

      dword crcvalue = area->crc();

      KEY retval = area->edit();

      if(crcvalue != area->crc())
      {
         if(ask_save())
            area->write(choice+1);
      }

      if(retval == KEY_PGUP && choice > 0)
      {
         choice--;
         reselect = FALSE;
      }
      if(retval == KEY_PGDN && choice < num_areas-1)
      {
         choice++;
         reselect = FALSE;
      }
      if(retval == KEY_ESC)
         reselect = TRUE;
   }

   delete area;
}

static void
ma_help(int i)
{
   char *txt[]=
      {
      "Name to use for this message area",
      "Squish: Full filename of message area ִִִ *.MSG: directory for .MSG files",
      "Kind of message area (Local/NetMail/EchoMail/Pvt EchoMail)",
      "Type of messages allowed (Private and/or Public)",
      "Can users write messages using an alias name?",
      "Type of message base (Squish/Hudson/*.MSG)",
      "Level needed to read messages in this area",
      "Flags needed to read messages in this area",
      "Level needed to write messages in this area",
      "Flags needed to write messages in this area",
      "Level needed to be allowed all operations on messages in this area",
      "Flags needed to be allowed all operations on messages in this area",
      "Origin line to use for this area (leave blank for default origin line)",
      "Press [Enter] to select an AKA address to use for this area",
      "Echomail tag name (only for \"Echomail\" type areas)",
      "QWK Area Name (limited to 12 characters)",
      "Area number where replies will be posted",
      "Sysop name for this area",
      "Group #1 this area belongs to",
      "Group #2 this area belongs to",
      "Group #3 this area belongs to",
      "Group #4 this area belongs to",
      "Does this message area belong to all groups?",
      "Messages will be killed after specified number of days",
      "Messages will be killed when received for specified number of days",
      "Maximum number of messages allowed in this area"
      };

   tsw_showfooter(txt[i], BAR_COLOR);
}

static void
fa_help(int i)
{
   char *txt[]=
   {
      "Name/description of this file area",
      "Directory where files are located",
      "Full path and filename of file listing for this area",
      "Flags needed to download from this area (<Enter>=Edit)",
      "Level needed to download from this area",
      "Use CD-ROM specific listing format?",
      "Include files from this area in TOPFILES.A??",
      "Free area?",
      "Group #1 this area belongs to",
      "Group #2 this area belongs to",
      "Group #3 this area belongs to",
      "Group #4 this area belongs to",
      "Does this area belong to all groups?",
      "Maximum number of downloaded files per day for this area (0=unlimited)",
      "Maximum number of downloaded Kbytes per day for this area (0=unlimited)",
      "The date format that is used in FILES.BBS for this area (if listed)"
   };

   tsw_showfooter(txt[i], BAR_COLOR);
}



KEY
MsgArea::edit()
{
   static char *msgkind[]     = { "Local" , "NetMail" , "EchoMail" , "Pvt EchoMail",NULL};
   static char *msgtype[]     = { "Private/Public" , "Private Only" , "Public Only" , "To-All" , NULL };
   static char *msgaccess[]   = { "Real Names Only" , "Free Alias" , "Fixed Alias (or real name)" ,"Fixed Alias (enforced)",NULL};
   static char *msgbasetype[] = { "Hudson" , "Squish" , "Fido *.MSG", "JAM", NULL};

   Field frm[]=
      {
         { FRM_STRING  , 0 , 0,0 , 0         , 80 ,32 , 17,2  },
         { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 80 ,32 , 17,3  },
         { FRM_CHOICE  , 0 , msgkind,0 , 0   , 12, 12,  17,4  },
         { FRM_CHOICE  , 0 , msgtype,0 , 0   , 14, 14,  17,5  },
         { FRM_CHOICE  , 0 , msgaccess,0 , 0 , 26, 26,  17,6  },
         { FRM_CHOICE  , 0 , msgbasetype,0,0 , 10, 10,  17,7  },
         { FRM_UNSIGNED, 0 , 0,0 , 0         , 5,  6,   17,9  },
         { FRM_FUNCTION, 0 , 0,flagonoff_select,0 , 26, 26,  38,9 },
         { FRM_UNSIGNED, 0 , 0,0 , 0         , 5,  6,   17,10 },
         { FRM_FUNCTION, 0 , 0,flagonoff_select,0 , 26, 26,  38,10 },
         { FRM_UNSIGNED, 0 , 0,0 , 0         , 5,  6,   17,11 },
         { FRM_FUNCTION, 0 , 0,flagonoff_select,0 , 26, 26,  38,11 },
         { FRM_STRING  , 0 , 0,0,0           , 61, 54,  17,13 },
         { FRM_FUNCTION, 0 , 0,aka_select ,0 , 20, 20,  17,14 },
         { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 60, 25,  17,15 },
         { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 12, 13,  17,16 },
         { FRM_INT     , 0 , 0,0 , 0         , 3,  4,   17,17 },
         { FRM_STRING  , 0 , 0,0 , 0         , 35 ,36 , 17,18 },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3,  4,   61,2  },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3,  4,   61,3  },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3,  4,   61,4  },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3,  4,   61,5  },
         { FRM_YESNO   , 0 , 0,0 , 0         , 3,  3,   61,7  },
         { FRM_INT     , 0 , 0,0 , 0         , 3 , 4 ,  60,15 },
         { FRM_INT     , 0 , 0,0 , 0         , 3 , 4 ,  60,16 },
         { FRM_INT     , 0 , 0,0 , 0         , 4 , 5 ,  60,17 }
      };


   if(!name[0])
   {
      readLevel   = 1;
      writeLevel  = 1;
      sysopLevel  = 32000;
      msgKillDays = 30;
      rcvKillDays = 14;
      maxMsgs     = 500;
   }


   AccessFlags *readflags[2];
   AccessFlags *writeflags[2];
   AccessFlags *sysopflags[2];

   readflags[0] = &readFlags;
   readflags[1] = &readFlagsNot;
   writeflags[0] = &writeFlags;
   writeflags[1] = &writeFlagsNot;
   sysopflags[0] = &sysopFlags;
   sysopflags[1] = &sysopFlagsNot;

   frm[0].value  = name;
   frm[1].value  = path;
   frm[2].value  = &msgKind;
   frm[3].value  = &msgType;
   frm[4].value  = &flags;
   frm[5].value  = &msgBaseType;
   frm[6].value  = &readLevel;
   frm[7].value  = readflags;
   frm[8].value  = &writeLevel;
   frm[9].value  = writeflags;
   frm[10].value = &sysopLevel;
   frm[11].value = sysopflags;
   frm[12].value =  origin;
   frm[13].value = &aka;
   frm[14].value =  tag;
   frm[15].value =  qwkname;
   frm[16].value = &replyBoard;
   frm[17].value =  sysop;
   frm[18].value = &groups[0];
   frm[19].value = &groups[1];
   frm[20].value = &groups[2];
   frm[21].value = &groups[3];
   frm[22].value = &allGroups;
   frm[23].value = &msgKillDays;
   frm[24].value = &rcvKillDays;
   frm[25].value = &maxMsgs;

   KEY form_keys[] = { KEY_PGUP , KEY_PGDN, 0 };

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

   w.define( frm,
             26,
             0x1B,
             0x4E,
             form_keys,
             ma_help );

   w.open();
   w.title(form( "Message Area #%d", areaNum ), 0x1E );

   w.attrib( 0x1F );
   w << "\n          Name:                                   Group #1:\n"
          "          Path:                                   Group #2:\n"
          "  Message Kind:                                   Group #3:\n"
          "  Message Type:                                   Group #4:\n"
          "  Name Options:\n"
          "  Message Base:                                 All groups:\n"
          "\n"
          "    Read Level:          Read Flags:\n"
          "   Write Level:         Write Flags:\n"
          "   Sysop Level:         Sysop Flags:\n\n"
          "   Origin Line:\n"
          "       Use AKA:\n"
          "  EchoMail Tag:                                 Kill after       days\n"
          " QWK Area Name:                            Kill rcvd after       days\n"
          "    Reply Area:                                        Max       msgs\n"
          "         Sysop:\n";

   w.direct( 3,20,0x1E," <PgUp> Previous Area ");
   w.direct(53,20,0x1E," <PgDn> Next Area ");

   tsw_cursoron();
   int retval = w.process();
   tsw_cursoroff();

   if(retval == ED_HOT)
      return w.scanHotKey;
   else
      return KEY_ESC;
}

KEY
FileArea::edit()
{
   char *dateFormats[] =
   {
      "MM/DD/YY", "YY/MM/DD", "DD/MM/YY",
      NULL
   };

   Field frm[]=
      {
         { FRM_STRING  , 0 , 0,0 , 0         , 79 , 45, 17,2  },
         { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 79 , 45, 17,4  },
         { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 79 , 45, 17,5  },
         { FRM_FUNCTION, 0 , 0,flagonoff_select ,0, 26 , 26, 17,7 },
         { FRM_UNSIGNED, 0 , 0,0 , 0         , 5  , 6 , 17,8  },
         { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,10 },
         { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,11 },
         { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,12 },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3  , 4 , 17,14 },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3  , 4 , 17,15 },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3  , 4 , 17,16 },
         { FRM_SHORT   , 0 , 0,0 , 0         , 3  , 4 , 17,17 },
         { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,18 },
         { FRM_INT     , 0 , 0,0 , 0         , 5  , 6 , 54,10 },
         { FRM_INT     , 0 , 0,0 , 0         , 5  , 6 , 54,11 },
         { FRM_CHOICE  , 0 ,dateFormats,0,0  , 8  , 8 , 54,13 }
      };

   AccessFlags *fl[2];

   fl[0] = &flags;
   fl[1] = &flagsNot;

   int tops = (notops) ? 0 : 1;

   frm[0].value = name;
   frm[1].value = filepath;
   frm[2].value = listpath;
   frm[3].value = fl;
   frm[4].value = &level;
   frm[5].value = &cdrom;
   frm[6].value = &tops;
   frm[7].value = &free;
   frm[8].value = &groups[0];
   frm[9].value = &groups[1];
   frm[10].value = &groups[2];
   frm[11].value = &groups[3];
   frm[12].value = &allGroups;
   frm[13].value = &maxfiles;
   frm[14].value = &maxkb;
   frm[15].value = &dateFormat;

   KEY form_keys[] = { KEY_PGUP , KEY_PGDN, 0 };

   FormWindow w( 6,
                 3,
                 71,
                 23,
                 0x1F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x19,
                 NULL,
                 0x10 );

   w.define( frm,
             16,
             0x1B,
             0x4E,
             form_keys,
             fa_help );

   w.open();

   w.title( form( "File Area #%d", areaNum ), 0x1E );

   w.attrib(0x1F);

   w << "\n     Area name:\n\n"
          " File Location:\n"
          "  Listing file:\n\n"
          "  Access Flags:\n"
          "  Access Level:\n\n"
          "    Copy local:                           Max files:\n"
          "   In TOPFILES:                              Max Kb:\n"
          "     Free Area:\n"
          "                              FILES.BBS Date Format:\n"
          "      Group #1:\n"
          "      Group #2:\n"
          "      Group #3:\n"
          "      Group #4:\n"
          "    All Groups:";

   w.direct( 2,20,0x1E," <PgUp> Previous Area ");
   w.direct(44,20,0x1E," <PgDn> Next Area ");

   CursorState _cursor(TRUE);

   int retval = w.process();

   notops = (tops) ? 0 : 1;

   if(!listpath[0] && filepath[0])
   {
      strcpy(listpath , filepath);
      append_backspace(listpath);
      strcat(listpath , "FILES.BBS");
   }

   if(retval == ED_HOT)
      return w.scanHotKey;
   else
      return KEY_ESC;
}

static void
list_func(int i,int x,int y)
{
   area->listFunc(i,x,y);
}

void
MsgArea::listFunc(int i,int x,int y)
{
   MsgArea ma;
   char *ptr;

   if(!ma.read(i+1))
      ptr = form("   <end> ³                  ³");
   else
   if(!ma.name[0])
      ptr = form("   %5d ³ תתתתתתתתתתתתתתתת ³ תתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתת",i+1);
   else
      ptr = form("   %5d ³ %-16.16s ³ %s",i+1,ma.tag,ma.name);

   if(strlen(ptr)>62) ptr[62]=0;

   tsw_maputs(x,y,0x7F,ptr);

   if(i == ma.selStart())
      tsw_maputs(x,y,0x7E,"Ú>");

   if(i == ma.selEnd())
      tsw_maputs(x,y,0x7E,"ְ>");

   if(i == ma.selEnd() && i == ma.selStart())
      tsw_maputs(x,y,0x7E,"ִ>");

   if(i > ma.selStart() && i < ma.selEnd())
      tsw_maputs(x,y,0x7F,"³");

   tsw_maputc(x+9,y,0x7F,'³');
   tsw_maputc(x+28,y,0x7F,'³');
}

void
FileArea::listFunc(int i,int x,int y)
{
   FileArea fa;
   char *ptr;

   if(!fa.read(i+1))
      ptr = form("   <end> ³                                  ³                                       ");
   else
   if(!fa.name[0])
      ptr = form("   %5d ³ תתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתת ³ תתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתת",i+1);
   else
      ptr = form("   %5d ³ %-32.32s ³ %s",i+1,fa.name,fa.filepath);

   if(strlen(ptr)>75) ptr[75]=0;

   tsw_maputs(x,y,0x7F,ptr);

   if(i == fa.selStart())
      tsw_maputs(x,y,0x7E,"Ú>");

   if(i == fa.selEnd())
      tsw_maputs(x,y,0x7E,"ְ>");

   if(i == fa.selEnd() && i == fa.selStart())
      tsw_maputs(x,y,0x7E,"ִ>");

   if(i > fa.selStart() && i < fa.selEnd())
      tsw_maputs(x,y,0x7E,"³");

   tsw_maputc(x+9,y,0x7F,'³');
   tsw_maputc(x+44,y,0x7F,'³');
}

