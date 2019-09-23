#include <string.h>
#include <tswin.hpp>
#include "proboard.hpp"

const int MAX_ICONS = 200;

static Window rip_window;

static void
open_sending_rip()
{
   tsw_cursoroff();
   rip_window.open(tsw_hsize/2-15 , tsw_vsize/2 - 1 , tsw_hsize/2+15 , tsw_vsize/2 + 1 , 0x1F , 0);
   tsw_centerline(tsw_vsize/2,"SENDING RIP SEQUENCE",0x9E);
}

static void
close_sending_rip()
{
   rip_window.close();
   tsw_cursoron();
}

String
meganum( word num )
{
   int n[2];
   char s[3] = "00";

   n[0] = num / 36;
   n[1] = num % 36;

   for(int i = 0 ; i < 2 ; i++)
   {
      if(n[i] < 10)
         s[i] = '0' + n[i];
      else
         s[i] = 'A' + n[i] - 10;
   }

   return String(s);
}

void
rip_text_window( int x0, int y0 , int x1 , int y1 , int font )
{
   if(rip_mode)
   {
      io.show_local = FALSE;

      String s;

      s  << "\r!|w" << meganum(x0)
                    << meganum(y0)
                    << meganum(x1)
                    << meganum(y1)
                    << '1'
                    << char(font + '0')
                    << '\n';

      io << s << char(0xFF);

      io.show_local = TRUE;
   }
}

void
rip_reset()
{
   if(rip_mode) rip_send("\r!|1K|*\n");
}

bool
show_rip( char *ripfile , bool reset )
{
   if(!rip_mode) return FALSE;

   FileName fn(cfg.RIPpath,ripfile);
   fn.changeExt("RIP");

   if(reset) rip_reset();

   open_sending_rip();

   io.show_local = FALSE;
   io << '\r';
   char k = io.sendfile(fn , NULL);
   io.show_local = TRUE;

   close_sending_rip();

   return (k != ANS_NOFILE);
}

void
rip_send( char *s )
{
   if(!rip_mode) return;

   open_sending_rip();
   io.show_local = FALSE;
   io << s << char(0xFF);
   io.show_local = TRUE;
   close_sending_rip();
}

bool
rip_icon_exists( char *fn )
{
   String cmd = "\r!|1F010000";
   cmd << fn << "\n";
   rip_send(cmd);

   String response;
   dword ticks = clockticks();

   while(clockdiff(ticks)<30)
   {
      char k = io.readkey();

      if(k)
      {
         if(k == '\r')
         {
            if(response == "0")
            {
               return FALSE;
            }
            if(response == "1")
            {
               return TRUE;
            }
         }

         if(k == '\n') continue;

         response << k;
      }
   }

   return FALSE;
}


void
rip_send_icons()
{
   if(rip_mode)
   {
      FileName fn(cfg.iconspath,"*.IC?");

      String *icons = new String[MAX_ICONS];

      DirScan dir(fn);

      for(int i = 0 ; int(dir) && i<MAX_ICONS ; )
      {
         if(!rip_icon_exists(dir))
         {
            icons[i++] = (char *)dir;
         }

         dir++;
      }

      fn(cfg.iconspath,"*.HI?");

      dir.first(fn);

      for(; int(dir) && i<MAX_ICONS; )
      {
         if(!rip_icon_exists(dir))
         {
            icons[i++] = (char *)dir;
         }

         dir++;
      }

      if(i)
      {
         fn(mypath,"RIPICONS.$$$");

         File f(fn,fmode_text | fmode_create);

         for( i-- ; i>=0 ; i--)
         {
            f << cfg.iconspath << (char *)icons[i] << '\n';
         }
         f.close();

         rip_send("\r!|907020000<>\n");

         download(form("/F=@%s /K=Z /I /Q /N /T",(char *)fn));
      }
   }
}

void
rip_query( char *cmd )
{
   rip_send(form("\r!|10000%s\n" , cmd));
}

void
rip_color( int color )
{
   rip_send(form("\r!|c%02X\n",color));
}

void
rip_fill_color( int pattern , int color)
{
   rip_send(form("\r!|S%02X%02X\n",pattern,color));
}

void
rip_bar( int x1 , int y1 , int x2 , int y2 )
{
   String cmd = "\r!|B";

   cmd << meganum(x1)
       << meganum(y1)
       << meganum(x2)
       << meganum(y2)
       << '\n';

   rip_send(cmd);
}

void
rip_kill_mouse_fields()
{
   rip_send("\r!|1K\n");
}

void
rip_nomore()
{
   rip_send("\r!|#|#|#\n");
}

void
rip_get_image( int x1,int y1,int x2,int y2 )
{
   String cmd = "\r!|1C";

   cmd << meganum(x1)
       << meganum(y1)
       << meganum(x2)
       << meganum(y2)
       << "0\n";

   rip_send(cmd);
}

void
rip_put_image( int x,int y )
{
   String cmd = "\r!|1P";

   cmd << meganum(x)
       << meganum(y)
       << "000\n";

   rip_send(cmd);
}

void
rip_button(int x1,int y1,int x2,int y2,int hotkey,bool def,char *icon,char *text,char *hostcmd)
{
   String cmd = "\r!|1U";

   cmd << meganum(x1)
       << meganum(y1)
       << meganum(x2)
       << meganum(y2)
       << meganum(hotkey)
       << meganum(def ? 2:0)
       << '0'
       << icon << "<>" << text << "<>" << hostcmd
       << '\n';

    rip_send(cmd);
}


void
rip_start_dialog( char *prompt, int max )
{
   int width = max;

   if(strlen(prompt) > width) width = strlen(prompt);

   int offset = width/2 + width%2;

   rip_save_textwindow();
   rip_get_image(297-offset*8, 50 , 342+offset*8, 108 );
   rip_send("\r!|Y00000100|1B0000020PVK030F000F080700000F07000000\n");
   rip_button(300-offset*8,53,339+offset*8,105,0,FALSE,"","","");
   rip_send("\r!|1B0000020PHK030F000F080700000F07000000\n");
   rip_button(310-offset*8,56,329+offset*8,75,0,FALSE,"",prompt,"");

   offset = max/2+max%2;

   rip_text_window(40-offset,11,39+offset,11,0);
   rip_send("\r!|1B00000200LC030B00080F0100080F07000000\n");
   rip_button(310-offset*8,87,329+offset*8,98,0,FALSE,"","","");
   rip_nomore();

   io.show_local = FALSE;
   io.color(6);
   io.show_local = TRUE;
}

void
rip_end_dialog()
{
   rip_paste_clipboard();
   rip_restore_textwindow();
}

void
rip_start_yesno_dialog( char *prompt , bool def)
{
   int width = 30;

   if(strlen(prompt) > width) width = strlen(prompt);

   int offset = width/2 + width%2;

   rip_save_textwindow();
   rip_get_image(297-offset*8, 50 , 358+offset*8, 125 );
   rip_send("\r!|Y00000100|1B0000020PVK030F000F080700000F07000000\n");
   rip_button(300-offset*8,53,339+offset*8,120,0,FALSE,"","","");
   rip_send("\r!|1B0000020PHK030F000F080700000F07000000\n");
   rip_button(310-offset*8,56,329+offset*8,75,0,FALSE,"",prompt,"");

   rip_send("\r!|1B0000020SAI030E000F080700020F07000000\n");

   rip_text_window(0,0,0,0,0);
   rip_button(250,85,310,108,'Y',def ,"","Yes","Y");
   rip_button(329,85,389,108,'N',!def,"","No" ,"N");
   rip_nomore();
}

void
rip_textmode()
{
   if(user.screenLength == 38) user.screenLength = 23;

   if(rip_mode)
   {
      if(user.ripFont)
      {
         rip_screenlines = 22;
         rip_text_window(0,0,79,21,2);
      }
      else
      {
         rip_screenlines = 38;
         rip_text_window(0,0,79,37,0);
      }

      rip_send("\r!|Y00000100\n"
               "!|1B0000020074030F000F080F00000F07000000|1U018OHR9A0000<><>\n"
               "!|1B0000020PHC030F000F080700000F07000000|1U028PHR9A0000<><>\n"
               "!|#|#|#\n");

      rip_show_enter();
   }
}

void
rip_clear_status()
{
   rip_fill_color(1,7);
   rip_color(7);
   rip_bar(3,315,637,332);
   rip_kill_mouse_fields();
}

void
rip_show_enter()
{
   rip_send("\r!|1B0000020QOY030F000B080300000E07000000|1UF58THN960000<>Enter<>^m\n!|#|#|#\n");
}

void
rip_show_more()
{
   rip_send(
            "\r!|1B0000020QOY020F000B080100020E07000000\n"
            "!|1U2T8T47962H00<>Yes<>Y|1U4H8T5V962600<>No<>N|1U658T8N961V00<>Continuous<>C\n"
            "!|1B0000020PI8020B000F080700000E07000000|1U0B8S2J970000<>More?<>|#|#|#\n"
           );
}

void
rip_show_more_tag()
{
   rip_send(
            "\r!|1B0000020QOY020F000B080100020E07000000\n"
            "!|1U2T8T47962H00<>Yes<>Y|1U4H8T5V962600<>No<>N|1U658T8N961V00<>Continuous<>C\n"
            "!|1UAK8TDC961X00<>Edit Tagged<>E|1U8W8TAA962C00<>Tag<>T\n"
            "!|1B0000020PI8020B000F080700000E07000000|1U0B8S2J970000<>More?<>|#|#|#\n"
           );
}
