#include <ctype.h>
#include <string.h>
#include "procfg.hpp"
//#include <deflang.hpp>

#include "extsel.hpp"

static void edit_properties();
static bool read_default_language();

typedef _LanguageString LanguageString;

const int MAX_LSTRINGS = 400;

static LanguageFile far lang_file;

static LanguageString *lstrings;
static LanguageString *default_lstrings;

static word            num_lstrings;

static void edit_language_string( LanguageString& l );

static int
list_func(ExtSelectWindow& w , int i,int y,int maxheight)
{
   char *s = lstrings[i].s;
   int count = 0;

   if(y <= 0)
   {
      for( ; *s ; s++)
         if(*s == '\n')
            count++;

      return count + 1;
   }

   bool highlight = FALSE;
   int   old_x,old_y;
   byte  old_attr;

   old_attr = w.attrib();
   w.getPos(old_x,old_y);
   w.setPos(1,y);
   w.attrib(lstrings[i].color);

   tsw_clearrect(w.minX,w.minY+y-1,w.maxX,w.minY+y-1,w.attrib());

   for(;*s;s++)
   {
      byte c = 0;

      c = *s;

      if(w.inEscape())
      {
         w << c;

         continue;
      }

      if(c == '\\')
      {
         char c = *(++s);

         if(c == '\\')
         {
            w << '\\';
            s++;
            continue;
         }
         else
         {
            byte col = 0;

            if(isxdigit(c))
            {
               for( int i = 0; i < 2 ; i++ , s++)
               {
                  c = *s;

                  if(isdigit(c))
                     col = col * 16 + c - '0';
                  else
                     col = col * 16 + toupper(c) - 'A' + 10;
               }

               w.attrib(col);

               s--;

               continue;
            }

            if(c == 'H' || c == 'L')
            {
               switch(*(++s))
               {
                  case 'B': col = 1;
                            break;
                  case 'G': col = 2;
                            break;
                  case 'C': col = 3;
                            break;
                  case 'R': col = 4;
                            break;
                  case 'P': col = 5;
                            break;
                  case 'Y': col = 6;
                            break;
                  case 'W': col = 7;
                            break;
               }

               if(c == 'H') col |= 8;
            }

            w.attrib(col);

            continue;
         }
      }

      if(c == '^')
      {
         highlight = !highlight;

         if(highlight)
            w.attrib(lstrings[i].highlight);
         else
            w.attrib(lstrings[i].color);

         continue;
      }

      if(c == '\n')
      {
         if(++count >= maxheight)
            break;

         tsw_clearrect(w.minX,w.minY+w.getY(),w.maxX,w.minY+w.getY(),w.attrib());
      }

      if(c != '\f')
         w << c;
   }

   w.setPos(old_x,old_y);
   w.attrib(old_attr);

   return count+1;
}

static void
action_func(int i)
{
   tsw_clearrect( 1, tsw_vsize , tsw_hsize , tsw_vsize , BAR_COLOR );
   tsw_mputs( 2, tsw_vsize , default_lstrings[i].s );
   tsw_maputs(2,2, 0x1B, form("Prompt #%-4d",i+1));
}

void
edit_language(char *langname)
{
   ScreenRestore restore;

   tsw_clearrect(1,1,tsw_hsize,2,0x1E);
   tsw_centerline(1,"[Alt-D] Restore default       [Alt-P] Language Properties       [Alt-S] Search");

   KEY hotkeys[] = { KEY_ALTD , KEY_ALTP , KEY_ALTS, KEY_ALTN, 0 };

   ExtSelectWindow sw;

   sw.open( 1,
            3,
            tsw_hsize,
            tsw_vsize - 1,
            0x0F,
            NOBORDER | NOSAVE );

   File f;
   int i;

   CLEAR_OBJECT(lang_file);

   default_lstrings = new LanguageString[ MAX_LSTRINGS ];

   if(!read_default_language())
   {
      tsw_beep();

      return;
   }

   lstrings         = new LanguageString[ MAX_LSTRINGS ];

   for( i=0 ; default_lstrings[i].len ; i++ )
   {
      lstrings[i] = default_lstrings[i];
   }

   num_lstrings = i;

   if(f.open(FileName(syspath,langname,".PBL")))
   {
      f.read(&lang_file , sizeof(LanguageFile));

      word numinfile;

      f >> numinfile;

      for(i=0;i<numinfile;i++)
      {
         if(!lstrings[i].read(f))
            break;
      }

      for( ; default_lstrings[i].len ; i++ )
      {
         lstrings[i] = default_lstrings[i];
      }

      num_lstrings = i;

      f.close();
   }
   else
   {
      delete [] lstrings;
      delete [] default_lstrings;

      return;
   }

   sw.define(num_lstrings,0x70,list_func,hotkeys,action_func);

   String search_spec;
   int last_search_num;

   bool state = tsw_cursorstate;

   tsw_cursoroff();

   for(;;)
   {
      int choice = sw.process();

      if(choice == SL_HOTKEY)
      {
         switch(sw.hotkey)
         {
            case KEY_ALTS:
               {
                  char s[80];

                  Window w;

                  w.open( 10,  
                          7, 
                          60,  
                          9, 
                          0x3F, 
                          SHADOW,
                          CHISEL_BORDER,
                          0x3B,
                          NULL,
                          0x31 );

                  s[0] = '\0';

                  w << " Search for : ";

                  tsw_cursoron();

                  w.attrib(0x31);

                  if(w.scan(s, 35, 32, SCAN_UPPER) == SF_ESC)
                  {
                     tsw_cursoroff();
                     continue;
                  }

                  search_spec = s;

                  tsw_cursoroff();
                  w.close();

                  for(int i=0;i<num_lstrings;i++)
                  {
                     String s = lstrings[i].s;

                     if(s.find(search_spec) >= 0)
                     {
                        sw.show(i);
                        break;
                     }
                  }

                  if(i == num_lstrings)
                  {
                     tsw_beep();
                     tsw_beep();
                  }
                  else
                     last_search_num = i;
               }
               continue;

            case KEY_ALTN:
               {
                  if(search_spec.len() < 2)
                     continue;

                  for(int i=last_search_num+1;i<num_lstrings;i++)
                  {
                     String s = lstrings[i].s;

                     if(s.find(search_spec) >= 0)
                     {
                        sw.show(i);
                        break;
                     }
                  }

                  if(i == num_lstrings)
                  {
                     tsw_beep();
                     tsw_beep();
                  }
                  else
                     last_search_num = i;
               }
               continue;
            case KEY_ALTD:
               {
                  lstrings[sw.current] = default_lstrings[sw.current];
               }
               continue;
            case KEY_ALTP:
               {
                  edit_properties();
               }
               continue;
         }
      }

      if(choice<0) break;

      int prevlines = list_func(sw,choice,0,0);

      edit_language_string( lstrings[choice] );

      if(prevlines != list_func(sw,choice,0,0)) sw.show(choice);
   }

   if(state)
      tsw_cursoron();

   f.open(FileName(syspath,langname,".PBL") , fmode_create , 4096);

   f.write(&lang_file,sizeof(LanguageFile));

   f << num_lstrings;

   for(i=0 ; i<num_lstrings ; i++)
   {
      lstrings[i].write(f);
   }

   delete [] lstrings;
   delete [] default_lstrings;

   tsw_fillrect(1,1,tsw_hsize,1,'°',0x70);
}

static void
edit_language_string( LanguageString& l )
{
   Field frm_all[]=
   {
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,2  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,3  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,4  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,5  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,6  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,7  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,8  },
      { FRM_STRING  , 0 , 0,0              , 0 , 200, 55, 14,9  },

      { FRM_STRING  , 0 , 0,0              , 0 , 49 , 50, 14,11 },

      { FRM_FUNCTION, 0 , 0,edit_color     , 0 , 20 , 20, 14,13 },
      { FRM_FUNCTION, 0 , 0,edit_color     , 0 , 20 , 20, 14,14 },
      { FRM_FUNCTION, 0 , 0,edit_color     , 0 , 20 , 20, 14,15 }
   };

   char *ss[8] =
      {
         new char[201] ,
         new char[201] ,
         new char[201] ,
         new char[201] ,
         new char[201] ,
         new char[201] ,
         new char[201] ,
         new char[201]
      };

   dword crc = 0;

   crc += crc32(l.s);
   crc += crc32(l.hotkeys);
   crc += crc32(&l.color,1);
   crc += crc32(&l,sizeof(l));

   *ss[0] = '\0';
   *ss[1] = '\0';
   *ss[2] = '\0';
   *ss[3] = '\0';
   *ss[4] = '\0';
   *ss[5] = '\0';
   *ss[6] = '\0';
   *ss[7] = '\0';

   char *s = l.s;

   for( int i = 0 , j = 0; *s && i < 8 ; s++)
   {
      if(*s == '\n')
      {
         ss[i++][j] = '\0';
         j = 0;
         continue;
      }

      ss[i][j++] = *s;
   }

   ss[i][j] = '\0';

   char *hk = new char[50];
   strcpy(hk,l.hotkeys);

   Field frm[12];

   int n_frm = 0;

   frm[n_frm++] = frm_all[0];
   frm[n_frm++] = frm_all[1];
   frm[n_frm++] = frm_all[2];
   frm[n_frm++] = frm_all[3];
   frm[n_frm++] = frm_all[4];
   frm[n_frm++] = frm_all[5];
   frm[n_frm++] = frm_all[6];
   frm[n_frm++] = frm_all[7];

   if(l.numHotkeys)
   {
      frm[n_frm  ] = frm_all[8];
      frm[n_frm++].value = hk;
   }

   if(!(l.flags & LANG_NOCOLOR))
   {
      frm[n_frm  ] = frm_all[ 9];
      frm[n_frm++].value = &l.color;
      frm[n_frm  ] = frm_all[10];
      frm[n_frm++].value = &l.highlight;
   }

   if(l.flags & LANG_PROMPT)
   {
      frm[n_frm  ] = frm_all[11];
      frm[n_frm++].value = &l.promptcolor;
   }

   frm[0].value  = ss[0];
   frm[1].value  = ss[1];
   frm[2].value  = ss[2];
   frm[3].value  = ss[3];
   frm[4].value  = ss[4];
   frm[5].value  = ss[5];
   frm[6].value  = ss[6];
   frm[7].value  = ss[7];

   FormWindow fw( 4,
                  5,
                  77,
                  22,
                  0x1F,
                  SHADOW | BRACKETS,
                  CHISEL_BORDER,
                  0x19,
                  NULL,
                  0x10 );

   fw.open();

   fw.title( "Edit Language String", 0x1E );

   fw.attrib(0x1B);

   fw << "\n  String #1:"
         "\n  String #2:"
         "\n  String #3:"
         "\n  String #4:"
         "\n  String #5:"
         "\n  String #6:"
         "\n  String #7:"
         "\n  String #8:\n"
         "\n    Hotkeys: n/a\n"
         "\n      Color: n/a"
         "\n  Highlight: n/a"
         "\n Prompt Col: n/a";

   fw.define( frm,
              n_frm,
              0x1B,
              0x4E );

   tsw_cursoron();
   fw.process();
   tsw_cursoroff();

   for(j=7;j>=0;j--) if(ss[j][0]) break;

   String result = ss[0];

   if(j > 0) result << '\n' << ss[1];
   if(j > 1) result << '\n' << ss[2];
   if(j > 2) result << '\n' << ss[3];
   if(j > 3) result << '\n' << ss[4];
   if(j > 4) result << '\n' << ss[5];
   if(j > 5) result << '\n' << ss[6];
   if(j > 6) result << '\n' << ss[7];

   if( crc32(result)+crc32(hk)+crc32(&l.color,1)+crc32(&l,sizeof(l)) != crc)
   {
      if(ask_save() == 0)
         l = LanguageString(result,hk,l.color,l.highlight,l.flags,l.promptcolor);
   }

   delete [] ss[0];
   delete [] ss[1];
   delete [] ss[2];
   delete [] ss[3];
   delete [] ss[4];
   delete [] ss[5];
   delete [] ss[6];
   delete [] ss[7];
   delete [] hk;
}

static void
edit_properties()
{
   Field frm[]=
      {
         { FRM_STRING  , lang_file.name   , 0,0 , 0         , 40 , 41, 17,2 },
         { FRM_YESNO   , &lang_file.avail , 0,0 , 0         , 3  , 3 , 17,4 },
         { FRM_UNSIGNED, &lang_file.level , 0,0 , 0         , 5  , 6 , 17,6 },
         { FRM_FUNCTION, &lang_file.flags , 0,flag_select ,0, 26 , 26, 17,7 },
         { FRM_STRING  , lang_file.menuPath, 0,0 , FRM_UPPER , 60 , 45, 17,9 },
         { FRM_STRING  , lang_file.txtPath, 0,0 , FRM_UPPER , 60 , 45, 17,10 },
         { FRM_STRING  , lang_file.questPath, 0,0 , FRM_UPPER , 60 , 45, 17,11 }
       //{ FRM_STRING  , lang_file.copyright , 0,0 , 0         , 80 , 45, 17,13 }
      };

   FormWindow w( 6,
                 7,
                 75,
                 20,
                 0x3F,
                 SHADOW | BRACKETS,
                 CHISEL_BORDER,
                 0x3B,
                 NULL,
                 0x31 );

   w.define( frm,
             7,
             0x31,
             0x4E );

   w.open();

   w.title( "Language Properties", 0x3E );

   w.attrib(0x3F);

   w << "\n   Description:\n\n"
          "     Available:\n\n"
          "         Level:\n"
          "         Flags:\n\n"
          "     Menu Path:\n"
          "     Text Path:\n"
          "   Quest. Path:";
        //"     Copyright:";

   tsw_cursoron();
   w.process();
   tsw_cursoroff();

   if(strlen(lang_file.menuPath) >= 3)
      append_backspace(lang_file.menuPath);
   else
      lang_file.menuPath[0] = '\0';

   if(strlen(lang_file.txtPath) >= 3)
      append_backspace(lang_file.txtPath);
   else
      lang_file.txtPath[0] = '\0';

   if(strlen(lang_file.questPath) >= 3)
      append_backspace(lang_file.questPath);
   else
      lang_file.questPath[0] = '\0';
}


static bool
read_default_language()
{
   File f;

   if(!f.open(FileName(syspath,"DEFLANG.PB") , fmode_read , 4096))
      return FALSE;

   f.seek(long(sizeof(LanguageFile) + 2));

   for(int i=0;;i++)
   {
         if(!default_lstrings[i].read(f)) break;
   }

   default_lstrings[i].len = 0;

   return TRUE;
}
