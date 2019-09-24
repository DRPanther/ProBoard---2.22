#include <dos.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <tswin.hpp>
#include "procfg.hpp"

struct MenuItem : _MenuItem
   {
      MenuItem()
         {
            CLEAR_OBJECT(*this);
            access.timeFrame.setAll();
            access.terminals = 0xFF;
         }
   };

struct MenuFile : _MenuFile {};

typedef MenuItem *pMenuItem;

#define MAX_MENU_ITEMS 255

static char menunames[200][13];

KEY
colorselect(int mode,Window& ww,int x,int y,char attr,void *data)
{
char *colnames[]={ "Black","Blue","Green","Cyan","Red","Magenta","Yellow","White"};
char cols[]={0,4,2,6,1,5,3,7};
char col=*((char *)data);
char fg=col&0x7;
char bg=(col&0x70)>>4;
char dark=(col&0x8)>>3;

fg=cols[fg];
bg=cols[bg];

col=(bg<<4)|fg;
if(!dark) col|=0x8;

if(!mode)
  {
  String s;
  if(!dark) s="Bright ";
  s << form("%s on %s",colnames[fg],colnames[bg]);
  ww.direct(x,y,attr,form("%-25.25s",(char *)s));
  ww.direct(x+26,y,col," Example Text ");
  return 0;
  }

   int starty = ww.minY+y-7;

   if(starty<1)
      starty = 1;

Window w( ww.minX + x + 2,
          starty,
          ww.minX + x + 23,
          starty + 11,
          0xF, 
          EXPLODE | BRACKETS,
          CHISEL_BORDER,
          0x07,
          NULL,
          0x08 );

w.open();
w.title("Edit Color",0xE);

int curx=fg,cury=bg;
if(!dark) curx|=8;

for(;;)
  {
  for(byte i=0;i<0x80;i++)
    {
    w.direct(i%16+2,i/16+2,i,'X');
    }
  w.direct(curx+1,cury+2,0xf,'³');
  w.direct(curx+1,cury+1,0xf,"ÚÄ¿");
  w.direct(curx+3,cury+2,0xf,'³');
  w.direct(curx+1,cury+3,0xf,"ÀÄÙ");


  KEY k=KB.get();
  if(k==KEY_ESC) return 0;
  if(k==KEY_RET) break;
  w.direct(curx+1,cury+2,0xf,' ');
  w.direct(curx+1,cury+1,0xf,"   ");
  w.direct(curx+3,cury+2,0xf,' ');
  w.direct(curx+1,cury+3,0xf,"   ");
  switch(k)
    {
    case KEY_UP: if(cury) cury--;    break;
    case KEY_DN: if(cury<7) cury++; break;
    case KEY_LT: if(curx) curx--;    break;
    case KEY_RT: if(curx<15) curx++; break;
    }
  }

fg=cols[curx&7];
bg=cols[cury];
dark=curx&8;

col=(bg<<4)|fg;
if(!dark) col|=8;

*((char *)data)=col;

return 0;
}

static void
menu_select_func(int i,int x,int y)
{
tsw_maputs(x+1,y,0x5F,menunames[i]);
}

static LanguageFile far langheader;

void
menu_editor(int n,char *args[])
{
   File lf;

   String langfile = select_language(FALSE);

   if(!langfile.len())
      return;

   if(langfile != "*")
   {
      if(!lf.open(FileName(syspath,langfile,".PBL")))
      {
         tsw_beep();
         tsw_beep();
         tsw_beep();
         tsw_beep();
         tsw_beep();

         return;
      }

      lf.read(&langheader , sizeof(LanguageFile));
      lf.close();

      if(langheader.menuPath[0] == '\0')
         strcpy(langheader.menuPath,cfg.mnupath);

      if(langheader.txtPath[0] == '\0')
         strcpy(langheader.txtPath,cfg.txtpath);
   }
   else
   {
      strcpy(langheader.menuPath,cfg.mnupath);
      strcpy(langheader.txtPath,cfg.txtpath);
   }

   String searchpath(langheader.menuPath);
   searchpath << "*.PBM";
   int num_menus;

if(n >= 1)
  {
   edit_menu(args[0]);
   return;
  }
 else
  {
   DirScan f(searchpath);

   for(int i=0;int(f);i++)
    {
    FileName fn(f.name());
    fn.stripExt();
    strcpy(menunames[i],fn);
    f++;
    };
   num_menus=i;

   qsort(menunames,num_menus,13,(int (*)(const void *,const void *))stricmp);
  }

KEY hotkeys[]={ KEY_INS , 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                KEY_DEL , 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                          '1','2','3','4','5','6','7','8','9','0','_','-', 8 , NULL };

SelectWindow sw( num_menus,
                 0x70,
                 menu_select_func,
                 0,
                 NULL,
                 hotkeys );

sw.open( 3,
         3,
         18,
         22,
         0x50,
         SHADOW | EXPLODE | BRACKETS,
         CHISEL_BORDER,
         0x5F,
         NULL,
         0x58 );

sw.title( "Select", 0x5E );

int index = 0;
int found = -1;
String cur_title;

for(;;)
  {
  tsw_showfooter("\x1<Ins>\x1 - Create Menu          \x1<Enter>\x1 - Edit Menu", BAR_NORM, BAR_HIGH );
  sw.redefine(num_menus);
  int choice=sw.process();


  if(choice==SL_HOTKEY)
     {
      if(sw.hotkey == KEY_INS)
        {
         Window w( 10,
                   10,
                   32,
                   12,
                   0x3F,
                   SHADOW | EXPLODE,
                   CHISEL_BORDER,
                   0x3F,
                   NULL,
                   0x38 );
         w.open();
         w.attrib(0x3E);
         w << " Create: ";
         w.attrib(0x31);
         tsw_cursoron();
         w.scan(menunames[num_menus],8U,9U,SCAN_UPPER);
         tsw_cursoroff();
         if(!menunames[num_menus][0]) continue;
         choice=num_menus++;
         sw.current=choice;
        }
       else if(sw.hotkey == KEY_DEL)
       {
         Window w( 1,
                   1,
                   40,
                   3,
                   0x4F,
                   SHADOW | EXPLODE | CENTERED_X | CENTERED_Y,
                   CHISEL_BORDER,
                   0x4C,
                   NULL,
                   0x40 );
         w.open();
         char *s = menunames[sw.current];
         w.centerLine(1,form("Delete menu '%s' ? [y/N]",s),0x4E);
         if(toupper(KB.get())=='Y')
         {
            FileName fn(langheader.menuPath,s,".PBM");
            unlink(fn);
         }

         continue;
       }
       else
        {
         if((sw.hotkey<0x0100 && isalnum(toupper(char(sw.hotkey)))) || sw.hotkey == '_' || sw.hotkey == '-')
           {
            if(sw.current != found)
              {
               cur_title = "";
               index = 0;
               found = -1;
              }

            String tmp_str = cur_title + String(char(toupper(char(sw.hotkey))));

            for(int i = 0 ; i < num_menus ; i++)
              if(strlen(menunames[i]) >= tmp_str.len() && !strnicmp(tmp_str,menunames[i],tmp_str.len()))
                {
                 found = i;
                 sw.show(found);
                 cur_title = tmp_str;
                 index++;
                 break;
                }

            if(i == num_menus) tsw_beep();
           }
          else
           {
            found = -1;
            index = 0;
            cur_title = "";
           }

         sw.title(cur_title,0x5e);

         continue;
        }
     }

  if(choice<0) break;
  edit_menu(menunames[choice]);
  }
}

struct m_item
  {
  union {
   char type;
   char highlight;
   };
  unsigned level;
  AccessFlags flags;
  char string[76];
  char key;
  char data[81];
  char color;
  byte attr;
  };

const byte MNU_RIP_SHOW_REMOTE = 1;
const byte MNU_RIP_HIDE_LOCAL  = 2;
const byte MNU_RIP_NO_RESET    = 4;

static MenuItem **mitem;
static MenuFile *mfile;

static int num_mitems;

char *menufunctions[]=
         {
          " 0 - Display only",
          " 1 - Goto menu",
          " 2 - Gosub menu",
          " 3 - Goto previous menu",
          " 4 - Clear stack and goto menu",
          " 5 - Show ANS/ASC file",
          " 6 - Change State",
          " 7 - Shell",
          " 8 - Version information",
          " 9 - Logoff",
          "10 - System usage graph by hour",
          "11 - Chat request",
          "12 - Execute Questionnaire",
          "13 - User list",
          "14 - Time info",
          "15 - Show .A??-file & wait",
          "16 - Change City",
          "17 - Change Password",
          "18 - Change Screen Size",
          "19 - Toggle Clearscreen",
          "20 - Toggle More Prompt",
          "21 - Toggle ANSI graphics",
          "22 - Check for mail & files",
          "23 - Read messages",
          "24 - Scan messages",
          "25 - Qscan messages",
          "26 - System usage graph by day",
          "27 - Write a message",
          "28 - Combined boards select",
          "29 - System usage graph per week",
          "30 - Show directory",
          "31 - List files",
          "32 - Download a file",
          "33 - Upload a file",
          "34 - View archive",
          "35 - Keyword search",
          "36 - Filename search",
          "37 - Show new files",
          "38 - View a file",
          "39 - View a named file",
          "40 - Display .A?? file w/hotkeys",
          "41 - Toggle fullscreen-editor",
          "42 - Toggle hotkeys",
          "43 - Clear marked messages",
          "44 - Global boards selection",
          "45 - Display textfile & wait",
          "46 - Change User's level/flags",
          "47 - Make a log entry",
          "48 - Display user hitparade",
          "49 - Select New MsgArea",
          "50 - Show users online",
          "51 - List last callers",
          "52 - Remote User Editor",
          "53 - Multi-line chat",
          "54 - Select New FileArea",
          "55 - View GIF-file type",
          "56 - Toggle IBM characters",
          "57 - Change Phone #",
          "58 - Change Data/Fax Phone #",
          "59 - Change Alias",
          "60 - Run ProBoard PEX File",
          "61 - Display Bulletin Menu",
          "62 - Toggle AVATAR",
          "63 - Toggle AVT/0+",
          "64 - Show graph (general)",
          "65 - Display A??/RIP file w/hotkeys",
          "66 - Change RIPscrip font",
          "67 - Toggle RIPscrip graphics",
          "68 - Edit Tagged Files",
          "69 - Select a new language",
          "70 - Change date format" ,
          "71 - Change mailing address" ,
          "72 - Change fax number",
          "73 - Change country",
          "74 - Change default protocol",
          "75 - Select message area group",
          "76 - Select file area group"
          };

static void simulate();

static void
mitem_select_func(int i,int x,int y)
{
   if(i>=num_mitems) return;

   char *s = mitem[i]->text;

   while(*s==' ') s++;

   if(mitem[i]->function)
      tsw_maputs(x+1,y,0x1E,form("%c ³ %-22.22s³ %-25.25s³ %-20.20s",mitem[i]->hotKey,s,&menufunctions[mitem[i]->function][5],mitem[i]->data));
   else
      tsw_maputs(x+1,y,0x1E,form("  ³ %-22.22s³                          ³",s,mitem[i]->data));
}

void
menufunc_select_func(int i,int x,int y)
{
   tsw_maputs(x+1,y,0x3E,menufunctions[i]);
}

KEY
func_select(int mode,Window& w,int x,int y,char attr,void *data)
{
   char *func=(char *)data;

   if(!mode)
      w.direct(x,y,attr,form("%-30.30s",menufunctions[*func]));
   else
   {
      SelectWindow sw( 77,
                       0x70,
                       menufunc_select_func );

      sw.open( 17,
               2,
               63,
               23,
               0x3F,
               SHADOW | EXPLODE | BRACKETS,
               CHISEL_BORDER,
               0x3F,
               NULL,
               0x38 );

      sw.title( "Select a function" );

      sw.show(*func);
      int i=sw.process();
      if(i<0) return 0;
      *func=i;
   }

   return 0;
}

void
edit_menu(char *menu)
{
 int i;
 static int runcount=0;
 static MenuItem copy_mitem;

 if(!runcount++) CLEAR_OBJECT(copy_mitem);

 FileName fn(langheader.menuPath,menu);
 fn << ".PBM";

 mitem = new pMenuItem[MAX_MENU_ITEMS];

 for(i=0;i<MAX_MENU_ITEMS;i++)
   mitem[i] = new MenuItem;

 mfile = new MenuFile;

 CLEAR_OBJECT(*mfile);

 File f(fn);
 if(!f.opened())
   {
    strcpy(mfile->prompt,"Select: ");
    mfile->color=7;
    mfile->highlight=3;
    num_mitems=0;
   }
  else
   {
    f.read(mfile,sizeof(*mfile));

    for(num_mitems=0;num_mitems<MAX_MENU_ITEMS;num_mitems++)
      if(f.read(mitem[num_mitems],sizeof(MenuItem)) != sizeof(MenuItem))
         break;

    f.close();
   }

 word crc = 0;

 for(i=0;i<num_mitems;i++)
   crc += word(i+1) * ::crc(mitem[i],sizeof(MenuItem));

 crc += ::crc(mfile,sizeof(*mfile));

 KEY hotkeys[]={ KEY_INS, KEY_DEL, KEY_ALTP, KEY_ALTS, KEY_ALTC, KEY_CTLP, NULL };

 SelectWindow sw( num_mitems + 1,
                  0x4E,
                  mitem_select_func,
                  0,
                  NULL,
                  hotkeys );

 sw.open( 1,
          1,
          80,
          24,
          0x1F,
          EXPLODE | BRACKETS,
          CHISEL_BORDER,
          0x19,
          NULL,
          0x10 );

 sw.title( menu, 0x1E );

 for(;;)
   {
    tsw_showfooter("\001AltC\001-Copy   \001CtrlP\001-Paste   \001AltP\001-Prompt/RIP   \001Ins\001-Insert   \001Del\001-Delete   \001AltS\001-Show", BAR_NORM, BAR_HIGH );

    sw.redefine(num_mitems+1);
    int choice=sw.process();

    if(choice==SL_ESC)
      {
       word newcrc = 0;

       for(i=0;i<num_mitems;i++)
         newcrc += word(i+1) * ::crc(mitem[i],sizeof(MenuItem));

       newcrc += ::crc(mfile,sizeof(*mfile));

       if(crc != newcrc)
         {
          int choice = ask_save();
          if(choice>=0)
            {
             if(choice)
               {
                File f;
                f.open(fn,fmode_create);
                if(f.opened())
                  {
                   f.write(mfile,sizeof(*mfile));
                   for(i=0;i<num_mitems;i++)
                     f.write(mitem[i],sizeof(MenuItem));
                   f.close();
                  }
               }
             break;
            }
          continue;
         }
       break;
      }
    if(choice==SL_HOTKEY)
      {
       switch(sw.hotkey)
         {
          case KEY_ALTC: {
                          if(!num_mitems || sw.current>=num_mitems) continue;
                          copy_mitem = *mitem[sw.current];
                          continue;
                         }
          case KEY_CTLP: {
                          if(sw.current<0) sw.current = 0;
                          for(i=num_mitems-1;i>=sw.current;i--) *mitem[i+1] = *mitem[i];
                          *mitem[sw.current] = copy_mitem;
                          num_mitems++;
                          continue;
                         }
          case KEY_INS: {
                        if(sw.current<0) sw.current=0;
                        for(i=num_mitems-1;i>=sw.current;i--) *mitem[i+1] = *mitem[i];
                        delete mitem[sw.current];
                        mitem[sw.current] = new MenuItem;
                        num_mitems++;
                        continue;
                        }
          case KEY_DEL: {
                         if(!num_mitems || sw.current>=num_mitems) continue;
                         for(i=sw.current;i<num_mitems-1;i++) *mitem[i]=*mitem[i+1];
                         num_mitems--;
                         continue;
                        }
          case KEY_ALTP:{
                         FormWindow w( 7,
                                       4,
                                       74,
                                       13,
                                       0x3F,
                                       SHADOW | EXPLODE | BRACKETS | SHADOW,
                                       CHISEL_BORDER,
                                       0x3F,
                                       NULL,
                                       0x38 );

                         w.open();

                         w.title( "Prompt", 0x3E );

                         w << "\n    Prompt:\n"
                              "\n     Color:"
                              "\n Highlight:\n"
                              "\n  RIP Menu:      (Name:          )";

                         Field frm[]={
                                      { FRM_STRING, 0, 0,0, 0,     50, 50, 13,2 },
                                      { FRM_FUNCTION, 0, 0,colorselect,0, 25, 25, 13,4 },
                                      { FRM_FUNCTION, 0, 0,colorselect,0, 25, 25, 13,5 },
                                      { FRM_YESNO,    0, 0,0 ,0,     3,3  , 13,7 },
                                      { FRM_STRING, 0, 0,0,FRM_UPPER,8,9,25,7}
                                     };
                         frm[0].value=mfile->prompt;
                         frm[1].value=&mfile->color;
                         frm[2].value=&mfile->highlight;
                         frm[3].value=&mfile->attr;
                         frm[4].value=mfile->RIPname;

                         //if(strlen(mitem->data) > 8 || !mitem->attr) mitem->data[0]='\0';

                         w.define( frm,
                                   5,
                                   0x31,
                                   0x4E );

                         tsw_cursoron();
                         w.process();
                         tsw_cursoroff();
                         continue;
                        }
          case KEY_ALTS: simulate();
                         continue;
         }
      }

    if(choice>=num_mitems)
      {
       num_mitems++;
       delete mitem[choice];
       mitem[choice] = new MenuItem;
      }

    char *sex_choices[] = { "Don't Care" , "Male Only" , "Female Only" , NULL };

    Field frm[]={
                 { FRM_STRING  , 0, 0, 0, 0, 159, 74 , 2,3  },
                 { FRM_FUNCTION, 0, 0, colorselect , 0, 25,25,13,5},
                 { FRM_CHAR    , 0, 0, 0, FRM_UPPER, 1, 1,  13,7 },
                 { FRM_FUNCTION, 0, 0, func_select, 0, 30, 30 , 13,8 },
                 { FRM_STRING  , 0, 0, 0, 0, 159, 60, 13,9 },
                 { FRM_UNSIGNED, 0, 0, 0, 0, 5 , 6, 13,11},
                 { FRM_UNSIGNED, 0, 0, 0, 0, 5 , 6, 13,12},
                 { FRM_FUNCTION, 0, 0,flagonoff_select,0,32,32,13,13},
                 { FRM_SHORT   , 0, 0, 0, 0, 3 , 4, 13,14},
                 { FRM_SHORT   , 0, 0, 0, 0, 3 , 4, 13,15},
                 { FRM_CHOICE  , 0, sex_choices,0,0,11,11,13,16 },
                 { FRM_UNSIGNED, 0, 0, 0, 0, 5 , 6, 13,17},
                 { FRM_UNSIGNED, 0, 0, 0, 0, 5 , 6, 13,18},
                 { FRM_FUNCTION, 0, 0,edit_timeframe,0,20,20, 13  ,19    },
                 { FRM_LONG    , 0, 0, 0, 0, 5 , 6, 13,20},
                 { FRM_LONG    , 0, 0, 0, 0, 5 , 6, 13,21},

                 { FRM_FUNCTION, 0, 0,edit_nodes,0,20,20, 52  ,11    },

                 { FRM_YESNO   , 0, 0,0,0,3,3,52,14},
                 { FRM_STRING  , 0, 0, 0, 0, 15, 16 , 52,15  },

                 { FRM_YESNO   , 0, 0,0,0,3,3,56,18},
                 { FRM_YESNO   , 0, 0,0,0,3,3,56,19},
                 { FRM_YESNO   , 0, 0,0,0,3,3,56,20}
                };

    if(!mitem[choice]->text[0])
      {
       mitem[choice]->color = 0x0F;
      }

    byte rip_remoteshow  = (mitem[choice]->attr & MNU_RIP_SHOW_REMOTE) ? 1 : 0;
    byte rip_localshow   = (mitem[choice]->attr & MNU_RIP_HIDE_LOCAL ) ? 0 : 1;
    byte rip_reset       = (mitem[choice]->attr & MNU_RIP_NO_RESET   ) ? 0 : 1;
    byte password_req    = (mitem[choice]->attr & MENU_PASSWORD      ) ? 1 : 0;

   AccessFlags *flags[2];

   flags[0] = &mitem[choice]->access.flags;
   flags[1] = &mitem[choice]->access.flagsNot;


    frm[0 ].value =  mitem[choice]->text;
    frm[1 ].value = &mitem[choice]->color;
    frm[2 ].value = &mitem[choice]->hotKey;
    frm[3 ].value = &mitem[choice]->function;
    frm[4 ].value =  mitem[choice]->data;
    frm[5 ].value = &mitem[choice]->access.minLevel;
    frm[6 ].value = &mitem[choice]->access.maxLevel;
    frm[7 ].value = flags;
    frm[8 ].value = &mitem[choice]->access.minAge;
    frm[9 ].value = &mitem[choice]->access.maxAge;
    frm[10].value = &mitem[choice]->access.sex;
    frm[11].value = &mitem[choice]->access.minTimeLeft;
    frm[12].value = &mitem[choice]->access.minTimeOnline;
    frm[13].value = &mitem[choice]->access.timeFrame;
    frm[14].value = &mitem[choice]->access.minSpeed;
    frm[15].value = &mitem[choice]->access.maxSpeed;
    frm[16].value =  mitem[choice]->access.nodes;
    frm[17].value = &password_req;
    frm[18].value =  mitem[choice]->password;
    frm[19].value = &rip_remoteshow;
    frm[20].value = &rip_localshow;
    frm[21].value = &rip_reset;

    FormWindow w( 1,
                  1,
                  80,
                  24,
                  0x7F,
                  EXPLODE | BRACKETS,
                  CHISEL_BORDER,
                  0x7F,
                  NULL,
                  0x78 );

    w.define( frm,
              22,
              0x71,
              0x4E );

    w.open();
    w.title( menu, 0x7E );

    w << " 0        1         2         3         4         5         6         7\n"
         " 12345678901234567890123456789012345678901234567890123456789012345678901234\n\n"
         "\n     Color:\n"
         "\n    Hotkey:"
         "\n  Function:"
         "\n      Data:\n"
         "\n Min.Level:                                 Nodes:"
         "\n Max.Level:"
         "\n     Flags:"
         "\n  Min. Age:                     Password required:"
         "\n  Max. Age:                              Password:"
         "\n       Sex:"
         "\n Time Left:                            ÚÄRIPÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"
         "\n Time Onl.:                            ³  Show remote:     ³"
         "\n Timeframe:                            ³   Show local:     ³"
         "\n Min.Speed:                            ³ Reset screen:     ³"
         "\n Max.Speed:                            ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ";


    tsw_cursoron();
    w.process();
    tsw_cursoroff();

    mitem[choice]->attr  = rip_remoteshow ? MNU_RIP_SHOW_REMOTE : 0;
    mitem[choice]->attr |= rip_localshow  ? 0 : MNU_RIP_HIDE_LOCAL ;
    mitem[choice]->attr |= rip_reset      ? 0 : MNU_RIP_NO_RESET   ;
    mitem[choice]->attr |= password_req   ? MENU_PASSWORD : 0;
   }

 for(i=0;i<MAX_MENU_ITEMS;i++)
   delete mitem[i];

 delete [] mitem;
 delete mfile;
}

static void
simulate()
{
   char cols[] = { 0,4,2,6,1,5,3,7 };

   tsw_showfooter("Press any key to return", BAR_COLOR);

   Window w;

   w.open( 1,
           1,
           tsw_hsize,
           tsw_vsize - 1,
           0xF,
           NOBORDER | EXPLODE );

   char hl_att =   cols[mfile->highlight  & 0x7]
                  | (cols[(mfile->highlight & 0x70 )>>4] << 4)
                  | (((mfile->highlight     & 0x8  )>>3)?0x00:0x08);

   for(int i=0;i<=num_mitems;i++)
   {
      char x[160];
      MenuItem mi = *mitem[i];

      if(i==num_mitems)
      {
         w << '\n';

         CLEAR_OBJECT(mi);
         strcpy(mi.text,mfile->prompt);
         mi.color = mfile->color;
      }

      char att =  cols[mi.color&0x7] | (cols[(mi.color&0x70)>>4] << 4) | (((mi.color&0x8)>>3)?0x00:0x08);

      strcpy(x,mi.text);

      w.attrib(att);

      if(x[strlen(x)-1] == ';')
      {
         x[strlen(x)-1] = '\0';
      }
      else
      {
         x[strlen(x)+1] = '\0';
         x[strlen(x)]   = '\n';
      }

      bool highlight = FALSE;

      for(int j=0;j<strlen(x);j++)
      {
         switch(x[j])
         {
            case '^' :
               {
                  highlight = !highlight;

                  if(highlight)
                     w.attrib(hl_att);
                  else
                     w.attrib(att);
               }
               break;

            case '\\':
               {
                  switch(x[j+1])
                  {
                     case '1':
                     case '2':
                     case '3':
                     case '4':
                     case '5':
                     case '6':
                     case '7': w.attrib(cols[x[j+1]-'0'] | 0x8);
                               j++;
                               break;
                     case '0': w.attrib(att);
                               j++;
                               break;
                     default : w << '\\';
                  }
               }
               break;

            default  : w << x[j];
                       break;
         }
      }

      if(mi.hotKey == 1)
      {
         if(mi.function==40)
         {
            append_backspace(langheader.txtPath);
            FileName fn(langheader.txtPath,mi.data,".ANS");
            File f(fn,fmode_read,5000);
            if(f.opened())
            {
               char c;
               for(;;)
               {
                  if(f.read(&c,1) != 1) break;
                  w << c;
               }
            }
         }
      }
   }

   KB.get();
}
