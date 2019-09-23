#include <string.h>
#include <stdio.h>
#include <tswin.hpp>
#include "proboard.hpp"

static void _near update_right_window();
static void _near update_bottom_window() {}

static Window w;

void
update_user_window()
{
 if(tsw_hsize>=132) update_right_window();
    else if(tsw_vsize>=43) update_bottom_window();
}

static void _near
update_right_window()
{
 static bool opened = FALSE;

 if(!opened)
   {
    opened = TRUE;
    SCREEN.change(1,1,80,tsw_vsize-1);
    w.open(81,1,tsw_hsize,tsw_vsize-1,0x70,NOSAVE);
    w.direct(3,0," USER ");
   }

 if(user_recnr<0) return;

 char s[80];

 s[0] = ' ';
 user.aFlags.flagstostr(&s[1]);
 s[33] = ' ';
 s[34] = 0;

 w.direct(14,0,s);

 tsw_cursoroff();
 w.setPos(1,1);
 sprintf(s,"\n Name       : %-30.30s\n",user.name);
 w << s;
 sprintf(s," Handle     : %-30.30s\n",user.alias);
 w << s;
 sprintf(s," City       : %-30.30s\n",user.city);
 w << s;
 sprintf(s," Country/St : %-30.30s\n",user.country);
 w << s;
 sprintf(s," Level      : %-5u \n",user.level);
 w << s;
 sprintf(s," VoicePhone : %-30.30s\n",user.voicePhone);
 w << s;
 sprintf(s," DataPhone  : %-30.30s\n\n",user.dataPhone);
 w << s;
 sprintf(s," Downloads  : %-5ld (%5ld Kb)        ANSI: [%c]\n",user.numDownloads,user.kbDownloaded,(ansi_mode)?'û':' ');
 w << s;
 sprintf(s," Uploads    : %-5ld (%5ld Kb)        FSED: [%c]\n",user.numUploads,user.kbUploaded,(fsed_mode)?'û':' ');
 w << s;
 sprintf(s," # Messages : %-5ld                  PAUSE: [%c]\n",user.msgsPosted,(pause_mode)?'û':' ');
 w << s;
 sprintf(s," # Calls    : %-5ld                    IBM: [%c]\n",user.timesCalled,(!no_ibm)?'û':' ');
 w << s;
 sprintf(s," Time online: %-3d min             STACKING: [%c]\n\n",timer.online(),(stack_mode)?'û':' ');
 w << s;
 sprintf(s," Last Call  : %2d-%3.3s-19%d at %02d:%02d:%02d\n\n\n\n",user.lastDate[0],months_short[user.lastDate[1]],user.lastDate[2],
                                                                user.lastTime[0],user.lastTime[1],user.lastTime[2]);
 w << s;

 sprintf(s," # Calls: %-5ld  # Msgs: -----  # Users: %-5d\n",totalcalls,num_users);
 w << s;
 sprintf(s," Last   : %-36.36s\n"
           "          [%02d:%02d - %02d:%02d]",lastcaller.name,lastcaller.timeIn[0],lastcaller.timeIn[1],lastcaller.timeOut[0],lastcaller.timeOut[1]);
 w << s;


 String line('Ä',tsw_hsize-84);
 line.insert(0,"Ç");
 line << "¶";

 w.direct(0,18,line);
 w.direct(3,18," SYSTEM ");

 tsw_cursoron();
 SCREEN.moveCursor(0,0);    // Trick to update physical cursor
}

