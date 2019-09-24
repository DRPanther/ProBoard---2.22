#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"



static 
void limit_select(int i,int x,int y)
{
     tsw_maputs( x,
                 y,
                 0x3E,
                 form( " %-5s ³ %5u ³%5d' ³%5u ³%4d' ºº%5d ³ %2d%% ³%3d ³ %5d ³%5d ",
                       limit[i].id,
                       limit[i].level,
                       limit[i].timelimit,
                       limit[i].daily_klimit,
                       limit[i].pre_download,
                       limit[i].free,
                       limit[i].factor,
                       limit[i].msgfactor,
                       limit[i].max_download,
                       limit[i].fallto ) );

     tsw_maputc(x+7 ,y,0x3F,'³');
     tsw_maputc(x+15,y,0x3F,'³');
     tsw_maputc(x+23,y,0x3F,'³');
     tsw_maputc(x+30,y,0x3F,'³');
     tsw_maputs(x+37,y,0x3F,"ºº");
     tsw_maputc(x+45,y,0x3F,'³');
     tsw_maputc(x+51,y,0x3F,'³');
     tsw_maputc(x+56,y,0x3F,'³');
     tsw_maputc(x+64,y,0x3F,'³');
}



static 
void limit_clear( int x,
                  int y )
{
     tsw_maputs( x,
                 y,
                 0x3F,
                 "       ³       ³       ³      ³      ºº      ³     ³    ³       ³      ");
}



void edit_limits()
{
     KEY hotkeys[]  =  { KEY_INS, KEY_DEL, 0 };

     Field frm[]  =  {
                       { FRM_UNSIGNED, 0, 0,0, 0, 5  , 6  ,23,1 },
                       { FRM_INT     , 0, 0,0, 0, 3  , 6  ,23,2 },
                       { FRM_UNSIGNED, 0, 0,0, 0, 5  , 6  ,23,3 },
                       { FRM_INT     , 0, 0,0, 0, 3  , 6  ,23,4 },
                       { FRM_STRING  , 0, 0,0, 0, 5  , 6  ,23,6 },
                       { FRM_UNSIGNED, 0, 0,0, 0, 5  , 6  ,23,9 },
                       { FRM_SHORT   , 0, 0,0, 0, 2  , 6  ,23,10 },
                       { FRM_UNSIGNED, 0, 0,0, 0, 3  , 6  ,23,11 },
                       { FRM_UNSIGNED, 0, 0,0, 0, 5  , 6  ,23,12 },
                       { FRM_UNSIGNED, 0, 0,0, 0, 5  , 6  ,23,13 }
                     };

     SelectWindow sw( num_limits,
                      0x1F,
                      limit_select,
                      0,
                      NULL,
                      hotkeys,
                      NULLFUNC,
                      limit_clear );


     sw.open( 2,
              10,
              77,
              21,
              0x30,
              SHADOW,
              CHISEL_BORDER,
              0x3F,
              NULL,
              0x38 );

     sw.title( "ÄIDÄÄÄÂÄLevelÄÂTmLimitÂDailyKÂPre-DLÒÒÄFreeÄÂFactrÂMsgKÂÄMaxKbÄÂFallTo");

     sw.direct( 1,
                11,
                0x38,
                "ÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄĞĞÄÄÄÄÄÄÁÄÄÄÄÄÁÄÄÄÄÁÄÄÄÄÄÄÄÁ" );

     for(;;)
     {
          for(int i=0;i<num_limits-1;i++)
          {
               for(int j=i+1;j<num_limits;j++)
               {
                    if(limit[i].level<limit[j].level)
                    {
                         limits temp = limit[i];
                         limit[i] = limit[j];
                         limit[j] = temp;
                    }
               }
          }


          tsw_showfooter("\x1<Del>\x1 - Delete      \x1<Ins>\x1 - Add      \x1<Enter>\x1 - Edit", BAR_NORM, BAR_HIGH);
          sw.redefine(num_limits);
          int choice=sw.process();
          
          
          if(choice==SL_ESC) 
          {
               break;
          }


          if(choice==SL_HOTKEY)
          {
               switch(sw.hotkey)
               {
                    case KEY_INS: 
                    {
                         memset(&limit[num_limits],0,sizeof(*limit));
                         choice=num_limits++;
                         sw.current=choice;
                    } 
                         break;


                    case KEY_DEL:
                    
                         for(int i=sw.current;i<num_limits-1;i++) 
                         {
                              limit[i]=limit[i+1];
                         }


                         if(num_limits) 
                         {
                              num_limits--;
                         }


                         continue;
               }
          }


          FormWindow w( 9,
                        4,
                        49,
                        18,
                        0x3F,
                        SHADOW | EXPLODE,
                        CHISEL_BORDER,
                        0x3B,
                        NULL,
                        0x31 );
          
          
          w.define( frm,
                    10,
                    0x31,
                    0x4E );
          w.open();

          w.attrib( 0x3F );

          w << "      Security Level:\n"
               "        Time per day:        min\n"
               " Kb Download per day:        Kbytes\n"
               "      Download delay:        min\n\n"
               "        Usergroup ID:\n\n\n"
               "       Free Download:        Kbytes\n"
               "       Upload needed:        %\n"
               "   Free download/msg:        Kbytes\n"
               "    Maximum download:        Kbytes\n"
               "       Fall to level:";


          frm[0].value=&limit[choice].level;
          frm[1].value=&limit[choice].timelimit;
          frm[2].value=&limit[choice].daily_klimit;
          frm[3].value=&limit[choice].pre_download;
          frm[4].value=limit[choice].id;
          frm[5].value=&limit[choice].free;
          frm[6].value=&limit[choice].factor;
          frm[7].value=&limit[choice].msgfactor;
          frm[8].value=&limit[choice].max_download;
          frm[9].value=&limit[choice].fallto;
          
          
          tsw_cursoron();
          w.process();
          tsw_cursoroff();


       // if(limit[choice].daily_klimit < 0)
       // {
       //      limit[choice].daily_klimit = 0;
       // }
     }
}

