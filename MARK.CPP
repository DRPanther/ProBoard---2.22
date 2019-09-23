#define Use_MsgBase

#include "proboard.hpp"

MarkedMsgList::MarkedMsgList()
{
 msgList = new MessageIndex[MAX_MARKED_MSGS];

 n = 0;
}

MarkedMsgList::MarkedMsgList(const MarkedMsgList &m)
{
 msgList = new MessageIndex[MAX_MARKED_MSGS];

 (*this) = m;
}

MarkedMsgList::~MarkedMsgList()
{
 delete [] msgList;
}

void
MarkedMsgList::operator=(const MarkedMsgList& m)
{
 for(int i=0 ; i < MAX_MARKED_MSGS ; i++) msgList[i] = m.msgList[i];

 n = m.n;
}

bool
MarkedMsgList::add(MessageIndex& mi)
{
 if(isMarked(mi)) return TRUE;

 if(n < MAX_MARKED_MSGS)
   {
    msgList[n++] = mi;

    return TRUE;
   }
  else
   {
    return FALSE;
   }
}

bool
MarkedMsgList::add(Message& msg)
{
 return add(MessageIndex(msg.areaNum(),msg.id));
}

bool
MarkedMsgList::isMarked(MessageIndex& mi)
{
 for(int i=0 ; i<n ; i++)
   {
    if(msgList[i].num == mi.num && msgList[i].area == mi.area) return TRUE;
   }

 return FALSE;
}

void
MarkedMsgList::sort()
{
 int x,y;

 for(x=0;x<n-1;x++)
   for(y=x+1;y<n;y++)
     if(msgList[x].area > msgList[y].area)
       {
        MessageIndex tmp  = msgList[x];
               msgList[x] = msgList[y];
               msgList[y] = tmp;
       }

 for(x=0;x<n-1;x++)
   for(y=x+1;y<n;y++)
     if(msgList[x].area==msgList[y].area && msgList[x].num>msgList[y].num)
       {
        MessageIndex tmp  = msgList[x];
               msgList[x] = msgList[y];
               msgList[y] = tmp;
       }
}


void
get_new_msgs(MarkedMsgList& marked,int area)
{
 Message msg;
 MsgArea ma;

 io << ' ';

 int start = 1;
 int end   = 1000;

 if(area)
 {
   start = end = area;
 }

 for(int i=start ; i <= end ; i++)
   {
    if(   (area && area != i)
       || (!user.combinedBoards.connected(i) && !area)
       || !ma.read(i)
      ) continue;

    if(   !check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)
       && !ma.sysopAccess()) continue;

    if(ma.highMsg() <= ma.lastRead(user_recnr)) continue;

    bool ok = msg.readFirst(READMSG_ALL,1,ma.lastRead(user_recnr) + 1,"",i);

    int cnt = 0;

    while(ok)
      {
       if(!marked.add(MessageIndex(msg.areaNum(),msg.id))) break;

       io << '\b' << (("/-\\|")[cnt]) ;

       cnt = (++cnt) % 4;

       ok = msg.readNext(READMSG_ALL,1,"");
      }

    if(ok) break;
   }

   io << "\b \b";
}
