#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"

static void
prot_list_func(int i,int x,int y)
{
char str[81];
strcpy(str,p[i].name);
if(strlen(str)>32) str[30]=0;
tsw_maputs(x+1,y,0x30,str);
}

void
edit_protocol()
{
Field frm[]=
   {
    { FRM_STRING  , 0 , 0,0 , 0         , 49 , 49, 17,2  },  // Protocol name
    { FRM_CHAR    , 0 , 0,0 , FRM_UPPER , 1  , 1 , 17,4  },  // Protocol key
    { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,5  },  // batch
    { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,6  },  // enabled
    { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,7  },  // both
    { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 3 , 17,8  },  // bimodem
    { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 12 , 12, 17,9  },  // Protocol logfile
    { FRM_STRING  , 0 , 0,0 , FRM_UPPER , 12 , 12, 17,10 },  // Protocol ctlfile
    { FRM_STRING  , 0 , 0,0 , 0         , 127, 59, 17,12 },  // Download command
    { FRM_STRING  , 0 , 0,0 , 0         , 127, 59, 17,14 },  // Upload command
    { FRM_STRING  , 0 , 0,0 , 0         , 79 , 59, 17,16 },  // Download string
    { FRM_STRING  , 0 , 0,0 , 0         , 79 , 59, 17,17 },  // Upload string
    { FRM_STRING  , 0 , 0,0 , 0         , 39 , 39, 17,19 },  // Download keyword
    { FRM_STRING  , 0 , 0,0 , 0         , 39 , 39, 17,20 },  // Upload keyword
    { FRM_INT     , 0 , 0,0 , 0         , 2  , 3 , 17,22 },  // Upload keyword
    { FRM_UNSIGNED, 0 , 0,0 , 0         , 3  , 4 , 65,5  },
    { FRM_YESNO   , 0 , 0,0 , 0         , 3  , 4 , 65,7  }
   };

SelectWindow sw;
sw.open( 40,
         5,
         75,
         17,
         0x30,
         SHADOW | BRACKETS,
         CHISEL_BORDER,
         0x3F,
         NULL,
         0x38 );

sw.title( "Select Protocol", 0x3E );

sw.define( num_proto,
           0x1E,
           prot_list_func,
           0,
           NULL,
           select_hotkeys );

for(;;)
  {
  int batch,both,enabled,bimodem,loconly;

  tsw_showfooter( "\x1<Del>\x1 - Delete Protocol      \x1<Ins>\x1 - Add Protocol     \x1<Enter>\x1 - Edit", 
                  BAR_NORM,
                  BAR_HIGH );

  sw.redefine(num_proto);
  int choice=sw.process();
  if(choice==SL_ESC) break;
  if(choice==SL_HOTKEY)
     {
     switch(sw.hotkey)
       {
       case KEY_INS: memset(&p[num_proto],0,sizeof(protocol));
                     choice=num_proto++;
                     sw.current=choice;
                     break;
       case KEY_DEL: for(int i=sw.current;i<num_proto-1;i++) p[i]=p[i+1];
                     if(num_proto) num_proto--;
                     continue;
                     break;
       }
     }

  if(!p[choice].name[0])
    {
     p[choice].flags = PROT_ENABLED | PROT_BATCH;
     strcpy(p[choice].logfile,"DSZLOG.TXT");
     strcpy(p[choice].ctlfile,"DSZCTL.TXT");
     strcpy(p[choice].dlstring,"#");
     strcpy(p[choice].dlkeyword,"S");
     strcpy(p[choice].ulkeyword,"R");
     p[choice].wordnr = 10;
     p[choice].eff    = 95;
    }


  if(p[choice].eff>100 || p[choice].eff<50) p[choice].eff=90;

  batch  =(p[choice].flags&PROT_BATCH  )?1:0;
  both   =(p[choice].flags&PROT_BOTH   )?1:0;
  enabled=(p[choice].flags&PROT_ENABLED)?1:0;
  bimodem=(p[choice].flags&PROT_BIM    )?1:0;
  loconly=(p[choice].flags&PROT_LOCAL  )?1:0;

  frm[0].value=p[choice].name;
  frm[1].value=&p[choice].key;
  frm[2].value=&batch;
  frm[3].value=&enabled;
  frm[4].value=&both;
  frm[5].value=&bimodem;
  frm[6].value=p[choice].logfile;
  frm[7].value=p[choice].ctlfile;
  frm[8].value=p[choice].dlcom;
  frm[9].value=p[choice].ulcom;
  frm[10].value=p[choice].dlstring;
  frm[11].value=p[choice].ulstring;
  frm[12].value=p[choice].dlkeyword;
  frm[13].value=p[choice].ulkeyword;
  frm[14].value=&p[choice].wordnr;
  frm[15].value=&p[choice].eff;
  frm[16].value=&loconly;

  FormWindow w( 1,
                1,
                80,
                25,
                0x1F,
                BRACKETS,
                CHISEL_BORDER,
                0x19,
                NULL,
                0x10 );

  w.open();

  w.title( "Protocol Configuration", 0x1E );

  w.attrib( 0x1F );

  w << "\n Protocol Name:\n\n"
         "        Hotkey:\n"
         "         Batch:                                     Efficiency:      %\n"
         "       Enabled:\n"
         "      Both-way:                                     Local only:\n"
         "       Bimodem:\n"
         "      Log-file:\n"
         "  Control-file:\n\n"
         "  Download cmd:\n\n"
         "    Upload cmd:\n\n"
         " Downl. string:\n"
         " Upload string:\n\n"
         "  Downl. keyw.:\n"
         "  Upload keyw.:\n\n"
         "  File word nr:";

  w.define( frm,
            17,
            0x1B,
            0x4E );

  tsw_cursoron();
  w.process();
  tsw_cursoroff();

  p[choice].flags=0;

  if(batch)   p[choice].flags |= PROT_BATCH;
  if(both)    p[choice].flags |= PROT_BOTH;
  if(enabled) p[choice].flags |= PROT_ENABLED;
  if(bimodem) p[choice].flags |= PROT_BIM;
  if(loconly) p[choice].flags |= PROT_LOCAL;
  }
}
