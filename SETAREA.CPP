#define Use_MsgBase

#include <string.h>
#include <stdlib.h>
#include "proboard.hpp"

void select_filegroup(char *);
void select_msggroup(char *);

void
set_msgarea(char *data)
{
   MsgArea ma;
   int areanum = 0;
   String param[50];
   word group = 0;
   int  columns = 2;
   bool group_specified = FALSE;
   bool listareas = FALSE;
   bool show_lastread = TRUE;
   bool list_specified = FALSE;

   BitArray arealist(MAX_MSG_AREAS,1);

   int npara = parse_data(data,param);

   for(int i=0;i<npara;i++)
   {
      param[i].upperCase();

      if(param[i] == "/L")
         listareas     = TRUE;

      if(param[i] == "/N")
         show_lastread = FALSE;

      if(!strnicmp(param[i],"/C=",3))
      {
         columns = atoi(&param[i][3]);
      }

      if(param[i] == "/MG")
      {
         group           = user.msgGroup;
         group_specified = TRUE;
      }

      if(!strnicmp(param[i],"/MG=",4))
      {
         group           = atoi(&param[i][4]);
         group_specified = TRUE;
      }

      if(param[i][0] != '/') list_specified = TRUE;
   }

   if(list_specified)
   {
      create_arealist(param,npara,arealist,TRUE);
   }
   else
   {
      for(i = 1 ; i <= MAX_MSG_AREAS ; i++)
         arealist.set(i);
   }

   if(group_specified && group == 0)
   {
      select_msggroup("");

      group = user.msgGroup;

      if(!group)
         return;

      io << '\n';
   }

   for(bool shown=FALSE;;shown=TRUE)
   {
      if(!listareas)
      {
         io << '\n' << S_ENTER_MESSAGE_AREA_NUMBER;
         if(!shown) io << S_PRESS_ENTER_TO_LIST;
         io << ": ";
         if(io.read(areanum,5)>=0) break;

         if(shown) return;

         io << "\n\f\n" << S_SELECT_MESSAGE_AREA_TITLE << "\n\n";

         linecounter(3);
      }

      io.enablestop();

      for(int i=1,count=0;i<=MsgArea::highAreaNum();i++)
      {
         String newmail = " ";

         if(!arealist[i] || !ma.read(i) || !check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot)) continue;

         if(group && !ma.allGroups)
         {
            for(int g=0;g<4;g++)
            {
               if(ma.groups[g] == group)
                  break;
            }

            if(g==4)
               continue;
         }

         if(!listareas && show_lastread)
            if(ma.lastRead(user_recnr) < ma.highMsg())
               newmail = "\7*";

         io << form("\3%4d. %s \6%-*.*s ",i,(char *)newmail,80/columns-10,80/columns-10,ma.name);

         if(stopped) break;

         if(!((++count)%columns))
         {
            io << '\n';
            if(linecounter()) break;
         }
      }

      if(count%columns) io << '\n';

      if(listareas) break;
   }

   if(!listareas)
   {
      io << '\n';

      if(!arealist[areanum] || !ma.read(areanum) || (!check_access(ma.readLevel,ma.readFlags,ma.readFlagsNot) && !ma.sysopAccess()))
      {
         io << '\n' << S_UNKNOWN_MESSAGE_AREA << ' ' << S_PRESS_ENTER_TO_CONTINUE;
         return;
      }

      user.msgArea = areanum;

      updatemenu = TRUE;
   }
}

void
set_filearea(char *data)
{
   FileArea fa;
   word group = 0;
   int areanum = 0;
   bool arealist_created = FALSE;
   bool group_specified = FALSE;
   int  columns = 2;
   String param[50];
   BitArray arealist(MAX_FILE_AREAS , 1);

   int npara=parse_data(data,param);

   for(int i=0;i<npara;i++)
   {
      param[i].upperCase();

      if(param[i] == "/FG")
      {
         group         = user.fileGroup;
         group_specified = TRUE;
      }
      if(!strnicmp(param[i],"/FG=",4))
      {
         group = atoi(&param[i][4]);
         group_specified = TRUE;
      }

      if(!strnicmp(param[i],"/C=",3))
      {
         columns = atoi(&param[i][3]);
      }
   }


if(group_specified && group == 0)
{
   select_filegroup("");

   group = user.fileGroup;

   if(!group)
      return;

   io << '\n';
}

for(bool shown=FALSE;;shown=TRUE)
  {
   io << '\n' << S_ENTER_FILE_AREA_NUMBER;
   if(!shown) io << S_PRESS_ENTER_TO_LIST;
   io << ": ";
   if(io.read(areanum,4)>=0) break;

   if(shown) return;

   io << "\f\n" << S_SELECT_FILE_AREA_TITLE << "\n\n";

   io << "\x17.";

   create_arealist(param,npara,arealist);
   arealist_created = TRUE;

   io << '\r';

   linecounter(3);
   io.enablestop();

   for(int i=1,count=0;i<=FileArea::highAreaNum();i++)
     {
      if(!arealist[i] || !fa.read(i)) continue;

      if(!check_access(fa.level,fa.flags,fa.flagsNot)) continue;

      if(group && !fa.allGroups)
      {
         for(int g=0;g<4;g++)
         {
            if(fa.groups[g] == group)
               break;
         }

         if(g==4)
            continue;
      }

      io << form("\3%4d. \6%-*.*s ",i,80/columns-8,80/columns-8,fa.name);

      if(stopped) break;
      if(!((++count)%columns))
        {
         io <<'\n';
         if(linecounter()) break;
        }
     }

   if(count%columns) io << '\n';
  }

io << '\n';

if((arealist_created && !arealist[areanum]) || !fa.read(areanum) || !check_access(fa.level,fa.flags,fa.flagsNot))
  {
   io << '\n' << S_UNKNOWN_FILE_AREA << ' ' << S_PRESS_ENTER_TO_CONTINUE;
   return;
  }

user.fileArea = areanum;

updatemenu = TRUE;
}

void
select_filegroup(char *)
{
   File f;
   AreaGroup group;

   f.open(FileName(syspath,"FGROUPS.PB"));

   io << "\f\n" << S_SELECT_FILE_AREA_GROUP_TITLE << "\n\n";

   linecounter(3);
   io.enablestop();

   for(int i=1,count=0;;i++)
   {
      if(f.read(&group,sizeof(group)) != sizeof(group))
         break;

      if(!check_access(group.level,group.flags,group.flagsNot))
         continue;

      if(!group.name[0])
         continue;

      io << form(" \3%3d. \6%-29.29s ",i,group.name);

      if(stopped) break;

      if((count++)%2)
      {
         io <<'\n';
         if(linecounter()) break;
      }
   }

   if(count%2) io << '\n';

   int groupnum = 0;

   io << '\n' << S_ENTER_GROUP_NUMBER;

   if(io.read(groupnum,3) < 0) return;

   io << '\n';

   if(groupnum < 1 || groupnum > 255)
      io << '\n' << S_UNKNOWN_GROUP << ' ' << S_PRESS_ENTER_TO_CONTINUE;

   f.seek(long(groupnum-1) * sizeof(group));

   if(f.read(&group,sizeof(group)) != sizeof(group) || !check_access(group.level,group.flags,group.flagsNot))
      io << '\n' << S_UNKNOWN_GROUP << ' ' << S_PRESS_ENTER_TO_CONTINUE;

   user.fileGroup = groupnum;

   updatemenu = TRUE;
}

void
select_msggroup(char *)
{
   File f;
   AreaGroup group;

   f.open(FileName(syspath,"MGROUPS.PB"));

   io << "\f\n" << S_SELECT_MSG_AREA_GROUP_TITLE << "\n\n";

   linecounter(3);
   io.enablestop();

   for(int i=1,count=0;;i++)
   {
      if(f.read(&group,sizeof(group)) != sizeof(group))
         break;

      if(!check_access(group.level,group.flags,group.flagsNot))
         continue;

      if(!group.name[0])
         continue;

      io << form(" \3%3d. \6%-29.29s ",i,group.name);

      if(stopped) break;

      if((count++)%2)
      {
         io <<'\n';
         if(linecounter()) break;
      }
   }

   if(count%2) io << '\n';

   int groupnum = 0;

   io << '\n' << S_ENTER_GROUP_NUMBER;

   if(io.read(groupnum,3) < 0) return;

   io << '\n';

   if(groupnum < 1 || groupnum > 255)
   {
      io << '\n' << S_UNKNOWN_GROUP << ' ' << S_PRESS_ENTER_TO_CONTINUE;
      return;
   }

   f.seek(long(groupnum-1) * sizeof(group));

   if(f.read(&group,sizeof(group)) != sizeof(group) || !check_access(group.level,group.flags,group.flagsNot))
   {
      io << '\n' << S_UNKNOWN_GROUP << ' ' << S_PRESS_ENTER_TO_CONTINUE;
      return;
   }

   user.msgGroup = groupnum;

   updatemenu = TRUE;
}


