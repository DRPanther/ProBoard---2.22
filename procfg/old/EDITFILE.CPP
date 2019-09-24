#include <stdio.h>
#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"

struct FileArea : _FileArea
   {
    static File f;
    static int file_count;
   public:
    FileArea();
    ~FileArea();

    bool read(int);
    bool write(int);
    bool clear(int);

    int  numAreas();
    void truncate();
   };

int FileArea::file_count = 0;
File FileArea::f;

FileArea::FileArea()
{
   if(file_count <= 0)
   {
      f.open(FileName(syspath,"FILECFG.PRO"),fmode_rw | fmode_copen);
      file_count = 0;
   }

   file_count++;
}

FileArea::~FileArea()
{
   if(file_count <= 1)
   {
      f.close();
      file_count = 1;
   }

   file_count--;
}

bool
FileArea::read(int rec)
{
   f.seek(long(rec-1) * sizeof(_FileArea));

   return (f.read(this,sizeof(*this)) == sizeof(*this)) ? TRUE : FALSE;
}

bool
FileArea::write(int rec)
{
   f.seek(long(rec-1) * sizeof(_FileArea));
   return (f.write(this,sizeof(*this)) == sizeof(*this)) ? TRUE : FALSE;
}

bool
FileArea::clear(int rec)
{
   CLEAR_OBJECT(*this);

   return write(rec);
}

int
FileArea::numAreas()
{
   return int(f.len() / sizeof(_FileArea));
}

void
FileArea::truncate()
{
   FileArea fa;
   int i = numAreas();

   for(;i>0;i--)
   {
      if(!fa.read(i)) continue;

      if(fa.name[0])
      {
         f.seek(long(i) * sizeof(_FileArea));
         f.cut();
         break;
      }
   }

   if(!i)
   {
      f.seek(0);
      f.cut();
   }
}


static void
farea_list_func(int i,int x,int y)
{
   FileArea fa;
   char *ptr;

   if(!fa.read(i+1) || !fa.name[0])
      ptr = form("%5d   תתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתת",i+1);
   else
      ptr = form("%5d   %s",i+1,fa.name);

   if(strlen(ptr)>43) ptr[43]=0;

   tsw_maputs(x,y,0x5F,ptr);
   tsw_maputc(x+6,y,0x5E,'³');
}

static void
helpfunction(int i)
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

void
edit_files()
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

   FileArea fa;

   int num_areas = fa.numAreas();

   KEY hotkeys[]={ '1','2','3','4','5','6','7','8','9',KEY_INS, KEY_DEL , NULL };

   SelectWindow sw;
   sw.open(3,3,50,tsw_vsize-3,0x5E,SHADOW);
   sw.title(" Select File Area ",0x5F);
   sw.define(num_areas,0x70,farea_list_func,0,NULL,hotkeys);

   sw.direct(7,0,'ׁ');
   sw.direct(7 ,tsw_vsize-6,'ֿ');

   bool reselect = TRUE;
   int choice;

   for(;;)
   {
      if(reselect)
      {
         tsw_showfooter("\001<Ins>\001 - Add Area      \001<Del>\001 - Delete Area      \001<Enter>\001 - Edit Area", BAR_NORM, BAR_HIGH);

         choice=sw.process();

         if(choice==SL_ESC) break;

         if(choice==SL_HOTKEY)
         {
            switch(sw.hotkey)
            {
               case KEY_DEL:
                  {
                     if(!fa.read(sw.current+1))
                     {
                        tsw_beep();
                        continue;
                     }

                     if(fa.name[0])
                        if(ask_save() > 0)
                           fa.clear(sw.current+1);

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
               case KEY_INS:
                  {
                     CursorState _cursor(TRUE);

                     int newareanum = num_areas+1;

                     Window w(10,5,40,7,0x3F);
                     w.open();
                     w << " New area number: ";
                     w.attrib(0x3E);
                     w.scan(newareanum,5,6);

                     if(newareanum > MAX_FILEAREA || newareanum < 1) continue;

                     if(newareanum > num_areas)
                     {
                        for(int i=num_areas+1;i<=newareanum;i++) fa.clear(i);

                        num_areas = newareanum;

                        sw.redefine(num_areas);
                     }

                     choice = newareanum - 1;

                     sw.show(choice);
                  }
            }
         }
      }

      if(!fa.read(choice+1))
      {
         tsw_beep();
         continue;
      }

      word crcvalue = crc(&fa,sizeof(fa));

      if(!fa.name[0])
      {
         memset(&fa,0,sizeof(fa));
         fa.level = 1;
      }

      AccessFlags *flags[2];

      flags[0] = &fa.flags;
      flags[1] = &fa.flagsNot;

      int tops = (fa.notops) ? 0 : 1;

      frm[0].value = fa.name;
      frm[1].value = fa.filepath;
      frm[2].value = fa.listpath;
      frm[3].value = flags;
      frm[4].value = &fa.level;
      frm[5].value = &fa.cdrom;
      frm[6].value = &tops;
      frm[7].value = &fa.free;
      frm[8].value = &fa.groups[0];
      frm[9].value = &fa.groups[1];
      frm[10].value = &fa.groups[2];
      frm[11].value = &fa.groups[3];
      frm[12].value = &fa.allGroups;
      frm[13].value = &fa.maxfiles;
      frm[14].value = &fa.maxkb;
      frm[15].value = &fa.dateFormat;

      KEY form_keys[] = { KEY_PGUP , KEY_PGDN, 0 };

      FormWindow w(6,3,71,23,0x1F,SHADOW);
      w.define(frm,16,0x1F,0x70,form_keys,helpfunction);
      w.open();
      w.title(form(" File Area #%d ",choice+1),0x1E);
      w.attrib(0x1E);

      w << "\n Area name    :\n\n"
            " File Location:\n"
            " Listing file :\n\n"
            " Access Flags :\n"
            " Access Level :\n\n"
            " Copy local   :                           Max files:\n"
            " In TOPFILES  :                           Max Kb   :\n"
            " Free Area    :\n"
            "                              FILES.BBS Date Format:\n"
            " Group #1     :\n"
            " Group #2     :\n"
            " Group #3     :\n"
            " Group #4     :\n"
            " All Groups   :";

      w.direct( 2,20,0x1B," <PgUp> Previous Area ");
      w.direct(44,20,0x1B," <PgDn> Next Area ");

      CursorState _cursor(TRUE);

      int retval = w.process();

      fa.notops = (tops) ? 0 : 1;

      if(!fa.listpath[0] && fa.filepath[0])
      {
         strcpy(fa.listpath , fa.filepath);
         append_backspace(fa.listpath);
         strcat(fa.listpath , "FILES.BBS");
      }

      if(crcvalue != crc(&fa,sizeof(fa)) && fa.name[0])
      {
         if(ask_save())
         {
            fa.write(choice+1);
         }
      }

      if(retval == ED_HOT)
      {
         if(w.scanHotKey == KEY_PGUP && choice > 0)
            choice--;
         if(w.scanHotKey == KEY_PGDN && choice < num_areas-1)
            choice++;

         reselect = FALSE;
      }
      else
         reselect = TRUE;

   }

   if(fa.read(num_areas))
   {
      if(!fa.name[0])
         fa.truncate();
   }
}


