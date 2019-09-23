#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include "proboard.hpp"

const OFF     = 0;
const ON      = 1;
const TOGGLE  = 2;
const ASK     = 3;
const NONE    = 4;
const SAVE    = 0x10;
const RESTORE = 0x20;
const QUIET   = 0x40;

static int _near
toggle_type(char *data)
{
 String param[5];
 int num_para;
 int ret=TOGGLE;
 char *strings[] = { "OFF", "ON" , "TOGGLE" , "ASK" , "SAVE" , "RESTORE" , "/Q", NULL };

 num_para = parse_data(data,param);

 for(int i=0;i<num_para;i++)
   {
    for(int j=0;strings[j];j++) if(param[i] == strings[j]) break;
    if(!strings[j]) continue;

    switch(j)
      {
       case 0:
       case 1:
       case 2:
       case 3: ret = j;
               break;
       case 4: ret |= SAVE;
               break;
       case 5: ret |= RESTORE;
               break;
       case 6: ret |= QUIET;
               break;
      }
   }

 if(ret == NONE) ret = TOGGLE;

 return ret;
}


void
city_change(char *)
{
   io << "\f\n" << S_CHANGE_CITY_TITLE_AND_PROMPT(user.city);

   char tmp[26];

   io.read(tmp,25);

   if(strlen(tmp)<2) return;

   strcpy(user.city,tmp);

   LOG(1,"City changed to '%s'",user.city);
}

void
phone_change(char *)
{
   io << "\f\n" << S_CHANGE_VOICE_PHONE_TITLE_AND_PROMPT(user.voicePhone);

   char tmp[16];

   io.read(tmp,15);

   if(strlen(tmp)<5) return;

   strcpy(user.voicePhone,tmp);

   LOG(1,"Voice Phone # changed to '%s'",user.voicePhone);
}

void
dataphone_change(char *)
{
   io << "\f\n" << S_CHANGE_DATA_PHONE_TITLE_AND_PROMPT(user.dataPhone);

   char tmp[16];

   io.read(tmp,15);

   if(strlen(tmp)<5) return;

   strcpy(user.dataPhone,tmp);

   LOG(1,"Data Phone # changed to '%s'",user.dataPhone);
}

void
change_faxphone(char *)
{
   io << "\f\n" << S_CHANGE_FAX_PHONE_TITLE_AND_PROMPT(user.faxPhone);

   char tmp[16];

   io.read(tmp,15);

   if(strlen(tmp)<5) return;

   strcpy(user.faxPhone,tmp);

   LOG(1,"Fax Phone # changed to '%s'",user.faxPhone);
}


void
change_state(char *)
{
   io << "\f\n" << S_CHANGE_STATE_TITLE_AND_PROMPT(user.state);

   char tmp[26];

   io.read(tmp,25);

   if(strlen(tmp)<2) return;

   strcpy(user.state,tmp);

   LOG(1,"State changed to '%s'",user.state);
}

void
change_country(char *)
{
   io << "\f\n" << S_CHANGE_COUNTRY_TITLE_AND_PROMPT(user.country);

   char tmp[26];

   io.read(tmp,25);

   if(strlen(tmp)<3) return;

   strcpy(user.country,tmp);

   LOG(1,"Country changed to '%s'",user.country);
}


void
change_address(char *)
{
   io << "\f\n" << S_CHANGE_MAILING_ADDRESS_TITLE_AND_PROMPT((char *)(String(user.address1)+"\n"+String(user.address2)+"\n"+String(user.address3)));

   char tmp[51];

   io << "\n\n";
   io.read(tmp,50);

   if(!tmp[0])
      return;

   strcpy(user.address1,tmp);

   io << '\n';
   io.read(user.address2,50);
   io << '\n';
   io.read(user.address3,50);

   LOG(1,"Mailing address changed");
}


void
handle_change(char *)
{
   io << "\f\n" << S_CHANGE_ALIAS_TITLE_AND_PROMPT(user.alias);

   char tmp[36];

   io.read(tmp,35,READMODE_UPFIRST);

   if(strlen(tmp)<3) return;

   User x;
   long a_rec=-1,u_rec=-1;
   bool a_found = x.search(tmp,TRUE,FALSE,TRUE);
   if(a_found) a_rec = x.record;
   bool u_found = x.search(tmp);
   if(u_found) u_rec = x.record;

   if(!strcmpl(tmp,"Sysop") || (a_rec>=0 && a_rec!=user_recnr) || (u_rec>=0 && u_rec!=user_recnr))
   {
      LOG(1,"Duplicate alias: '%s'",tmp);

      io << "\n\n" << S_ALIAS_BEING_USED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
      return;
   }

   strcpy(user.alias,tmp);

   LOG(1,"Alias changed to '%s'",user.alias);
}



void password_change( char * )
{
     char pass[ 16 ];


     io << "\f\n" 
        << S_CHANGE_PASSWORD_TITLE;


     do
     {
          io << "\n\n" 
             << S_ENTER_NEW_PASSWORD( form( "%d", cfg.min_passlength ) );

          io.read( pass,
                   15,
                   READMODE_PWD );

          
          if ( strlen( pass )  <  cfg.min_passlength )
          {
               io << "\n\n" 
                  << S_ILLEGAL_PASSWORD 
                  << '\n';

               sleep( 2 );

               return;
          }


          io << "\n\n" 
             << S_ASK_PASSWORD_AGAIN;

          io.read( user.passWord,
                   15,
                   READMODE_PWD );
     } 
     while( strcmpl( pass, user.passWord ) );


     user.passWordCRC  =  RaCrc( user.passWord );


     io << "\n\n" 
        << S_PASSWORD_CHANGED_SUCCESSFULLY 
        << "\n\n" 
        << S_PRESS_ENTER_TO_CONTINUE;


     LOG( 1, 
          "Password changed to '%s'", 
          user.passWord );
}



void lines_change(char *)
{
   char newlen[4];

   io << "\f\n" << S_CHANGE_SCREENLENGTH_TITLE_AND_PROMPT(form("%d",user.screenLength));

   io.read(newlen,2,READMODE_DIGITS);

   if(atoi(newlen) >= 5)
      user.screenLength = atoi(newlen);

   LOG(1,"Screenlength changed to %d",user.screenLength);
}

#define TOGGLE_DISPLAY(var)  { io << ((var) ? string_enabled : string_disabled); io << S_PRESS_ENTER_TO_CONTINUE; }

void
ansi_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_ANSI) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_ANSI;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_ANSI;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_ANSI;
                 break;
    case ASK   : io << '\n' << S_ASK_ANSI;
                 if(io.ask()) user.uFlags |= UFLAG_ANSI;
                         else
                           {
                            if(ansi_mode) io << "\x1b[0m";
                            user.uFlags &= ~UFLAG_ANSI;
                           }
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_ANSI;
        else user.uFlags &= ~UFLAG_ANSI;
   }

 if(!(action & QUIET))
   {
    if(ansi_mode) LOG(1,"ANSI enabled");
             else LOG(1,"ANSI disabled");

    if(ansi_mode)
      io << '\n' << S_ANSI_ENABLED;
    else
      io << '\n' << S_ANSI_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
toggle_avatar(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_AVATAR) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_AVATAR;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_AVATAR;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_AVATAR;
                 break;
    case ASK   : io << '\n' << S_ASK_AVATAR;
                 if(io.ask()) user.uFlags |= UFLAG_AVATAR;
                         else user.uFlags &= ~UFLAG_AVATAR;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_AVATAR;
        else user.uFlags &= ~UFLAG_AVATAR;
   }

 if(!(user.uFlags & UFLAG_AVATAR)) user.uFlags &= ~UFLAG_AVTPLUS;

 if(!(action & QUIET))
   {
    if(avatar) LOG(1,"AVATAR enabled");
          else LOG(1,"AVATAR disabled");

    if(avatar)
      io << '\n' << S_AVATAR_ENABLED;
    else
      io << '\n' << S_AVATAR_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
toggle_avtplus(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_AVTPLUS) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_AVTPLUS;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_AVTPLUS;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_AVTPLUS;
                 break;
    case ASK   : io << '\n' << S_ASK_AVATAR_PLUS;
                 if(io.ask()) user.uFlags |= UFLAG_AVTPLUS;
                         else user.uFlags &= ~UFLAG_AVTPLUS;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_AVTPLUS;
        else user.uFlags &= ~UFLAG_AVTPLUS;
   }

 if(!(action & QUIET))
   {
    if(avtplus) LOG(1,"AVT/0+ enabled");
           else LOG(1,"AVT/0+ disabled");

    if(avtplus)
      io << '\n' << S_AVTPLUS_ENABLED;
    else
      io << '\n' << S_AVTPLUS_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
cls_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_CLEAR) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_CLEAR;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_CLEAR;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_CLEAR;
                 break;
    case ASK   : io << '\n' << S_ASK_CLS;
                 if(io.ask()) user.uFlags |= UFLAG_CLEAR;
                         else user.uFlags &= ~UFLAG_CLEAR;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_CLEAR;
        else user.uFlags &= ~UFLAG_CLEAR;
   }

 if(!(action & QUIET))
   {
    if(cls_mode) LOG(1,"Screen clearing enabled");
            else LOG(1,"Screen clearing disabled");

    if(cls_mode)
      io << '\n' << S_CLS_ENABLED;
    else
      io << '\n' << S_CLS_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
pause_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_PAUSE) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_PAUSE;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_PAUSE;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_PAUSE;
                 break;
    case ASK   : io << '\n' << S_ASK_PAUSE;
                 if(io.ask()) user.uFlags |= UFLAG_PAUSE;
                         else user.uFlags &= ~UFLAG_PAUSE;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_PAUSE;
        else user.uFlags &= ~UFLAG_PAUSE;
   }

 if(!(action & QUIET))
   {
    if(pause_mode) LOG(1,"Page pausing enabled");
              else LOG(1,"Page pausing disabled");

    if(pause_mode)
      io << '\n' << S_PAUSE_ENABLED;
    else
      io << '\n' << S_PAUSE_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
fsed_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_FSED) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_FSED;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_FSED;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_FSED;
                 break;
    case ASK   : io << '\n' << S_ASK_FSED;
                 if(io.ask()) user.uFlags |= UFLAG_FSED;
                         else user.uFlags &= ~UFLAG_FSED;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_FSED;
        else user.uFlags &= ~UFLAG_FSED;
   }

 if(!(action & QUIET))
   {
    if(fsed_mode) LOG(1,"FsEd enabled");
             else LOG(1,"FsEd disabled");

    if(fsed_mode)
      io << '\n' << S_FSED_ENABLED;
    else
      io << '\n' << S_FSED_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
hotkey_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_HOTKEYS) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case ON    : user.uFlags |= UFLAG_HOTKEYS;
                 break;
    case OFF   : user.uFlags &= ~UFLAG_HOTKEYS;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_HOTKEYS;
                 break;
    case ASK   : io << '\n' << S_ASK_HOTKEYS;
                 if(io.ask()) user.uFlags |= UFLAG_HOTKEYS;
                         else user.uFlags &= ~UFLAG_HOTKEYS;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_HOTKEYS;
        else user.uFlags &= ~UFLAG_HOTKEYS;
   }

 if(!(action & QUIET))
   {
    if(!stack_mode) LOG(1,"Hotkeys enabled");
              else LOG(1, "Hotkeys disabled");

    if(!stack_mode)
      io << '\n' << S_HOTKEYS_ENABLED;
    else
      io << '\n' << S_HOTKEYS_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
ibm_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_NOIBM) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case OFF   : user.uFlags |= UFLAG_NOIBM;
                 break;
    case ON    : user.uFlags &= ~UFLAG_NOIBM;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_NOIBM;
                 break;
    case ASK   : io << '\n' << S_ASK_IBMCHARS;
                 if(!io.ask()) user.uFlags |= UFLAG_NOIBM;
                          else user.uFlags &= ~UFLAG_NOIBM;
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_NOIBM;
        else user.uFlags &= ~UFLAG_NOIBM;
   }

 if(!(action & QUIET))
   {
    if(!no_ibm) LOG(1,"IBM graphics enabled");
           else LOG(1,"IBM graphics disabled");

    if(!no_ibm)
      io << '\n' << S_IBMCHARS_ENABLED;
    else
      io << '\n' << S_IBMCHARS_DISABLED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}


void
user_list(char *data)
{
   bool aliases = FALSE;

   char s[40];

   if(String(data) == "/H")
      aliases = TRUE;

   io << "\f\n" << S_LIST_USERS_TITLE_AND_PROMPT;

   io.read(s,39,READMODE_UPALL);

   LOG(2,"User list requested. Search for \"%s\"",s);

   io << "\n\f";

   user.list(s,aliases);

   io << '\n' << S_END_OF_USER_LIST << ' ' << S_PRESS_ENTER_TO_CONTINUE;
}

void
ripfont_change(char *data)
{
 static byte prev = 0;

 int action = toggle_type(data);

 if(action & SAVE) prev = user.ripFont;

 switch(action & 0xF)
   {
    case OFF   : user.ripFont = 1;
                 break;
    case ON    : user.ripFont = 0;
                 break;
    case TOGGLE: user.ripFont = !user.ripFont;
                 break;
    case ASK   : io << '\n' << S_ASK_RIP_FONT;
                 user.ripFont = io.ask(0);
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    user.ripFont = prev;
   }

 if(!(action & QUIET))
   {
    if(user.ripFont) LOG(1,"Large RIP font selected");
                 else LOG(1,"Small RIP font selected");

    if(user.ripFont)
      io << '\n' << S_LARGE_RIP_FONT_SELECTED;
    else
      io << '\n' << S_SMALL_RIP_FONT_SELECTED;

     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
rip_toggle(char *data)
{
 static bool prev = FALSE;

 int action = toggle_type(data);

 if(action & SAVE) prev = (user.uFlags & UFLAG_NORIP) ? TRUE:FALSE;

 switch(action & 0xF)
   {
    case OFF   : user.uFlags |= UFLAG_NORIP;
                 break;
    case ON    : user.uFlags &= ~UFLAG_NORIP;
                 break;
    case TOGGLE: user.uFlags ^= UFLAG_NORIP;
                 break;
    case ASK   : io << '\n' << S_ASK_RIP;
                 if(!io.ask()) user.uFlags |= UFLAG_NORIP;
                         else
                           {
                            if(ansi_mode) io << "\x1b[0m";
                            user.uFlags &= ~UFLAG_NORIP;
                           }
                 io << '\n';
                 break;
   }

 if(action & RESTORE)
   {
    if(prev) user.uFlags |= UFLAG_NORIP;
        else user.uFlags &= ~UFLAG_NORIP;
   }

 if(!(action & QUIET))
   {
    if(user.uFlags & UFLAG_NORIP)
    {
      io << '\n' << S_RIP_DISABLED;
      LOG(1,"RIP disabled");
    }
    else
    {
      io << '\n' << S_RIP_ENABLED;
      LOG(1,"RIP enabled");
    }

    io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

 updatemenu = TRUE;
}

void
change_dateformat(char *data)
{
   if(data)
   {
      io << "\f\n";

      io << S_CHANGE_DATE_FORMAT_TITLE(date_formats_long[user.dateFormat]);

      io << "\n\n";
   }

   io << S_SELECT_DATE_FORMAT_TITLE << "\n\n";

   io << "\3   1. \6" << date_formats_long[0] << '\n';
   io << "\3   2. \6" << date_formats_long[1] << '\n';
   io << "\3   3. \6" << date_formats_long[2] << '\n';

   io << '\n' << S_SELECT_DATE_FORMAT_PROMPT;

   char tmp[2];

   io.read(tmp,1);

   if(strlen(tmp)<1 || atoi(tmp) < 1 || atoi(tmp) > 3) return;

   user.dateFormat = atoi(tmp) - 1;

   if(data)
      LOG(1,"Date format changed to '%s'",date_formats_long[user.dateFormat]);
}
