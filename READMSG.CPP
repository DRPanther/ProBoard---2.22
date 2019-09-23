#define Use_MsgBase

#include <stdlib.h>
#include <string.h>
#include "proboard.hpp"

static void draw_button(int num , char *text , char cmd , char hotkey);
static int  readmsg_more();
static void draw_readmsg_screen();
static void kill_readmsg_buttons();

void
readmsg(char *data)
{
   MsgArea ma , reply_area;
   long orgmsg;
   String param[10];
   int searchmethod = 0;
   int searchorder  = 0;
   char searchdata[80];
   long firstmsg=0;
   int area;
   bool inchain = FALSE;
   bool nonext  = FALSE;
   bool newmode = FALSE;

   Message msg;

   MarkedMsgList marked = markedmsglist;

   parse_data(data,param);

   if(param[0][0]=='*') area = user.msgArea;
                  else area = atoi(param[0]);

   if(area)
   {
      if(!ma.read(area)) return;

      if(!check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot) && !ma.sysopAccess())
      {
         io << "\n\n" << S_ACCESS_DENIED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
         return;
      }

      long n = ma.msgBase->numMsgs(ma);

      if(n < 0)
      {
         io << '\n' << S_THIS_MESSAGE_AREA_IS_LOCKED << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         return;
      }

      //if(!rip_mode)
      if(n)
      {
         io << '\n' << S_THERE_ARE_X_MESSAGES_IN_THIS_AREA(form("%d",n)) << '\n';
      }
      else
      {
         io << '\n' << S_THERE_ARE_NO_MESSAGES_IN_THIS_AREA <<
               "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         return;
      }

      msg.setArea(area);
   }


   if(param[1][0]=='/' && param[1][1]=='N')
      {
      searchmethod = READMSG_MARKED;
      searchorder  = +1;
      }
   else
      {
      String hotkeys = K_READ_MESSAGES_ORDER_SELECTION;
      hotkeys << '\r';

      /*char *hotkeys="FRANSTIM\r";*/

      char key;

      if(area)
         {
         io << '\n';
         key = io.send(S_READ_MESSAGES_ORDER_SELECTION,hotkeys);

         if(!key) key=io.wait(hotkeys);

         key = hotkeys.find(String(char(key)));
         }
      else key = 3;  // read new!

      io << "\n\n";

      switch(key)
         {
         case 7  : return;
         case 1  : io << S_READMSG_FORWARD << '\n';
                  searchmethod = READMSG_ALL;
                  searchorder  = 1;
                  break;
         case 2  : io << S_READMSG_REVERSE << '\n';
                  searchmethod = READMSG_ALL;
                  searchorder  = -1;
                  break;
         case 0  : io << S_READMSG_ALL << '\n';
                  searchmethod = READMSG_ALL;
                  break;
         case 3  : io << S_READMSG_NEW << '\n';
                  searchmethod = READMSG_NEW;
                  searchorder  = 1;
                  break;
         case 4  : {
                  io << S_READMSG_SELECTED;

                  char k = wait_language_hotkeys(K_READMSG_SELECTED);

                  if(k == '\r') return;

                  io << K_READMSG_SELECTED[k];

                  switch(k)
                     {
                     case 0: k = 'F'; break;
                     case 1: k = 'T'; break;
                     case 2: k = 'S'; break;
                     case 3: k = 'E'; break;
                     }
                  searchdata[0] = k;
                  io << "\n\n" << S_READMSG_SELECTED_SEARCH_FOR;
                  io.read(&searchdata[1],60,READMODE_UPALL);
                  if(!searchdata[1]) return;
                  searchmethod = READMSG_SEL;
                  io << '\n';
                  } break;
         case 5  : io << S_READMSG_INDIVIDUAL << '\n';
                  searchmethod = READMSG_INDIV;
                  searchorder  = 1;
                  break;
         case 6  : io << S_READMSG_MARKED << '\n';
                  searchmethod = READMSG_MARKED;
                  break;
         }

      if(!searchorder)
         {
         io << S_READMSG_ALL_FORWARD_OR_REVERSE;
         key=wait_language_hotkeys(K_READMSG_ALL_FORWARD_OR_REVERSE);
         if(key==1)
            {
            searchorder = -1;
            io << S_READMSG_REVERSE << '\n';
            }
         else
            {
            searchorder = +1;
            io << S_READMSG_FORWARD << '\n';
            }
         }

      if(area)
         {
         long highnum = ma.msgNum(ma.highMsg());
         long lownum  = ma.msgNum(ma.lowMsg());

         if(searchorder<0) firstmsg = highnum;
                  else     firstmsg = lownum;

         if(searchmethod != READMSG_MARKED && searchmethod != READMSG_INDIV && searchmethod != READMSG_NEW)
            {
            io << '\n' << S_READMSG_START_AT_MESSAGE;
            io.read(firstmsg);

            if(firstmsg > highnum && searchorder < 0) firstmsg = highnum;
            if(firstmsg < lownum  && searchorder > 0) firstmsg = lownum;

            firstmsg = ma.msgId(firstmsg);

            io << '\n';
            }
         }
      } // No "/N" parameter

   if(searchmethod == READMSG_NEW)
      {
      newmode = TRUE;

      marked.clear();

      io << '\n' << S_READMSG_SEARCHING_NEW_MESSAGES;

      get_new_msgs(marked,area);

      if(!marked.numMarked())
         {
         io << "\n\n" << S_READMSG_NO_NEW_MESSAGES_FOUND <<
               "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         return;
         }

      io << S_READMSG_X_NEW_MESSAGES_FOUND(form("%d",marked.numMarked())) << '\n';

      searchmethod = READMSG_MARKED;
      }

   if(searchmethod != READMSG_INDIV && searchmethod != READMSG_MARKED)
      {
      if(!msg.readFirst(searchmethod,searchorder,firstmsg,searchdata))
         {
         io << '\n' << S_READMSG_NO_MESSAGES_FOUND << "\n\n"
            << S_PRESS_ENTER_TO_CONTINUE;
         return;
         }

      nonext = TRUE;
      }

   int markedmsg_index = 0;

   bool continuous = FALSE;

   if(searchmethod!=READMSG_INDIV)
   {
      io << '\n' << S_READMSG_WAIT_AFTER_EACH_MSG;
      continuous = !io.ask(1);

      if(continuous)
      {
         io << "\f\n\n" << S_PRESS_S_TO_STOP_OR_P_TO_PAUSE;
      }
   }

   if(!continuous)
   {
   draw_readmsg_screen();
   }


   for(;;)
   {
      if(searchmethod==READMSG_INDIV && !inchain)
      {
         long nr;

         io << "\n\n" << S_READMSG_INDIV_ASK_MSGNUM;

         if(io.read(nr)<0) break;

         nr = ma.msgId(nr);

         if(!msg.read(nr))
         {
            io << "\n\n" << S_READMSG_INDIV_MSG_NOT_FOUND;

            continue;
         }

         io << '\n';
      }

      if(searchmethod == READMSG_MARKED && !inchain && !nonext)
      {
         if(markedmsg_index >= marked.numMarked()) break;

         MessageIndex mi = marked[markedmsg_index++];

         if(area)
            if(mi.area != area)
               continue;

         if(!msg.read(mi.num,mi.area))
            continue;
      }

   if(!area)
      {
      if(!ma.read(msg.areaNum())) continue;
      }

   if(nonext || inchain)
      nonext=0;
   else
      {
         if(searchmethod!=READMSG_INDIV && searchmethod!=READMSG_MARKED)
            if(!msg.readNext(searchmethod,searchorder,searchdata))
               break;
      }

   #pragma warn -def
   if(orgmsg == msg.id) inchain = FALSE;
   #pragma warn .def

   if(!msg.readAccess()) continue;

   if((searchmethod!=READMSG_MARKED || ma.msgType==MSG_TOALL || newmode) && !inchain)
   {
      if(msg.id > ma.lastRead(user_recnr)) ma.setLastRead(user_recnr , msg.id);

      if(   int(msg.id) > user.highMsgRead
         && ma.areaNum <= 200
         && ma.msgBaseType == MSGBASE_HUDSON) user.highMsgRead = int(msg.id);
   }

   if(    !strcmpl(msg.to,user.name)
      || (!strcmpl(msg.to,user.alias) && ma.flags>0 && user.alias[0]))
            {
            if(!(msg.attr & MSGATTR_RECEIVED))
               {
               msg.setFlag(MSGATTR_RECEIVED);

               LOG(1,"Message #%ld received (area %d)",msg.num,msg.areaNum());
               }
            }
            else
            {
            LOG(2,"Message #%ld read (area %d)",msg.num,msg.areaNum());
            }

   again:
      /*
      if(rip_mode && !continuous)
      {
      kill_readmsg_buttons();
      }
      */
   stopped=0;

   if(!cls_mode || continuous)
      {
      io << '\n';
      if(linecounter()) stopped = TRUE;
      io << '\n';
      if(linecounter()) stopped = TRUE;
      }

   if(!continuous) io << '\f';

   if(!stopped)
      switch(msg.show())
      {
      case 'N':
      case '\r': continue;
      case 'S': stopped=1;
                  break;
      case 'A':goto again;
      }

   /*
   if(rip_mode && !continuous)
      rip_send("\r!|1B0000020QPE020F000F080300020E07000000|Y00000100\n");
   */

   if(continuous)
      {
      if(stopped) break;
      continue;
      }

   String optline("\n\6");
   String hotkeys("?\r");

   int button_num = 0;

   if(!inchain)
      {
      //hotkeys << 'N';

      optline << S_MSGPROMPT_NEXT << ' ';
      hotkeys << K_MSGPROMPT_NEXT;

      //if(ma.sysopAccess()) optline << "[N]xt ";
      //               else  optline << "[N]ext ";

      if(!continuous) draw_button(button_num++,"Next",'N','N');
      }

   if(searchmethod != READMSG_MARKED || markedmsg_index>1)
      {
      optline << S_MSGPROMPT_PREV << ' ';
      hotkeys << K_MSGPROMPT_PREV;
      //optline << "[P]rev ";
      //hotkeys << 'P';
      if(!continuous) draw_button(button_num++,"Previous",'P','P');
      }

   optline << S_MSGPROMPT_AGAIN << ' ' << S_MSGPROMPT_STOP << ' ' << S_MSGPROMPT_MARK << ' ';
   hotkeys << K_MSGPROMPT_AGAIN << K_MSGPROMPT_STOP << K_MSGPROMPT_MARK;

   //optline << "[A]gain [S]top [M]ark ";

   if(!continuous) draw_button(button_num++,"Again",'A','A');
   if(!continuous) draw_button(button_num++,"Stop",'S','S');
   if(!continuous) draw_button(button_num++,"Mark",'M','M');

   reply_area = ma;

   if(ma.replyBoard)
      if(!reply_area.read(ma.replyBoard))
         reply_area = ma;

   if(check_access(reply_area.writeLevel,reply_area.writeFlags,reply_area.writeFlagsNot) || reply_area.sysopAccess())
      {
      optline << S_MSGPROMPT_REPLY << ' ';
      hotkeys << K_MSGPROMPT_REPLY;

//      if(reply_area.sysopAccess()) optline << "[R]ep ";
//                             else  optline << "[R]eply ";
//      hotkeys << 'R';
      if(!continuous) draw_button(button_num++,"Reply",'R','R');
      }

   if(ma.sysopAccess() || !strcmpl(msg.from,user.name) || !strcmpl(msg.to,user.name))
      {
      optline << S_MSGPROMPT_UNREAD << ' ';
      hotkeys << K_MSGPROMPT_UNREAD;
      //optline << "[U]nrd ";
      //hotkeys << 'U';
      if(!continuous) draw_button(button_num++,"UnRead",'U','U');
      if(   ma.sysopAccess()
         || (ma.msgKind != MSG_ECHO)
         || (   ma.msgKind == MSG_ECHO
            && msg.attr & MSGATTR_UNSENT_ECHO
            && !strcmpl(msg.from,user.name)
            )
         )
         {
         optline << S_MSGPROMPT_DEL << ' ';
         hotkeys << K_MSGPROMPT_DEL;
         //optline << "[D]el ";
         //hotkeys << 'D';
         if(!continuous) draw_button(button_num++,"Delete",'D','D');
         }
      }
   if(ma.sysopAccess())
      {
      optline << S_MSGPROMPT_SYSOPFUNC << ' ';
      hotkeys << K_MSGPROMPT_SYSOPFUNC;
      //if(ma.msgKind != MSG_ECHO)
      //   {
      //     optline << "[FVTX] ";
      //     hotkeys << "FVTX";
      //   }
      //  else
      //   {
      //     optline << "[FVX] ";
      //     hotkeys << "FVX";
      //   }

      if(!continuous) draw_button(button_num++,"Private",'T','T');
      if(!continuous) draw_button(button_num++,"Forward",'F','F');
      if(!continuous) draw_button(button_num++,"Move",'V','V');
      if(!continuous) draw_button(button_num++,"Export",'X','X');
      }
   if(inchain)
      {
      optline << S_MSGPROMPT_ORIG << ' ';
      hotkeys << K_MSGPROMPT_ORIG;
      //optline << "[O]rg ";
      //hotkeys << 'O';
      if(!continuous) draw_button(button_num++,"Original",'O','O');
      }

   if(msg.next)
      {
         optline << "[+";
         if(msg.prev) optline << '/';
         hotkeys << '+';
      }
      else
      {
         if(msg.prev) optline << '[';
      }

   if(msg.prev)
      {
         optline << "-] ";
         hotkeys << '-';
      }
      else
      {
         if(msg.next) optline << "] ";
      }

   if(inchain) optline << "[?]: <\7" << K_MSGPROMPT_ORIG << "\6> \7";
         else   optline << "[?]: <\7" << K_MSGPROMPT_NEXT << "\6> \7";

   /*
   if(rip_mode && !continuous)
      {
         rip_nomore();
         io.show_remote = FALSE;
      }
   */

   char k=io.send(optline,hotkeys);

   if(!k) k=io.wait(hotkeys);
   if(k=='\r') k=(inchain)?K_MSGPROMPT_ORIG[0]:K_MSGPROMPT_NEXT[0];
   io << k;

   //if(rip_mode && !continuous) io.show_remote = TRUE;

   if(k==K_MSGPROMPT_STOP[0])
   {
      break;
   }

   if(k==K_MSGPROMPT_AGAIN[0])
   {
      goto again;
   }

   if(k==K_MSGPROMPT_REPLY[0])
   {
      kill_readmsg_buttons();
      replymsg(msg);
      updatemenu = TRUE;
      draw_readmsg_screen();
      if(!msg.read(msg.id))
         continue;
      else
         goto again;
   }

   if(k==K_MSGPROMPT_DEL[0])
   {
      if(!ma.sysopAccess())
      {
         io << "\n\n" << S_ASK_DELETE_MSG;

         if(!io.ask()) break;
      }
      LOG(1,"Msg #%ld deleted (area %d)",msg.num,msg.areaNum());
      msg.remove();
      if(inchain)
      {
         if(msg.read(orgmsg)) nonext = 1;
      }
      else
      {
         msg.readPrev(searchmethod,searchorder,searchdata);
      }
      inchain=0;
      updatemenu = TRUE;
      continue;
   }

   if(k==K_MSGPROMPT_MARK[0])
   {
      LOG(2,"Msg #%ld marked (area %d)",msg.num,msg.areaNum());
      markedmsglist.add(msg);
      io << "\n\n" << S_MESSAGE_MARKED
         << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   }

   if(k==K_MSGPROMPT_SYSOPFUNC[2])
   {
      msg.toggleFlag(MSGATTR_PRIVATE);
      msg.read(msg.id);
      goto again;
   }

   if(k==K_MSGPROMPT_SYSOPFUNC[0])
   {
      msg.forward();
      updatemenu = TRUE;
      goto again;
   }

   if(k==K_MSGPROMPT_UNREAD[0])
   {
      msg.clearFlag(MSGATTR_RECEIVED);
      msg.read(msg.id);
   }

   if(k==K_MSGPROMPT_SYSOPFUNC[1])
   {
      msg.move();
      if(inchain)
      {
         if(msg.read(orgmsg))
            nonext = 1;
      }
      else
      {
         msg.readPrev(searchmethod,searchorder,searchdata);
      }
      inchain = 0;
      updatemenu = TRUE;
   }

   if(k==K_MSGPROMPT_SYSOPFUNC[3])
   {
      char fn[50];

      /*
      if(rip_mode)
      {
         rip_start_dialog("Enter file name to export message to",49);
         io.show_remote = FALSE;
      }
      */

      io << "\n\n" << S_EXPORT_MESSAGE_TO_FILE;

      io.show_remote = TRUE;

      io << "\3";

      io.read(fn,49,READMODE_UPALL);

      //if(rip_mode) rip_end_dialog();

      if(!fn[0]) goto again;

      io << '\n';

      msg.export(fn);

      goto again;
   }

   if(k==K_MSGPROMPT_PREV[0])
   {
      nonext = TRUE;

      if(searchmethod == READMSG_MARKED)
      {
         markedmsg_index -= 2;
         nonext = FALSE;
         continue;
      }
      for(;;)
      {
         if(msg.readPrev(searchmethod,searchorder,searchdata))
         {
            if(!msg.readAccess()) continue;

            break;
         }
         else
         {
            io << "\n\n" << S_THERE_IS_NO_PREVIOUS_MSG << ' ' << S_PRESS_ENTER_TO_CONTINUE;
            break;
         }
      }
   }

   if(k==K_MSGPROMPT_ORIG[0])
   {
      if(inchain)
         if(msg.read(orgmsg))
            nonext=1;
      inchain=0;
   }

   switch(k)
      {
      case '+': {
               if(!inchain) orgmsg = msg.id;

               for(;;)
                  {
                  if(!msg.next)
                     {
                     io << "\n\n" << S_NO_ACCESS_TO_REPLIES
                        << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

                     if(msg.read(orgmsg)) nonext = TRUE;
                     inchain=0;
                     break;
                     }

                  if(msg.read(msg.next))
                     {
                     if(!ma.read(msg.areaNum())) continue;

                     if(!msg.readAccess()) continue;

                     inchain = TRUE;
                     break;
                     }
                  else
                     {
                     if(msg.read(orgmsg)) nonext = TRUE;
                     inchain = FALSE;
                     break;
                     }
                  }
               } break;
      case '-': {
               if(!inchain) orgmsg = msg.id;

               for(;;)
                  {
                  if(!msg.prev)
                     {
                     io << "\n\n" << S_NO_ACCESS_TO_REPLIES
                        << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
                     if(msg.read(orgmsg)) nonext=1;
                     inchain = FALSE;
                     break;
                     }

                  if(msg.read(msg.prev))
                     {
                     if(!msg.readAccess()) continue;

                     inchain = TRUE;
                     break;
                     }
                  else
                     {
                     if(msg.read(orgmsg)) nonext=1;
                     inchain=0;
                     break;
                     }
                  }
               } break;
      case '?' : {
                  if(showansasc("MSGHELP",NULL)==ANS_NOFILE)
                  {
                     io << "\n\n\f" << S_MSGHELP_TITLE << "\n\n";

                     io << S_MSGHELP_NEXT    << '\n';
                     io << S_MSGHELP_PREV    << '\n';
                     io << S_MSGHELP_AGAIN   << '\n';
                     io << S_MSGHELP_STOP    << '\n';
                     io << S_MSGHELP_MARK    << '\n';
                     io << S_MSGHELP_REPLY   << '\n';
                     io << S_MSGHELP_DEL     << '\n';
                     io << S_MSGHELP_UNREAD  << '\n';
                     io << S_MSGHELP_PLUS    << '\n';
                     io << S_MSGHELP_MINUS   << '\n';
                     io << S_MSGHELP_ORIG    << '\n';

                     if(ma.sysopAccess())
                     {
                        io << S_MSGHELP_FORWARD << '\n';
                        io << S_MSGHELP_PRIVATE << '\n';
                        io << S_MSGHELP_MOVE    << '\n';
                        io << S_MSGHELP_EXPORT  << '\n';
                     }

                     io << '\n' << S_MSGHELP_NOTE    << '\n';

                     io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE << '\n';
                  }
                  goto again;
                  }
      //default: goto again;
      }
   }// for

   //if(!rip_mode)
      io << "\n\n" << S_END_OF_MESSAGES
         << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

   /*
   if(rip_mode)
   {
      rip_reset();
      io.more_func = NULLFUNC;
   }
   */
}

static void
draw_button(int num , char *text , char cmd , char hotkey)
{
   /*
   if(rip_mode)
   {
      String buttons;

      buttons << "!|1U" << meganum((num%6) * 100 + 30 )
                        << meganum((num/6) *  24 + 285)
                        << meganum((num%6) * 100 + 110)
                        << meganum((num/6) *  24 + 297)
                        << meganum(hotkey)
                        << "00<>" << text << "<>" << cmd << '\n';

      rip_send(buttons);
   }
   */
}

static int
readmsg_more()
{
   rip_kill_mouse_fields();

   draw_button(0,"Cont?"      ,K_MORE[0],0);
   draw_button(1,"Yes"        ,K_MORE[0],'Y');
   draw_button(2,"No"         ,K_MORE[1],'N');
   draw_button(3,"Scroll"     ,K_MORE[2],'S');

   rip_nomore();

   int k = wait_language_hotkeys(K_MORE);

   if(k=='\r') k=0;

   rip_kill_mouse_fields();
   rip_color(7);
   rip_fill_color(1,7);

   rip_bar(12,279,625,326);

   return k;
}

static void
draw_readmsg_screen()
{
   /*
   if(rip_mode)
   {
      rip_reset();
      show_rip("_MSGFRAM");
      io.more_func = readmsg_more;
   }
   */
}

static void
kill_readmsg_buttons()
{
   /*
   if(rip_mode)
   {
      rip_kill_mouse_fields();
      rip_color(7);
      rip_fill_color(1,7);

      rip_bar(12,279,625,326);
      rip_bar(355,8,624,25);
      rip_bar(355,40,624,54);
   }
   */
}
