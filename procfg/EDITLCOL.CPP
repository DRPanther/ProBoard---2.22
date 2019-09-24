#include "procfg.hpp"

void
EditListColors()
{
   Window sample_w( 3,
                    16,
                    78,
                    23,
                    0x7,
                    0,
                    CHISEL_BORDER,
                    0x07,
                    NULL,
                    0x08 );

   sample_w.open();

   menu_item savemenu[]=
   {
      { 'T', 12,12," File Tag " },
      { 'N', 7,7, " File Name"   },
      { 'S', 7,7, " File Size"  },
      { 'D', 7,7, " File Date"  },
      { 'C', 7,7, " File Counter"  },
      { 'E', 8,8, " File Description"  },
      { 'X', 3,3, " Extended descriptions & seperators" },
      { 'M', 2,2, " Missing files" },
      { 'H', 2,2, " Hide/Show File Counter" },
      {  0 , 0,0, NULL                                }
   };

   PopMenu menu( savemenu,
                 0x70,
                 0x3E );

   menu.open( 20,
              3,
              61,
              13,
              0x3F,
              SHADOW | BRACKETS,
              CHISEL_BORDER,
              0x3B,
              NULL,
              0x31 );

   menu.title( "File List Format", 0x3E );

   for(;;)
   {
      sample_w.attrib(7);
      sample_w.clear();

      sample_w << "" << cfg.listColorTag      << "A ";
      sample_w << "" << cfg.listColorFileName << "PB_200.ZIP   ";
      sample_w << "" << cfg.listColorDate     << "01/02/1994 ";
      sample_w << "" << cfg.listColorSize     << " 450k ";
      if(!cfg.hideFileCounter)
      sample_w << "" << cfg.listColorCounter  << "[14] ";
      sample_w << "" << cfg.listColorDesc     << "ProBoard v2.00 BBS Software\n";
      sample_w.moveCursor(37-5*cfg.hideFileCounter,0);
      sample_w << "" << cfg.listColorDesc     << "\"The Choice of Professionals\"\n";
      sample_w << "" << cfg.listColorSeperator << "-----------------------------------------------------------------------\n";
      sample_w << "" << cfg.listColorTag      << "B ";
      sample_w << "" << cfg.listColorFileName << "PB_ESP.ZIP   ";
      sample_w << "" << cfg.listColorDate     << "18/04/1994 ";
      sample_w << "" << cfg.listColorSize     << "  10k ";
      if(!cfg.hideFileCounter)
      sample_w << "" << cfg.listColorCounter  << "[04] ";
      sample_w << "" << cfg.listColorDesc     << "Spanish language file for ProBoard\n";
      sample_w.moveCursor(37-5*cfg.hideFileCounter,0);
      sample_w << "" << cfg.listColorDesc     << "v2.00 or higher\n";
      sample_w << "" << cfg.listColorTag      << "  ";
      sample_w << "" << cfg.listColorFileName << "PB_DUT.ZIP   ";
      sample_w << "" << cfg.listColorMissing  << "   <MISSING>     ";
      if(!cfg.hideFileCounter)
      sample_w << "" << cfg.listColorCounter  << "[00] ";
      sample_w << "" << cfg.listColorDesc     << "Dutch language file for ProBoard";

      int choice = menu.process();
      byte *colorPtr;

      if(choice < 1)
         break;

      switch(choice)
      {
         case 1: colorPtr = &cfg.listColorTag;
                 break;
         case 2: colorPtr = &cfg.listColorFileName;
                 break;
         case 3: colorPtr = &cfg.listColorSize;
                 break;
         case 4: colorPtr = &cfg.listColorDate;
                 break;
         case 5: colorPtr = &cfg.listColorCounter;
                 break;
         case 6: colorPtr = &cfg.listColorDesc;
                 break;
         case 7: colorPtr = &cfg.listColorSeperator;
                 break;
         case 8: colorPtr = &cfg.listColorMissing;
                 break;
         case 9: cfg.hideFileCounter = !cfg.hideFileCounter;
                 continue;
      }

      edit_color(1,menu,7,5,menu.attrib(),colorPtr);
   }
}
