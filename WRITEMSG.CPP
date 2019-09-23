#define Use_MsgBase

#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "proboard.hpp"

void
writemsg(char *data)
{
Message msg;
MsgArea ma;
String param[5];
bool goodbye = FALSE;
int cost=0;
bool to_specified = FALSE;
bool subj_specified = FALSE;
char netmail_address[30];

netmail_address[0] = '\0';

parse_data(data,param);

if(param[0][0]=='*') msg.setArea(user.msgArea);
                else msg.setArea(atoi(param[0]));

if(!ma.read(msg.areaNum())) return;

io << "\f\n" << S_WRITE_MESSAGE_TITLE(ma.name) << '\n';

msg.to[0]=0;
for(int i=0;i<5;i++)
    if(param[i][0]=='/')
       switch(toupper(param[i][1]))
          {
          case 'T': strcpy(msg.to,(char *)(param[i])+3);
                    to_specified = TRUE;
                    break;
          case 'S': strcpy(msg.subj,(char *)(param[i])+3);
                    if(msg.subj[0])
                     subj_specified = TRUE;
                    break;
          case 'L': goodbye = TRUE;
                    break;
          case 'N': strncpy(netmail_address,(char *)(param[i])+3,29);
                    netmail_address[29] = '\0';
                    break;
          }

if(!check_access(ma.writeLevel,ma.writeFlags,ma.writeFlagsNot) && !ma.sysopAccess())
  {
   io << '\n' << S_NOT_ALLOWED_TO_WRITE_IN_AREA << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

   return;
  }

strcpy(msg.from,user.name);

if(ma.flags == 3)
   strcpy(msg.from,user.alias);
else
   if(ma.flags && (ma.flags!=2 || user.alias[0]))
   {
      io << '\n';

      if(ma.flags > 1)
         io << S_ASK_WRITE_MSG_USING_YOUR_ALIAS;
      else
         io << S_ASK_WRITE_MSG_USING_AN_ALIAS;

      if(io.ask(0))
      {
         if(ma.flags==2)
         {
            if(user.alias[0] == '\0') strcpy(user.alias,user.name);
            strcpy(msg.from,user.alias);
         }
         else
            for(;;)
            {
               User tmpuser;

               io << "\n\n" << S_ENTER_ALIAS_TO_USE;

               io.read(msg.from,35,READMODE_UPFIRST);
               if(!msg.from[0]) return;
               if(tmpuser.search(msg.from,TRUE,FALSE,TRUE)) continue;
               if(!tmpuser.search(msg.from)) break;
            }
      }

      io << '\n';
   }

if(!msg.to[0])
 for(;;)
  {
  User tuser;

  if(ma.msgType != MSG_PVT && ma.msgKind != MSG_NET)
   io << '\n' << S_ENTER_ALL_IF_TO_EVERYONE << '\n';

  io << '\n' << S_WRITE_MSG_TO;
  io.read(msg.to,35,READMODE_UPFIRST);

  if(!msg.to[0]) return;
  if(!strcmp(msg.to,"All")   && ma.msgKind!=MSG_NET && ma.msgType != MSG_PVT) break;
  if(!strcmp(msg.to,"Sysop") && ma.msgKind==MSG_LOCAL) break;

  if(ma.msgKind!=MSG_LOCAL) break;

  if(ma.flags==1) break;

  io << "\n\n\7" << S_SCANNING_USER_FILE << '\xFF';

  if(tuser.search(msg.to)) break;

  if(ma.flags==2)
   if(tuser.search(msg.to,TRUE,FALSE,TRUE)) break;

   bool found = tuser.search(msg.to,FALSE,TRUE,FALSE);

  for(int count = 0;found && count<2;count++)
   found = tuser.searchNext();

  if(count==1)
    {
     tuser.search(msg.to,FALSE,TRUE,FALSE);

     io << "\n\n" << S_CONFIRM_WRITE_MESSAGE_TO(tuser.name);

     if(io.ask(TRUE))
       {
        strcpy(msg.to,tuser.name);
        break;
       }
      else
       {
        io << '\n';
        continue;
       }
    }

  if(count>1)
    {
     io.enablestop();

     io << "\n\n" << S_FUZZY_SEARCH_LIST_HEADER << '\n';

     linecounter(2);

     found = tuser.search(msg.to,FALSE,TRUE,FALSE);
     while(found)
       {
        if(!(tuser.uFlags & UFLAG_HIDDEN))
         io << form("\6 %-32.32s \5%-22.22s \2%-22.22s\n",tuser.name,tuser.city,tuser.country);
        if(stopped || linecounter()) break;
        found = tuser.searchNext();
       }
     io.disablestop();
     if(!found)
      io << S_FUZZY_SEARCH_LIST_FOOTER << '\n';

     continue;
    }

  io << S_USER_NOT_LOCATED << '\n';
  }

if(!strcmpl(msg.to,"Sysop")) strcpy(msg.to,ma.sysop);

if(to_specified)
   io << '\n' << S_WRITE_MSG_TO << msg.to;


if(ma.msgKind==MSG_NET)
     {
     aka a;
     a.read(ma.aka);
     char tmp[30];

     if(netmail_address[0])
     {
       int point;

       node_entry ne;
       ne.zone=a.zone;
       ne.net=a.net;
       ne.node=a.node;

       if((point=ne.read(netmail_address))<0)
       {
         LOG("Unknown netmail address: %s",netmail_address);
         return;
       }

      msg.destZone = ne.zone;
      msg.destNet  = ne.net;
      msg.destNode = ne.node;
      msg.destPoint= point;
      msg.cost     = cost;
     }
     else
     {
     io << "\n\n" << S_ENTER_NETMAIL_ADDRESS_HEADER;

     for(;;)
       {
       int point;

       io << "\n\n" << S_ENTER_NETMAIL_ADDRESS;

       io.read(tmp,25);
       if(!tmp[0]) return;
       io << "\n\n";

       node_entry ne;
       ne.zone=a.zone;
       ne.net=a.net;
       ne.node=a.node;

       if((point=ne.read(tmp))<0) continue;

       cost=ne.cost;

       io << S_CONFIRM_NETMAIL_ADDRESS(form("%d:%d/%d.%d",ne.zone,ne.net,ne.node,point),ne.name,form("%d",ne.cost));

       if(io.ask(1))
         {
         if(user.credit<cost)
           {
           io << "\n\n" << S_NOT_ENOUGH_NETMAIL_CREDIT(form("%d",user.credit))
              << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

           return;
           }
         msg.destZone = ne.zone;
         msg.destNet  = ne.net;
         msg.destNode = ne.node;
         msg.destPoint= point;
         msg.cost     = cost;
         break;
         }
       }
     }
     if(check_access(cfg.crashlevel,cfg.crashflags))
       {
       io << "\n\n" << S_ASK_SEND_CRASH;
       if(io.ask(0)) msg.attr |= MSGATTR_CRASH;
       }
     if(check_access(cfg.attachlevel,cfg.attachflags))
       {
       io << "\n\n" << S_ASK_ATTACH_FILE;
       if(io.ask(0))
          {
          msg.attr |= MSGATTR_FILE;
          io << "\n\n" << S_FILE_TO_ATTACH;
          io.read(msg.subj,60);
          if(!msg.subj[0]) return;
          }
       }
     if(cfg.killsent) msg.attr |= MSGATTR_KILL;
     }

if(subj_specified)
{
   io << "\n\n" << S_SHOW_MESSAGE_SUBJECT(msg.subj);
}
else
if(!(msg.attr & MSGATTR_FILE))
  {
   io << "\n\n" << S_ENTER_MESSAGE_SUBJECT;
  io.read(msg.subj,65);
  if(!msg.subj[0]) return;
  }

switch(ma.msgType)
 {
  case MSG_BOTH: {
                 if(strcmp(msg.to,"All"))
                    {
                     io << "\n\n" << S_ASK_PRIVATE_MESSAGE;
                     if(io.ask(0)) msg.attr |= MSGATTR_PRIVATE;
                    }
                 } break;
  case MSG_PVT:  msg.attr |= MSGATTR_PRIVATE; break;
 }

io << '\n';
unlink("MSGTMP");

write_msginf( msg.from , msg.to , msg.subj , ma.highMsg()+1 , ma.name , (msg.attr & MSGATTR_PRIVATE) ? "YES":"NO");

edit_message();

unlink("MSGINF");

if(!access("MSGTMP",0))
  {
  io << "\n\n" << S_SAVING_MESSAGE;

  msg.postDate.today();
  msg.postTime.now();

  long nr = msg.add();

  if(nr >= 1)
  {
   nr = msg.msgBase()->msgNum(*msg.msgArea,nr);
  }

  if(nr<1) io << S_SAVING_MESSAGE_ERROR << '\n';
       else
        {
        io << form("\3#%d\n",nr);
        user.msgsPosted++;
        LOG(1,"Msg #%ld entered (Area %d)",nr,msg.areaNum());
        if(ma.msgKind == MSG_NET) user.pending += cost;
        }
  }
 else
  LOG(1,"Msg aborted (Area %d)",msg.areaNum());

if(goodbye) logoff(NULL);
    else    io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
}

void
edit_message()
{
 if(fsed_mode && (ansi_mode || avatar))
   {
   if(run_sdkfile("/I FSED") < 0)
     {
      if(!cfg.editorname[0] && !cfg.internalfsed)
        {
         msged();
         return;
        }
      String editorcommand(cfg.editorname);

      if(cfg.internalfsed)
         editorcommand = "@_FSED";

      if(editorcommand[0] == '@')
      {
         editorcommand.del(0,1);
         run_sdkfile(replace_stringvars(editorcommand));
      }
      else
      {
         editorcommand << "*N*Q *P *B *T *I";
         shell(replace_stringvars(editorcommand));
      }

      switch(returnvalue)
        {
         case 1: io << "\n\n" << S_MESSAGE_ABORTED << '\n';
                 unlink("MSGTMP");
                 break;
         case 2: LOG("Carrier lost while editing.");
                 exit_proboard();
        }
     }
   }
  else msged();
}

void
write_msginf( char *from , char *to , char *subj , long num , char *area , char *pvt)
{
 File f;

 if(f.open("MSGINF",fmode_create | fmode_text))
   {
    f << from << '\n';
    f << to   << '\n';
    f << subj << '\n';
    f << form("%ld\n",num);
    f << area << '\n';
    f << pvt  << '\n';
   }
}
