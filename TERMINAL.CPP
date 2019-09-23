#include "proboard.hpp"

void
fossilio::move(int x,int y)
{
 if(!ansi_mode && !avatar) return;

 if(avatar) io << char(0x16) << char(8) << char(y) << char(x);
       else io << form("\x1b[%d;%dH",y,x);
}

String
fullcolor_string(byte x)
{
   String str;

   if(!ansi_mode && !avatar) return str;

   if(avatar)
   {
      str << char(22) << char(1) << char(x & 0x7F);
      if(x & 0x80) str << char(22) << char(2);
   }
   else
   {
      static char colors[] = { 0,4,2,6,1,5,3,7 };

      int fg = colors[x&7];
      int bg = colors[(x>>4)&7];
      int blink = (x & 0x80) ? 1:0;
      int high  = (x & 0x08) ? 1:0;

      str << char(27) << "[0;";

      if(high)
         str << "1;";
      if(blink) str << "5;";

      str << form("%d;%dm",fg+30,bg+40);
   }

   return str;
}

void
fossilio::fullcolor(byte x)
{
   io << fullcolor_string(x);
}

void
fossilio::clreol()
{
 if(!ansi_mode && !avatar) return;

 if(avatar) io << "\x16\x07";
       else io << "\x1b[K";
}
