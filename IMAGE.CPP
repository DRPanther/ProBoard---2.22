#include <stdio.h>
#include "proboard.hpp"
#include <tswin.hpp>

void
screen_image()
{
 File fp(form("%sIMAGE.TXT",syspath),fmode_write |
                                     fmode_text  |
                                     fmode_copen |
                                     fmode_append );

 if(!fp.opened()) return;

 String line;

 for(int y=SCREEN.minY;y<=SCREEN.maxY;y++)
   {
    int lastchar = 0;

    line = "";

    for(int x=SCREEN.minX;x<=SCREEN.maxX;x++)
      {
       char c = tsw_whaton(x,y);

       if(c!=' ') lastchar = x-SCREEN.minX+1;

       line << c;
      }

    line << "\n\n";

    line[lastchar] = '\n';
    line[lastchar+1] = '\0';

    fp.printf("%s",(char *)line);
   }
}


