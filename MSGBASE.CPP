#define Use_MsgBase

#include "proboard.hpp"

MsgBaseList::MsgBaseList()
{
    mblist[0] = NULL;
}

void
MsgBaseList::add(MsgBase *mb)
{
    for(int i=0; mblist[i] ; i++) {}

    mblist[i] = mb;
    mblist[i+1] = NULL;
}

MsgBaseList::~MsgBaseList()
{
    for(int i=0; mblist[i] ; i++) delete mblist[i];
}

void
MsgBaseList::open()
{
    for(int i=0 ; mblist[i] ; i++) mblist[i]->open();
}

void
MsgBaseList::close()
{
    for(int i=0 ; mblist[i] ; i++) mblist[i]->close();

    MsgArea::close();
}

bool
MsgBaseList::lock()
{
    for(int i=0 ; mblist[i] ; i++) 
      if(!mblist[i]->lock()) 
         return FALSE;

    return TRUE;
}

bool
MsgBaseList::unlock()
{
    for(int i=0 ; mblist[i] ; i++) 
      if(!mblist[i]->unlock()) 
         return FALSE;

    return TRUE;
}

word
MsgBaseList::scanMail(MessageIndex *mi,word maxmsgs)
{
 word left = maxmsgs;

 for(int i=0; mblist[i] ; i++)
   {
    if(left>0) left -= mblist[i]->scanMail(&mi[maxmsgs-left],left);
   }

 return maxmsgs-left;
}

long
MsgBaseList::totalMsgs()
{
 long total = 0;

 for(int i=0; mblist[i] ; i++)
   {
    total += mblist[i]->totalMsgs();
   }

 return total;
}
