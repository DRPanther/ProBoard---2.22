#define Use_MsgBase
#define INCLUDE_REGIS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <stdlib.h>
#include "proboard.hpp"


struct MenuItem : public _MenuItem
   {
      bool checkAccess() const;
   };

bool
MenuItem::checkAccess() const
{
   if(!check_access(access.minLevel , access.flags , access.flagsNot))
      return FALSE;

   if(access.maxLevel && user.level > access.maxLevel)
      return FALSE;

   int age = -1;

   if(user.birthDate.ok())
      age = Date(TODAY).age(user.birthDate);

   if(access.minAge || access.maxAge)
   {
      if(age < 0)
         return FALSE;

      if((access.minAge && age < access.minAge) || (access.maxAge && age > access.maxAge))
         return FALSE;
   }

   if(access.minTimeLeft && timer.left() < access.minTimeLeft)
      return FALSE;

   if(access.minTimeOnline && timer.online() < access.minTimeOnline)
      return FALSE;

   if(!access.timeFrame.enabled())
      return FALSE;

   if(access.sex)
   {
      if(!user.sex)
         return FALSE;

      if(user.sex == SEX_MALE)
      {
         if(access.sex == 2)
            return FALSE;
      }

      if(user.sex == SEX_FEMALE)
      {
         if(access.sex == 1)
            return FALSE;
      }
   }

   dword speed = io.baud;

   if(speed < 1)
      speed = 38400L;

   if(access.minSpeed && speed < access.minSpeed)
      return FALSE;

   if(access.maxSpeed && speed > access.maxSpeed)
      return FALSE;

   if(access.nodes[(node_number-1)/8] & (1 << ((node_number-1) & 7)))
      return FALSE;

   return TRUE;
}


typedef char *str;

#define MAX_MENUSTACK 100

static
 class menu_stack
  {
   str *menus;
   int sp;
 public:
   menu_stack()
     {
      menus = new str[MAX_MENUSTACK];
      for(int i=0;i<MAX_MENUSTACK;i++) menus[i] = new char[9];
      sp=0;
     }
  ~menu_stack()
     {
      for(int i=0;i<MAX_MENUSTACK;i++) delete [] menus[i];
      delete [] menus;
     }
   void clear()       { sp=0; }
   void push(char *m) { if(sp<MAX_MENUSTACK) strcpy(menus[sp++],m); }
   void pop(char *m)  { if(sp>0) strcpy(m,menus[--sp]); }
  } mstack;

void
logoff(char *data)
{
 if(data==NULL || data[0]!='/') io.cls();

 run_sdkfile("/I GOODBYE");
 showansascrip("GOODBYE");
 run_sdkfile("/I GOODBYE2");

 LOG("User logged off");

 if(io.baud) sleep(2);

 exit_proboard();
}

static void
gotomenu(char *data)
{
 String param[4];
 char newmenu[20];
 static int reg_counter = 0;

 if(reg_counter++ == 10)
   {
    CHECK_REG();
    reg_counter = 0;
   }

 int n=parse_data(data,param);

 for(int i=0;i<n;i++)
   {
    if(param[i][0]=='/')
      {
       switch(toupper(param[i][1]))
         {
          case 'M': {
                     MsgArea ma;

                     if(param[i][3]=='+')
                       {
                        for(int i=user.msgArea+1;i<=user.msgArea+MsgArea::highAreaNum();i++)
                          {
                           int area = ((i-1)%MsgArea::highAreaNum())+1;
                           if(!ma.read(area) || !check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)) continue;
                           user.msgArea = area;
                           break;
                          }
                        break;
                       }

                     if(param[i][3]=='-')
                       {
                        for(int i=user.msgArea+MsgArea::highAreaNum()-1;i>=user.msgArea;i--)
                          {
                           int area = ((i-1)%MsgArea::highAreaNum())+1;
                           if(!ma.read(area) || !check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)) continue;
                           user.msgArea = area;
                           break;
                          }
                        break;
                       }

                     user.msgArea =atoi(&(param[i][3]));
                     break;
                    }
          case 'F': {
                     FileArea fa;

                     if(param[i][3]=='+')
                       {
                        for(int i=user.fileArea+1;i<=user.fileArea+FileArea::highAreaNum();i++)
                          {
                           int area = ((i-1)%FileArea::highAreaNum())+1;
                           if(!fa.read(area) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;
                           user.fileArea = area;
                           break;
                          }
                        break;
                       }

                     if(param[i][3]=='-')
                       {
                        for(int i=user.fileArea+FileArea::highAreaNum()-1;i>=user.fileArea;i--)
                          {
                           int area = ((i-1)%FileArea::highAreaNum())+1;
                           if(!fa.read(area) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;
                           user.fileArea = area;
                           break;
                          }
                        break;
                       }

                     user.fileArea=atoi(&(param[i][3]));
                     break;
                    }
          case 'P': {
                     char s[20];

                     io << '\n' << S_ENTER_MENU_PASSWORD;
                     io.read(s,15,READMODE_PWD);
                     io << "\n\xFF";

                     if(!s[0]) return;

                     if(strcmpl(&(param[i][3]),s))
                       {
                        LOG(1,"Wrong password for menu <%s> : %s",newmenu,strupr(s));
                        io << '\n' << S_ACCESS_DENIED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
                        return;
                       }
                    }
                    break;
         }
      }
     else strcpy(newmenu,param[i]);
   }

 strcpy(curmenu,newmenu);

 menuchanged = TRUE;
}

static void
gosubmenu(char *data)
{
 mstack.push(curmenu);
 gotomenu(data);
 if(!menuchanged) mstack.pop(curmenu);
}

static void
prevmenu(char *)
{
 mstack.pop(curmenu);

 menuchanged = TRUE;
}

static void
gotomenu_clear(char *data)
{
 mstack.clear();
 gotomenu(data);
}

#define MAX_ITEMS 255

struct menu_item
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
   byte color;
   byte attr;
  };

const byte MNU_RIP_SHOW_REMOTE = 1;
const byte MNU_RIP_HIDE_LOCAL  = 2;
const byte MNU_RIP_NO_RESET    = 4;

static void nofunction(char *) {}

void (*menufunctions[])(char *) =
    {
     (void (*)(char *)) nofunction,       // function  0 -> niets!! raar maar waar
     (void (*)(char *)) gotomenu,         // function  1 -> goto menu
     (void (*)(char *)) gosubmenu,        // function  2 -> gosub menu
     (void (*)(char *)) prevmenu,         // function  3 -> previous menu
     (void (*)(char *)) gotomenu_clear,   // function  4 -> clear stack and goto menu
     showansasc,       // function  5 -> show ans/asc file
     (void (*)(char *)) change_state,     // function  6 -> change state (!!)
     (void (*)(char *)) shell,            // function  7 -> shell
     (void (*)(char *)) version_info,     // function  8 -> version information
     (void (*)(char *)) logoff,           // function  9 -> logoff
     (void (*)(char *)) usage_graph,      // function 10 -> system usage graph by hour
     (void (*)(char *)) pagesysop,        // function 11 -> Chat request
     (void (*)(char *)) questionnaire,    // function 12 -> questionnaire
     (void (*)(char *)) user_list,        // function 13 -> user list
     (void (*)(char *)) time_stat,        // function 14 -> time info
     (void (*)(char *)) view_ans_wait,    // function 15 -> View ans/asc and wait
     (void (*)(char *)) city_change,      // function 16 -> change city
     (void (*)(char *)) password_change,  // function 17 -> change password
     (void (*)(char *)) lines_change,     // function 18 -> change screen size
     (void (*)(char *)) cls_toggle,       // function 19 -> toggle clearscreen
     (void (*)(char *)) pause_toggle,     // function 20 -> toggle more prompt
     (void (*)(char *)) ansi_toggle,      // function 21 -> toggle ANSI graphics
     (void (*)(char *)) mailcheck,        // function 22 -> check for mail
     (void (*)(char *)) readmsg,          // function 23 -> read messages
     (void (*)(char *)) scan_msg,         // function 24 -> scan messages
     (void (*)(char *)) qscan_msg,        // function 25 -> qscan messages
     (void (*)(char *)) days_graph,       // function 26 -> usage graph by day
     (void (*)(char *)) writemsg,         // function 27 -> write a message
     (void (*)(char *)) combined_select,  // function 28 -> combined boards select
     (void (*)(char *)) weeks_graph,      // function 29 -> usage graph per week
     (void (*)(char *)) raw_dir,          // function 30 -> show directory
     (void (*)(char *)) list_files,       // function 31 -> list files
     (void (*)(char *)) download,         // function 32 -> download a file
     (void (*)(char *)) upload,           // function 33 -> upload a file
     (void (*)(char *)) list_archive,     // function 34 -> view archive
     (void (*)(char *)) keyword_search,   // function 35 -> keyword search
     (void (*)(char *)) filename_search,  // function 36 -> filename search
     (void (*)(char *)) new_files,        // function 37 -> show new files
     (void (*)(char *)) view_file,        // function 38 -> view a file
     (void (*)(char *)) view_named_file,  // function 39 -> display named file
     (void (*)(char *)) nofunction,       // function 40 -> display ans/asc w/hotkeys
     (void (*)(char *)) fsed_toggle,      // function 41 -> toggle fs-editor
     (void (*)(char *)) hotkey_toggle,    // function 42 -> toggle hotkeys
     (void (*)(char *)) clearmarked,      // function 43 -> clear marked messages
     (void (*)(char *)) combined_clear,   // function 44 -> reset combined boards
     (void (*)(char *)) view_file_wait,   // function 45 -> display textfile and wait
     (void (*)(char *)) change_access,    // function 46 -> Change Access (flags/level)
     (void (*)(char *)) logentry,         // function 47 -> make a log entry
     (void (*)(char *)) tops,             // function 48 -> Tops
     (void (*)(char *)) set_msgarea,      // function 49 -> Set msg-area
     (void (*)(char *)) show_users_online,// function 50 -> show users online
     (void (*)(char *)) lastcallers,      // function 51 -> list last callers
     (void (*)(char *)) usereditor,       // function 52 -> toggle no-disturb
     (void (*)(char *)) multichat,        // function 53 -> multi-line chat
     (void (*)(char *)) set_filearea,     // function 54 -> Set File Area
     (void (*)(char *)) view_gif,         // function 55 -> View GIF type
     (void (*)(char *)) ibm_toggle,       // function 56 -> Toggle IBM chars
     (void (*)(char *)) phone_change,     // function 57 -> Change phone #
     (void (*)(char *)) dataphone_change, // function 58 -> Change data phone #
     (void (*)(char *)) handle_change,    // function 59 -> Change handle
     (void (*)(char *)) run_sdkfile,      // function 60 -> Load a ProBoard SDK File
     (void (*)(char *)) bulletin,         // function 61 -> Show bulletin
     (void (*)(char *)) toggle_avatar,    // function 62 -> Toggle AVT/0
     (void (*)(char *)) toggle_avtplus,   // function 63 -> Toggle AVT/0+
     (void (*)(char *)) show_graph,       // function 64 -> Show general usage graph
     (void (*)(char *)) showansascrip,    // function 65 -> Show ANS/ASC/RIP w/hotkeys
     (void (*)(char *)) ripfont_change,   // function 66 -> Change RIP font
     (void (*)(char *)) rip_toggle,       // function 67 -> Toggle RIP graphics
     (void (*)(char *)) edit_taglist,     // function 68 -> Edit file tags
     (void (*)(char *)) select_language,  // function 69 -> Select a language
     (void (*)(char *)) change_dateformat,// function 70 -> Change date format
     (void (*)(char *)) change_address,   // function 71 -> Change address
     (void (*)(char *)) change_faxphone,  // function 72 -> Change fax phone
     (void (*)(char *)) change_country,   // function 73 -> Change country
     (void (*)(char *)) change_default_protocol, // function 74 -> Change default protocol
     (void (*)(char *)) select_msggroup,  // function 75 -> Select message area group
     (void (*)(char *)) select_filegroup  // function 76 -> Select file area group
    };

static void _near
execute_function(MenuItem *item)
{
   rip_reset();

   if(   item->function>4
      && item->function != 23
      && item->function != 32
      && item->function != 33
      && !(item->attr & MNU_RIP_NO_RESET))
   {
      rip_textmode();
   }

   if(item->attr & MENU_PASSWORD)
   {
      char s[20];

      io << '\n' << S_ENTER_SELECTION_PASSWORD;
      io.read(s,15,READMODE_PWD);
      io << "\n\xFF";

      if(!s[0]) return;

      if(stricmp(item->password,s))
      {
         LOG(1,"Wrong password in menu <%s>",(char *)curmenu);
         io << '\n' << S_ACCESS_DENIED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
         return;
      }
   }

   (*menufunctions[item->function])(replace_stringvars(item->data));
}

static char * near
colorstring(char col)
{
 char cols[] = { 0,4,2,6,1,5,3,7 };
 String s;
 static char str[30];

 str[0]=0;

 char fg=col&7;
 char bg=(col&0x70)>>4;
 char bright=!(col&8);

 if(avatar)
   {
    fg = cols[fg];
    bg = cols[bg];
    s << char(22) << char(1) << char(fg|(bg<<4)|(bright?8:0));
    strcpy(str,s);

    return str;
   }

 if(!ansi_mode) return str;

 s="[";
 if(bright) s << "1;";
       else s << "0;";

 s << form("3%d;4%dm",fg,bg);

 strcpy(str,s);

 return str;
}

static String near
translate_menutext(char *s,byte color,byte highlight,bool prompt)
{
 String str;
 bool high = FALSE;

 if(*s == ';')  return "";
 if(*s == '\0') return "\n";

 str << colorstring(color);

 for(;*s;s++)
   {
    char c = *s;

    switch(c)
      {
       case '^': high = !high;
                 if(high)
                  str << colorstring(highlight);
                 else
                  str << colorstring(color);
                 break;
       case '~': str << "@<TMLEFT>@";
                 break;
       case '`': str << "@<CURMSGAREA>@";
                 break;
       case '@': if(*(s+1)=='<' || (*(s-1))=='>')
                     str << '@';
                 else
                     str << "@<CURFILEAREA>@";
                 break;
       case '\\': c = *(s+1);
                  if(c>='1' && c<='7')
                    {
                     str << char(c - '0');
                     s++;
                     continue;
                    }
                  if(c=='0')
                    {
                     str << colorstring(color);
                     s++;
                     continue;
                    }
                 c = '\\';
       default : str << c;
       }
    }

 if(!prompt)
   {
    if(str[str.len()-1] == ';') str[str.len()-1] = '\0';
                           else str << '\n';
   }

 return replace_stringvars(str);
}


static bool
menu_show_at_all(MenuItem *item , _MenuFile *prompt)
{
   return
          (    (item->attr & MNU_RIP_SHOW_REMOTE)
           || !(item->attr & MNU_RIP_HIDE_LOCAL)
           || !rip_mode
           || !prompt->attr
          );
}

static bool
menu_show_local(MenuItem *item , _MenuFile *prompt)
{
   return
          (
               !(item->attr & MNU_RIP_HIDE_LOCAL)
            || !rip_mode
            || !prompt->attr
           );
}

static bool
menu_show_remote(MenuItem *item , _MenuFile *prompt)
{
   return (
               (item->attr & MNU_RIP_SHOW_REMOTE)
            || !rip_mode
            || !prompt->attr
          );
}


void
menu_processor()
{
 int i,j;
 char hotkeys[MAX_ITEMS];
 File f;
 _MenuFile *menufile = NULL;
 MenuItem **items  = NULL;
 int numallocated  = 0;
 int numitems      = 0;
 int glob_numitems = 0;

 menufile = new _MenuFile;

 strcpy(curmenu,"TOP");

 for(;;)
   {
    LOG(3,"Menu: %s",curmenu);
    menuchanged = FALSE;
    updatemenu  = FALSE;

    if(f.open(FileName(cfg.mnupath,"GLOBAL.PBM")))
    {
       glob_numitems = int((f.len() - sizeof(_MenuFile)) / sizeof(MenuItem));
       f.close();
    }

    if(!f.open(FileName(cfg.mnupath,curmenu,".PBM")))
      {
       if(!strcmpl(curmenu,"TOP")) fatalerror("NO TOP MENU");
       mstack.clear();
       LOG("ERROR: Menu %s not found!",curmenu);
       strcpy(curmenu,"TOP");
       continue;
     }

   if(items    != NULL)
   {
      for(i=0;i<numallocated;i++)
         delete items[i];

      delete [] items;
   }

   numitems=int((f.len() - sizeof(_MenuFile)) / sizeof(MenuItem));

   items = new MenuItem *[numitems + glob_numitems];
   for(i=0;i<numitems+glob_numitems;i++)
      items[i] = new MenuItem;

   numallocated = i;

   f.read(menufile,sizeof(_MenuFile));
   for(i=0;i<numitems;i++)
      f.read(items[i],sizeof(MenuItem));
   f.close();

   if(f.open(FileName(cfg.mnupath,"GLOBAL.PBM")))
   {
      f.seek(sizeof(_MenuFile));
      for(i=numitems;i<numitems+glob_numitems;i++)
         f.read(items[i],sizeof(MenuItem));
      f.close();

      numitems += glob_numitems;
   }

   for(i=0,j=0;i<numitems;i++)
      if(items[i]->checkAccess())
         if(items[i]->hotKey>13 && items[i]->function)
            hotkeys[j++] = toupper(items[i]->hotKey);

   hotkeys[j++]='\r';
   hotkeys[j++]=';';
   hotkeys[j]=0;

  for(;;)
    {
    char k;

   again:

    if(updatemenu) break;

    rip_reset();

    k=toupper(comstack.pollnext());
    if(strchr(hotkeys,k) && k!=13 && k)
       {
        k=toupper(comstack.getnext());
        for(i=0;i<numitems;i++)
          if(k==items[i]->hotKey && items[i]->checkAccess())
             {
             if(!stack_mode) io << k << "\n\xFF";
             execute_function(items[i]);
             if(menuchanged) break;
             }
        if(menuchanged) break;

        io << "\n\xFF";
        continue;
       }

    io.disablestop();
    io << "\n\f\xFF";

    if(menufile->attr)
      show_rip(menufile->RIPname,FALSE);

    for(i=0;i<numitems;i++)
      {
      if(!items[i]->checkAccess())
         continue;

      if(items[i]->function==40)
          {
           if(menu_show_at_all(items[i],menufile))
              {
               if(!menu_show_local(items[i],menufile))  io.show_local  = FALSE;
               if(!menu_show_remote(items[i],menufile)) io.show_remote = FALSE;

               k=showansasc(replace_stringvars(items[i]->data),hotkeys);

               io.show_local  = TRUE;
               io.show_remote = TRUE;

               if(k>2)
                   {
                   io << " \7" << k << "\n\xFF";
                   for(int j=0;j<numitems;j++)
                     if(items[j]->hotKey==k && items[j]->checkAccess())
                       {
                       if(items[j]->function) execute_function(items[j]);
                       if(menuchanged) break;
                       }

                   if(menuchanged) break;
                   goto again;
                   }
              }
          }
        else if(items[i]->hotKey==1)
               {
                if(items[i]->function == 5 || items[i]->function == 38)
                {
                   if(menu_show_at_all(items[i],menufile))
                   {
                     if(!menu_show_local(items[i],menufile))  io.show_local  = FALSE;
                     if(!menu_show_remote(items[i],menufile)) io.show_remote = FALSE;

                     execute_function(items[i]);

                     io.show_local  = TRUE;
                     io.show_remote = TRUE;
                   }
                }
                else
                {
                  execute_function(items[i]);
                }
               if(menuchanged) break;
               }

      k=0;

      if(menu_show_at_all(items[i],menufile))
      {
         if(!menu_show_local(items[i],menufile))  io.show_local  = FALSE;
         if(!menu_show_remote(items[i],menufile)) io.show_remote = FALSE;

         k = io.send(translate_menutext(items[i]->text,
                                        items[i]->color,
                                        menufile->highlight,
                                        FALSE),
                    hotkeys);

         io.show_local  = TRUE;
         io.show_remote = TRUE;
      }

      if(k==1) break;

      if(k>1)
        {
        io << " \7" << k << "\n\xFF";
        for(int j=0;j<numitems;j++)
          if(items[j]->hotKey==k && items[j]->checkAccess())
            {
            if(items[j]->function) execute_function(items[j]);
            if(menuchanged) break;
            }

        if(menuchanged) break;

        goto again;
        }
      }

    if(menuchanged) break;

    io << '\n' << translate_menutext(menufile->prompt,   // hidden constructor
                                     menufile->color,
                                     menufile->highlight,
                                     TRUE);

    if(!stack_mode) k=io.wait(hotkeys);

    if(k==';') io << "> ";

    if(k==';' || stack_mode)
      {
      if(k==';' || !strchr(hotkeys,k) || !k)
         {
         comstack.clear();

         char command[60];
         io.read(command,59,READMODE_NOFIELD);
         if(!command[0]) { command[0]='\r'; command[1]=0; }
         io << "\n\xFF";
         k=toupper(command[0]);
         if(!strchr(hotkeys,k)) k='\r';
            else comstack.parse(&command[1]);
         }
      }

    for(i=0;i<numitems;i++)
      if(k==items[i]->hotKey && items[i]->checkAccess())
         {
         if(!stack_mode) io << k << "\n\xFF";
         execute_function(items[i]);
         if(menuchanged) break;
         }

     if(menuchanged) break;

     io << "\n\xFF";
    }
  }
}

