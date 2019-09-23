#include <string.h>
#include "proboard.hpp"

static void near zip(File&);
static void near arc(File&);
static void near arj(File&);
static void near lzh(File&);
static void near zoo(File&);
static void near rar(File&);

static bool find_area(char *fn,FileArea &fa);

static void near print(char *,unsigned,unsigned,long,long);

# define NUM_EXT 5

void
list_archive(char *data)
{
char input[13];

io << "\f\n" << S_VIEW_ARCHIVE_TITLE_AND_PROMPT;

io.read(input,12,READMODE_UPALL);
if(!input[0]) return;

FileName fn;
File fh;
String param[40];
BitArray arealist(MAX_FILE_AREAS,1);

int npara=parse_data(data,param);

create_arealist(param,npara,arealist);

FileArea fa;

if(!find_area(input,fa))
  {
   io << "\n\n" << S_ARCHIVE_FILE_NOT_FOUND << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
   return;
  }

fn(fa.filepath,input);

fn.upperCase();

if(!fh.open(fn))
   return;

fn.stripPath();

LOG(2,"Archive list requested (%s)",(char *)fn);

linecounter(0);
io << form("\n\f\n\7"
          "ษออออออออออออออัอออออออออออออออออออออออออออออออออออออออออออออป\n"
          "บ \3%-12s \7ณ\3 %-44.44s\7บ\n",(char *)fn,fa.name);

io << form("ฬออออออออออออออฯอหอออออออออออออหอออออออหออออออออออหออออออออออน\n"
      "บ\6 %-15.15s\7บ\6 %-12.12s\7บ\6 %-6.6s\7บ\6 %-9.9s\7บ\6 %-9.9s\7บ\n"
      "ฬออออออออออออออออฮอออออออออออออฮอออออออฮออออออออออฮออออออออออน\n",
          S_ARCHIVE_HEADER_FILENAME,
          S_ARCHIVE_HEADER_DATE,
          S_ARCHIVE_HEADER_TIME,
          S_ARCHIVE_HEADER_ORIGSIZE,
          S_ARCHIVE_HEADER_COMPSIZE
          );

linecounter(5);

stopped=0;
io.enablestop();

char ident_buf[20];

fh.read(ident_buf,20);
fh.seek(0);

     if(ident_buf[0] == '\x1a')             arc(fh);
else if(!strncmp(ident_buf,"PK\x03\x04",4)) zip(fh);
else if(!strncmp(&ident_buf[2],"-lh",3))    lzh(fh);
else if(!strncmp(ident_buf,"ZOO",3))        zoo(fh);
else if(!strncmp(ident_buf,"\x60\xEA",2))   arj(fh);
else if(!strncmp(ident_buf,"Rar",3))        rar(fh);

if(!stopped) io << "ศออออออออออออออออสอออออออออออออสอออออออสออออออออออสออออออออออผ\n\n"
                << S_PRESS_ENTER_TO_CONTINUE;

io.disablestop();
}

static void near
zip(File &f)
{
struct zipfile
    {
    unsigned long signature;
    unsigned version,gp,method,time,date;
    long crc,comsize,uncomsize;
    unsigned length,extra;
    } zip_record;

char name[80];

for(;;)
   {
   f.read(&zip_record,sizeof(zip_record));
   if(zip_record.signature!=0x04034b50L) break;
   f.read(name,zip_record.length);
   name[zip_record.length]='\0';

   print(name,zip_record.date,zip_record.time,zip_record.comsize,zip_record.uncomsize);
   if(linecounter() || stopped) break;

   f.seek(zip_record.comsize+zip_record.extra,seek_cur);
   }
}

static void near
arc(File &f)
{
struct arcfile
    {
    char name[13];
    long size;
    unsigned date,time,crc;
    long length;
    } arc_record;

for(;;)
   {
   char mark;

   f.read(&mark,1);
   if(!mark) break;
   f.read(&mark,1);
   if(mark==1)
   {
      if(f.read(&arc_record,23) != 23) break;

      arc_record.length=arc_record.size;
    }
    else
    {
      if(f.read(&arc_record,27) != 27) break;
    }

   print(arc_record.name,arc_record.date,arc_record.time,arc_record.size,arc_record.length);
   if(linecounter() || stopped) break;

   f.seek(arc_record.size,seek_cur);
   }
}

static void near
print(char *name,unsigned date,unsigned time,long csize,long usize)
{
Date d; d.fileDate(date);
Time t; t.fileTime(time);

strip_path(name);
io << form("บ\6 %-15s\7บ\5 %02d %3.3s %04d \7บ\2 %02d:%02d \7บ\3 %8ld \7บ\6 %8ld \7บ\n",name,d[0],months_short[d[1]],d[2]+1900,t[0],t[1],usize,csize);
}

static void near
lzh(File &f)
{
struct lzhfile
   {
   unsigned char headsiz,headchk;
   char headid[3];
   int method;
   unsigned long pacsiz,orgsiz;
   unsigned int time,date,attr;
   unsigned char fnlen;
   char Fname[80];
   } lzh_record;

for(;;)
  {
  if(f.read(&lzh_record.headsiz,1)<1) break;
  if(f.read(&lzh_record.headchk,(int)lzh_record.headsiz+1)<((int)lzh_record.headsiz+1)) break;
  if(strncmp(lzh_record.headid,"-lh",3)) break;
  lzh_record.Fname[lzh_record.fnlen]='\0';

  print(lzh_record.Fname,lzh_record.date,lzh_record.time,lzh_record.pacsiz,lzh_record.orgsiz);
  if(linecounter() || stopped) break;

  f.seek(lzh_record.pacsiz,seek_cur);
  }
}

static void near
zoo(File &f)
{
struct zoo_header
   {
   char text[20];
   unsigned long zoo_tag;
   long zoo_start;
   // De rest moeme nie-n!
   } zoo_record;

struct direntry
   {
   long zoo_tag;
   char type,packing_method;
   long next,offset;
   unsigned date,time,file_crc;
   long org_size,size_now;
   unsigned char major_ver,minor_ver,deleted,struc;
   long comment;
   unsigned int cmt_size;
   char fname[13];
   } zoo_head;

f.read(&zoo_record,sizeof(zoo_record));
f.seek(zoo_record.zoo_start);

for(;;)
  {
  f.read(&zoo_head,sizeof(zoo_head));
  if(!zoo_head.next) break;

  print(zoo_head.fname,zoo_head.date,zoo_head.time,zoo_head.size_now,zoo_head.org_size);
  if(linecounter() || stopped) break;

  f.seek(zoo_head.next);
  }
}

static void near
arj(File &f)
{
struct {
   unsigned id;
   unsigned hdrsize;
   byte first_hdrsize;
   byte version;
   byte version_needed;
   byte OS;
   byte arjflags;
   byte method;
   byte filetype;
   byte reserved1;
   unsigned time;
   unsigned date;
   long comsize;
   long uncomsize;
   long crc;
   unsigned filespecpos;
   unsigned accessmode;
  } arj_record;

char name[80];

for(int count = 1;;count++)
   {
    //unsigned extheadersize;
    long pos = f.pos();

    f.read(&arj_record,sizeof(arj_record));

    if(arj_record.id != 0xEA60 || !arj_record.hdrsize) break;

    f.seek(pos+arj_record.first_hdrsize+4);

    f.read(name,80);

    if(count > 1)
      {
       print(name,arj_record.date,arj_record.time,arj_record.comsize,arj_record.uncomsize);

       if(linecounter() || stopped) break;
      }

    if(count == 1)
      f.seek(pos+arj_record.hdrsize+10);
    else
      f.seek(pos+arj_record.hdrsize+10+arj_record.comsize);
   }
}

static void near
rar(File &f)
{
   struct block_header
      {
         word crc;
         byte type;
         word flags;
         word blockSize;
      };
   struct file_header
      {
         word  crc;
         byte  type;
         word  flags;
         word  blockSize;
         long  packSize;
         long  unpackSize;
         byte  hostOs;
         dword fileCrc;
         word  fileTime;
         word  fileDate;
         byte  unpackVersion;
         byte  method;
         word  nameSize;
         dword fileAttr;
      };


   for(;;)
   {
      block_header bh;
      file_header  fh;

      long pos = f.pos();

      if(f.read(&bh,sizeof(bh)) != sizeof(bh))
         break;

      if(bh.type == 0x74)
      {
         f.seek(pos);
         f.read(&fh,sizeof(fh));

         char *fname = new char[fh.nameSize+1];

         f.read(fname,fh.nameSize);
         fname[fh.nameSize] = '\0';

         print(fname,fh.fileDate,fh.fileTime,fh.packSize,fh.unpackSize);

         f.seek(pos + fh.blockSize + fh.packSize);
      }
      else
      {
         f.seek(pos + bh.blockSize);
      }
   }
}


bool
find_area(char *fn,FileArea &fa)
{
   File idx;

   if(idx.open(FileName(syspath,"FILESIDX.PB") , fmode_read , cfg.fastmode ? 8192:4096))
   {
      FilesIdx fidx;

      for(;;)
      {
         if(idx.read(&fidx,sizeof(fidx)) != sizeof(fidx)) break;

         if(!stricmp(fidx.name,fn))
         {
            if(fa.read(fidx.area) && check_access(fa.level,fa.flags,fa.flagsNot))
               return TRUE;
         }
      }
   }

   return FALSE;
}
