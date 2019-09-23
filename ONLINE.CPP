#include <string.h>
#include <dos.h>
#include "proboard.hpp"

void
user_online::write()
{
   memset(this,0,sizeof(*this));

   File f(FN_ONLINE_PRO,fmode_rw);

   if(!f.opened())
     {
      f.open(FN_ONLINE_PRO,fmode_create | fmode_rw);
      for(int i=0;i<cfg.numnodes;i++) f.write(this,sizeof(*this));
     }

   strcpy(name,user.name);
   strncpy(city,user.city,15);
   city[15]=0;
   baud=io.baud;
   status=0;

   f.seek(long(node_number-1)*sizeof(*this));
   f.write(this,sizeof(*this));
}


void
user_online::clear()
{
File f(FN_ONLINE_PRO,fmode_rw);

memset(this,0,sizeof(*this));

if(!f.opened())
  {
   f.open(FN_ONLINE_PRO,fmode_rw | fmode_create);
   for(int i=0;i<cfg.numnodes;i++) f.write(this,sizeof(*this));
   return;
  }

status=-2;
f.seek(long(node_number-1)*sizeof(*this));
f.write(this,sizeof(*this));
}

void
show_users_online(char *data)
{
   String param[5];
   File f(FN_ONLINE_PRO);
   user_online uo;
   bool mchat = FALSE;
   bool handles = FALSE;

   int npara = parse_data(data,param);

   for(int i=0;i<npara && i<5;i++)
   {
      if(param[i] == "/MC")
         mchat = TRUE;
      if(param[i] == "/H")
         handles = TRUE;
   }

   if(!mchat) io << "\n\n\f";

   io << S_SHOW_USERS_ONLINE_HEADER << '\n';

   char *status[5];

   status[0] = S_USER_ONLINE_STATUS_BROWSING;
   status[1] = S_USER_ONLINE_STATUS_CHATTING;
   status[2] = S_USER_ONLINE_STATUS_ONLINE  ;
   status[3] = S_USER_ONLINE_STATUS_INSHELL ;
   status[4] = S_USER_ONLINE_STATUS_BUSY    ;

   for(i=0;i<cfg.numnodes;i++)
      {
      CLEAR_OBJECT(uo);
      f.read(&uo,sizeof(uo));
      io << form("\1%5d    ",i+1);
      if(uo.name[0] && handles)
      {
         User x;
         if(x.search(uo.name))
         {
            if(x.alias[0])
               strcpy(uo.name,x.alias);
         }
      }

      if(uo.name[0] && (strcmpl(uo.name,cfg.sysopname) || !cfg.discrete))
            io << form("\6%-35s   \3%-15.15s  \7%s\n",uo.name,uo.city,status[(uo.status<=0)?(-uo.status):1]);
         else io << S_USER_ONLINE_NO_USER_ONLINE << '\n';
      }

   f.close();

   if(!mchat) io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
}

void
user_online::setstatus(int s,int node)
{
File f(FN_ONLINE_PRO,fmode_rw | fmode_excl);

if(!f.opened()) return;

if(!node) node=node_number;

f.seek(long(node-1)*sizeof(*this));
f.read(this,sizeof(*this));

status=s;

f.seek(long(node-1)*sizeof(*this));
f.write(this,sizeof(*this));
}

int
user_online::getstatus(int node)
{
File f(FN_ONLINE_PRO);

CLEAR_OBJECT(*this);

if(!f.opened()) return 0;

if(!node) node=node_number;

f.seek(long(node-1)*sizeof(*this));
f.read(this,sizeof(*this));

return status;
}

int
user_online::chatreq(int node)
{
if(getstatus(node)) return 2;

setstatus(node_number,node);

for(int i=0;i<20;i++)
  {
  int req=getstatus(node);

  if(req>=0) { sleep(1); continue; }

  switch(req)
    {
    case UO_CHATTING: setstatus(UO_CHATTING);
                      return 0;
    default:          return 2;
    }
  }
return 1;
}

int
user_online::check_chat()
{
int x=getstatus();

return (x>0)?x:0;
}

void
user_online::read(int node)
{
   File f(FN_ONLINE_PRO);
   if(!f.opened()) return;

   if(!node) node=node_number;

   f.seek(long(node-1)*sizeof(*this));
   f.read(this,sizeof(*this));
}
