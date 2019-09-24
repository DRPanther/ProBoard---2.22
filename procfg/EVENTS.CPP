#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"


/*
_AccessEvent ev[27];

void
event_show(int i,int x,int y)
{
 Dstring s;

 s << (i ? char('A'+i-1) : '@') << 'ณ';

 for(int j = 0 ; j < 48 ; j++)
   {
    if(ev[i].hoursActive[j]) s << 'þ';
                      else   s << ' ';
    if((j & 1) && j < 47) s << 'ณ';
   }

 tsw_mputs(x+1,y,s);
}


void
edit_accessevents()
{
KEY hotkeys[]={ KEY_INS, KEY_DEL, 0 };

memset(ev,0,sizeof(ev));

memset(ev[0].hoursActive,1,48);

ev[1].hoursActive[20] = TRUE;
ev[1].hoursActive[21] = TRUE;
ev[1].hoursActive[22] = TRUE;

ev[2].hoursActive[12] = TRUE;
ev[2].hoursActive[23] = TRUE;
ev[2].hoursActive[24] = TRUE;
ev[2].hoursActive[25] = TRUE;

SelectWindow sw( 27,
                 0x70,
                 event_show,
                 0,
                 NULL,
                 hotkeys );

sw.open( 2,
         10,
         79,
         21,
         0x3F,
         EXPLODE );

sw.title(      "ออั00ั01ั02ั03ั04ั05ั06ั07ั08ั09ั10ั11ั12ั13ั14ั15ั16ั17ั18ั19ั20ั21ั22ั23");
sw.direct(1,11,"ออฯ00ฯ01ฯ02ฯ03ฯ04ฯ05ฯ06ฯ07ฯ08ฯ09ฯ10ฯ11ฯ12ฯ13ฯ14ฯ15ฯ16ฯ17ฯ18ฯ19ฯ20ฯ21ฯ22ฯ23");

for(;;)
  {
  tsw_showfooter("\001<Del>\001 - Delete      \001<Ins>\001 - Add      \001<Enter>\001 - Edit", BAR_NORM, BAR_HIGH);
  int choice=sw.process();
  if(choice==SL_ESC) break;
  }
}
*/



KEY
days_select(int mode,Window& w,int x,int y,char attr,void *data)
{
byte *days=(byte *)data;
int mon=0,tue=0,wed=0,thu=0,fri=0,sat=0,sun=0;

if(mode)
   {
   FormWindow ww( x + w.minX - 4,
                  y + w.minY - 3,
                  x + w.minX + 18,
                  y + w.minY + 7,
                  0xF,
                  EXPLODE | BRACKETS,
                  CHISEL_BORDER,
                  0x07,
                  NULL,
                  0x08 );

   ww.open();

   ww.title( "Days", 0xE );

   Field frm[]=
     {
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,2 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,3 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,4 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,5 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,6 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,7 },
      { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,8 }
     };

   ww.define( frm,
              7,
              0xE );

   mon=(*days&1)?1:0;
   tue=(*days&2)?1:0;
   wed=(*days&4)?1:0;
   thu=(*days&8)?1:0;
   fri=(*days&16)?1:0;
   sat=(*days&32)?1:0;
   sun=(*days&64)?1:0;

   frm[0].value=&mon;
   frm[1].value=&tue;
   frm[2].value=&wed;
   frm[3].value=&thu;
   frm[4].value=&fri;
   frm[5].value=&sat;
   frm[6].value=&sun;

   ww << "\n"
         "     Monday:\n"
         "    Tuesday:\n"
         "  Wednesday:\n"
         "   Thursday:\n"
         "     Friday:\n"
         "   Saturday:\n"
         "     Sunday:";

   ww.process();

   *days=0;

   if(mon) *days|=1;
   if(tue) *days|=2;
   if(wed) *days|=4;
   if(thu) *days|=8;
   if(fri) *days|=16;
   if(sat) *days|=32;
   if(sun) *days|=64;
   }
  else
   {
   String s("MTWTFSS");

   for(int i=0;i<7;i++) if(!((*days)&(1<<i))) s[i]='-';
   w.direct(x,y,(byte)attr,s);
   }

   return 0;
}

void
event_select(int i,int x,int y)
{
   String s("MTWTFSS");

   for(int d=0;d<7;d++) if(!(events[i].days&(1<<d))) s[d]='-';

   tsw_maputs(x+1,y,0x5E,form("[%c]  On %s, at %02d:%02d on node %d for %2d minutes",events[i].enabled?'+':'-',(char *)s,events[i].start[0],events[i].start[1],events[i].node,events[i].duration));
}

static char *types[]={ "Errorlevel","Command",NULL};

void
edit_events()
{
 //edit_accessevents();

KEY hotkeys[]={ KEY_INS, KEY_DEL, 0 };
Field frm[]=
   {
    { FRM_YESNO   , 0, 0,0, 0, 3  , 3  ,15,1 },
    { FRM_FUNCTION, 0, 0,days_select,0 , 7, 7,  15,3 },
    { FRM_STRING  , 0, 0,0, 0, 5  , 6 ,15,4 },
    { FRM_INT     , 0, 0,0, 0, 3  , 4  ,15,5 },
    { FRM_CHOICE  , 0, types,0,0,10,10,15,7 },
    { FRM_INT     , 0, 0,0, 0, 4  , 5  ,15,9 },
    { FRM_STRING  , 0, 0,0, 0, 79 , 20 ,15,10 },
    { FRM_SHORT   , 0, 0,0, 0, 3  , 4  ,15,12 }
   };
SelectWindow sw( num_events,
                 0x70,
                 event_select,
                 0,
                 NULL,
                 hotkeys );

sw.open( 12,
         10,
         67,
         21,
         0x5F,
         SHADOW | EXPLODE | BRACKETS,
         CHISEL_BORDER,
         0x5D,
         NULL,
         0x51 );

sw.title( "Events", 0x5E );

for(;;)
  {
  tsw_showfooter("\x1<Del>\x1 - Delete      \x1<Ins>\x1 - Add      \x1<Enter>\x1 - Edit", BAR_NORM, BAR_HIGH );
  sw.redefine(num_events);
  int choice=sw.process();
  if(choice==SL_ESC) break;
  if(choice==SL_HOTKEY)
     switch(sw.hotkey)
        {
        case KEY_INS: {
                      memset(&events[num_events],0,sizeof(event));
                      choice=num_events++;
                      sw.current=choice;
                      events[choice].node=1;
                      } break;
        case KEY_DEL: for(int i=sw.current;i<num_events-1;i++) events[i]=events[i+1];
                      if(num_events) num_events--;
                      continue;
        }

  FormWindow w( 9,
                4,
                47,
                17,
                0x3F,
                SHADOW | EXPLODE | BRACKETS,
                CHISEL_BORDER,
                0x3B,
                NULL,
                0x31 );

  w.open();

  w.attrib( 0x3F );

  w << "     Enabled:\n\n"
       " Active days:\n"
       "  Event Time:\n"
       "    Duration:     min.\n\n"
       "  Event type:\n\n"
       "  Errorlevel:\n"
       " DOS-Command:\n\n"
       " Node number:";

  char time[6];
  sprintf(time,"%02d:%02d",events[choice].start[0],events[choice].start[1]);

  frm[0].value=&events[choice].enabled;
  frm[1].value=&events[choice].days;
  frm[2].value=time;
  frm[3].value=&events[choice].duration;
  frm[4].value=&events[choice].type;
  frm[5].value=&events[choice].errorlevel;
  frm[6].value=events[choice].command;
  frm[7].value=&events[choice].node;

  w.define( frm,
            8,
            0x31,
            0x4E );

  tsw_cursoron();
  w.process();
  tsw_cursoroff();

  events[choice].start[0]=atoi(strtok(time,":"));
  events[choice].start[1]=atoi(strtok(NULL,":"));

  for(int i=0;i<num_events-1;i++)
     for(int j=i+1;j<num_events;j++)
       if(events[i].start>events[j].start)
          {
          event temp;
          temp=events[i];
          events[i]=events[j];
          events[j]=temp;
          }
  }
}
