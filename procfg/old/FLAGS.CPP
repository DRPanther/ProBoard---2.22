#include <ctype.h>
#include "procfg.hpp"

KEY
flagonoff_select(int mode,Window& w,int x,int y,char attr,void *data)
{
   AccessFlags *af_on=((AccessFlags **)data)[0];
   AccessFlags *af_off=((AccessFlags **)data)[1];

   tsw_cursoroff();

   if(mode)
   {
      Window ww( x + w.minX - 1,
                 y + w.minY - 2,
                 x + w.minX + 36,
                 y + w.minY,
                 0xF,
                 BRACKETS,
                 CHISEL_BORDER,
                 0x07,
                 NULL,
                 0x08 );

      ww.open();

      ww.title( "Press A..Z , 1..6 to toggle", 0xE );

      ww.direct( 9,
                 2,
                 0xE,
                 " [F1] - Flag Help " );

      for(;;)
      {
         int x = 2;

         for(int i=1;i<=32;i++)
         {
            char c = (i<=26) ? ('A'+i-1) : ('0'+i-26);

            if(af_on->getflag(i))
            {
               ww.direct(x++,1,0xF,c);
            }
            else
            if(af_off->getflag(i))
            {
               ww.direct(x++,1,0x70,c);
            }
            else
            {
               ww.direct(x++,1,0xF,'-');
            }
         }

         KEY k=KB.get();

         if(k == KEY_F1) show_flags();

         if(k==27 || k==13) return 0;

         if(isalpha(k))
         {
            k = toupper(k)-'A'+1;
         }
         else
            if(k>='1' && k<='6')
            {
               k -= '1'-27;
            }
            else
               k = 0;

         if(k)
         {
            if(af_on->getflag(k))
            {
               af_on->clearflag(k);
               af_off->setflag(k);
            }
            else
            if(af_off->getflag(k))
            {
               af_on->clearflag(k);
               af_off->clearflag(k);
            }
            else
            {
               af_on->setflag(k);
               af_off->clearflag(k);
            }
         }
      }
   }

   for(int i=1;i<=32;i++)
   {
      char c = (i<=26) ? ('A'+i-1) : ('0'+i-26);

      if(af_on->getflag(i))
      {
         w.direct(x++,y,attr,c);
      }
      else
      if(af_off->getflag(i))
      {
         w.direct(x++,y,0x70,c);
      }
      else
      {
         w.direct(x++,y,attr,'-');
      }
   }

   tsw_cursoron();

   return 0;
}
