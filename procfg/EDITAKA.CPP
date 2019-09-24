#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"

void
edit_akas()
{
   KEY hotkeys[]={ KEY_INS, KEY_DEL, 0 };
   Field frm[]=
      {
         { FRM_INT     , 0, 0,0, 0, 5  , 6  , 10,2 },
         { FRM_INT     , 0, 0,0, 0, 5  , 6  , 10,3 },
         { FRM_INT     , 0, 0,0, 0, 5  , 6  , 10,4 },
         { FRM_INT     , 0, 0,0, 0, 5  , 6  , 10,5 }
      };

   SelectWindow sw( num_aka,
                    0x70,
                    aka_select_func,
                    0,
                    NULL,
                    hotkeys );

   sw.open( 37,
            8,
            62,
            20,
            0x5F,
            SHADOW | BRACKETS,
            CHISEL_BORDER,
            0x5D,
            NULL,
            0x51 );

   sw.title( "Matrix Addresses", 0x5E );

   for(;;)
   {
      tsw_showfooter("\x1<Del>\x1 - Delete Aka      \x1<Ins>\x1 - Add Aka      \x1<Enter>\x1 - Edit", BAR_NORM, BAR_HIGH );

      sw.redefine(num_aka);

      int choice = sw.process();

      if(choice==SL_ESC) break;

      if(choice==SL_HOTKEY)
         switch(sw.hotkey)
         {
            case KEY_INS:
               {
                  memset(&akas[num_aka],0,sizeof(aka));
                  choice=num_aka++;
                  sw.current=choice;
               }
               break;

            case KEY_DEL:
               {
                  for(int i=sw.current;i<num_aka-1;i++)
                     akas[i] = akas[i+1];

                  if(num_aka)
                     num_aka--;

                  continue;
               }
         }

      FormWindow w( 31,
                    8,
                    51,
                    15,
                    0x3F,
                    SHADOW | EXPLODE | BRACKETS,
                    CHISEL_BORDER,
                    0x3B,
                    NULL,
                    0x38 );

      w.define( frm,
                4,
                0x31,
                0x4E );

      w.open();
      w.title( "Edit AKA", 0x3F );

      w.attrib( 0x3F );

      w << "\n   Zone:\n    Net:\n   Node:\n  Point:";

      frm[0].value=&akas[choice].zone;
      frm[1].value=&akas[choice].net;
      frm[2].value=&akas[choice].node;
      frm[3].value=&akas[choice].point;

      CursorState _cursor(TRUE);

      w.process();
   }
}

