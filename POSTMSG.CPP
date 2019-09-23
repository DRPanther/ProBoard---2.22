#define Use_MsgBase

#include <string.h>
#include <io.h>
#include "proboard.hpp"

long
post_message(char *from,char *to,char *subj,int area,int pvt)
{
   Message msg;

   if(!msg.setArea(area)) return -1;

   if(msg.msgArea->msgKind == MSG_NET) return -1;

   strcpy(msg.from , from);
   strcpy(msg.to   , to);
   strcpy(msg.subj , subj);

   if(pvt) msg.attr = MSGATTR_PRIVATE;

   msg.postDate.today();
   msg.postTime.now();

   long nr = msg.add();

   if(nr>0)
      if(msg.msgArea->msgKind == MSG_PVTECHO || msg.msgArea->msgKind == MSG_ECHO)
         echo_entered++;

   unlink("MSGTMP");

   return nr;
}


long
post_netmail_message(char *from,char *to,char *subj,int area,int zone,int net,int node,int point,bool crash,bool attach,bool kill)
{
   Message msg;

   if(!msg.setArea(area)) return -1;

   if(msg.msgArea->msgKind != MSG_NET) return -1;

   strcpy(msg.from,from);
   strcpy(msg.to,to);
   strcpy(msg.subj,subj);

   msg.destZone = zone;
   msg.destNet  = net;
   msg.destNode = node;
   msg.origNode = point;

   msg.attr = MSGATTR_PRIVATE;
   msg.attr |=  (crash  ? MSGATTR_CRASH : 0)
            | (attach ? MSGATTR_FILE  : 0)
            | (kill   ? MSGATTR_KILL  : 0);

   msg.postDate.today();
   msg.postTime.now();

   long nr = msg.add();

   unlink("MSGTMP");

   return nr;
}

