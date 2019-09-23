class Message;
class MsgBase;
class SquishMsgBase;
class HudsonMsgBase;
class JamMsgBase;
class MsgArea;
class MessageIndex;

class HudsonMsgIdx;
class HudsonMsgHdr;
class HudsonMsgToIdx;
class HudsonMsgInfo;

class JamFileHeader;
class JamHeader;
class JamExtHeader;
class JamIndex;
class JamLastRead;

class MsgBase
  {
 public:
   MsgBase() {}
   virtual ~MsgBase() {}

   virtual bool open()   = 0;
   virtual void close()  = 0;

   virtual bool lock()   { return TRUE; }
   virtual bool unlock() { return TRUE; }

   virtual long readMsg(Message& msg , long num)                                = 0;
   virtual word readMsgText(Message& msg, char *text, long offset , word size)  = 0;
   virtual word scanMail(MessageIndex *mi,word maxmsgs)                         = 0;
   virtual long appendMsg(Message&)                                             = 0;
   virtual bool updateMsg(Message&)                                             = 0;

   virtual long highMsg(MsgArea&)                                               = 0;
   virtual long lowMsg (MsgArea&)                                               = 0;
   virtual long numMsgs(MsgArea&)                                               = 0;
   virtual long totalMsgs()                                                     = 0;

   virtual bool deleteMsg(Message&)                                             = 0;

   virtual long lastRead(MsgArea&,long rec)                                     = 0;
   virtual void setLastRead(MsgArea&,long rec,long num)                         = 0;

   virtual long msgNum(MsgArea&,long id)                                        = 0;
   virtual long msgId (MsgArea&,long num)                                       = 0;
  };

class SquishMsgBase : public MsgBase
  {
   MSG *lastarea;
   int lastareanum;

   MSG *readArea(MsgArea&);
 public:
   SquishMsgBase()   { open();  }
   ~SquishMsgBase()  { close(); }

   bool open();
   void close();

   bool lock()   { return TRUE; }
   bool unlock() { return TRUE; }

   long readMsg(Message& msg , long num);
   word readMsgText(Message& msg, char *text, long offset , word size);
   word scanMail(MessageIndex *mi,word maxmsgs);
   long appendMsg(Message&);
   bool updateMsg(Message&);

   long highMsg(MsgArea&);
   long lowMsg (MsgArea&);
   long numMsgs(MsgArea&);
   long totalMsgs() { return 0; }

   bool deleteMsg(Message&);

   long lastRead(MsgArea&,long rec);
   void setLastRead(MsgArea&,long rec,long num);

   long msgNum(MsgArea&,long id);
   long msgId (MsgArea&,long num);
  };

class HudsonMsgBase : public MsgBase
  {
   File f_msghdr;
   File f_msgtxt;
   File f_msgtoidx;
   File f_msgidx;
   File f_msginfo;

   HudsonMsgIdx *lastMsgIdx;
   int          *highArray;
   int          *lastReadArray;
   long lastIndex;
   long lastReadRecord;
   ///////////////////

   void flushCache();
   void readHighArray();
   void readLastRead(long record);

   void readInfo(HudsonMsgInfo&);
   void writeInfo(HudsonMsgInfo&);

   long findIdx(HudsonMsgIdx &mi,int num,int area,int order);

 public:
   HudsonMsgBase();
   ~HudsonMsgBase();

   bool open();
   void close();

   bool lock();
   bool unlock();

   long readMsg(Message& msg , long num);
   word readMsgText(Message& msg, char *text, long offset , word size);
   word scanMail(MessageIndex *mi,word maxmsgs);
   long appendMsg(Message&);
   bool updateMsg(Message&);

   long highMsg(MsgArea&);
   long lowMsg (MsgArea&);
   long numMsgs(MsgArea&);
   long totalMsgs();

   bool deleteMsg(Message&);

   long lastRead(MsgArea&,long rec);
   void setLastRead(MsgArea&,long rec,long num);

   long msgNum(MsgArea&,long id);
   long msgId(MsgArea& ma, long num);
  };

class JamMsgBase : public MsgBase
  {
   File f_jhr;
   File f_jdt;
   File f_jdx;
   File f_jlr;

   int            lastAreaNum;
   JamFileHeader *jamHeader;

   bool      openArea( MsgArea& );

   bool      jamLock();
   bool      jamUnLock();

   bool      readHeader();
   bool      writeHeader();

   long      findIdx( JamIndex &mi,long num,int order );

 public:
   JamMsgBase();
   ~JamMsgBase();

   bool open();
   void close();

   bool lock()    { return TRUE; }
   bool unlock()  { return TRUE; }

   long readMsg(Message& msg , long num);
   word readMsgText(Message& msg, char *text, long offset , word size);
   word scanMail(MessageIndex *mi,word maxmsgs);
   long appendMsg(Message&);
   bool updateMsg(Message&);

   long highMsg(MsgArea&);
   long lowMsg (MsgArea&);
   long numMsgs(MsgArea&);
   long totalMsgs()
      {
         return 0;
      }

   bool deleteMsg(Message&);

   long lastRead(MsgArea&,long rec);
   void setLastRead(MsgArea&,long rec,long num);

   long msgNum(MsgArea&,long id);
   long msgId (MsgArea&,long num);
  };


class MsgBaseList
  {
   MsgBase *mblist[5];
  public:
    MsgBaseList();
   ~MsgBaseList();

   void add(MsgBase *);

   void close();
   void open();
   bool lock();
   bool unlock();

   word scanMail(MessageIndex *,word maxmsgs);
   long totalMsgs();
  };

class MsgArea : public _MsgArea
  {
   static File f;
   static int lastAreaNum;
   static MsgArea *lastArea;
   static int numAreas;

   static void open();
 public:
   MsgBase *msgBase;

   bool read(int);
   bool sysopAccess();

   static void close();
   static int  highAreaNum();

   long numMsgs()
      {
         return (msgBase==NULL) ? 0L : msgBase->numMsgs(*this);
      }
   long lowMsg()
      {
         return (msgBase==NULL) ? 0L : msgBase->lowMsg (*this);
      }
   long highMsg()
      {
         return (msgBase==NULL) ? 0L : msgBase->highMsg(*this);
      }
   long lastRead(long rec)
      {
         return (msgBase==NULL) ? 0L : msgBase->lastRead(*this,rec);
      }
   void setLastRead(long rec,long num)
      {
         if(msgBase)
            msgBase->setLastRead(*this,rec,num);
      }
   long msgNum(long id)
      {
         return (msgBase==NULL) ? 0L : msgBase->msgNum(*this,id);
      }
   long msgId (long num)
      {
         return (msgBase==NULL) ? 0L : msgBase->msgId(*this,num);
      }
  };

const dword MSGATTR_PRIVATE     = 0x00000001L;
const dword MSGATTR_RECEIVED    = 0x00000002L;
const dword MSGATTR_DELETED     = 0x00000004L;
const dword MSGATTR_NETMAIL     = 0x00000008L;
const dword MSGATTR_UNSENT_ECHO = 0x00000010L;
const dword MSGATTR_UNSENT_NET  = 0x00000020L;
const dword MSGATTR_LOCAL       = 0x00000040L;
const dword MSGATTR_KILL        = 0x00000080L;
const dword MSGATTR_CRASH       = 0x00000100L;
const dword MSGATTR_SENT        = 0x00000200L;
const dword MSGATTR_FILE        = 0x00000400L;
const dword MSGATTR_FILEREQ     = 0x00000800L;
const dword MSGATTR_AUDITREQ    = 0x00001000L;
const dword MSGATTR_RECEIPTREQ  = 0x00002000L;
const dword MSGATTR_TRANSIT     = 0x00004000L;
const dword MSGATTR_HOLD        = 0x00008000L;
const dword MSGATTR_IMMEDIATE   = 0x00010000L;
const dword MSGATTR_DIRECT      = 0x00020000L;
const dword MSGATTR_TRUNCFILE   = 0x00040000L;
const dword MSGATTR_DELFILE     = 0x00080000L;
const dword MSGATTR_ORPHAN      = 0x00100000L;
const dword MSGATTR_ECHOMAIL    = 0x00200000L;
const dword MSGATTR_NODISP      = 0x00400000L;
const dword MSGATTR_LOCKED      = 0x00800000L;


struct Message
  {
   long  num;
   dword id;  // for Squish only, others -> == num

   char  from[36];
   char  to  [36];
   char  subj[66];

   dword attr;

   Date  postDate;
   Time  postTime;
   Date  recvDate;
   Time  recvTime;

   long  next;
   long  prev;

   int   origZone,
         origNet,
         origNode,
         origPoint;

   int   destZone,
         destNet,
         destNode,
         destPoint;

   int   cost;

   /************************************************************/

   static long lastIndex;

   union {
          MsgArea *msgArea;
          int area;
         };

   Message(int area = 0);
   Message(Message *, int area = 0);

   ~Message();

   int areaNum() ;

   MsgBase *msgBase()
     {
      return msgArea->msgBase;
     }

   void clear();

   bool setArea  (int num);
   bool read     (long num, int area = 0);
   bool readFirst(int method,int order,long first,char *data,int area = 0);
   bool readNext (int method,int order,char *data);
   bool readPrev (int method,int order,char *data);

   word readText(char *text,long offset,word size)
     {
      return msgBase()->readMsgText(*this, text, offset , size);
     }

   void createMsgTextFile(char *fname , bool append);
   void createMsgTextString(char *string , word maxsize);
   void export(char *fname);
   void forward();
   void move();

   void setFlag   (dword msgflag);
   void clearFlag (dword msgflag);
   void toggleFlag(dword msgflag);

   bool testFlag (dword msgflag) { return (attr & msgflag) ? TRUE:FALSE; }

   void operator=(HudsonMsgHdr&);

   void addReply(Message&);
   void delReply();
   void remove();

   byte show();
   void addOrigin();
   int  pointNum(int mode);
   long add();

   long msgNum() { return msgBase()->msgNum(*msgArea,id); }

   bool readAccess();
  };

struct MessageIndex
  {
   MessageIndex() {}
   MessageIndex(int a,long n) { area = a; num = n; }

   int  area;
   long num;
  };

class MarkedMsgList
  {
   MessageIndex *msgList;
   int n;
  public:
   MarkedMsgList();
   MarkedMsgList(const MarkedMsgList&);
   ~MarkedMsgList();

   void operator=(const MarkedMsgList&);

   bool add(MessageIndex&);
   bool add(Message&);
   void clear() { n = 0; }
   bool remove(MessageIndex&);
   void sort();
   int  numMarked() { return n; }
   bool isMarked(MessageIndex&);
   MessageIndex getMarked(int n)  { return msgList[n]; }
   MessageIndex operator[](int n) { return msgList[n]; }
  };

const int MAX_MARKED_MSGS = 500;

extern MsgBase       *squishMsgBase;
extern MsgBase       *hudsonMsgBase;
extern MsgBase       *jamMsgBase;
extern MsgBaseList    msgbase;
extern MarkedMsgList  markedmsglist;

void get_new_msgs(MarkedMsgList& marked,int area);


inline long
HudsonMsgBase::msgNum(MsgArea&,long id)
{
 return id;
}

inline long
HudsonMsgBase::msgId(MsgArea&, long num)
{
 return num;
}
