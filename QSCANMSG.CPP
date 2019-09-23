#define Use_MsgBase

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "proboard.hpp"

void
qscan_msg(char *data)
{
 MsgArea ma;
 String param[10];
 int searchmethod = 0;
 int searchorder  = 0;
 char searchdata[80];
 long firstmsg=0;
 int area;
 bool nonext = FALSE;

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

    if(!n)
      {
       io << '\n' << S_THERE_ARE_NO_MESSAGES_IN_THIS_AREA << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
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
      String hotkeys = K_QSCAN_MESSAGES_ORDER_SELECTION;
      hotkeys << '\r';
     char key;

     if(area)
       {
        io << '\n';
        key=io.send(S_QSCAN_MESSAGES_ORDER_SELECTION,hotkeys);
       }
      else
       key = K_QSCAN_MESSAGES_ORDER_SELECTION[3];

     if(!key) key=io.wait(hotkeys);

     key = hotkeys.find(String(char(key)));

     io << "\n\n";
     switch(key)
       {
       case 6  : return;
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
       case 5  : io << S_READMSG_MARKED << '\n';
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
        long highnum = ma.msgBase->msgNum(ma,ma.msgBase->highMsg(ma));
        long lownum  = ma.msgBase->msgNum(ma,ma.msgBase->lowMsg(ma));

        if(searchorder<0) firstmsg = highnum;
                 else     firstmsg = lownum;

        if(searchmethod != READMSG_MARKED && searchmethod != READMSG_INDIV)
          {
           io << '\n' << S_READMSG_START_AT_MESSAGE;
           io.read(firstmsg);

           if(firstmsg > highnum && searchorder < 0) firstmsg = highnum;
           if(firstmsg < lownum  && searchorder > 0) firstmsg = lownum;

           firstmsg = ma.msgBase->msgId(ma,firstmsg);

           io << '\n';
          }
       }
    } // No "/N" parameter

  if(searchmethod == READMSG_NEW)
    {
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

  if(searchmethod != READMSG_MARKED)
    {
     if(!msg.readFirst(searchmethod,searchorder,firstmsg,searchdata))
       {
        io << '\n' << S_READMSG_NO_MESSAGES_FOUND
           << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
        return;
       }

     nonext = TRUE;
    }

 int markedmsg_index = 0;

 io << '\n' << S_QSCAN_MSG_HEADER << '\n';

 stopped=0;
 io.enablestop();
 linecounter(3);

 for(;;)
   {
   if(searchmethod == READMSG_MARKED)
     {
      if(markedmsg_index >= marked.numMarked()) break;

      MessageIndex mi = marked[markedmsg_index++];

      if(area) if(mi.area != area) continue;

      if(!msg.read(mi.num,mi.area)) continue;
     }

   if(searchmethod!=READMSG_MARKED && !nonext)
           if(!msg.readNext(searchmethod,searchorder,searchdata)) break;

   nonext = FALSE;

   ma = *msg.msgArea;

   if(!msg.readAccess()) continue;

    io << S_QSCAN_MSG_DATALINE(form("%d",msg.areaNum()) , form("%ld",msg.msgBase()->msgNum(*msg.msgArea,msg.id)) , msg.from,msg.to,msg.subj) << '\n';

//    io << form("\7%4d  %5ld  \6%-20.20s \3%-20.20s \5%-24.24s\n",msg.areaNum(),msg.msgBase()->msgNum(*msg.msgArea,msg.id),msg.from,msg.to,msg.subj);
    if(stopped) break;
    if(linecounter()) break;
   }

 if(param[1][0] != '/' || param[1][1]!='N')
   io << '\n' << S_END_OF_MESSAGES
      << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
}
