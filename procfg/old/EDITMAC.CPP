#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"

void
edit_macros()
{
Field frm[]=
   {
    { FRM_STRING  , cfg.sysopkeys[0]     , 0,0 , 0         , 59 , 40 , 11,2  },
    { FRM_STRING  , cfg.sysopkeys[1]     , 0,0 , 0         , 59 , 40 , 11,3  },
    { FRM_STRING  , cfg.sysopkeys[2]     , 0,0 , 0         , 59 , 40 , 11,4  },
    { FRM_STRING  , cfg.sysopkeys[3]     , 0,0 , 0         , 59 , 40 , 11,5  },
    { FRM_STRING  , cfg.sysopkeys[4]     , 0,0 , 0         , 59 , 40 , 11,6  },
    { FRM_STRING  , cfg.sysopkeys[5]     , 0,0 , 0         , 59 , 40 , 11,7  },
    { FRM_STRING  , cfg.sysopkeys[6]     , 0,0 , 0         , 59 , 40 , 11,8  },
    { FRM_STRING  , cfg.sysopkeys[7]     , 0,0 , 0         , 59 , 40 , 11,9  },
    { FRM_STRING  , cfg.sysopkeys[8]     , 0,0 , 0         , 59 , 40 , 11,10 },
    { FRM_STRING  , cfg.sysopkeys[9]     , 0,0 , 0         , 59 , 40 , 11,11 },
   };

FormWindow w( 23,
              8,
              78,
              21,
              0x5F,
              SHADOW | BRACKETS | CENTERED_X,
              CHISEL_BORDER,
              0x5D,
              NULL,
              0x51 );

w.define( frm,
          10,
          0x5E,
          0x4E );

w.open();
w.attrib( 0x5F );
w.title("Sysop Macros", 0x5E );
w << "\n  Alt-F1:\n"
       "  Alt-F2:\n"
       "  Alt-F3:\n"
       "  Alt-F4:\n"
       "  Alt-F5:\n"
       "  Alt-F6:\n"
       "  Alt-F7:\n"
       "  Alt-F8:\n"
       "  Alt-F9:\n"
       " Alt-F10:\n";

tsw_showfooter("Enter  \"\001@<doscommand>\001\"  OR  \"\001<keystrokes>\001\" ('\001|\001'=Enter , '\001^\001'=Esc)", BAR_NORM, BAR_HIGH );
tsw_cursoron();
w.process();
tsw_cursoroff();
}

