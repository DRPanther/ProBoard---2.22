#include <string.h>
#include <stdio.h>
#include <tswin.hpp>
#include "procfg.hpp"

void edit_modem(int node);

static void
edit_connect()
{
    Field frm[]=
    {
        { FRM_STRING  , pbcfg.msgCon300     , 0,0 , 0         , 79 , 20, 19,2 },
        { FRM_STRING  , pbcfg.msgCon1200    , 0,0 , 0         , 79 , 20, 19,3 },
        { FRM_STRING  , pbcfg.msgCon1275    , 0,0 , 0         , 79 , 20, 19,4  },
        { FRM_STRING  , pbcfg.msgCon2400    , 0,0 , 0         , 79 , 20, 19,5  },
        { FRM_STRING  , pbcfg.msgCon4800    , 0,0 , 0         , 79 , 20, 19,6  },
        { FRM_STRING  , pbcfg.msgCon7200    , 0,0 , 0         , 79 , 20, 19,7  },
        { FRM_STRING  , pbcfg.msgCon9600    , 0,0 , 0         , 79 , 20, 19,8  },
        { FRM_STRING  , pbcfg.msgCon12000   , 0,0 , 0         , 79 , 20, 19,9  },
        { FRM_STRING  , pbcfg.msgCon14400   , 0,0 , 0         , 79 , 20, 19,10 },
        { FRM_STRING  , pbcfg.msgCon16800   , 0,0 , 0         , 79 , 20, 19,11 },
        { FRM_STRING  , pbcfg.msgCon19200   , 0,0 , 0         , 79 , 20, 19,12 },
        { FRM_STRING  , pbcfg.msgCon21600   , 0,0 , 0         , 79 , 20, 19,13 },
        { FRM_STRING  , pbcfg.msgCon24000   , 0,0 , 0         , 79 , 20, 19,14 },
        { FRM_STRING  , pbcfg.msgCon26400   , 0,0 , 0         , 79 , 20, 19,15 },

        { FRM_STRING  , pbcfg.msgCon28800   , 0,0 , 0         , 79 , 20, 57,2  },
        { FRM_STRING  , pbcfg.msgCon31200   , 0,0 , 0         , 79 , 20, 57,3  },
        { FRM_STRING  , pbcfg.msgCon33600   , 0,0 , 0         , 79 , 20, 57,4  },
        { FRM_STRING  , pbcfg.msgCon36000   , 0,0 , 0         , 79 , 20, 57,5  },
        { FRM_STRING  , pbcfg.msgCon38400   , 0,0 , 0         , 79 , 20, 57,6  },
        { FRM_STRING  , pbcfg.msgCon57600   , 0,0 , 0         , 79 , 20, 57,7  },
        { FRM_STRING  , pbcfg.msgCon64000   , 0,0 , 0         , 79 , 20, 57,8  },
        { FRM_STRING  , pbcfg.msgCon115200  , 0,0 , 0         , 79 , 20, 57,9  },
        { FRM_LONG    , &pbcfg.userConBps1   , 0,0 , 0         ,  5 ,  6, 40,10 },
        { FRM_STRING  , pbcfg.msgConUser1   , 0,0 , 0         , 79 , 20, 57,10 },
        { FRM_LONG    , &pbcfg.userConBps2   , 0,0 , 0         ,  5 ,  6, 40,11 },
        { FRM_STRING  , pbcfg.msgConUser2   , 0,0 , 0         , 79 , 20, 57,11 },
        { FRM_LONG    , &pbcfg.userConBps3   , 0,0 , 0         ,  5 ,  6, 40,12 },
        { FRM_STRING  , pbcfg.msgConUser3   , 0,0 , 0         , 79 , 20, 57,12 },
        { FRM_LONG    , &pbcfg.userConBps4   , 0,0 , 0         ,  5 ,  6, 40,13 },
        { FRM_STRING  , pbcfg.msgConUser4   , 0,0 , 0         , 79 , 20, 57,13 },
        { FRM_LONG    , &pbcfg.userConBps5   , 0,0 , 0         ,  5 ,  6, 40,14 },
        { FRM_STRING  , pbcfg.msgConUser5   , 0,0 , 0         , 79 , 20, 57,14 },
        { FRM_LONG    , &pbcfg.userConBps6   , 0,0 , 0         ,  5 ,  6, 40,15 },
        { FRM_STRING  , pbcfg.msgConUser6   , 0,0 , 0         , 79 , 20, 57,15 }
    };

    FormWindow w( 1,
                  5,
                  80,
                  22,
                  0x3F,
                  SHADOW | BRACKETS,
                  CHISEL_BORDER,
                  0x3B,
                  NULL,
                  0x31 );

    w.open();

    w.title( "Connect Result Strings", 0x3E );

    w.attrib(0x3F);

    w  << "\n    300 bps call:                      28800 bps call:\n"
            "   1200 bps call:                      31200 bps call:\n"
            "1200/75 bps call:                      33600 bps call:\n"
            "   2400 bps call:                      36000 bps call:\n"
            "   4800 bps call:                      38400 bps call:\n"
            "   7200 bps call:                      57600 bps call:\n"
            "   9600 bps call:                      64000 bps call:\n"
            "  12000 bps call:                     115200 bps call:\n"
            "  14400 bps call:                            bps call:\n"
            "  16800 bps call:                            bps call:\n"
            "  19200 bps call:                            bps call:\n"
            "  21600 bps call:                            bps call:\n"
            "  24000 bps call:                            bps call:\n"
            "  26400 bps call:                            bps call:\n";


    w.define( frm,
              34,
              0x31,
              0x4E );

    tsw_cursoron();
    w.process();
    tsw_cursoroff();
}



static void
edit_commands()
{
    bool manual_answer = !!(pbcfg.flags & MODEM_MANUAL_ANSWER);

    Field frm[]=
    {
        { FRM_STRING  , pbcfg.cmdInit1       , 0,0 , 0         , 79 , 30, 21,2  },
        { FRM_STRING  , pbcfg.cmdInit2       , 0,0 , 0         , 79 , 30, 21,3  },
        { FRM_STRING  , pbcfg.cmdInit3       , 0,0 , 0         , 79 , 30, 21,4  },
        { FRM_STRING  , pbcfg.msgOk          , 0,0 , 0         , 79 , 30, 21,6  },
        { FRM_STRING  , pbcfg.cmdOffHook     , 0,0 , 0         , 79 , 30, 21,7  },
        { FRM_STRING  , pbcfg.cmdDown        , 0,0 , 0         , 79 , 30, 21,8  },
        { FRM_YESNO   , &manual_answer       , 0,0 , 0         , 3  , 3 , 21,10 },
        { FRM_STRING  , pbcfg.msgRing        , 0,0 , 0         , 79 , 30, 21,11 },
        { FRM_STRING  , pbcfg.cmdAnswer      , 0,0 , 0         , 79 , 30, 21,12 },
        { FRM_STRING  , pbcfg.msgConExternal , 0,0 , 0         , 79 , 30, 21,14 },
        { FRM_INT     , &pbcfg.externalErrorLevel,0,0,0        , 3  , 4 , 21,15 }
    };

    FormWindow w( 4,
                  6,
                  60,
                  22,
                  0x3F,
                  SHADOW | BRACKETS,
                  CHISEL_BORDER,
                  0x3B,
                  NULL,
                  0x31 );

    w.open();

    w.title( "Command/Message Strings", 0x3E);

    w.attrib(0x3F);

    w  << "\n    Init Command 1:\n"
            "    Init Command 2:\n"
            "    Init Command 3:\n\n"
            "        Ok Message:\n"
            "  Off Hook Command:\n"
            "      Down Command:\n\n"
            "     Manual Answer:\n"
            "      RING Message:\n"
            "    Answer Command:\n\n"
            "  External/Fax Msg:\n"
            "     юд>Errorlevel:";

    w.define( frm,
              11,
              0x31,
              0x4E );

    tsw_cursoron();
    w.process();
    tsw_cursoroff();

    if(manual_answer)
      pbcfg.flags |= MODEM_MANUAL_ANSWER;
    else
      pbcfg.flags &= ~MODEM_MANUAL_ANSWER;
}

static void
edit_hardware()
{
    bool locked = !!(pbcfg.flags & MODEM_LOCKED);

    Field frm[]=
    {
        { FRM_INT     , &pbcfg.port          , 0,0 , 0         , 1  , 2 , 18,2  },
        { FRM_YESNO   , &locked              , 0,0 , 0         , 3  , 3 , 18,4  },
        { FRM_LONG    , &pbcfg.maxBps        , 0,0 , 0         , 6  , 7 , 18,5  },
        { FRM_INT     , &pbcfg.commandDelay  , 0,0 , 0         , 2  , 2 , 18,7  },
        { FRM_INT     , &pbcfg.answerDelay   , 0,0 , 0         , 2  , 2 , 18,8  }
    };

    FormWindow w( 24,
                  7,
                  55,
                  17,
                  0x3F,
                  SHADOW | BRACKETS,
                  CHISEL_BORDER,
                  0x3B,
                  NULL,
                  0x31 );

    w.open();

    w.title( "Modem Hardware Setup", 0x3E );

    w.attrib(0x30);

    w  << "\n       COM Port:\n"
            "\n"
            "   Locked Speed:\n"
            "   Max Bps Rate:\n"
            "\n"
            "    Modem Delay:\n"
            "   Answer Delay:\n";

    w.define( frm,
              5,
              0x31,
              0x4E );

    tsw_cursoron();
    w.process();
    tsw_cursoroff();

    if(locked)
      pbcfg.flags |= MODEM_LOCKED;
    else
      pbcfg.flags &= ~MODEM_LOCKED;
}


/*
static void
edit_other()
{
    Field frm[]=
    {
        { FRM_STRING  , pbcfg.ext_connect1   , 0,0 , 0         , 39 , 30, 18,1 },
        { FRM_INT     , &pbcfg.ext_errlevel1 , 0,0 , 0         ,  3 ,  4, 62,1 },
        { FRM_STRING  , pbcfg.ext_connect2   , 0,0 , 0         , 39 , 30, 18,2 },
        { FRM_INT     , &pbcfg.ext_errlevel2 , 0,0 , 0         ,  3 ,  4, 62,2 }
    };

    FormWindow w(4,6,73,9,0x3F,SHADOW|EXPLODE);
    w.open();
    w.title(" External Mail Result Strings ",0x3E);
    w.attrib(0x3F);
    w  << " Match String 1:                                 Errorlevel:\n"
          " Match String 2:                                 Errorlevel:";

    w.define(frm,4,0x3E,0x70);
    tsw_cursoron();
    w.process();
    tsw_cursoroff();
}

*/

/*
void
edit_pb()
{
Field frm[]=
   {
    { FRM_UNSIGNED, &pbcfg.baud          , 0,0 , 0         , 5  , 6 , 18,2  },
    { FRM_INT     , &pbcfg.port          , 0,0 , 0         , 1  , 2 , 18,3  },
    { FRM_INT     , &pbcfg.blanktime     , 0,0 , 0         , 4  , 4 , 18,4  },
    { FRM_INT     , &pbcfg.delay         , 0,0 , 0         , 2  , 2 , 18,5  },
    { FRM_STRING  , pbcfg.initstring     , 0,0 , 0         , 69 , 30, 18,6  },
    { FRM_STRING  , pbcfg.initok         , 0,0 , 0         , 39 , 30, 18,7  },
    { FRM_STRING  , pbcfg.busystring     , 0,0 , 0         , 69 , 30, 18,8  },
    { FRM_YESNO   , &pbcfg.forceanswer   , 0,0 , 0         , 3  , 3 , 18,9  },
    { FRM_STRING  , pbcfg.ring           , 0,0 , 0         , 39 , 30, 18,10 },
    { FRM_STRING  , pbcfg.answer         , 0,0 , 0         , 39 , 30, 18,11 },
    { FRM_STRING  , pbcfg.connect300     , 0,0 , 0         , 39 , 30, 18,13 },
    { FRM_STRING  , pbcfg.connect1200    , 0,0 , 0         , 39 , 30, 18,14 },
    { FRM_STRING  , pbcfg.connect2400    , 0,0 , 0         , 39 , 30, 18,15 },
    { FRM_STRING  , pbcfg.connect4800    , 0,0 , 0         , 39 , 30, 18,16 },
    { FRM_STRING  , pbcfg.connect7200    , 0,0 , 0         , 39 , 30, 18,17 },
    { FRM_STRING  , pbcfg.connect9600    , 0,0 , 0         , 39 , 30, 18,18 },
    { FRM_STRING  , pbcfg.connect12000   , 0,0 , 0         , 39 , 30, 18,19 },
    { FRM_STRING  , pbcfg.connect14400   , 0,0 , 0         , 39 , 30, 18,20 },
    { FRM_STRING  , pbcfg.connect19200   , 0,0 , 0         , 39 , 30, 18,21 },
    { FRM_STRING  , pbcfg.connect38400   , 0,0 , 0         , 39 , 30, 18,22 }
   };

FormWindow w(4,1,55,25,0x3F,SHADOW|EXPLODE);
w.open();
w.title(" Modem Parameters ",0x3E);
w.attrib(0x3F);
w  << "\n Max Baud Rate :\n"
        " Com-port      :\n"
        " Blanktime (s) :\n"
        " Modem delay   :\n"
        " Init string   :\n"
        " Init response :\n"
        " Busy string   :\n"
        " Manual Answer :\n"
        " RING string   :\n"
        " Answer string :\n\n"
        "   300 bps call:\n"
        "  1200 bps call:\n"
        "  2400 bps call:\n"
        "  4800 bps call:\n"
        "  7200 bps call:\n"
        "  9600 bps call:\n"
        " 12000 bps call:\n"
        " 14400 bps call:\n"
        " 19200 bps call:\n"
        " 38400 bps call:";

w.define(frm,20,0x3E,0x70);
tsw_cursoron();
w.process();
tsw_cursoroff();
}
*/



void
edit_pb()
{
    menu_item menuitems[]=
        {
        { 'D', 8,8, "       Default"  },
        { 'N', 5,5, "    Node Specific" },
        { 0,0,0,NULL }
        };

    PopMenu menu( menuitems,
                  0x70,
                  0x5E );

    menu.open( 45,
               15,
               70,
               18,
               0x5F,
               SHADOW | BRACKETS,
               CHISEL_BORDER,
               0x5D,
               NULL,
               0x51 );

    //menu.border(0x5E);

    menu.title( "Modem Parameters", 0x5E );


    for(;;)
    {
        tsw_showfooter("", BAR_COLOR);
        int choice=menu.process();
        if(choice<1) break;
        switch(choice)
        {
            case 1: edit_modem(0); break;
            case 2:
               {
                  int n = 1;

                  tsw_cursoron();

                  Window w( 40,
                            17,
                            65,
                            19,
                            0x30,
                            SHADOW,
                            CHISEL_BORDER,
                            0x3B,
                            NULL,
                            0x31 );

                  w.open();
                  w << " Enter Node #: ";

                  w.attrib(0x31);

                  w.scan(n,3,4);

                  tsw_cursoroff();

                  if(n < 1 || n > cfg.numnodes)
                     tsw_beep();
                  else
                  {
                     w.close();
                     edit_modem(n);
                  }
               }
               break;
        }
    }

}


void
edit_modem(int node)
{
   FileName fn,fn2;

   menu_item menuitems[]=
      {
         { 'H', 2,2, " Hardware Setup"  },
         { 'M', 4,4, " coMmand Strings" },
         { 'N', 4,4, " coNnect Strings" },
         { 0,0,0,NULL }
      };

   PopMenu menu( menuitems,
                 0x70,
                 0x5E );

   menu.open( 45,
              15,
              70,
              19,
              0x5F,
              SHADOW | BRACKETS,
              CHISEL_BORDER,
              0x5D,
              NULL,
              0x51 );

   //menu.border(0x5F);

   if(node)
      menu.title( form( "Setup #%d", node ), 0x5E );
   else
      menu.title( "Default Setup", 0x5E );

   if(node)
     fn(syspath,form("MODEM%03d.PB",node));
   else
     fn(syspath,"MODEM.PB");

   File f;
   word crc;

   memset(&pbcfg,0,sizeof(pbcfg));

   if(!f.open(fn))
      f.open(FileName(syspath,"MODEM.PB"));

   if(f.opened())
   {
      f.read(&pbcfg,sizeof(pbcfg));
      f.close();

      crc = ::crc(&pbcfg,sizeof(pbcfg));
   }
   else
   {
      crc=0xFFFF;
   }

    again:

    for(;;)
    {
        tsw_showfooter("", BAR_COLOR);
        int choice=menu.process();
        if(choice<1) break;
        switch(choice)
        {
            case 1: edit_hardware(); break;
            case 2: edit_commands(); break;
            case 3: edit_connect();  break;
 //           case 4: edit_other();    break;
        }
    }

   if(crc != ::crc(&pbcfg,sizeof(pbcfg)))
   {
      int save_choice = ask_save();

      if(save_choice < 0)
         goto again;

      if(save_choice == 0 )
      {
         fn2 = fn;
         fn2.changeExt("BAK");
         unlink(fn2);
         rename(fn,fn2);
         f.open(fn,fmode_create);
         f.write(&pbcfg,sizeof(pbcfg));
         f.close();
      }
   }

}
