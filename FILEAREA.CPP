#define Use_MsgBase

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "proboard.hpp"

File FileArea::f;
int  FileArea::lastAreaNum = -1;
int  FileArea::numAreas = -1;

FileArea *FileArea::lastArea = NULL;

void
FileArea::open()
{
   if(!f.opened())
   {
      if(!f.open(FileName(syspath,"FILECFG.PRO"),fmode_read,cfg.fastmode ? BUFSIZE_FAST:BUFSIZE_SLOW))
         file_error("FILECFG.PRO");

      numAreas = int(f.len() / sizeof(_FileArea));
   }
}

bool
FileArea::read(int a)
{
   open();

   if(a<1 || a>numAreas) return FALSE;

   if(lastArea == NULL)
   {
      lastArea = new FileArea;
      lastAreaNum = -1;
   }

   if(a != lastAreaNum)
   {
      f.seek(long(a-1) * sizeof(_FileArea));

      if(   f.read(lastArea,sizeof(_FileArea)) != sizeof(_FileArea)
         || lastArea->name[0] == '\0' )
      {
         lastAreaNum = -1;

         return FALSE;
      }
   }

   (*this) = (*lastArea);


   append_backspace(filepath);

   strip_trailing( listpath );
   strip_leading( listpath );

   if(listpath[0] == '\0')
   {
      strcpy(listpath , filepath);
      strcat(listpath , "FILES.BBS");
   }

   lastAreaNum = a;

   areaNum = a;

   strip_trailing( name );
   strip_leading( name );

   return (name[0]) ? TRUE:FALSE;
}

void
FileArea::close()
{
    f.close();

    if(lastArea != NULL) delete lastArea;

    lastAreaNum = -1;
    numAreas    = -1;
    lastArea    = NULL;
}

int
FileArea::highAreaNum()
{
    open();

    return numAreas;
}

void
create_arealist(String param[],int numpara,BitArray& arealist,bool msgareas)
{
   FileArea fa;
   MsgArea ma;
   int MAX;

   if(msgareas)
      MAX = MsgArea::highAreaNum();
   else
      MAX = FileArea::highAreaNum();

   for(int i=0;i<numpara;i++)
   {
      char *ptr=param[i];
      int incl=1,offset=0;
      int kindfilter = 0;

      if(ptr[0]=='/') continue;

      if(ptr[0]=='-') { incl=0; offset=1; }
      if(ptr[0]=='+') { incl=1; offset=1; }

      switch(toupper(ptr[offset]))
      {
         case 'E': kindfilter = MSG_ECHO;   break;
         case 'N': kindfilter = MSG_NET;    break;
         case 'L': kindfilter = MSG_LOCAL;  break;
      }

      switch(toupper(ptr[offset]))
      {
         case '*':
            {
               for(int j=1;j<=MAX;j++)
                  if(incl)
                     arealist.set(j);
                  else
                     arealist.clear(j);
            }
            break;

         case 'C':
            {
               if(msgareas) break;

               for(int j=1;j<=MAX;j++)
               {
                  if(fa.read(j))
                     if(fa.cdrom)
                        if(incl)
                           arealist.set(j);
                        else
                           arealist.clear(j);
               }
            }
            break;

         case 'X':
            {
               if(msgareas)
               {
                  if(incl)
                     arealist.set(user.msgArea);
                  else
                     arealist.clear(user.msgArea);
               }
               else
               {
                  if(incl)
                     arealist.set(user.fileArea);
                  else
                     arealist.clear(user.fileArea);
               }
            }
            break;

         case 'N':
         case 'L':
         case 'E':
            {
               if(!msgareas) break;

               for(int j=1;j<=MAX;j++)
               {
                  if(ma.read(j))
                     if(ma.msgKind == kindfilter)
                        if(incl)
                           arealist.set(j);
                        else
                           arealist.clear(j);
               }
            }
            break;

         default:
            {
               if(strchr(ptr+offset,'-'))
               {
                  int from = atoi(strtok(ptr+offset,"-"));
                  int to   = atoi(strtok(NULL,"-"));

                  for(int j=from;j<=to;j++)
                  {
                     if(incl)
                        arealist.set(j);
                     else
                        arealist.clear(j);
                  }
               }
               else
               {
                  int j = atoi(ptr+offset);

                  if(incl)
                     arealist.set(j);
                  else
                     arealist.clear(j);
               }
            }
            break;
      }
   }
}
