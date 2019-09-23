const dword JAM_SIGNATURE = 0x004D414ALU;

struct JamFileHeader
   {
      dword    signature;
      dword    dateCreated;
      dword    modCounter;
      dword    activeMsgs;
      dword    passwordCrc;
      dword    baseMsgNum;
      byte     reserved[1000];
   };

struct JamHeader
   {
      dword    signature;
      word     revision;
      word     reserved;
      dword    subFieldLen;
      dword    timesRead;
      dword    msgidCrc;
      dword    replyCrc;
      dword    replyTo;
      dword    reply1St;
      dword    replyNext;
      dword    dateWritten;
      dword    dateReceived;
      dword    dateProcessed;
      dword    messageNumber;
      dword    attribute;
      dword    attribute2;
      dword    offset;
      dword    txtLen;
      dword    passwordCrc;
      dword    cost;
   };

struct JamExtHeader
   {
      int      fromZone;
      int      fromNet;
      int      fromNode;
      int      fromPoint;
      int      toZone;
      int      toNet;
      int      toNode;
      int      toPoint;

      char     from[36];
      char     to[36];
      char     subject[72];
      char     pid[80];
   };

struct JamSubField
   {
      word     loId;
      word     hiId;
      dword    datLen;
   };

struct JamIndex
   {
      dword    crc32;
      dword    offset;
   };

struct JamLastRead
   {
      dword    userCrc;
      dword    userId;
      dword    lastReadMsg;
      dword    highReadMsg;
   };


const word JAMID_OADDRESS           =    0;
const word JAMID_DADDRESS           =    1;
const word JAMID_SENDERNAME         =    2;
const word JAMID_RECEIVERNAME       =    3;
const word JAMID_MSGID              =    4;
const word JAMID_REPLYID            =    5;
const word JAMID_SUBJECT            =    6;
const word JAMID_PID                =    7;
const word JAMID_TRACE              =    8;
const word JAMID_ENCLOSEDFILE       =    9;
const word JAMID_ENCLOSEDFILEWALIAS =   10;
const word JAMID_ENCLOSEDFREQ       =   11;
const word JAMID_ENCLOSEDFILEWCARD  =   12;
const word JAMID_ENCLOSEDINDIRECTFILE=  13;
const word JAMID_EMBINDAT           = 1000;
const word JAMID_FTSKLUDGE          = 2000;
const word JAMID_SEENBY2D           = 2001;
const word JAMID_PATH2D             = 2002;
const word JAMID_FLAGS              = 2003;
const word JAMID_TZUTCINFO          = 2004;


const dword JAM_MSG_LOCAL           = 0x00000001L;
const dword JAM_MSG_INTRANSIT       = 0x00000002L;
const dword JAM_MSG_PRIVATE         = 0x00000004L;
const dword JAM_MSG_READ            = 0x00000008L;
const dword JAM_MSG_SENT            = 0x00000010L;
const dword JAM_MSG_KILLSENT        = 0x00000020L;
const dword JAM_MSG_ARCHIVESENT     = 0x00000040L;
const dword JAM_MSG_HOLD            = 0x00000080L;
const dword JAM_MSG_CRASH           = 0x00000100L;
const dword JAM_MSG_IMMEDIATE       = 0x00000200L;
const dword JAM_MSG_DIRECT          = 0x00000400L;
const dword JAM_MSG_GATE            = 0x00000800L;
const dword JAM_MSG_FILEREQUEST     = 0x00001000L;
const dword JAM_MSG_FILEATTACH      = 0x00002000L;
const dword JAM_MSG_TRUNCFILE       = 0x00004000L;
const dword JAM_MSG_KILLFILE        = 0x00008000L;
const dword JAM_MSG_RECEIPTREQ      = 0x00010000L;
const dword JAM_MSG_CONFIRMREQ      = 0x00020000L;
const dword JAM_MSG_ORPHAN          = 0x00040000L;
const dword JAM_MSG_ENCRYPT         = 0x00080000L;
const dword JAM_MSG_COMPRESS        = 0x00100000L;
const dword JAM_MSG_ESCAPED         = 0x00200000L;
const dword JAM_MSG_FPU             = 0x00400000L;
const dword JAM_MSG_TYPELOCAL       = 0x00800000L;
const dword JAM_MSG_TYPEECHO        = 0x01000000L;
const dword JAM_MSG_TYPENET         = 0x02000000L;
const dword JAM_MSG_NODISP          = 0x20000000L;
const dword JAM_MSG_LOCKED          = 0x40000000L;
const dword JAM_MSG_DELETED         = 0x80000000L;
