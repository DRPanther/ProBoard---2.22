#include <string.h>
#include <tswin.hpp>
#include "proboard.hpp"

static int near ring_bell(char *);

static void
ask_leave_message(char *subj)
{
   io << "\n\n" << S_LEAVE_MESSAGE_TO_SYSOP;

   if(io.ask())
   {
      io << "\n\xFF";

      if(cfg.pageArea == 0)
         cfg.pageArea = cfg.securityboard;

      writemsg(form("%d /T=Sysop /S=\"%s\"",cfg.pageArea,subj));

      LOG(1,"Wrote a message to the sysop");
   }
}

void
pagesysop(char *data)
{
   if(num_yells>=cfg.max_sysop_pages)              // Paged too many times?
   {
      LOG(1,"Trying to page too many times");

      if(showansascrip("MAXPAGE")==ANS_NOFILE)   // Show ANS-file if available
      {
         io << '\n' << S_PAGED_TOO_MANY_TIMES << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         ask_leave_message("");
      }
      return;                                      // You're out of luck mate
   }

   if(!cfg.pagingHours.enabled())  // Outside paging hours?
   {
      LOG(1,"Trying to page outside hours");

      if(showansascrip("NOTAVAIL")==ANS_NOFILE)
      {
         io << '\n'
            << S_PAGING_NOT_ALLOWED_RIGHT_NOW
            << S_PRESS_ENTER_TO_CONTINUE;

         ask_leave_message("");
      }

      return;
   }

   char reason[56];
   io << '\n' << S_ASK_CHAT_REASON;

   io.read(reason,55);

   strip_all(reason);

   if(strlen(reason)<5)
   {
      io << "\n\n" << S_CHAT_REASON_TOO_SHORT("5")
         << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

      return;                    // Must be 5 chars!
   }

   LOG(1,"Sysop paged: %s",reason);

   num_yells++;                                    // Increment yell-count
   io << "\n\n" << data << char(0xFF);             // Show "Yelling..."

   if(!ring_bell(reason))                          // Did sysop react?
   {
      io << "\n\n\xFF";                                // No, inform user

      if(showansascrip("PAGED")==ANS_NOFILE)
      {
         io << S_SYSOP_NOT_RESPONDING << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
      }

      strcpy(page_reason,reason);

      ask_leave_message(reason);

      return;
   }

   LOG(1,"Chat started");                          // yell-count
   io << "\n\xFF";
   chat();                                         // Start chat
}

void
chat()
{
   if(chatflag) return;                            // Are we chatting already?

   rip_reset();

   chatflag=1;                                     // We're chatting!

   num_yells=0;                                    // Allow yelling again

   timer.suspend();                                // Suspend timer

   user_online uo;
   uo.setstatus(UO_CHATTING);                      // Inform other users

   if(cfg.extChat[0] && (user.uFlags & (UFLAG_ANSI|UFLAG_AVATAR)))
   {
      if(cfg.extChat[0] == '@')
         run_sdkfile(replace_stringvars(&cfg.extChat[1]));
      else
         shell(replace_stringvars(cfg.extChat));
   }
   else
   {
      io << '\n' << S_SYSOP_CHAT_START << "\n\n\xFF";

      int last=2;                                     // Force color change!
      String s,wrap;

      for(;;)
      {
         byte k=io.wait();
         if(k==27)
            if(io.external)
               continue;          // Local escape pressed?
            else
               break;             // Yep, exit

         if(k=='\t') k=' ';                           // I don't like TABS

         if(io.external!=last && (ansi_mode||avatar)) // Change color?
         {
            if(io.external)
               io << (avatar?"\x16\x01\x07\xFF":"[0;37m\xFF");
            else
               io << (avatar?"\x16\x01\x0B\xFF":"[1;36m\xFF");

            last = io.external;                         // Prevent color change
         }

         if(k != 8)
            io << k << char(0xFF);                            // Show char

         switch(k)
         {
            case  8:
               if(s.len()>0)                    // Backspace pressed
               {
                  s[s.len()-1]=0;
                  io << "\b \b\xFF";
               }
               break;

            case 13:
               io << "\n\xFF";
               s = "";                // Enter pressed
               break;

            default:
               s << (char)k;                    // Any other key
         }

         if(s.len()>78)                               // Wordwrap needed??
         {
            int l = wordwrap(s,wrap,78);                // Yep, do it!

            for(int i=0;i<l;i++) io << "\b \b\xFF";   // Delete last word

            s = wrap;

            io << '\n' << s << char(0xFF);            // Go to next line
         }
      }

      io << '\n' << S_SYSOP_CHAT_ENDED << "\n\xFF";
   }

   LOG(1,"End of chat");
   timer.restart();                                // Restart timer
   uo.setstatus(UO_BROWSING);                      // User status normal again
   chatflag=0;                                     // No more chatting
}

static int near
ring_bell(char *reason)
{
   Window w(11,10,70,16,0x1E,SHADOW|EXPLODE);
   w.open();

   tsw_centerline(12,"Chat request. Press [C] to chat , or [A] to abort");
   tsw_centerline(14,reason,0x9f);

   char c = playsong("PAGE","CA\x1b",cfg.pagebell_length);  // Play nice song

   return (c == 'C') ? 1 : 0;
}
