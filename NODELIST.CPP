#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proboard.hpp"

static void near
getnode(char *temp,node_entry& n)
{
char *options[]={"ZONE","REGION","HOST","HUB","PVT","DOWN","HOLD"};
char slices[6][100];

for(int i=0;i<6;i++)
    {
    int y=0;
    while (*temp!=',') if (*temp!='_') slices[i][y++]=*temp++;
                          else {slices[i][y++]=' '; temp++;}
    temp++;
    slices[i][y]='\0';
    }

for(i=0;i<7;i++) if(!strcmpl(slices[0],options[i])) break;

n.type=i+1;

strcpy(n.name,slices[2]);
strcpy(n.city,slices[3]);

switch(i+1)
  {
  case NODE_ZONE   : n.zone=atoi(slices[1]);
  case NODE_REGION : n.region=atoi(slices[1]);
  case NODE_HOST   : {
                     n.net=atoi(slices[1]);
                     n.node=0;
                     } break;
  default          : n.node=atoi(slices[1]);
                     break;
  }
}

struct node_index
   {
   byte type;
   long offset;
   union
     {
     int zone,net;
     };
   int cost;
   long fill;
   };

#define NODE_ZONE 1
#define NODE_REGION 2
#define NODE_HOST 3
#define NODE_HUB 4
#define NODE_PVT 5
#define NODE_DOWN 6
#define NODE_HOLD 7
#define NODE_NODE 7

#define LISTNODE_SINGLE   1
#define LISTNODE_MULTIPLE 2

static int near
list_node(node_index& idx,char *fname,char mode)
{
char *types[]={ "","ZONE","REGION","NET","HUB","PVT","DOWN","HOLD","" };

File fp(FileName(cfg.nodelistdir,fname),fmode_read | fmode_text);

if(!fp.opened()) return -1;

fp.seek(idx.offset);
node_entry n;
n.net=idx.net;

for(;;)
  {
  int num;
  char s[300];
  if(!fp.readLine(s,299)) break;
  if(s[0]==';') continue;

  getnode(s,n);

  if(mode==LISTNODE_MULTIPLE && n.net!=idx.net) break;

  switch(n.type)
    {
    case NODE_ZONE  : num=n.zone; break;
    case NODE_REGION: num=n.region; break;
    case NODE_HOST  : num=n.net;    break;
    default         : num=n.node;   break;
    }

  io.enablestop();
  stopped=0;
  io << form("\2 %-6s \3%5d \7%-28.28s \6%-28.28s \3%4d\n",types[n.type],num,n.name,n.city,idx.cost);
  io.disablestop();
  if(stopped || linecounter()) return -1;
  if(mode==LISTNODE_SINGLE) break;
  }

return 0;
}

int
node_entry::read(char *s)
{
char nodenr[14];
char *tmp=nodenr;
int point=0;

strcpy(nodenr,s);

if((s=strchr(tmp,'.'))!=NULL)
  {
  point=atoi(s+1);
  *s=0;
  }

if((s=strchr(tmp,':'))!=NULL)
  {
  *s++=0;
  zone=atoi(tmp);
  tmp=s;
  }
 else if(tmp[0]=='?') zone=-1;

if((s=strchr(tmp,'/'))!=NULL)
  {
  *s++=0;
  if(tmp[0]!='?') net=atoi(tmp);
            else  net=-1;
  tmp=s;
  }
 else if(tmp[0]=='?') net=-1;

if(tmp[0]!='?') node=atoi(tmp);
        else    node=-1;

node_index idx;

char nl_name[13];

File f(FileName(syspath,"NODE_IDX.PRO"),fmode_read,512);
if(!f.opened()) return -1;

char xor[13]={ 234,123,76,245,87,12,43,56,23,12,98,55,34 };

int curzone=0;

if(zone==-1)
      io << S_ZONELIST_HEADER << '\n';
 else if(net==-1)
      io << S_NETLIST_HEADER(form("%d",zone)) << '\n';
 else if(node==-1)
      {
      char *s=form("%d:%d",zone,net);
      io << S_NODELIST_HEADER(s) << '\n';
      }

linecounter(2);

for(;;)
  {
  if(f.read(&idx,sizeof(idx))!=sizeof(idx)) break;

  char *ptr=(char *)&idx;
  for(int i=0;i<13;i++) ptr[i]^=xor[i];

  if(idx.type>10) { strcpy(nl_name,ptr); continue; }

  if(idx.type==NODE_ZONE)
    {
    curzone=idx.zone;
    if(zone==-1)
      {
      if(list_node(idx,nl_name,LISTNODE_SINGLE)<0) break;
      continue;
      }
    idx.net=curzone;
    if(curzone!=zone) continue;
    }

  if(curzone==zone && net==-1)
    {
    if(list_node(idx,nl_name,LISTNODE_SINGLE)<0) break;
    continue;
    }

  if(idx.net==net && zone==curzone)
    {
    if(node==-1)
       {
       if(list_node(idx,nl_name,LISTNODE_MULTIPLE)<0) return -1;
       break;
       }

    node_entry tmp;
    tmp.zone=curzone;
    tmp.net=idx.net;
    tmp.cost=idx.cost;

    File fp(FileName(cfg.nodelistdir,nl_name),fmode_read | fmode_text);
    if(!fp.opened()) return -1;

    fp.seek(idx.offset);
    for(;;)
      {
      char s[300];
      if(fp.readLine(s,299)==NULL) break;
      if(s[0]==';') continue;
      getnode(s,tmp);
      if(tmp.type<NODE_HUB && tmp.net!=net) break;
      if(tmp.node==node)
        {
         (*this)=tmp;
         return point;
        }
      }
    fp.close();
    break;
    }
  }

if(zone!=-1 && net!=-1 && node!=-1) io << S_NODE_NUMBER_NOT_FOUND;

return -1;
}

