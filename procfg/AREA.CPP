#define Use_DynamicArrayFile

#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"
#include "area.hpp"


_MsgArea
MsgArea::fEmpty(long r)
{
   _MsgArea ma;

   CLEAR_OBJECT(ma);

   ma.areaNum = r+1;

   return ma;
}

void
MsgArea::fMove(long /*r1*/,long r2,_MsgArea& ma)
{
   ma.areaNum = r2+1;
}

_FileArea
FileArea::fEmpty(long)
{
   _FileArea fa;

   CLEAR_OBJECT(fa);

   return fa;
}


static int
MsgArea_sortfunc(_MsgArea& ma1,_MsgArea& ma2)
{
   return stricmp(ma2.name,ma1.name);
}

static int
FileArea_sortfunc(_FileArea& fa1,_FileArea& fa2)
{
   return stricmp(fa2.name,fa1.name);
}



int Area::selected_start = -1;
int Area::selected_end   = -1;

int MsgArea::file_count = 0;
File MsgArea::f;
DynamicArrayFile<_MsgArea> *MsgArea::df = NULL;

int FileArea::file_count = 0;
File FileArea::f;
DynamicArrayFile<_FileArea> *FileArea::df = NULL;

MsgArea::MsgArea()
{
   if(file_count <= 0)
   {
      f.open(FileName(syspath,"MESSAGES.PB"),fmode_rw | fmode_copen);

      file_count = 0;

      if(!f.opened()) return;

      df = new DynamicArrayFile<_MsgArea>(f);

      df->setEmptyFunc(fEmpty);
      df->setMoveFunc(fMove);

      selStart(-1);
      selEnd(-1);
   }

   file_count++;
}

MsgArea::~MsgArea()
{
    if(file_count <= 1 && f.opened())
    {
       delete df;

       f.close();

       file_count = 1;
    }

    file_count--;
}

bool
MsgArea::read(int rec)
{
   return df->read(rec-1,*this);
}

bool
MsgArea::write(int rec)
{
   areaNum = rec;

   return df->write(rec-1,*this);
}

bool
MsgArea::clear(int rec)
{
   CLEAR_OBJECT(*((_MsgArea *)this));

   return write(rec);
}

int
MsgArea::numAreas()
{
   return int(df->numRecords());
}

bool
MsgArea::insert(int rec)
{
   return df->insert(rec-1);
}

bool
MsgArea::remove(int rec)
{
   return df->remove(rec-1);
}


dword
MsgArea::crc()
{
   return crc32(this,sizeof(_MsgArea));
}

bool
MsgArea::sort(int from,int to)
{
   return df->sort(from,to,MsgArea_sortfunc);
}

bool
MsgArea::move(int start,int end,int to)
{
   return df->move(start,end,to);
}

FileArea::FileArea()
{
   if(file_count <= 0)
   {
      f.open(FileName(syspath,"FILECFG.PRO"),fmode_rw | fmode_copen);

      file_count = 0;

      if(!f.opened()) return;

      df = new DynamicArrayFile<_FileArea>(f);

      df->setEmptyFunc(fEmpty);

      selStart(-1);
      selEnd(-1);
   }

   file_count++;
}

FileArea::~FileArea()
{
    if(file_count <= 1 && f.opened())
    {
       delete df;

       f.close();

       file_count = 1;
    }

    file_count--;
}

bool
FileArea::read(int rec)
{
   areaNum = rec;

   return df->read(rec-1,*this);
}

bool
FileArea::write(int rec)
{
   areaNum = rec;

   return df->write(rec-1,*this);
}

bool
FileArea::clear(int rec)
{
   CLEAR_OBJECT(*((_FileArea *)this));

   return write(rec);
}

int
FileArea::numAreas()
{
   return int(df->numRecords());
}

bool
FileArea::insert(int rec)
{
   return df->insert(rec-1);
}

bool
FileArea::remove(int rec)
{
   return df->remove(rec-1);
}

dword
FileArea::crc()
{
   return crc32(this,sizeof(_FileArea));
}

bool
FileArea::sort(int from,int to)
{
   return df->sort(from,to,FileArea_sortfunc);
}

bool
FileArea::move(int start,int end,int to)
{
   return df->move(start,end,to);
}

