#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <ctype.h>
#include <tswin.hpp>
#include "procfg.hpp"

// de VERSION define staat nu in PROCFG.HPP

extern unsigned _stklen = 10000;

protocol *p;
Config far cfg;
pbconfig far pbcfg;
limits *limit;
aka *akas;
event *events;

int num_proto=0;
int num_limits=0;
int num_aka=0;
int num_events=0;

KEY select_hotkeys[]=
{
   KEY_INS, KEY_DEL ,
   NULL
};

char *yesnoask[] =
{
   "No ","Yes","Ask",
   NULL
};

char *loglevels[]=
{
   "Friend" , "Normal" , "Suspicious" , "Dangerous",
   NULL
};

char *months_short[]=
{
   "   ",
   "Jan","Feb","Mar","Apr","May","Jun",
   "Jul","Aug","Sep","Oct","Nov","Dec"
};

char *days_short[]=
{
   "Mon","Tue","Wed","Thu","Fri","Sat","Sun"
};

char syspath[80];

int
ask_save()
{
   Window w( 16,
             9,
             65,
             16,
             0x70,
             SHADOW | CENTERED_X,
             CHISEL_BORDER,
             0x7F,
             NULL,
             0x78 );
   w.open();

   w.centerLine( 2,
                 "Changes Have Been Made",
                 0xF4 );

   menu_item savemenu[]=
   {
      { 'S', 18,18, "                Save Changes"   },
      { 'C', 17,17, "               Cancel Changes"  },
      {  0 , 0,0, NULL                               }
   };

   PopMenu menu( savemenu,
                 0x70,
                 0xF );

   menu.open( 19,
              13,
              63,
              14,
              0x7,
              NOBORDER | CENTERED_X );

   int choice = menu.process();


   if(choice<1)
      return -1;
   else
      return 2-choice;
}

int
ask_confirm(char *title)
{
   Window w( 16,
             9,
             65,
             15,
             0x70,
             SHADOW | CENTERED_X | CENTERED_Y,
             CHISEL_BORDER,
             0x7F,
             NULL,
             0x78 );

   w.open();

   w.centerLine( 2, title, 0x71 );

   w.centerLine( 4, "Y/N", 0x74 );

   if(KB.ask())
      return 1;
   else
      return 0;
}


struct FlagDesc
{
   char desc[32][40];
};

void
show_flags()
{
   FlagDesc *flagdesc = new FlagDesc;

   CLEAR_OBJECT(*flagdesc);

   File f(FileName(syspath,"FLAGDESC.PB"),fmode_read|fmode_copen);
   f.read(flagdesc,sizeof(*flagdesc));
   f.close();

   FormWindow w( 1,
                 3,
                 80,
                 22,
                 0x70,
                 EXPLODE,
                 CHISEL_BORDER,
                 0x7F,
                 NULL,
                 0x78 );
   w.open();

   w.attrib(0x71);
   w << " A -                                   Q -\n"
        " B -                                   R -\n"
        " C -                                   S -\n"
        " D -                                   T -\n"
        " E -                                   U -\n"
        " F -                                   V -\n"
        " G -                                   W -\n"
        " H -                                   X -\n"
        " I -                                   Y -\n"
        " J -                                   Z -\n"
        " K -                                   1 -\n"
        " L -                                   2 -\n"
        " M -                                   3 -\n"
        " N -                                   4 -\n"
        " O -                                   5 -\n"
        " P -                                   6 -";

   Field frm[] =
   {
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,1  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,2  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,3  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,4  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,5  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,6  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,7  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,8  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,9  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,10 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,11 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,12 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,13 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,14 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,15 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 , 6,16 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,1  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,2  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,3  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,4  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,5  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,6  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,7  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,8  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,9  },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,10 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,11 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,12 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,13 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,14 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,15 },
      { FRM_STRING  , 0 , 0,0 , 0 , 31 , 32 ,44,16 }
   };

   w.attrib(0x7E);

   for(int i = 0 ; i < 32 ; i++)
   {
      w.setPos( i / 16 * 38 + 6 , i % 16 + 1 );
      w << flagdesc->desc[i];

      frm[i].value = flagdesc->desc[i];
   }

   w.direct(4,18,0x74," [Alt-E] Edit ");

   for(;;)
   {
      KEY k = KB.get();

      if(k == KEY_ESC)
         break;

      if(k == KEY_ALTE)
      {
         w.define(frm,32,0x1E,0x4E);
         tsw_cursoron();
         w.process();
         tsw_cursoroff();

         f.open(FileName(syspath,"FLAGDESC.PB"),fmode_create);
         f.write(flagdesc,sizeof(*flagdesc));
         f.close();
      }
   }

   delete flagdesc;
}

KEY
flag_select(int mode,Window& w,int x,int y,char attr,void *data)
{
   AccessFlags *af=(AccessFlags *)data;
   char str[33];

   tsw_cursoroff();

   if(mode)
   {
      Window ww( x + w.minX + 3,
                 y + w.minY - 4,
                 x + w.minX + 42,
                 y + w.minY + 2,
                 0x70,
                 SHADOW,
                 CHISEL_BORDER,
                 0x7F,
                 NULL,
                 0x78 );

      ww.open();

      ww.centerLine( 1, "Press A..Z , 1..6 to toggle", 0x71 );
    //ww.title(" Press A..Z , 1..6 to toggle ",0xE);
      ww.direct(9,5,0x70," [F1] - Flag Help ");

      for(;;)
      {
         af->flagstostr(str);

         ww.direct( 2,  3, 0x4E, " " );
         ww.direct( 3,  3, 0x4E, str );
         ww.direct( 35, 3, 0x4E, " " );

         KEY k=KB.get();

         if(k == KEY_F1)
            show_flags();

         if(k==27 || k==13)
            return 0;

         if(isalpha(k))
         {
            k=toupper(k)-'A'+1;
            if(af->getflag(k))
               af->clearflag(k);
            else
               af->setflag(k);
         }
         else
         {
            if(k>='1' && k<='6')
            {
               k -= '1'-27;
               if(af->getflag(k))
                  af->clearflag(k);
               else
                  af->setflag(k);
            }
         }
      }
   }

   af->flagstostr(str);
   w.direct(x,y,attr,form("%s ",str));
   tsw_cursoron();

   return 0;
}

void
aka_select_func(int i,int x,int y)
{
   tsw_maputs( x + 1,
               y,
               0x5F,
               form( "%d:%d/%d.%d",
                     akas[ i ].zone,
                     akas[ i ].net,
                     akas[ i ].node,
                     akas[ i ].point ) );
}

KEY
aka_select(int mode,Window& w,int x,int y,char attr,void *data)
{
   int a=*((int *)data);

   tsw_cursoroff();

   tsw_showfooter("Select an AKA address", BAR_COLOR);

   if(!mode)
   {
      char s[30];
      sprintf(s,"%d:%d/%d.%d",akas[a].zone,akas[a].net,akas[a].node,akas[a].point);

      w.direct(x,y,attr,form("%-20.20s",s));
   }
   else
   {
      SelectWindow sw(num_aka,0x70,aka_select_func);

      sw.open( w.minX + x - 2,
               w.minY + y - 8,
               w.minX + x + 22,
               w.minY + y + 2,
               0x70,
               SHADOW | BRACKETS,
               CHISEL_BORDER,
               0x7F,
               NULL,
               0x78 );

      sw.title( "Select AKA", 0x70 );

      int i=sw.process();

      if(i<0)
         return 0;

      *((int *)data)=i;
   }

   tsw_cursoron();

   return 0;
}

KEY
edit_timeframe(int mode,Window& w,int x,int y,char attr,void *data)
{
   TimeFrame *tf = (TimeFrame *) data;

   if(!mode)
   {
      TimeFrame ref;
      bool empty,full;

      empty = !memcmp(&ref,tf,sizeof(ref));
      ref.setAll();
      full  = !memcmp(&ref,tf,sizeof(ref));

      if(full )
         w.direct(x,y,attr,   "Fully enabled        ");
      else
         if(empty)
            w.direct(x,y,attr,"Fully disabled       ");
         else
            w.direct(x,y,attr,"Press Enter to edit  ");

      return 0;
   }

   tsw_clearrect(1,tsw_vsize,tsw_hsize,tsw_vsize,BAR_COLOR);
   tsw_centerline(tsw_vsize,"Use [Space] to toggle segment on/off.  Alt-E/D: Enable/Disable All", BAR_COLOR);

   tsw_cursoroff();

   int ystart = y + w.minY - 4;

   if(ystart < 3) ystart = 3;
   if(ystart > 16) ystart = 16;

   int y_base  =  ystart - 4;


   while ( y_base < 0 )
   {
     y_base++;
   }


   Window ww( 1,
              y_base,
              80,
              y_base + 12,
              0x70,
              BRACKETS,
              CHISEL_BORDER,
              0x7F,
              NULL,
              0x78);

   ww.open();

   ww.title( "Time Frames", 0x70 );

   ww.attrib(0xF);
   
   ww.RecessBox( 3,
                 1,
                 75,
                 11,
                 RECESS_BORDER,
                 0x18,
                 0x1F,
                 0x18,
                 0x7F );

   ww.attrib(0x1F);
   ww.direct(4, 2,  "00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23");


   ww.attrib(0x17);

   for ( int i = 6;  i < 75;  i += 3 )
   {
     ww.direct( i, 2, "³" );  
   }
   
   ww.direct(4, 3,  "ÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍØÍÍ");
   ww.direct(4, 4,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 5,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 6,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 7,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 8,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 9,  "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");
   ww.direct(4, 10, "  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³  ³");

   ww.attrib(0x70);
   
   ww.direct( 1, 4,  "Mo" );
   ww.direct( 1, 5,  "Tu" );
   ww.direct( 1, 6,  "We" );
   ww.direct( 1, 7,  "Th" );
   ww.direct( 1, 8,  "Fr" );
   ww.direct( 1, 9,  "Sa" );
   ww.direct( 1, 10, "Su" );

   ww.attrib(0xB);

   int cur_dow , prev_dow;
   int cur_tm  , prev_tm ;

   for(cur_dow = 0;cur_dow<7;cur_dow++)
      for(cur_tm = 0;cur_tm<48;cur_tm++)
      {
         ww.direct(4+cur_tm +cur_tm/2  , cur_dow+4  , 0x1E , tf->enabled(cur_dow,cur_tm)   ? 'þ' : 'ú');
      }

   cur_dow = prev_dow = 0;
   cur_tm  = prev_tm  = 0;

   for(;;)
   {
      ww.direct(4+prev_tm+prev_tm/2 , prev_dow+4 , 0x1E , tf->enabled(prev_dow,prev_tm) ? 'þ' : 'ú');
      ww.direct(4+cur_tm +cur_tm/2  , cur_dow+4  , 0x7E , tf->enabled(cur_dow,cur_tm)   ? 'þ' : 'ú');

      prev_tm = cur_tm;
      prev_dow = cur_dow;

      KEY k = KB.get();

      if(k == KEY_ESC)
         break;

      switch(k)
      {
         case KEY_UP:
         {
            if(cur_dow < 1) cur_dow = 7;
               cur_dow--;
         }
         break;

         case KEY_DN:
         {
            if(cur_dow > 5) cur_dow = -1;
               cur_dow++;
         }
         break;

         case KEY_LT:
         {
            if(cur_tm < 1) cur_tm = 48;
               cur_tm--;
         }
         break;

         case ' '   :
         {
            if(tf->enabled(cur_dow,cur_tm))
               tf->disable(cur_dow,cur_tm);
            else
               tf->enable(cur_dow,cur_tm);
         }                             // NO BREAK!!!

         case KEY_RT:
         {
            if(cur_tm > 46) cur_tm = -1;
               cur_tm++;
         }
         break;

         case KEY_ALTE:
         {
            tf->setAll();

            for(int d = 0;d<7;d++)
               for(int t = 0;t<48;t++)
               {
                  ww.direct(4+t +t/2  , d+4  , 0x1E , tf->enabled(d,t)   ? 'þ' : 'ú');
               }
         }
         break;

         case KEY_ALTD:
         {
            tf->clear();

            for(int d = 0;d<7;d++)
               for(int t = 0;t<48;t++)
               {
                  ww.direct(4+t +t/2  , d+4  , 0x1E , tf->enabled(d,t)   ? 'þ' : 'ú');
               }
         }
         break;

         case KEY_HOME: cur_tm  =  0;  break;
         case KEY_END : cur_tm  = 47;  break;
         case KEY_PGUP: cur_dow =  0;  break;
         case KEY_PGDN: cur_dow =  6;  break;
      }
   }

   tsw_cursoroff();

   return 0;
}

KEY
edit_color(int mode,Window& ww,int x,int y,char attr,void *data)
{
   byte col         = *((byte *)data);
   byte fg          = col & 0xF;
   byte bg          = (col & 0x70) >> 4;
   bool blink       = !!(col & 0x80);

   if(!mode)
   {
      ww.direct(x,y,attr,"Press [Enter] to edit");
      ww.direct(x+24,y,col,"[Example Text Line]");

      return 0;
   }

   int starty = ww.minY+y-7;

   if(starty<2)
      starty = 2;

   Window w( ww.minX + x,
             starty - 1,
             ww.minX + x + 26,
             starty  + 12, 
             0x70,
             BRACKETS | SHADOW,
             CHISEL_BORDER,
             0x7F,
             NULL,
             0x78 );

   w.open();

   w.title( "Edit Color", 0x70);

   w.RecessBox( 2,
                1,
                22,
                12,
                RECESS_BORDER,
                0x08,
                0x0F,
                0x08,
                0x7F );


   for ( int i = 2;  i < 12;  i++ )
   {
     w.attrib( 0x00 );

     w.direct( 3, i, "                   " );
   }

   int curx = fg , cury = bg;

   for(;;)
   {
      for(byte i=0;i<0x80;i++)
      {
         w.direct(i%16+4 , i/16+3 , i ,'X');
      }

      if(blink) w.direct(curx+4,cury+3,(cury << 4) | curx | 0x80 , 'B');

      w.direct(curx+3 , cury+3 , 0xf , '³'  );
      w.direct(curx+3 , cury+2 , 0xf , "ÚÄ¿");
      w.direct(curx+5 , cury+3 , 0xf ,   '³'  );
      w.direct(curx+3 , cury+4 , 0xf , "ÀÄÙ");

      KEY k = KB.get();

      if(k == KEY_ESC) return 0;

      if(k == KEY_RET) break;

      if(k == KEY_ALTB) blink = !blink;

      w.direct(curx+3 , cury+3 , 0xf , ' '  );
      w.direct(curx+3 , cury+2 , 0xf , "   ");
      w.direct(curx+5 , cury+3 , 0xf ,   ' ');
      w.direct(curx+3 , cury+4 , 0xf , "   ");

      switch(k)
      {
         case KEY_UP:
         {
            if(cury)
               cury--;
         }
         break;

         case KEY_DN:
         {
            if(cury < 7)
               cury++;
         }
         break;

         case KEY_LT:
         {
            if(curx)
               curx--;
         }
         break;

         case KEY_RT:
         {
            if(curx < 15)
               curx++;
         }
         break;
      }
   }

   fg = curx;
   bg = cury;

   *((byte *)data) = byte((bg << 4) | fg | (blink ? 0x80:0x00));

   return 0;
}

main(int argc,char *argv[])
{
   if(farcoreleft() < 200000L)
   {
      printf("\nInsufficient memory! (%ld Kb extra needed)\n",(200000L-farcoreleft())/1024L);
      return 1;
   }

   char *modes[] =
   {
      "OP"    ,           //  [F1] Options
      "PR"    ,           //  [F2] Protocol Configuration
      "MA"    ,           //  [F3] Message areas
      "MG"    ,
      "FA"    ,           //  [F4] File areas
      "FG"    ,
      "LI"    ,           //  [F5] Time/Download Limits
      "UE"    ,           //  [F6] User Editor
      "ME"    ,           //  [F7] Menu Editor
      "AK"    ,           //  [F8] Matrix Addresses
      "MO"    ,           //  [F9] Modem Parameters
      "MAC"   ,           // [F10] Sysop Macros
      "EV"    ,           // [F1] Events
      "PF"    ,           // [F2] Personal Files
      "LE"    ,           // [F3] Language Editor
      "VTE"   ,           // [F4] Validate Template Editor
      NULL
   };

   KEY  keys  [] =
   {
      KEY_F1  ,           //  [F1] Options
      KEY_F2  ,           //  [F2] Protocol Configuration
      KEY_F3  ,           //  [F3] Message areas
      KEY_F4  ,           //  [F4] File areas
      KEY_F5  ,           //  [F5] Time/Download Limits
      KEY_F6  ,           //  [F6] User Editor
      KEY_F7  ,           //  [F7] Menu Editor
      KEY_F8  ,           //  [F8] Matrix Addresses
      KEY_F9  ,           //  [F9] Modem Parameters
      KEY_F10 ,           // [F10] Sysop Macros
      KEY_SF1 ,           // [F1] Events
      KEY_SF2 ,           // [F2] Personal Files
      KEY_SF3 ,           // [F3] Language Editor
      KEY_SF4 ,           // [F4] Validate Template Editor
      KEY_SF5 ,           // [F4] Validate Template Editor
      KEY_SF6 ,           // [F4] Validate Template Editor
      KEY_SF9             // [F4] Validate Template Editor
   };

   menu_item mainmenu[]=
   {
      { KEY_F1    ,  3, 4, "  F1 - Options"                    },
      { KEY_F2    ,  3, 4, "  F2 - Protocol Configuration"     },
      { KEY_F3    ,  3, 4, "  F3 - Message Areas"              },
      { KEY_F4    ,  3, 4, "  F4 - Message Area Groups"        },
      { KEY_F5    ,  3, 4, "  F5 - File Areas"                 },
      { KEY_F6    ,  3, 4, "  F6 - File Area Groups"           },
      { KEY_F7    ,  3, 4, "  F7 - Time/Download Limits"       },
      { KEY_F8    ,  3, 4, "  F8 - User Editor"                },
      { KEY_F9    ,  3, 4, "  F9 - Menu Editor"                },
      { KEY_F10   ,  2, 4, " F10 - Matrix Addresses"           },
      { KEY_SF1   ,  2, 4, " F1 - Modem Parameters"           },
      { KEY_SF2   ,  2, 4, " F2 - Sysop Macros"               },
      { KEY_SF3   ,  2, 4, " F3 - Events"                     },
      { KEY_SF4   ,  2, 4, " F4 - Personal Files"             },
      { KEY_SF5   ,  2, 4, " F5 - Language Editor"            },
      { KEY_SF6   ,  2, 4, " F6 - Validate Template Editor"   },
      { KEY_SF9   ,  2, 4, " F9 - About ProCFG"               },
      { 0         ,  0, 0, NULL                                  }
   };

   ScreenRestore restore(1);

   unsigned events_pro_crc;
   unsigned aka_pro_crc;
   unsigned limits_pro_crc;
   unsigned protocol_pro_crc;
   unsigned proboard_cfg_crc;

   p        = new protocol[MAX_PROTOCOL];
   limit    = new limits  [MAX_LIMITS  ];
   akas     = new aka     [MAX_AKA     ];
   events   = new event   [MAX_EVENT   ];

   memset(akas,0,sizeof(aka));

   syspath[0]=0;
   if(getenv("PROBOARD")!=NULL)
   {
      strcpy(syspath,getenv("PROBOARD"));
      append_backspace(syspath);
   }

   FileName fn(argv[0]);
   if(!syspath[0])
   {
      fn.stripName();
      strcpy(syspath,fn);
      append_backspace(syspath);
   }

   tsw_cursoroff();
   tsw_fillscreen( '±', 0x19 );
   
   Window t( 1,
             1,
             tsw_hsize,
             3,
             0x70,
             0,
             CHISEL_BORDER,
             0x7F,
             NULL,
             0x78 );

   t.open();



   t.attrib( 0x71 );
   t.centerLine( 1, "ProBoard Configuration Utility v" VERSION );


   PopMenu menu( mainmenu,
                 0x1E,
                 0x7E );

   menu.ShadowColor( 0x10 );

   menu.open( 23,
              5,
              58,
              23,
              0x70,
              SHADOW | EXPLODE | NOSAVE,
              CHISEL_BORDER,
              0x7F,
              NULL,
              0x78 ); // main menu

   //menu.border(0x1F);
   //tsw_clearrect(1,1,tsw_hsize,1,0x1E);

   memset(&cfg,0,sizeof(cfg));

   File f;

   memset (&cfg,0,sizeof(cfg));

   strcpy (cfg.shellmsg     ,"Please Wait..."           );
   strcpy (cfg.sysopname    ,"Philippe Leybaert"        );
   strcpy (cfg.editorname   ,"*SGEDIT.EXE"              );
   strcpy(cfg.originline    ,"Your BBS description"     );
   strcpy(cfg.quotestring   ,"@> "                      );

   sprintf(cfg.txtpath      ,"%sTXTFILES"       ,syspath);
   sprintf(cfg.mnupath      ,"%sMENUS"          ,syspath);
   sprintf(cfg.msgpath      ,"%sMSGBASE"        ,syspath);
   sprintf(cfg.uploadpath   ,"%sFILES\\UPLOADS" ,syspath);
   sprintf(cfg.pexpath      ,"%sPEX"            ,syspath);
   sprintf(cfg.nodelistdir  ,"%sNODELIST"       ,syspath);
   sprintf(cfg.pvtuploadpath,"%sPVTFILES"       ,syspath);

   cfg.newuserlevel    = 10   ;
   cfg.newuserloglevel = 2    ;
   cfg.max_passinput   = 3    ;
   cfg.min_passlength  = 3    ;
   cfg.inactivity_time = 240  ;
   cfg.max_sysop_pages = 2    ;
   cfg.pagebell_length = 20   ;
   cfg.mailcheck       = 1    ;
   cfg.europe          = 1    ;
   cfg.numnodes        = 1    ;
   cfg.allowansi       = 1    ;
   cfg.discrete        = 0    ;
   cfg.askvoicephone   = 1    ;
   cfg.askdataphone    = 1    ;
   cfg.askfaxphone     = 0    ;
   cfg.asksex          = 1    ;
   cfg.askdateformat   = 1    ;
   cfg.askaddress      = 1    ;
   cfg.allowoneword    = 0    ;
   cfg.crashlevel      = 100  ;
   cfg.attachlevel     = 100  ;
   cfg.allowmsgupload  = 1    ;
   cfg.allowstacking   = 1    ;
   cfg.page_start[0]   = 18   ;
   cfg.page_end[0]     = 23   ;
   cfg.handshaking     = 2    ;
   cfg.loglocal        = 1    ;
   cfg.doswap          = 1    ;
   cfg.dl_start[0]     = 2    ;
   cfg.dl_end[0]       = 17   ;
   cfg.uploadspace     = 300  ;
   cfg.fastmode        = 0    ;
   cfg.killsent        = 0    ;
   cfg.allowquicklogin = 1    ;
   cfg.hidePassword    = TRUE ;
   cfg.valConfirm      = FALSE;
   cfg.fuzzyRate       = 70   ;

   proboard_cfg_crc    = 0xFFFF;

   fn(syspath,"CONFIG.PRO");
   if(f.open(fn,fmode_read))
   {
      f.read(&cfg,sizeof(cfg));
      f.close();

      proboard_cfg_crc=crc(&cfg,sizeof(cfg));
   }

   if(cfg.fuzzyRate < 10)
      cfg.fuzzyRate = 75;
   if(cfg.fuzzyRate > 100)
      cfg.fuzzyRate = 100;

   protocol_pro_crc=0xFFFF;
   fn(syspath,"PROTOCOL.PRO");
   if(f.open(fn))
   {
      for(num_proto=0;;num_proto++)
         if(f.read(&p[num_proto],sizeof(*p))!=sizeof(*p))
            break;

      f.close();
      protocol_pro_crc=crc(p,sizeof(*p)*num_proto);
   }

   limits_pro_crc=0xFFFF;
   fn(syspath,"LIMITS.PRO");
   if(f.open(fn))
   {
      for(num_limits=0;;num_limits++)
         if(f.read(&limit[num_limits],sizeof(*limit))!=sizeof(*limit))
            break;

      f.close();
      limits_pro_crc=crc(limit,sizeof(*limit)*num_limits);
   }

   aka_pro_crc=0xFFFF;
   fn(syspath,"AKA.PRO");
   if(f.open(fn))
   {
      for(num_aka=0;;num_aka++)
         if(f.read(&akas[num_aka],sizeof(aka))!=sizeof(aka))
            break;

      f.close();
      aka_pro_crc=crc(akas,sizeof(aka)*num_aka);
   }

   events_pro_crc=0xFFFF;
   fn(syspath,"EVENTS.PRO");
   if(f.open(fn))
   {
      for(num_events=0;;num_events++)
         if(f.read(&events[num_events],sizeof(event))!=sizeof(event))
            break;

      f.close();
      events_pro_crc=crc(events,sizeof(event)*num_events);
   }

   if(argc > 1)
   {
      for(int i = 0; modes[i] ; i++)
      {
         if(!strnicmp(argv[1],modes[i],strlen(modes[i])))
         {
            KB.push(keys[i]);
            break;
         }
      }
   }

   again:

   for(;;)
   {
      tsw_showfooter("Copyright (c) 1990-1999  TeleGrafix Communications, Inc.  All Rights Reserved", BAR_COLOR);
      //tsw_fillrect(1,tsw_vsize-1,tsw_hsize,tsw_vsize-1,'°',0x70);

      int choice=menu.process();
      if(choice<1)
         break;

      switch(choice)
      {
         case 1:  editSysParam   (                          ); break;
         case 2:  edit_protocol  (                          ); break;
         case 3:  edit_areas     ( 0                        ); break;
         case 4:  edit_mgroups   (                          ); break;
         case 5:  edit_areas     ( 1                        ); break;
         case 6:  edit_fgroups   (                          ); break;
         case 7:  edit_limits    (                          ); break;
         case 8:  editUser       ( syspath, argc-2,&argv[2] ); break;
         case 9:  menu_editor    ( argc-2,&argv[2]          ); break;
         case 10: edit_akas      (                          ); break;
         case 11: edit_pb        (                          ); break;
         case 12: edit_macros    (                          ); break;
         case 13: edit_events    (                          ); break;
         case 14: editPvtFile    (                          ); break;
         case 15: select_language( TRUE                     ); break;
         case 16: editTemplate   ( syspath                  ); break;
         case 17: showAbout      (                          ); break;
      }

      if(argc > 2)
         break;
   }

   if(   events_pro_crc   != crc(events  ,sizeof(event)*num_events)
      || aka_pro_crc      != crc(akas    ,sizeof(aka)*num_aka)
      || limits_pro_crc   != crc(limit   ,sizeof(*limit)*num_limits)
      || protocol_pro_crc != crc(p       ,sizeof(*p)*num_proto)
      || proboard_cfg_crc != crc(&cfg    ,sizeof(cfg))
     )
   {
      int choice = ask_save();
      if(choice<0)
         goto again;

      if ( choice == 0 )
      {
         //---------------  
         // Cancel changes
         //---------------  

         FileName fn2;

         if(!num_proto || protocol_pro_crc != crc(p,sizeof(*p)*num_proto))
         {
            fn2(syspath,"PROTOCOL.BAK");
            unlink(fn2);
            fn(syspath,"PROTOCOL.PRO");
            rename(fn,fn2);
            f.open(fn,fmode_create);
            f.write(p,sizeof(protocol)*num_proto);
            f.close();
         }

         if(proboard_cfg_crc != crc(&cfg,sizeof(cfg)))
         {
            fn2(syspath,"CONFIG.BAK");
            unlink(fn2);
            fn(syspath,"CONFIG.PRO");
            rename(fn,fn2);
            f.open(fn,fmode_create);
            f.write(&cfg,sizeof(cfg));
            f.close();
         }

         if(!num_limits || limits_pro_crc   != crc(limit,sizeof(*limit)*num_limits))
         {
            fn2(syspath,"LIMITS.BAK");
            unlink(fn2);
            fn(syspath,"LIMITS.PRO");
            rename(fn,fn2);
            f.open(fn,fmode_create);
            f.write(limit,sizeof(*limit)*num_limits);
            f.close();
         }

         if(!num_aka || aka_pro_crc      != crc(akas,sizeof(aka)*num_aka))
         {
            fn2(syspath,"AKA.BAK");
            unlink(fn2);
            fn(syspath,"AKA.PRO");
            rename(fn,fn2);
            f.open(fn,fmode_create);
            f.write(akas,sizeof(aka)*num_aka);
            f.close();
         }

         if(!num_events || events_pro_crc   != crc(events,sizeof(event)*num_events))
         {
            fn2(syspath,"EVENTS.BAK");
            unlink(fn2);
            fn(syspath,"EVENTS.PRO");
            rename(fn,fn2);
            f.open(fn,fmode_create);
            f.write(events,sizeof(event)*num_events);
            f.close();
         }
      }

   }

   tsw_cursoron();
   return 0;
}

