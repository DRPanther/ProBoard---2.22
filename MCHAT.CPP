#include <string.h>
#include <dos.h>
#include <time.h>
#include <ctype.h>
#include "desqview.hpp"
#include "proboard.hpp"

const MAX_LOCK_TRIES = 500;
const LOCK_PAUSE = 10;

static char *lock_err_msg = "CHAT FILE LOCKING ERROR (%d)";

class chatdata
  {
   int online;
   int head,tail;
   char data[240];
   static File f[2];
   static int  node[2];
  public:
   chatdata(int n1,int n2);
  ~chatdata();
   void queue(char x);
   int getnext(char *);
  };

File chatdata::f[2];

int chatdata::node[2];

chatdata::chatdata(int n1,int n2)
{
   node[0] = n1;
   node[1] = n2;

   if(!f[0].open(form("%sCHAT%d.DAT",syspath,node[0]),fmode_rw | fmode_shared | fmode_copen))
      return;

   memset(this,0,sizeof(*this));

   f[0].write(this,sizeof(*this));

   time_t t = time(NULL);

   for(;;)
   {
      if(f[1].open(form("%sCHAT%d.DAT",syspath,node[1]),fmode_rw | fmode_shared)) break;

      if((time(NULL)-t)>20)
      {
         LOG("File sharing error during chat!");
         break;
      }
   }
}


chatdata::~chatdata()
{
   f[0].close();
   f[1].close();
}

void
chatdata::queue(char c)
{
   for(int i=0;i<MAX_LOCK_TRIES;i++)
   {
      if(f[0].lock(0,sizeof(*this))) break;
      msleep(LOCK_PAUSE);
   }

   if(i==MAX_LOCK_TRIES)
      LOG(lock_err_msg,f[0].error());

   f[0].seek(0);
   f[0].read(this,sizeof(*this));

   data[tail++]=c;

   if(tail==240)
      tail=0;

   f[0].seek(0);
   f[0].write(this,sizeof(*this));

   f[0].unlock(0,sizeof(*this));
}

int
chatdata::getnext(char *buffer)
{
   for(int i=0;i<MAX_LOCK_TRIES;i++)
   {
      if(f[1].lock(0,sizeof(*this))) break;
      msleep(LOCK_PAUSE);
   }

   if(i==MAX_LOCK_TRIES)
      LOG(lock_err_msg,f[1].error());

   f[1].seek(0);
   f[1].read(this,sizeof(*this));

   for(i=0;i<240;i++)
   {
      if(head==tail)
         break;

      buffer[i] = data[head++];

      if(head==240)
         head=0;
   }

   f[1].seek(0);
   f[1].write(this,sizeof(*this));
   f[1].unlock(0,sizeof(*this));

   return i;
}

void
multichat(char *data)
{
   int node2;

   io << "\n\n\f" << S_MCHAT_TITLE << "\n\n";

   show_users_online(String("/MC ") + data);

   io << "\n\n" << S_MCHAT_ENTER_NODE_NUMBER;

   if(io.read(node2,3)<0)
      return;

   io << "\n\n" << S_MCHAT_CALLING_OTHER_NODE << '\xFF';

   if(node2==node_number || node2>cfg.numnodes || node2<=0)
      return;

   user_online uo;
   uo.setstatus(UO_BROWSING);

   switch(uo.chatreq(node2))
   {
      case 1:
         uo.setstatus(UO_BUSY,node2);

      case 2:
         io << S_MCHAT_NODE_IS_NOT_AVAILABLE << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
         return;
   }

   io << S_MCHAT_STARTED << "\n\n\xFF";

   multilinechat(node2);

   uo.setstatus(UO_BROWSING);

   io << "\n\n" << S_MULTILINE_CHAT_ENDED << ' ' << S_PRESS_ENTER_TO_CONTINUE;
}

void
multilinechat(int n)
{
   int node1,node2;
   char buffer[240];
   user_online uo;
   int last=0;
   String s;

   node1 = node_number;
   node2 = n;

   LOG(1,"Multiline chat started");

   chatdata cd(node1,node2);

   uo.setstatus(UO_CHATTING);

   for(int i=0;;i++)
   {
      char x;

      if((x=io.readkey())!=0)
      {
         if(x<8)
            continue;

         if(x==27)
            break;

         if(last!=1)
         {
            if(avatar)
               io << "\x16\x01\x0B\xFF";
            else
               if(ansi_mode)
                  io << "[1;36m\xFF";
         }

         last=1;

         if(x=='\t') x=' ';

         if(x!=8) io << x << char(0xFF);

         switch(x)
         {
            case 8:
               if(s.len()>0)
               {
                  s[s.len()-1]=0;
                  io << "\b \b\xFF";
               }
               break;

            case 13:
               io << '\n' << char(255);
               s="";
               break;

            default:
               s << (char)x;
         }

         if(s.len()>78)
         {
            String wrap;

            int l = wordwrap(s,wrap,78);

            for(int i=0;i<l;i++)
               io << "\b \b\xFF";

            s = wrap;

            io << '\n' << s << char(0xFF);
         }

         cd.queue(x);
         DV_timeslice();
         msleep(10);
         continue;
      }

      DV_timeslice();
      msleep(100);

      if(i>5)
      {
         i = uo.getstatus(node2);

         if(i==UO_OFFLINE)
         {
            io << "\n\n" << S_MCHAT_OTHER_NODE_HUNG_UP << '\xFF';
            return;
         }
         i=0;
      }

      int count = cd.getnext(buffer);

      for(int i=0;i<count;i++)
      {
         char x=buffer[i];

         if(last!=2)
         {
            if(avatar)
               io << "\x16\x01\x07\xFF";
            else
               if(ansi_mode)
                  io << "[0;37m\xFF";
         }

         last=2;

         if(x==1) return;

         if(x=='\t') x=' ';

         if(x!=8)
            io << x << char(0xFF);

         switch(x)
         {
            case 8:
               if(s.len()>0)
               {
                  s[s.len()-1]=0;
                  io << "\b \b\xFF";
               }
               break;

            case 13:
               io << "\n\xFF";
               s = "";
               break;

            default:
               s << (char)x;
         }

         if(s.len()>78)
         {
            String wrap;
            int l=wordwrap(s,wrap,78);
            for(int i=0;i<l;i++) io << "\b \b\xFF";
            s=wrap;
            io << '\n' << s << char(0xFF);
         }
      }
   }

   cd.queue(1);

   LOG(1,"Multiline chat ended");
}

