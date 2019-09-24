#include <string.h>
#include "procfg.hpp"

static Window *busywin = NULL;

void
ShowBusy(char *msg)
{
   if(busywin)
      return;

   busywin = new Window;

   int len = strlen(msg);

   len += 10;

   if(len > tsw_hsize-6)
      len = tsw_hsize-6;

   busywin->open( 1,
                  1,
                  len,
                  5,
                  0x0F,
                  CENTERED_X | CENTERED_Y | SHADOW,
                  CHISEL_BORDER,
                  0x07,
                  NULL,
                  0x08 );

   busywin->centerLine( 2,
                        msg,
                        0x0E );
}

void
HideBusy()
{
   if(busywin)
      delete busywin;

   busywin = NULL;
}



void KeyItem( Window &w,
              int     x,
              int     y,
              ATTR    iKeyColor,
              char   *pKey,
              ATTR    iDescColor,
              char   *pDesc )
{
     w.direct( x - strlen( pKey ) - 1,
               y,
               iKeyColor,
               pKey );

     w.direct( x,
               y,
               iDescColor,
               pDesc );
}
