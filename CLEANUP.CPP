#define Use_LinkedList
#define Use_Handlers

#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <tswin.hpp>
#include "proboard.hpp"

void
exit_proboard(int level)
{
 if(sysop_next)
 {
   playsong("SYSOPNXT","\x1b",0);
   File f;
   f.open(FileName(syspath,"SYSOPNXT.SEM"),fmode_write|fmode_create);
   f.close();
 }

 tsw_cursoron();
 SCREEN.enableCursor();

 if(level)
   exit(level);

 if( net_entered && echo_entered ) exit(5);
 if( net_entered                 ) exit(3);
 if( echo_entered                ) exit(4);

 exit(0);
}

void
nomemory()
{
   fatalerror("OUT OF MEMORY!!");
}

extern "C" void
cleanup()
{
   io.hangup();

   if(!fatal)
   {
      for( hangup_handlers.rewind() ; !hangup_handlers.eol() ; hangup_handlers++)
      {
         CallPEX( &hangup_handlers.get() );
      }
   }

   SCREEN.clear();

   tsw_cursoroff();

   Window w(tsw_hsize/2 - 14 , tsw_vsize/2 - 1 , tsw_hsize/2 + 16 , tsw_vsize/2 + 1 , 0x70 , 0);
   w.open();

   tsw_centerline(tsw_vsize/2,"UPDATING DATA FILES",0x70);

   if(!fatal && user_recnr>=0)
   {
      timelog tl;
      tl.update();

      user_online uo;
      uo.clear();

      user.lastDate       = login_date;
      user.lastTime       = login_time;
      user.timeUsed      += timer.used();
      user.totalTimeUsed += timer.online();
      user.timesCalled++;

      if(newFilesChecked)
         user.lastNewFilesCheck = user.lastDate;

      user.write(FALSE);

      BinLog bl;

      strcpy( bl.name    , user.name    );
      strcpy( bl.alias   , user.alias   );
      strcpy( bl.city    , user.city    );
      strcpy( bl.country , user.country );

      bl.timeOut.now();

      bl.date   = login_date;
      bl.timeIn = login_time;
      bl.baud   = io.baud;
      bl.node   = node_number;
      bl.kbDown = 0;
      bl.kbUp   = 0;
      bl.yells  = num_yells;
      bl.uflags = user.uFlags;

      if(io.baud || cfg.binloglocal) bl.append();

      write_taglist();
   }

   if(!quiet && io.baud>0 && !fatal)
   {
      tsw_beep(1500,50);
      msleep(50);
      tsw_beep(1000,50);
   }

   w.close();

   tsw_cursoron();

   SCREEN.change(1,1,tsw_hsize,tsw_vsize);
   SCREEN.attrib(7);
   SCREEN.clear();

   setvideomode(org_videomode);

   if(org_numlines>=43 && org_videomode==3) set43();
}
