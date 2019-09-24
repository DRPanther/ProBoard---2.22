#include <tswin.hpp>
#include "procfg.hpp"

void edit_hourly();
void edit_weekly();

void
edit_stats()
{
 menu_item mi[] = {
                   { 'H' , 2,2, " Hourly Usage" },
           //        { 'D' , 2,2, " Daily Usage"  },
                   { 'W' , 2,2, " Weekly Usage" },
                   { 0   , 0,0, NULL            }
                  };
 PopMenu pm(mi,0x70,0x5E);
 pm.open(40,18,65,22,0x5F,SHADOW);
 pm.title(" Edit Statistics ");

 for(;;)
   {
    int item = pm.process();
    switch(item)
      {
       case  1: edit_hourly(); break;
    // case  2: edit_daily();  break;
       case  2: edit_weekly(); break;
       default: return;
      }
   }
}

struct timelog
  {
   int numdays;
   Date lastdate;
   long hours[24];
   long days[7];
   long weeks[53];
   long totalcalls;
   int  daysactive;
   byte extra[100];
  };

static timelog tl;

static int
read_timelog()
{
 File f;

 if(!f.open(FileName(syspath,"TIMELOG.PRO"),fmode_read)) return -1;

 f.read(&tl,sizeof(tl));

 return 0;
}

static void
write_timelog()
{
 File f;

 f.open(FileName(syspath,"TIMELOG.PRO"),fmode_create);

 f.write(&tl,sizeof(tl));
}


void
edit_hourly()
{
 if(read_timelog()<0)
   {
    tsw_beep();
    return;
   }

 Field fld[] = {
                { FRM_LONG, &tl.hours[0]   , 0,0 , 0         , 7  , 8 , 16,2  },
                { FRM_LONG, &tl.hours[1]   , 0,0 , 0         , 7  , 8 , 16,3  },
                { FRM_LONG, &tl.hours[2]   , 0,0 , 0         , 7  , 8 , 16,4  },
                { FRM_LONG, &tl.hours[3]   , 0,0 , 0         , 7  , 8 , 16,5  },
                { FRM_LONG, &tl.hours[4]   , 0,0 , 0         , 7  , 8 , 16,6  },
                { FRM_LONG, &tl.hours[5]   , 0,0 , 0         , 7  , 8 , 16,7  },
                { FRM_LONG, &tl.hours[6]   , 0,0 , 0         , 7  , 8 , 16,8  },
                { FRM_LONG, &tl.hours[7]   , 0,0 , 0         , 7  , 8 , 16,9  },
                { FRM_LONG, &tl.hours[8]   , 0,0 , 0         , 7  , 8 , 16,10 },
                { FRM_LONG, &tl.hours[9]   , 0,0 , 0         , 7  , 8 , 16,11 },
                { FRM_LONG, &tl.hours[10]  , 0,0 , 0         , 7  , 8 , 16,12 },
                { FRM_LONG, &tl.hours[11]  , 0,0 , 0         , 7  , 8 , 16,13 },
                { FRM_LONG, &tl.hours[12]  , 0,0 , 0         , 7  , 8 , 41,2  },
                { FRM_LONG, &tl.hours[13]  , 0,0 , 0         , 7  , 8 , 41,3  },
                { FRM_LONG, &tl.hours[14]  , 0,0 , 0         , 7  , 8 , 41,4  },
                { FRM_LONG, &tl.hours[15]  , 0,0 , 0         , 7  , 8 , 41,5  },
                { FRM_LONG, &tl.hours[16]  , 0,0 , 0         , 7  , 8 , 41,6  },
                { FRM_LONG, &tl.hours[17]  , 0,0 , 0         , 7  , 8 , 41,7  },
                { FRM_LONG, &tl.hours[18]  , 0,0 , 0         , 7  , 8 , 41,8  },
                { FRM_LONG, &tl.hours[19]  , 0,0 , 0         , 7  , 8 , 41,9  },
                { FRM_LONG, &tl.hours[20]  , 0,0 , 0         , 7  , 8 , 41,10 },
                { FRM_LONG, &tl.hours[21]  , 0,0 , 0         , 7  , 8 , 41,11 },
                { FRM_LONG, &tl.hours[22]  , 0,0 , 0         , 7  , 8 , 41,12 },
                { FRM_LONG, &tl.hours[23]  , 0,0 , 0         , 7  , 8 , 41,13 },
                { FRM_LONG, &tl.totalcalls , 0,0 , 0         , 7  , 8 , 34,15 }
               };

 FormWindow w(9,3,62,20,0x3F,SHADOW);
 w.open();
 w.title(form(" Hourly System Usage (%d days) ",tl.numdays));
 w << "\n";

 for(int i=0;i<12;i++) w << form(" %02d:00-%02d:59 :            %02d:00-%02d:59 :\n",i,i,i+12,i+12);

 w << "\n          Total # remote calls :";

 w.define(fld,25,0x3E);

 tsw_cursoron();

 w.process();

 tsw_cursoroff();

 write_timelog();
}


void
edit_weekly()
{
 if(read_timelog()<0)
   {
    tsw_beep();
    return;
   }

 Field fld[] = {
                { FRM_LONG, &tl.weeks[0]   , 0,0 , 0         , 5  , 6 ,  7, 2  },
                { FRM_LONG, &tl.weeks[1]   , 0,0 , 0         , 5  , 6 ,  7, 3  },
                { FRM_LONG, &tl.weeks[2]   , 0,0 , 0         , 5  , 6 ,  7, 4  },
                { FRM_LONG, &tl.weeks[3]   , 0,0 , 0         , 5  , 6 ,  7, 5  },
                { FRM_LONG, &tl.weeks[4]   , 0,0 , 0         , 5  , 6 ,  7, 6  },
                { FRM_LONG, &tl.weeks[5]   , 0,0 , 0         , 5  , 6 ,  7, 7  },
                { FRM_LONG, &tl.weeks[6]   , 0,0 , 0         , 5  , 6 ,  7, 8  },
                { FRM_LONG, &tl.weeks[7]   , 0,0 , 0         , 5  , 6 ,  7, 9  },
                { FRM_LONG, &tl.weeks[8]   , 0,0 , 0         , 5  , 6 ,  7,10  },
                { FRM_LONG, &tl.weeks[9]   , 0,0 , 0         , 5  , 6 ,  7,11  },
                { FRM_LONG, &tl.weeks[10]  , 0,0 , 0         , 5  , 6 ,  7,12  },
                { FRM_LONG, &tl.weeks[11]  , 0,0 , 0         , 5  , 6 , 19, 2  },
                { FRM_LONG, &tl.weeks[12]  , 0,0 , 0         , 5  , 6 , 19, 3  },
                { FRM_LONG, &tl.weeks[13]  , 0,0 , 0         , 5  , 6 , 19, 4  },
                { FRM_LONG, &tl.weeks[14]  , 0,0 , 0         , 5  , 6 , 19, 5  },
                { FRM_LONG, &tl.weeks[15]  , 0,0 , 0         , 5  , 6 , 19, 6  },
                { FRM_LONG, &tl.weeks[16]  , 0,0 , 0         , 5  , 6 , 19, 7  },
                { FRM_LONG, &tl.weeks[17]  , 0,0 , 0         , 5  , 6 , 19, 8  },
                { FRM_LONG, &tl.weeks[18]  , 0,0 , 0         , 5  , 6 , 19, 9  },
                { FRM_LONG, &tl.weeks[19]  , 0,0 , 0         , 5  , 6 , 19,10  },
                { FRM_LONG, &tl.weeks[20]  , 0,0 , 0         , 5  , 6 , 19,11  },
                { FRM_LONG, &tl.weeks[21]  , 0,0 , 0         , 5  , 6 , 19,12  },
                { FRM_LONG, &tl.weeks[22]  , 0,0 , 0         , 5  , 6 , 31, 2  },
                { FRM_LONG, &tl.weeks[23]  , 0,0 , 0         , 5  , 6 , 31, 3  },
                { FRM_LONG, &tl.weeks[24]  , 0,0 , 0         , 5  , 6 , 31, 4  },
                { FRM_LONG, &tl.weeks[25]  , 0,0 , 0         , 5  , 6 , 31, 5  },
                { FRM_LONG, &tl.weeks[26]  , 0,0 , 0         , 5  , 6 , 31, 6  },
                { FRM_LONG, &tl.weeks[27]  , 0,0 , 0         , 5  , 6 , 31, 7  },
                { FRM_LONG, &tl.weeks[28]  , 0,0 , 0         , 5  , 6 , 31, 8  },
                { FRM_LONG, &tl.weeks[29]  , 0,0 , 0         , 5  , 6 , 31, 9  },
                { FRM_LONG, &tl.weeks[30]  , 0,0 , 0         , 5  , 6 , 31,10  },
                { FRM_LONG, &tl.weeks[31]  , 0,0 , 0         , 5  , 6 , 31,11  },
                { FRM_LONG, &tl.weeks[32]  , 0,0 , 0         , 5  , 6 , 31,12  },
                { FRM_LONG, &tl.weeks[33]  , 0,0 , 0         , 5  , 6 , 43, 2  },
                { FRM_LONG, &tl.weeks[34]  , 0,0 , 0         , 5  , 6 , 43, 3  },
                { FRM_LONG, &tl.weeks[35]  , 0,0 , 0         , 5  , 6 , 43, 4  },
                { FRM_LONG, &tl.weeks[36]  , 0,0 , 0         , 5  , 6 , 43, 5  },
                { FRM_LONG, &tl.weeks[37]  , 0,0 , 0         , 5  , 6 , 43, 6  },
                { FRM_LONG, &tl.weeks[38]  , 0,0 , 0         , 5  , 6 , 43, 7  },
                { FRM_LONG, &tl.weeks[39]  , 0,0 , 0         , 5  , 6 , 43, 8  },
                { FRM_LONG, &tl.weeks[40]  , 0,0 , 0         , 5  , 6 , 43, 9  },
                { FRM_LONG, &tl.weeks[41]  , 0,0 , 0         , 5  , 6 , 43,10  },
                { FRM_LONG, &tl.weeks[42]  , 0,0 , 0         , 5  , 6 , 43,11  },
                { FRM_LONG, &tl.weeks[43]  , 0,0 , 0         , 5  , 6 , 43,12  },
                { FRM_LONG, &tl.weeks[44]  , 0,0 , 0         , 5  , 6 , 55, 2  },
                { FRM_LONG, &tl.weeks[45]  , 0,0 , 0         , 5  , 6 , 55, 3  },
                { FRM_LONG, &tl.weeks[46]  , 0,0 , 0         , 5  , 6 , 55, 4  },
                { FRM_LONG, &tl.weeks[47]  , 0,0 , 0         , 5  , 6 , 55, 5  },
                { FRM_LONG, &tl.weeks[48]  , 0,0 , 0         , 5  , 6 , 55, 6  },
                { FRM_LONG, &tl.weeks[49]  , 0,0 , 0         , 5  , 6 , 55, 7  },
                { FRM_LONG, &tl.weeks[50]  , 0,0 , 0         , 5  , 6 , 55, 8  },
                { FRM_LONG, &tl.weeks[51]  , 0,0 , 0         , 5  , 6 , 55, 9  },
                { FRM_LONG, &tl.weeks[52]  , 0,0 , 0         , 5  , 6 , 55,10  },
                { FRM_LONG, &tl.totalcalls , 0,0 , 0         , 5  , 6 , 40,14  }
               };

 FormWindow w(8,5,72,20,0x1F,SHADOW);
 w.open();
 w.title(" Weekly System Usage ");
 w << "\n";

 for(int i=1;i<=11;i++) w << form(" %2d :        %2d :        %2d :        %2d :        %2d :\n",i,i+11,i+22,i+33,i+44);

 w.attrib(0x1B);
 w << "\n                Total # remote calls :";

 w.define(fld,54,0x1E);

 tsw_cursoron();

 w.process();

 tsw_cursoroff();

 write_timelog();
}
