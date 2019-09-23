#define Use_MsgBase

#include <string.h>
#include "proboard.hpp"

File MsgArea::f;
int  MsgArea::lastAreaNum = -1;
int  MsgArea::numAreas = -1;

MsgArea *MsgArea::lastArea = NULL;

void
MsgArea::open()
{
    if(!f.opened())
    {
       if(!f.open(FN_MSGAREAS_PRO,fmode_read,cfg.fastmode ? BUFSIZE_FAST:BUFSIZE_SLOW))
         file_error(FN_MSGAREAS_PRO);

       numAreas = int(f.len() / sizeof(_MsgArea));
    }
}

bool
MsgArea::read(int a)
{
   open();

   msgBase = NULL;

   if(a<1 || a>numAreas) return FALSE;

   if(lastArea == NULL)
   {
      lastArea = new MsgArea;
      lastAreaNum = -1;
   }

   if(a != lastAreaNum)
      {
      f.seek(long(a-1) * sizeof(_MsgArea));

      if(   f.read(lastArea,sizeof(_MsgArea)) != sizeof(_MsgArea)
         || lastArea->name[0] == '\0' )
         {
         lastAreaNum = -1;

         return FALSE;
         }

      switch(lastArea->msgBaseType)
         {
         case MSGBASE_SDM   :
         case MSGBASE_SQUISH: lastArea->msgBase = squishMsgBase;
                              break;
         case MSGBASE_JAM   : lastArea->msgBase = jamMsgBase;
                              break;
         case MSGBASE_HUDSON: lastArea->msgBase = hudsonMsgBase;
                              if((a<1 || a>200) && lastArea->name[0])
                              {
                                 LOG("Message area %d: Hudson type not allowed",a);
                                 return FALSE;
                              }
                              break;
         default            : LOG("Bad message base type for area #%d",a);
                              return FALSE;
         }
      }

   (*this) = (*lastArea);

   if(msgBaseType != MSGBASE_HUDSON)
   {
      append_backspace(path);

      if(msgBaseType !=  MSGBASE_SDM) path[strlen(path)-1] = '\0';
   }

   lastAreaNum = areaNum;

   if(!sysop[0]) strcpy(sysop,cfg.sysopname);

   if(!replyBoard) replyBoard = areaNum;

   strip_trailing( name );
   strip_leading( name );

   return (name[0]) ? TRUE:FALSE;
}

void
MsgArea::close()
{
    f.close();

    if(lastArea != NULL) delete lastArea;

    lastAreaNum = -1;
    numAreas    = -1;
    lastArea    = NULL;
}

int
MsgArea::highAreaNum()
{
    open();

    return numAreas;
}

bool
MsgArea::sysopAccess()
{
    return (    check_access(sysopLevel,sysopFlags,sysopFlagsNot)
            || !strcmpl(sysop,user.name)
            || !strcmpl(cfg.sysopname,user.name)
           );
}

