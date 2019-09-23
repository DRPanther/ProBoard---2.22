#define Use_MsgBase

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>

extern "C" {
#include <msgapi.h>
}

#include "proboard.hpp"

/*
#include "cache.hpp"

const byte cachemode_lowmsg   = 0;
const byte cachemode_highmsg  = 1;
const byte cachemode_nummsgs  = 2;
const byte cachemode_lastread = 3;


struct CacheIndexKey
  {
    int area;
    byte mode;

    CacheIndexKey(int a,byte m) { area = a ; mode = m; }

    int operator==(const CacheIndexKey& k) { return !memcmp(&k,this,sizeof(k)); }
  };


static ObjectCache<long,CacheIndexKey> squishCache(300);
*/

static void _near
xmsg2message(XMSG *xmsg,Message *msg)
{
 strcpy(msg->from , xmsg->from);
 strcpy(msg->to   , xmsg->to);
 strcpy(msg->subj , xmsg->subj);

 msg->origZone  = xmsg->orig.zone;
 msg->origNet   = xmsg->orig.net;
 msg->origNode  = xmsg->orig.node;
 msg->origPoint = xmsg->orig.point;
 msg->destZone  = xmsg->dest.zone;
 msg->destNet   = xmsg->dest.net;
 msg->destNode  = xmsg->dest.node;
 msg->destPoint = xmsg->dest.point;

 msg->postDate[0] = xmsg->date_written.date.da;
 msg->postDate[1] = xmsg->date_written.date.mo;
 msg->postDate[2] = xmsg->date_written.date.yr+80;
 msg->postTime[0] = xmsg->date_written.time.hh;
 msg->postTime[1] = xmsg->date_written.time.mm;
 msg->postTime[2] = xmsg->date_written.time.ss;
 msg->recvDate[0] = xmsg->date_arrived.date.da;
 msg->recvDate[1] = xmsg->date_arrived.date.mo;
 msg->recvDate[2] = xmsg->date_arrived.date.yr+80;
 msg->recvTime[0] = xmsg->date_arrived.time.hh;
 msg->recvTime[1] = xmsg->date_arrived.time.mm;
 msg->recvTime[2] = xmsg->date_arrived.time.ss;

 msg->attr = 0;

 if(xmsg->attr & MSGLOCAL)
 {
    msg->attr |= MSGATTR_LOCAL;

    if(xmsg->attr & MSGSCANNED)
      msg->attr |= MSGATTR_SENT;
 }

 if(xmsg->attr & MSGPRIVATE) msg->attr |= MSGATTR_PRIVATE;
 if(xmsg->attr & MSGREAD   ) msg->attr |= MSGATTR_RECEIVED;
 if(xmsg->attr & MSGCRASH  ) msg->attr |= MSGATTR_CRASH;
 if(xmsg->attr & MSGFILE   ) msg->attr |= MSGATTR_FILE;
 if(xmsg->attr & MSGKILL   ) msg->attr |= MSGATTR_KILL;
 if(xmsg->attr & MSGSENT   ) msg->attr |= MSGATTR_SENT;
 if(xmsg->attr & MSGORPHAN ) msg->attr |= MSGATTR_ORPHAN;
 if(xmsg->attr & MSGFRQ    ) msg->attr |= MSGATTR_FILEREQ;
 if(xmsg->attr & MSGRRQ    ) msg->attr |= MSGATTR_RECEIPTREQ;
 if(xmsg->attr & MSGHOLD   ) msg->attr |= MSGATTR_HOLD;

 if(msg->msgArea->msgKind == MSG_ECHO)    msg->attr |= MSGATTR_ECHOMAIL;
 if(msg->msgArea->msgKind == MSG_PVTECHO) msg->attr |= MSGATTR_ECHOMAIL;
 if(msg->msgArea->msgKind == MSG_NET)     msg->attr |= MSGATTR_NETMAIL;
}

static void _near
message2xmsg(XMSG *xmsg,Message *msg)
{
 strcpy(xmsg->from , msg->from);
 strcpy(xmsg->to   , msg->to);
 strcpy(xmsg->subj , msg->subj);

 xmsg->orig.zone  = msg->origZone;
 xmsg->orig.net   = msg->origNet;
 xmsg->orig.node  = msg->origNode;
 xmsg->orig.point = msg->origPoint;

 xmsg->dest.zone  = msg->destZone;
 xmsg->dest.net   = msg->destNet;
 xmsg->dest.node  = msg->destNode;
 xmsg->dest.point = msg->destPoint;

 xmsg->date_written.date.da = msg->postDate[0];
 xmsg->date_written.date.mo = msg->postDate[1];
 xmsg->date_written.date.yr = msg->postDate[2]-80;
 xmsg->date_written.time.hh = msg->postTime[0];
 xmsg->date_written.time.mm = msg->postTime[1];
 xmsg->date_written.time.ss = msg->postTime[2];
 xmsg->date_arrived.date.da = msg->recvDate[0];
 xmsg->date_arrived.date.mo = msg->recvDate[1];
 xmsg->date_arrived.date.yr = msg->recvDate[2]-80;
 xmsg->date_arrived.time.hh = msg->recvTime[0];
 xmsg->date_arrived.time.mm = msg->recvTime[1];
 xmsg->date_arrived.time.ss = msg->recvTime[2];

 xmsg->attr &= ~dword(  MSGPRIVATE | MSGREAD | MSGLOCAL
                      | MSGSCANNED | MSGSENT | MSGCRASH
                      | MSGFILE    | MSGKILL | MSGSENT
                      | MSGORPHAN  | MSGFRQ  | MSGRRQ
                      | MSGHOLD
                     );

 if(msg->attr & MSGATTR_LOCAL)
   {
    xmsg->attr |= MSGLOCAL;

    if( ((msg->attr & MSGATTR_ECHOMAIL) || (msg->attr & MSGATTR_NETMAIL))
       && (msg->attr & MSGATTR_SENT))
    {
      xmsg->attr |= MSGSCANNED | MSGSENT;
    }
   }

 if(msg->attr & MSGATTR_PRIVATE)    xmsg->attr |= MSGPRIVATE ;
 if(msg->attr & MSGATTR_RECEIVED)   xmsg->attr |= MSGREAD    ;
 if(msg->attr & MSGATTR_CRASH)      xmsg->attr |= MSGCRASH   ;
 if(msg->attr & MSGATTR_FILE)       xmsg->attr |= MSGFILE    ;
 if(msg->attr & MSGATTR_KILL)       xmsg->attr |= MSGKILL    ;
 if(msg->attr & MSGATTR_SENT)       xmsg->attr |= MSGSENT    ;
 if(msg->attr & MSGATTR_ORPHAN)     xmsg->attr |= MSGORPHAN  ;
 if(msg->attr & MSGATTR_FILEREQ)    xmsg->attr |= MSGFRQ     ;
 if(msg->attr & MSGATTR_RECEIPTREQ) xmsg->attr |= MSGRRQ     ;
 if(msg->attr & MSGATTR_HOLD)       xmsg->attr |= MSGHOLD    ;
}

struct __sqidx
{
  dword ofs;
  UMSGID umsgid;
  dword hash;
};


bool
SquishMsgBase::open()
{
 _minf mi = { 0x200 , 0 };

 aka a;

 if(a.read(0) >= 0) mi.def_zone = a.zone;

 if(MsgOpenApi(&mi) < 0)
   {
    fatalerror("Unable to initialize Squish API");
   }

 lastarea = NULL;
 lastareanum = -1;

 //squishCache.purge();

 return TRUE;
}

void
SquishMsgBase::close()
{
 if(lastarea != NULL)
   {
    MsgCloseArea(lastarea);
    lastarea = NULL;
    lastareanum = -1;
   }

 MsgCloseApi();

 //squishCache.purge();
}

MSG *
SquishMsgBase::readArea(MsgArea& ma)
{
 if(ma.areaNum != lastareanum || lastarea == NULL)
   {
    if(lastarea != NULL) MsgCloseArea(lastarea);

    lastarea = MsgOpenArea(ma.path,MSGAREA_CRIFNEC,(ma.msgBaseType == MSGBASE_SQUISH) ?
                                                        MSGTYPE_SQUISH :
                                                        MSGTYPE_SDM);

    if(lastarea == NULL) return NULL;


    if(ma.msgBaseType == MSGBASE_SQUISH)
      {
       SquishSetMaxMsg(lastarea, ma.maxMsgs, 0, ma.msgKillDays);
      }

    lastareanum = ma.areaNum;
   }

 return lastarea;
}

long
SquishMsgBase::readMsg(Message &msg,long id)
{
 int areanum = msg.areaNum();
 MSG *sqarea = readArea(*msg.msgArea);

 if(sqarea == NULL) return -1;

 MSGH *mh;
 XMSG xmsg;

 if(msg.msgArea->msgBaseType == MSGBASE_SDM)
   {
    bool found = FALSE;

    if(id > 0)
      {
       for(; id <= MsgGetHighMsg(sqarea) ; id++)
         {
          if((mh = MsgOpenMsg(sqarea , MOPEN_READ , id)) != NULL)
            {
             found = TRUE;

             break;
            }
         }

       if(!found) return -1;
      }
     else
      {
       for( id = -id ; id >= 1 ; id--)
         {
          if((mh = MsgOpenMsg(sqarea , MOPEN_READ , id)) != NULL)
            {
             found = TRUE;

             break;
            }
         }

       if(!found) return -1;
      }

   }
  else
   {
    long msgnum;

    if(id < 0) msgnum = MsgUidToMsgn(sqarea,-id,UID_PREV);
          else msgnum = MsgUidToMsgn(sqarea, id,UID_NEXT);

    if((mh = MsgOpenMsg(sqarea , MOPEN_READ , msgnum)) == NULL) return -1;

    id = msgnum;
   }

 id = labs(id);

 MsgReadMsg(mh,&xmsg,0,0,0,0,0);

 MsgCloseMsg(mh);

 msg.clear();

 xmsg2message(&xmsg,&msg);

 msg.prev = xmsg.replyto;
 msg.next = xmsg.replies[0];

 msg.id  = MsgMsgnToUid(sqarea,id);
 msg.num = id;

 msg.setArea(areanum);

 return msg.id;
}

long
SquishMsgBase::lowMsg(MsgArea &ma)
{
    long x;
    //CacheIndexKey idx(ma.areaNum,cachemode_lowmsg);

    //if(squishCache.find(x,idx)) return x;

    MSG *sqarea = readArea(ma);

    if(sqarea == NULL) return -1;

    x = MsgMsgnToUid(sqarea,1);

    //squishCache.add(x,idx);

    return x;
}

long
SquishMsgBase::numMsgs(MsgArea &ma)
{
    long x;
    //CacheIndexKey idx(ma.areaNum,cachemode_nummsgs);

    //if(squishCache.find(x,idx)) return x;

    MSG *sqarea = readArea(ma);

    if(sqarea == NULL) return -1;

    x = MsgGetNumMsg(sqarea);

    //squishCache.add(x,idx);

    return x;
}

long
SquishMsgBase::highMsg(MsgArea &ma)
{
    MSG *sqarea = readArea(ma);

    if(sqarea == NULL) return -1;

    return MsgMsgnToUid(sqarea,MsgGetHighMsg(sqarea));
}


word
SquishMsgBase::readMsgText(Message& msg,char *ptr,long offset,word size)
{
 MSGH *mh;
 MSG *sqarea;

 if((sqarea = readArea(*msg.msgArea)) == NULL) return 0;

 msg.num = MsgUidToMsgn(sqarea,msg.id,UID_EXACT);

 if(msg.num < 1) return 0;

 if((mh = MsgOpenMsg(sqarea , MOPEN_READ , msg.num)) == NULL) return 0;

 if(msg.msgArea->msgBaseType == MSGBASE_SDM)
    MsgReadMsg(mh,NULL,0,0,0,0,0);  // Workaround for a bug in Squish API

 int l = (int) MsgReadMsg(mh,NULL,offset,size,ptr,0,0);

 MsgCloseMsg(mh);

 if(l < 0) l = 0;

 return l;
}


long
SquishMsgBase::appendMsg(Message& msg)
{
 MSG *sqarea;
 MSGH *mh;
 XMSG xmsg;
 File f;
 String control_text;
 String text;
 String kludge_text;

 if((sqarea = readArea(*msg.msgArea)) == NULL) return -1;

 if( !f.open("MSGTMP",fmode_read,512) )
   {
    return -1;
   }

 long bytecount = 0;

 for(bool inkludge = FALSE;;)
   {
    int c = f.readByte();

    if(c < 0) break;

    switch( byte(c) )
      {
       case '\n' :
       case  '' : continue;
       case    1 : inkludge = TRUE;
                   kludge_text = "";
                   break;
       case '\r' : if(inkludge)
                     {
                      inkludge = FALSE;
                      bytecount--;
                      control_text << char(1) << kludge_text;
                      if(!strncmp(kludge_text,"FMPT",4)) msg.origPoint = atoi(&kludge_text[5]);
                      if(!strncmp(kludge_text,"TOPT",4)) msg.destPoint = atoi(&kludge_text[5]);
                     }
                    else
                     {
                      text << char(c);
                     }
                   break;
       default   : if(inkludge)
                     {
                      kludge_text << char(c);
                     }
                    else
                     {
                      text << char(c);
                     }
                   break;
      }

    if(!inkludge) bytecount++;
   }

 f.close();

 if(!bytecount) return -1;

 control_text << "\x01PID: " PID_STRING_SQUISH;

     //----------------------------  
     // JDR: REGISTRATION REFERENCE
     //----------------------------  

 control_text << (registered ? 'r':'u');

 if((mh = MsgOpenMsg(sqarea , MOPEN_CREATE , 0)) == NULL) return -1;

 CLEAR_OBJECT(xmsg);

 message2xmsg(&xmsg,&msg);

 MsgWriteMsg(mh,0,&xmsg,(char *)text,text.len()+1,bytecount,control_text.len() + 1,(char *)control_text);

 msg.num = mh->sq->high_msg;

 MsgCloseMsg(mh);

 msg.id  = MsgMsgnToUid(sqarea,msg.num);

 //squishCache.purge();

 return msg.id;
}

bool
SquishMsgBase::updateMsg(Message& msg)
{
 MSG *sqarea = readArea(*msg.msgArea);

 if(sqarea == NULL) return FALSE;

 msg.num = MsgUidToMsgn(sqarea,msg.id,UID_EXACT);

 if(msg.num < 1) return FALSE;

 MSGH *mh;
 XMSG xmsg;

 if((mh = MsgOpenMsg(sqarea , MOPEN_RW , msg.num)) == NULL) return -1;

 MsgReadMsg(mh,&xmsg,0,0,0,0,0);

 message2xmsg(&xmsg,&msg);

 xmsg.replyto    = msg.prev;
 xmsg.replies[0] = msg.next;

 for(int i=1; i < MAX_REPLY; i++) xmsg.replies[i] = 0;

 MsgWriteMsg(mh,0,&xmsg,0,0,0,0,0);

 MsgCloseMsg(mh);

 //squishCache.purge();

 if(msg.msgArea->msgBaseType != MSGBASE_SQUISH) return TRUE;

 File f;

 if(!f.open(FileName(msg.msgArea->path,".SQI"),fmode_rw,1024)) return FALSE;

 for(;;)
   {
    struct __sqidx sidx;

    if(f.read(&sidx,sizeof(sidx)) != sizeof(sidx)) break;

    if(sidx.umsgid == msg.id)
      {
       if(msg.attr & MSGATTR_RECEIVED) sidx.hash |= 0x80000000L;
                                  else sidx.hash &= 0x7FFFFFFFL;

       f.seek(f.pos() - sizeof(sidx));
       f.flush();
       f.write(&sidx,sizeof(sidx));

       break;
      }
   }

 return TRUE;
}

long
SquishMsgBase::msgNum(MsgArea& ma,long id)
{
 MSG *sqarea = readArea(ma);

 return MsgUidToMsgn(sqarea,id,UID_EXACT);
}

long
SquishMsgBase::msgId(MsgArea& ma,long num)
{
 MSG *sqarea = readArea(ma);

 return MsgMsgnToUid(sqarea,num);
}


bool
SquishMsgBase::deleteMsg(Message &msg)
{
 MSG *sqarea = readArea(*msg.msgArea);

 if(sqarea == NULL) return FALSE;

 long num = MsgUidToMsgn(sqarea,msg.id,UID_EXACT);

 msg.num = 0;

 //squishCache.purge();

 return (MsgKillMsg(sqarea,num) ? FALSE:TRUE);
}

static void
showbusy()
{
 static int x = 0;

 char *ss[] = {
               "|",
               "/",
               "-",
               "\\",
               NULL
              };

 /*
 char *ss1[] = {
               "*....",
               "**...",
               "***..",
               ".***.",
               "..***",
               "...**",
               "....*",
               "...**",
               "..***",
               ".***.",
               "***..",
               "**...",
               NULL
              };
 */

 //char **ss = (!io.baud || io.baud > 4800) ? ss1:ss2;

 io << '\b' << ss[x] << char(0xFF);

 x = ++x % 4;
}


word
SquishMsgBase::scanMail(MessageIndex *mi,word maxmsgs)
{
 MsgArea ma;
 FileName sqname;
 int index = 0;

 io << "\3 ";

 for(int i = 1; i <= MsgArea::highAreaNum() ; i++)
   {
    if(!ma.read(i)) continue;

    if(ma.msgBaseType != MSGBASE_SQUISH && ma.msgBaseType != MSGBASE_SDM) continue;

    if(!user.mailCheckBoards.connected(i)) continue;

    if(!check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)) continue;

    XMSG xmsg;
    MSGH *msgh;

    if(ma.msgBaseType != MSGBASE_SQUISH || ma.msgType == MSG_TOALL)
      {
       MSG *sqarea = readArea(ma);

       if(sqarea == NULL) continue;

       long startnum = ma.lowMsg();

       if(ma.msgType == MSG_TOALL)
         {
          startnum = ma.lastRead(user_recnr);

          if(startnum < 1)
            {
             startnum = 1;
            }
           else
            {
             startnum = MsgUidToMsgn(sqarea,startnum,UID_EXACT);
            }

          if(startnum < 1) startnum = MsgUidToMsgn(sqarea,ma.lastRead(user_recnr),UID_NEXT);
                     else  startnum++;

          if(ma.lastRead(user_recnr) < 1) startnum = 1;

          if(startnum < 1) continue;
         }

       if(startnum < 1) startnum = 1;

       for(long num = startnum ; num <= ma.highMsg() && index < maxmsgs;num++)
         {
          if((msgh = MsgOpenMsg(sqarea , MOPEN_READ , num)) == NULL) continue;

          showbusy();

          MsgReadMsg(msgh,&xmsg,0,0,0,0,0);

          MsgCloseMsg(msgh);

          if(xmsg.attr & MSGREAD) continue;

          if((   !stricmp(xmsg.to,user.name)
             || (!stricmp(xmsg.to,user.alias) && ma.flags && strcmp(user.name,user.alias))
             || ma.msgType == MSG_TOALL )
             && (check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot) || ma.sysopAccess())
            )
            {
             mi[index].num  = ma.msgId(num);
             mi[index].area = ma.areaNum;
             index++;
            }
         }

       continue;
      }


    dword name_hashvalue  = SquishHash(user.name);
    dword alias_hashvalue = SquishHash(user.alias);

    sqname = ma.path;

    sqname.changeExt("SQI");

    File f(sqname,fmode_read | fmode_copen,2048);

    if(!f.opened())
      {
       LOG("Can't open Squish index file '%s'",(char *)sqname);
       continue;
      }

    showbusy();

    while( index < maxmsgs )
      {
       struct __sqidx si;

       if(f.read(&si,sizeof(si)) != sizeof(si)) break;

       if(si.hash == name_hashvalue || si.hash == alias_hashvalue)
         {
          for(int i=index-1 ; i >= 0 ; i--)
          {
              if(si.umsgid == mi[i].num && ma.areaNum == mi[i].area) break;
          }

          if(i>=0) continue;

          MSG *sqarea = readArea(ma);

          if(sqarea == NULL) continue;

          long id = si.umsgid;

          long num = MsgUidToMsgn(sqarea,id,UID_EXACT);

          if(num < 1) continue;

          if((msgh = MsgOpenMsg(sqarea , MOPEN_READ , num)) == NULL) continue;

          MsgReadMsg(msgh,&xmsg,0,0,0,0,0);

          MsgCloseMsg(msgh);

          if(xmsg.attr & MSGREAD) continue;

          if((   !stricmp(xmsg.to,user.name)
             || (!stricmp(xmsg.to,user.alias) && ma.flags && strcmp(user.name,user.alias))
             )
             && (check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot) || ma.sysopAccess())
             )
            {
             mi[index].num  = id;
             mi[index].area = ma.areaNum;
             index++;
            }
         }
      }
   }


 io << '\b';

 return index;
}

long
SquishMsgBase::lastRead(MsgArea& ma,long rec)
{
    long n = 0;

    File f;
    FileName sqname;

    if(ma.msgBaseType == MSGBASE_SQUISH) sqname(ma.path,".SQL");
                                    else sqname(ma.path,"LASTREAD");

    int lr_size = (ma.msgBaseType == MSGBASE_SQUISH) ? 4 : 2;

    if(!f.open(sqname)) return 0;

    f.seek(rec * lr_size);
    if(f.read(&n,lr_size) != lr_size) return 0;

    MSG *sqarea = readArea(ma);

    if(sqarea == NULL) return 0;

    n = MsgUidToMsgn(sqarea,n,UID_PREV);
    n = MsgMsgnToUid(sqarea,n);

    return n;
}

void
SquishMsgBase::setLastRead(MsgArea& ma,long rec,long num)
{
 File f;
 FileName sqname;

 //squishCache.purge();

 if(ma.msgBaseType == MSGBASE_SQUISH) sqname(ma.path,".SQL");
                                 else sqname(ma.path,"LASTREAD");

 if(!f.open(sqname,fmode_rw | fmode_copen)) return;

 int lr_size = (ma.msgBaseType == MSGBASE_SQUISH) ? 4 : 2;

 if(f.len() < (rec+1)*lr_size)
   {
    byte x = 0;

    f.seek(f.len());
    for(long i = f.len() ; i < (rec+1)*lr_size ; i++) f.write(&x,1);
   }

 f.seek(rec * lr_size);
 f.write(&num,lr_size);
}
