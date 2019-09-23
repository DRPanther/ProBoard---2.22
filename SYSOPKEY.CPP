#define Use_LinkedList
#define Use_Handlers

#include <string.h>
#include <tswin.hpp>
#include "proboard.hpp"

void
sysopkey(KEY k)
{
 int i;

 if(k>256)
   {
      for( sysopkey_handlers.rewind() ; !sysopkey_handlers.eol() ; sysopkey_handlers++)
      {
         if( CallPEX( &sysopkey_handlers.get() , k ) ) return;
      }
   }

 if(k>=KEY_AF1 && k<=KEY_AF10)
   {
    String str = replace_stringvars(cfg.sysopkeys[(k/256)-0x68]);

    if(str[0]=='@')
      {
       shell(&str[1]);
      }
     else
     if(str[0]=='`')
       {
        io << &str[1];
       }
     else
      {
       for(i=0;i<strlen(str);i++)
          switch(str[i])
            {
             case '|': str[i] = ';'; break;
             case '^': str[i] = 27;  break;
            }
       io.inputbuffer.parse(str);
      }
   }

switch(k)
  {
  case KEY_HOME:
  case KEY_F2  : display_mode = 1;
                 break;
  case KEY_F3  : display_mode = 2;
                 break;
  case KEY_F4  : display_mode = 3;
                 break;
  case KEY_F5  : display_mode = 4;
                 break;
  case KEY_F6  : display_mode = 5;
                 break;
  case KEY_F7  : display_mode = 6;
                 break;
  case KEY_F8  : display_mode = 7;
                 break;
  case KEY_F9  : display_mode = 8;
                 break;
  case KEY_F10 : display_mode = 9;
                 break;
  case KEY_F1  : toggle_help();
                 break;
  case KEY_SF1 : toggle_macrohelp();
                 break;
//  case KEY_SF2 : toggle_userinfo();
//                 break;
  case KEY_ALTJ: shell("*\\*!*C*N*Q");
                 break;
//  case KEY_ALTX: no_hangup = TRUE;
//                 exit_proboard();
//                 break;
  case KEY_ALTE: if(!password_ok || user_recnr < 0) break;
                 io << '\n' << S_ONE_MOMENT_PLEASE << '\xFF';
                 edit_user();
                 io << "\n\n";
                 updatemenu = TRUE;
                 break;
  case KEY_ALTM: if(!curmenu[0]) break;
                 io << '\n' << S_ONE_MOMENT_PLEASE << '\xFF';
                 shell(replace_stringvars("*Q*N*SPROCFG.EXE ME @<CURMENU>@"));
                 io << "\n\n";
                 updatemenu = TRUE;
                 break;
  case KEY_ALTC: LOG(1,"Sysop breaks in for chat");
                 chat();
                 break;
  case KEY_ALTL: LOG("User locked out by sysop");
                 user.level=0;
                 exit_proboard();
  case KEY_ALTH: LOG("Sysop hung up");
                 exit_proboard();
  case KEY_ALTY: noyell_flag=!noyell_flag;
                 soundclick();
                 break;
  case KEY_ALTN: sysop_next = !sysop_next;
                 soundclick();
                 break;
  case KEY_ALTI: screen_image();
                 break;
  case KEY_ALTR: num_yells = 0;
                 break;
  case KEY_PGUP: display_mode--;
                 if(display_mode<1) display_mode=9;
                 break;
  case KEY_PGDN: display_mode++;
                 if(display_mode>9) display_mode=1;
                 break;
  case KEY_UP  : timer.increase();
                 display_mode = 1;
                 break;
  case KEY_DN  : timer.decrease();
                 display_mode = 1;
                 break;
  case KEY_CTL_LT  : {
                  soundclick();
                  soundclick();
                  soundclick();
                  user.level--;
                  for(i=0;i<num_limits;i++)
                    if(user.level>=limit[i].level)
                        {
                         user.level=limit[i].level;
                         break;
                        }
                  if(i==num_limits) user.level++;
                  adjust_limits();
                  timer.check();
                  display_mode = 1;
                  soundclick();
                  soundclick();
                  soundclick();
                 } break;
  case KEY_CTL_RT  : {
                  soundclick();
                  soundclick();
                  soundclick();
                  user.level++;
                  for(i=num_limits-1;i>=0;i--)
                    if(user.level<=limit[i].level)
                        {
                         user.level=limit[i].level;
                         break;
                        }
                  if(i<0) user.level--;
                  adjust_limits();
                  timer.check();
                  display_mode = 1;
                  soundclick();
                  soundclick();
                  soundclick();
                 } break;
   case KEY_ALTS: {
                   static char *blurbs="û üüäì þàü û åñþô þàü û åõöü ýùûûýþüüäì ñþôý üüäì þàü û åñþôý";
                   int off = int(clockticks()%50);
                   for(int i=0;i<6;i++)
                     {
                      io << blurbs[off+i];
                     }
                  } break;
  }

 update_display();
}

