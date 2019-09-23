#define Use_MsgBase

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "proboard.hpp"
#include "jam.hpp"


////////////////////////////////////////////////////////////////////////////////////

static dword
PbDate2JamDate( const Date& pbd , const Time& pbt)
{
   int years = pbd.year() - 70;

   dword unixtime = dword(years) * 365L + (dword(years + 1) / 4L);

   return ((unixtime + pbd.dayNum() - 1) * 86400L + long(pbt));
}



static 
void JamDate2PbDate( Date  &pbd, 
                     Time&  pbt , 
                     dword  jamdate )
{
     struct tm *tms      =  localtime( (long *) & jamdate );
     long       seconds  =  long( jamdate )  %  86400L;


     jamdate -= seconds;

     pbt = seconds;


     pbd[ 0 ] = tms->tm_mday;
     pbd[ 1 ] = tms->tm_mon+1;
     pbd[ 2 ] = tms->tm_year;           // Y2K - LOOKS OK, BUT CHECK IT OUT
}



static void
JamMsg2Message(JamHeader *jam,JamExtHeader *xjam,Message *msg)
{
   strcpy(msg->from , xjam->from);
   strcpy(msg->to   , xjam->to);
   strcpy(msg->subj , xjam->subject);

   msg->origZone  = xjam->fromZone;
   msg->origNet   = xjam->fromNet;
   msg->origNode  = xjam->fromNode;
   msg->origPoint = xjam->fromPoint;
   msg->destZone  = xjam->toZone;
   msg->destNet   = xjam->toNet;
   msg->destNode  = xjam->toNode;
   msg->destPoint = xjam->toPoint;

   JamDate2PbDate( msg->postDate, msg->postTime , jam->dateWritten );
   JamDate2PbDate( msg->recvDate, msg->recvTime , jam->dateReceived);

   msg->num  = jam->messageNumber;
   msg->id   = msg->num;

   msg->prev = jam->replyTo;
   msg->next = jam->reply1St;

   msg->attr = 0;

   if(jam->attribute & JAM_MSG_LOCAL      ) msg->attr |= MSGATTR_LOCAL;
   if(jam->attribute & JAM_MSG_INTRANSIT  ) msg->attr |= MSGATTR_TRANSIT;
   if(jam->attribute & JAM_MSG_PRIVATE    ) msg->attr |= MSGATTR_PRIVATE;
   if(jam->attribute & JAM_MSG_READ       ) msg->attr |= MSGATTR_RECEIVED;
   if(jam->attribute & JAM_MSG_SENT       ) msg->attr |= MSGATTR_SENT;
   if(jam->attribute & JAM_MSG_KILLSENT   ) msg->attr |= MSGATTR_KILL;
   if(jam->attribute & JAM_MSG_HOLD       ) msg->attr |= MSGATTR_HOLD;
   if(jam->attribute & JAM_MSG_CRASH      ) msg->attr |= MSGATTR_CRASH;
   if(jam->attribute & JAM_MSG_IMMEDIATE  ) msg->attr |= MSGATTR_IMMEDIATE;
   if(jam->attribute & JAM_MSG_DIRECT     ) msg->attr |= MSGATTR_DIRECT;
   if(jam->attribute & JAM_MSG_FILEREQUEST) msg->attr |= MSGATTR_FILEREQ;
   if(jam->attribute & JAM_MSG_FILEATTACH ) msg->attr |= MSGATTR_FILE;
   if(jam->attribute & JAM_MSG_TRUNCFILE  ) msg->attr |= MSGATTR_TRUNCFILE;
   if(jam->attribute & JAM_MSG_KILLFILE   ) msg->attr |= MSGATTR_DELFILE;
   if(jam->attribute & JAM_MSG_RECEIPTREQ ) msg->attr |= MSGATTR_RECEIPTREQ;
   if(jam->attribute & JAM_MSG_ORPHAN     ) msg->attr |= MSGATTR_ORPHAN;
   if(jam->attribute & JAM_MSG_NODISP     ) msg->attr |= MSGATTR_NODISP;
   if(jam->attribute & JAM_MSG_LOCKED     ) msg->attr |= MSGATTR_LOCKED;
   if(jam->attribute & JAM_MSG_DELETED    ) msg->attr |= MSGATTR_DELETED;

   if(jam->attribute & JAM_MSG_TYPEECHO   ) msg->attr |= MSGATTR_ECHOMAIL;
   if(jam->attribute & JAM_MSG_TYPENET    ) msg->attr |= MSGATTR_NETMAIL;
}

static void
Message2JamMsg(JamHeader *jam,JamExtHeader *xjam,Message *msg)
{
   if(xjam != NULL)
   {
      strcpy(xjam->from    , msg->from);
      strcpy(xjam->to      , msg->to  );
      strcpy(xjam->subject , msg->subj);

      xjam->fromZone  = msg->origZone ;
      xjam->fromNet   = msg->origNet  ;
      xjam->fromNode  = msg->origNode ;
      xjam->fromPoint = msg->origPoint;
      xjam->toZone    = msg->destZone ;
      xjam->toNet     = msg->destNet  ;
      xjam->toNode    = msg->destNode ;
      xjam->toPoint   = msg->destPoint;
   }

   if(msg->postDate.ok())
      jam->dateWritten = PbDate2JamDate( msg->postDate, msg->postTime );

   jam->dateReceived = 0;

   if(msg->attr & MSGATTR_RECEIVED && msg->recvDate.ok())
      jam->dateReceived = PbDate2JamDate( msg->recvDate, msg->recvTime );

   jam->messageNumber = msg->num;

   jam->replyTo  = msg->prev;
   jam->reply1St = msg->next;

   jam->attribute = 0;

   if(msg->attr & MSGATTR_LOCAL     ) jam->attribute |= JAM_MSG_LOCAL      ;
   if(msg->attr & MSGATTR_TRANSIT   ) jam->attribute |= JAM_MSG_INTRANSIT  ;
   if(msg->attr & MSGATTR_PRIVATE   ) jam->attribute |= JAM_MSG_PRIVATE    ;
   if(msg->attr & MSGATTR_RECEIVED  ) jam->attribute |= JAM_MSG_READ       ;
   if(msg->attr & MSGATTR_SENT      ) jam->attribute |= JAM_MSG_SENT       ;
   if(msg->attr & MSGATTR_KILL      ) jam->attribute |= JAM_MSG_KILLSENT   ;
   if(msg->attr & MSGATTR_HOLD      ) jam->attribute |= JAM_MSG_HOLD       ;
   if(msg->attr & MSGATTR_CRASH     ) jam->attribute |= JAM_MSG_CRASH      ;
   if(msg->attr & MSGATTR_IMMEDIATE ) jam->attribute |= JAM_MSG_IMMEDIATE  ;
   if(msg->attr & MSGATTR_DIRECT    ) jam->attribute |= JAM_MSG_DIRECT     ;
   if(msg->attr & MSGATTR_FILEREQ   ) jam->attribute |= JAM_MSG_FILEREQUEST;
   if(msg->attr & MSGATTR_FILE      ) jam->attribute |= JAM_MSG_FILEATTACH ;
   if(msg->attr & MSGATTR_TRUNCFILE ) jam->attribute |= JAM_MSG_TRUNCFILE  ;
   if(msg->attr & MSGATTR_DELFILE   ) jam->attribute |= JAM_MSG_KILLFILE   ;
   if(msg->attr & MSGATTR_RECEIPTREQ) jam->attribute |= JAM_MSG_RECEIPTREQ ;
   if(msg->attr & MSGATTR_ORPHAN    ) jam->attribute |= JAM_MSG_ORPHAN     ;
   if(msg->attr & MSGATTR_NODISP    ) jam->attribute |= JAM_MSG_NODISP     ;
   if(msg->attr & MSGATTR_LOCKED    ) jam->attribute |= JAM_MSG_LOCKED     ;
   if(msg->attr & MSGATTR_DELETED   ) jam->attribute |= JAM_MSG_DELETED    ;

   if(msg->attr & MSGATTR_ECHOMAIL  ) jam->attribute |= JAM_MSG_TYPEECHO   ;
   if(msg->attr & MSGATTR_NETMAIL   ) jam->attribute |= JAM_MSG_TYPENET    ;

   if(!(msg->attr & (MSGATTR_ECHOMAIL|MSGATTR_NETMAIL)))
      jam->attribute |= JAM_MSG_TYPELOCAL;
}

static bool
SubFields2ExtHeader( byte *subfields , word size , JamExtHeader& ext_hdr )
{
   JamSubField *sub;
   word pos;

   for(pos = 0 ; pos < size ; )
   {
      sub = (JamSubField *)(&subfields[pos]);

      pos += sizeof(JamSubField);

      byte *buf = new byte[word(sub->datLen)+1];
      memcpy(buf , &subfields[pos] , word(sub->datLen));
      buf[word(sub->datLen)] = '\0';

      pos += sub->datLen;

      // At this point we have all data!

      switch(sub->loId)
      {
         case JAMID_SENDERNAME:
            {
               strncpy( ext_hdr.from , buf , 35 );
               ext_hdr.from[35] = '\0';
            }
            break;
         case JAMID_RECEIVERNAME:
            {
               strncpy( ext_hdr.to , buf , 35 );
               ext_hdr.to[35] = '\0';
            }
            break;
         case JAMID_SUBJECT:
            {
               strncpy( ext_hdr.subject , buf , 71 );
               ext_hdr.subject[71] = '\0';
            }
            break;
         case JAMID_PID:
            {
               strncpy( ext_hdr.pid , buf , 79 );
               ext_hdr.pid[79] = '\0';
            }
            break;
         case JAMID_OADDRESS:
            {
               char *s = strtok( buf , ":");

               ext_hdr.fromZone  = 0;
               ext_hdr.fromNet   = 0;
               ext_hdr.fromNode  = 0;
               ext_hdr.fromPoint = 0;

               if(s)
               {
                  ext_hdr.fromZone  = atoi(s);
                  s = strtok( NULL , "/");
                  if(s)
                  {
                     ext_hdr.fromNet   = atoi(s);
                     s = strtok( NULL , ".");
                     if(s)
                     {
                        ext_hdr.fromNode  = atoi(s);
                        s = strtok( NULL , "@");
                        if(s)
                        {
                           ext_hdr.fromPoint = atoi(s);
                        }
                     }
                  }
               }
            }
            break;
         case JAMID_DADDRESS:
            {
               char *s = strtok( buf , ":");

               ext_hdr.toZone  = 0;
               ext_hdr.toNet   = 0;
               ext_hdr.toNode  = 0;
               ext_hdr.toPoint = 0;

               if(s)
               {
                  ext_hdr.toZone  = atoi(s);
                  s = strtok( NULL , "/");
                  if(s)
                  {
                     ext_hdr.toNet   = atoi(s);
                     s = strtok( NULL , ".");
                     if(s)
                     {
                        ext_hdr.toNode  = atoi(s);
                        s = strtok( NULL , ".");
                        if(s)
                        {
                           ext_hdr.toPoint = atoi(s);
                        }
                     }
                  }
               }
            }
            break;
      }

      delete [] buf;
   }

   return TRUE;
}

word
ExtHeader2SubFields(byte *subfields, JamExtHeader& xjam)
{
   char addr_buf[30];

   JamSubField *sub;
   word index = 0;

   sub = (JamSubField *) &subfields[index];

   sub->loId   = JAMID_SENDERNAME;
   sub->hiId   = 0;
   sub->datLen = strlen(xjam.from);

   index += sizeof(JamSubField);

   strcpy(&subfields[index] , xjam.from);

   index += sub->datLen;

   sub = (JamSubField *) &subfields[index];

   sub->loId   = JAMID_RECEIVERNAME;
   sub->hiId   = 0;
   sub->datLen = strlen(xjam.to);

   index += sizeof(JamSubField);

   strcpy(&subfields[index] , xjam.to);

   index += sub->datLen;

   sub = (JamSubField *) &subfields[index];

   sub->loId   = JAMID_SUBJECT;
   sub->hiId   = 0;
   sub->datLen = strlen(xjam.subject);

   index += sizeof(JamSubField);

   strcpy(&subfields[index] , xjam.subject);

   index += sub->datLen;

   if(xjam.fromZone)
   {
      sprintf(addr_buf,"%d:%d/%d.%d",xjam.fromZone,xjam.fromNet,xjam.fromNode,xjam.fromPoint);

      sub = (JamSubField *) &subfields[index];

      sub->loId   = JAMID_OADDRESS;
      sub->hiId   = 0;
      sub->datLen = strlen(addr_buf);

      index += sizeof(JamSubField);

      strcpy(&subfields[index] , addr_buf);

      index += sub->datLen;
   }

   if(xjam.toZone)
   {
      sprintf(addr_buf,"%d:%d/%d.%d",xjam.toZone,xjam.toNet,xjam.toNode,xjam.toPoint);

      sub = (JamSubField *) &subfields[index];

      sub->loId   = JAMID_DADDRESS;
      sub->hiId   = 0;
      sub->datLen = strlen(addr_buf);

      index += sizeof(JamSubField);

      strcpy(&subfields[index] , addr_buf);

      index += sub->datLen;
   }

   if(xjam.pid[0])
   {
      sub = (JamSubField *) &subfields[index];

      sub->loId   = JAMID_PID;
      sub->hiId   = 0;
      sub->datLen = strlen(xjam.pid);

      index += sizeof(JamSubField);

      strcpy(&subfields[index] , xjam.pid);

      index += sub->datLen;
   }

   return index;
}


JamMsgBase::JamMsgBase()
{
   lastAreaNum = -1;

   jamHeader = new JamFileHeader;

   CLEAR_OBJECT(*jamHeader);
}

JamMsgBase::~JamMsgBase()
{
   close();

   delete jamHeader;
}

bool
JamMsgBase::open()
{
   return TRUE;
}

void
JamMsgBase::close()
{
   f_jhr.close();
   f_jdt.close();
   f_jdx.close();
   f_jlr.close();

   lastAreaNum = -1;
}

bool
JamMsgBase::jamLock()
{
   return f_jhr.lock(0,1);
}

bool
JamMsgBase::jamUnLock()
{
   return f_jhr.unlock(0,1);
}

bool
JamMsgBase::readHeader()
{
   f_jhr.rewind();

   return (f_jhr.read( jamHeader , sizeof(JamFileHeader) ) == sizeof(JamFileHeader));
}

bool
JamMsgBase::writeHeader()
{
   f_jhr.rewind();

   return (f_jhr.write( jamHeader , sizeof(JamFileHeader) ) == sizeof(JamFileHeader));
}

bool
JamMsgBase::openArea(MsgArea& ma)
{
   if(ma.areaNum == lastAreaNum)
      return TRUE;

   lastAreaNum = -1;

   FileName fn_jhr( ma.path , ".JHR" );
   FileName fn_jdt( ma.path , ".JDT" );
   FileName fn_jdx( ma.path , ".JDX" );
   FileName fn_jlr( ma.path , ".JLR" );

   f_jhr.close();
   f_jdt.close();
   f_jdx.close();
   f_jlr.close();

   if(!f_jhr.open( fn_jhr , fmode_rw | fmode_shared , 4096))
   {
      if(!f_jhr.open( fn_jhr , fmode_create | fmode_rw | fmode_shared , 4096))
         return FALSE;

      CLEAR_OBJECT(*jamHeader);

      jamHeader->signature    = JAM_SIGNATURE;
      jamHeader->baseMsgNum   = 1;
      jamHeader->passwordCrc  = dword(-1);
      jamHeader->dateCreated  = PbDate2JamDate( Date(TODAY) , Time(NOW) );

      if(!writeHeader())
         return FALSE;
   }
   else
   {
      if(!readHeader())
         return FALSE;
   }

   if(   !f_jdt.open( fn_jdt , fmode_rw | fmode_copen | fmode_shared , 512  )
      || !f_jdx.open( fn_jdx , fmode_rw | fmode_copen | fmode_shared , 2048 )
      || !f_jlr.open( fn_jlr , fmode_rw | fmode_copen | fmode_shared , 512  )
     )
   {
      return FALSE;
   }

   lastAreaNum = ma.areaNum;

   return TRUE;
}

long
JamMsgBase::readMsg(Message &msg,long id)
{
   int order = 1;
   JamIndex idx;

   if(id < 0)
   {
      order = -1;
      id = -id;
   }

   if(!openArea(*msg.msgArea))
      return -1;

   long num_found = findIdx( idx , id , order );

   if(num_found < 0)
      return -1;

   JamHeader    hdr;
   JamExtHeader ext_hdr;

   f_jhr.seek(idx.offset);
   if(f_jhr.read( &hdr , sizeof(JamHeader) ) != sizeof(JamHeader))
      return -1;

   if(hdr.signature != JAM_SIGNATURE)
      return -1;

   byte *subfields = new byte[word(hdr.subFieldLen)];

   if(f_jhr.read( subfields , hdr.subFieldLen ) != hdr.subFieldLen)
      return -1;

   SubFields2ExtHeader( subfields , word(hdr.subFieldLen) , ext_hdr );

   JamMsg2Message( &hdr , &ext_hdr , &msg );

   delete [] subfields;

   return num_found;
}

long
JamMsgBase::lowMsg(MsgArea &ma)
{
   if(!openArea(ma))
      return -1;

   readHeader();

   return jamHeader->baseMsgNum;
}

long
JamMsgBase::numMsgs(MsgArea &ma)
{
   if(!openArea(ma))
      return -1;

   readHeader();

   return jamHeader->activeMsgs;
}

long
JamMsgBase::highMsg(MsgArea &ma)
{
   if(!openArea(ma))
      return -1;

   readHeader();

   return (f_jdx.len() / sizeof(JamIndex) + jamHeader->baseMsgNum - 1);
}

word
JamMsgBase::readMsgText(Message& msg,char *ptr,long offset,word size)
{
   JamIndex idx;

   long num_found = findIdx( idx , msg.num , 0 );

   if(num_found != msg.num)
      return 0;

   JamHeader    hdr;

   f_jhr.seek(idx.offset);
   if(f_jhr.read( &hdr , sizeof(JamHeader) ) != sizeof(JamHeader))
      return 0;

   if(hdr.signature != JAM_SIGNATURE)
      return 0;

   if(offset + size > hdr.txtLen)
   {
      if(offset > hdr.txtLen)
         return 0;

      size = hdr.txtLen - offset;
   }

   f_jdt.seek(hdr.offset + offset);

   return f_jdt.read( ptr , size );
}


long
JamMsgBase::appendMsg(Message& msg)
{
   JamIndex     j_idx;
   JamHeader    j_hdr;
   JamExtHeader j_ext_hdr;

   if(!openArea(*msg.msgArea))
      return -1;

   jamLock();
   readHeader();

   long next_num = f_jdx.len() / sizeof(JamIndex);

   j_idx.crc32  = JamCrc(msg.to);
   j_idx.offset = f_jhr.len();

   f_jdx.seek(next_num * sizeof(JamIndex));
   f_jdx.write(&j_idx , sizeof(JamIndex));

   byte *subfields = new char[1024];

   CLEAR_OBJECT( j_hdr );
   CLEAR_OBJECT( j_ext_hdr );

   j_hdr.signature   = JAM_SIGNATURE;
   j_hdr.revision    = 1;
   j_hdr.passwordCrc = dword(-1);
   j_hdr.offset      = f_jdt.len();

   msg.num = next_num + jamHeader->baseMsgNum;
   msg.id  = msg.num;

   Message2JamMsg(&j_hdr,&j_ext_hdr,&msg);

   strcpy(j_ext_hdr.pid,PID_STRING_JAM);

   j_hdr.subFieldLen = ExtHeader2SubFields(subfields, j_ext_hdr);

   File f_txt;

   if(f_txt.open("MSGTMP",fmode_read,512))
   {
      f_jdt.seek( j_hdr.offset );

      for(int len = 0;;)
      {
         int c = f_txt.readByte();

         if(c < 0)
            break;

         if(char(c) == '' || char(c) == '\n')
            continue;

         f_jdt << char(c);
         len++;
      }

      j_hdr.txtLen = len;
   }

   f_jhr.seek ( j_idx.offset );
   f_jhr.write( &j_hdr , sizeof(JamHeader));
   f_jhr.write( subfields , j_hdr.subFieldLen);

   delete [] subfields;

   jamHeader->activeMsgs++;
   jamHeader->modCounter++;

   writeHeader();
   jamUnLock();

   return msg.num;
}

bool
JamMsgBase::updateMsg(Message& msg)
{
   JamIndex  idx;
   JamHeader hdr;

   if(!openArea(*msg.msgArea))
      return FALSE;

   if(!jamLock())
      return FALSE;

   readHeader();

   long found = findIdx( idx , msg.num , 0 );

   if(found != msg.num)
   {
      jamUnLock();

      return FALSE;
   }

   f_jhr.seek(idx.offset);
   if(f_jhr.read( &hdr , sizeof(JamHeader) ) != sizeof(JamHeader))
   {
      jamUnLock();

      return FALSE;
   }

   Message2JamMsg( &hdr , NULL , &msg );

   f_jhr.seek(idx.offset);
   f_jhr.write( &hdr , sizeof(JamHeader) );

   jamHeader->modCounter++;

   writeHeader();

   jamUnLock();

   return TRUE;
}

long
JamMsgBase::msgNum(MsgArea&,long id)
{
   return id;
}

long
JamMsgBase::msgId(MsgArea&,long num)
{
   return num;
}


bool
JamMsgBase::deleteMsg(Message &msg)
{
   JamIndex  idx;
   JamHeader hdr;

   if(!openArea(*msg.msgArea))
      return FALSE;

   if(!jamLock())
      return FALSE;

   readHeader();

   long found = findIdx( idx , msg.num , 0 );

   if(found != msg.num)
   {
      jamUnLock();

      return FALSE;
   }

   f_jhr.seek(idx.offset);
   if(f_jhr.read( &hdr , sizeof(JamHeader) ) != sizeof(JamHeader))
   {
      jamUnLock();

      return FALSE;
   }

   if(hdr.attribute & JAM_MSG_DELETED)
   {
      jamUnLock();

      return FALSE;
   }

   hdr.attribute |= JAM_MSG_DELETED;
   hdr.txtLen     = 0;

   f_jhr.seek(idx.offset);
   f_jhr.write( &hdr , sizeof(JamHeader) );

   idx.crc32  = dword(-1);

   f_jdx.seek ( (found - jamHeader->baseMsgNum) * sizeof(JamIndex) );
   f_jdx.write( &idx , sizeof(JamIndex) );

   jamHeader->activeMsgs--;
   jamHeader->modCounter++;

   writeHeader();

   jamUnLock();

   return TRUE;
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

   io << '\b' << ss[x] << char(0xFF);

   x = ++x % 4;
}

word
JamMsgBase::scanMail(MessageIndex *msgidx,word maxmsgs)
{
   MsgArea ma;
   int index = 0;

   io << "\3 ";

   for(int i = 1; i <= MsgArea::highAreaNum() ; i++)
   {
      if(!ma.read(i)) continue;

      if(ma.msgBaseType != MSGBASE_JAM) continue;

      if(!user.mailCheckBoards.connected(i)) continue;

      if(!check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)) continue;

      dword name_crc  = JamCrc(user.name);
      dword alias_crc = JamCrc(user.alias);

      if(!openArea(ma))
         continue;

      long lr = ma.lastRead(user_recnr);

      showbusy();

      f_jdx.rewind();
      long rec = 0;

      while( index < maxmsgs )
      {
         JamIndex jamidx;

         if(f_jdx.read(&jamidx,sizeof(JamIndex)) != sizeof(JamIndex)) break;

         if(jamidx.crc32 == name_crc || jamidx.crc32 == alias_crc || (ma.msgType == MSG_TOALL && (rec+jamHeader->baseMsgNum) > lr))
         {
            Message msg(ma.areaNum);

            //*msg.msgArea = ma;

            long msgread = readMsg( msg , rec + jamHeader->baseMsgNum );

            if(msgread == rec + jamHeader->baseMsgNum && !(msg.attr & MSGATTR_RECEIVED))
            {
               if((   !stricmp(msg.to,user.name)
                  || (!stricmp(msg.to,user.alias) && ma.flags && strcmp(user.name,user.alias))
                  || (ma.msgType == MSG_TOALL && (rec+jamHeader->baseMsgNum) > lr)
                  )
                  && (check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot) || ma.sysopAccess())
                  )
               {
                  msgidx[index].num  = msgread;
                  msgidx[index].area = ma.areaNum;
                  index++;
               }
            }

            f_jdx.seek( (rec+1) * sizeof(JamIndex) );
         }

         rec++;
      }
   }

   io << '\b';

   return index;
}

long
JamMsgBase::lastRead(MsgArea& ma,long)
{
   if(!openArea(ma))
      return -1;

   dword crc = JamCrc(user.name);

   JamLastRead jlr;

   f_jlr.rewind();

   for(;;)
   {
      if(f_jlr.read( &jlr , sizeof(JamLastRead) ) != sizeof(JamLastRead))
         break;

      if(crc == jlr.userCrc)
         return jlr.highReadMsg;
   }

   return 0;
}

void
JamMsgBase::setLastRead(MsgArea& ma,long,long num)
{
   if(!openArea(ma))
      return;

   dword crc = JamCrc(user.name);

   JamLastRead jlr;

   f_jlr.rewind();

   for(long rec = 0 ;; rec++)
   {
      if(f_jlr.read( &jlr , sizeof(JamLastRead) ) != sizeof(JamLastRead))
         break;

      if(crc == jlr.userCrc)
      {
         f_jlr.seek( rec * sizeof(JamLastRead) );

         jlr.highReadMsg = num;

         f_jlr.write( &jlr , sizeof(JamLastRead) );

         return;
      }
   }

   jlr.userCrc     = crc;
   jlr.userId      = crc;
   jlr.lastReadMsg = num;
   jlr.highReadMsg = num;

   f_jlr.seek( rec * sizeof(JamLastRead) );
   f_jlr.write( &jlr , sizeof(JamLastRead) );
}

long
JamMsgBase::findIdx( JamIndex &mi,long num,int order )
{
   JamIndex local_idx;

   if(order < 0)
      order = -1;

   if(order > 0)
      order = 1;

   if(num < jamHeader->baseMsgNum)
   {
      if(order <= 0)
         return -1;

      num = jamHeader->baseMsgNum;
   }

   f_jdx.seek((num - jamHeader->baseMsgNum) * sizeof(JamIndex));

   if(f_jdx.read( &local_idx , sizeof(JamIndex) ) != sizeof(JamIndex))
   {
      if(order >= 0)      // exact or forward
         return -1;

      num = f_jdx.len() / sizeof(JamIndex) + jamHeader->baseMsgNum - 1;

      f_jdx.seek((num - jamHeader->baseMsgNum) * sizeof(JamIndex));
      if(f_jdx.read( &local_idx , sizeof(JamIndex) ) != sizeof(JamIndex))
         return -1;

      if(num <= 0)
         return -1;
   }

   if(local_idx.crc32 != dword(-1))
   {
      mi = local_idx;

      return num;
   }

   if(!order)
      return -1;

   while( local_idx.crc32 == dword(-1) )
   {
      num += order;

      if(order < 0)
      {
         f_jdx.seek((num - jamHeader->baseMsgNum) * sizeof(JamIndex));
      }

      if(f_jdx.read( &local_idx , sizeof(JamIndex) ) != sizeof(JamIndex))
         return -1;
   }

   mi = local_idx;

   return (local_idx.crc32 != dword(-1)) ? num : -1;
}
