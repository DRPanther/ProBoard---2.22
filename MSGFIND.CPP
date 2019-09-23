#define Use_MsgBase

#include "proboard.hpp"

bool
Message::readFirst(int method,int order,long first,char *data,int area)
{
 if(area)
   {
    if(!setArea(area)) return FALSE;
   }

 if(order<0)
   {
    lastIndex = first + 1;
   }
  else
   {
    lastIndex = (first > 0) ? (first - 1) : 0;
   }

 return readNext(method,order,data);
}

bool
Message::readNext(int method,int order,char *data)
{
 for(lastIndex += order ; lastIndex ; lastIndex+=order)
   {
    lastIndex = msgBase()->readMsg(*this,(order<0) ? (-lastIndex) : (lastIndex));

    if(lastIndex < 0) break;

    switch(method)
      {
       case READMSG_ALL:
         {
          return TRUE;
         }

       case READMSG_SEL:
         {
          switch(data[0])
            {
             case 'F': if(strstrl(from,&data[1])) return TRUE;
                       break;
             case 'T': if(strstrl(to,&data[1]))   return TRUE;
                       break;
             case 'S': if(strstrl(subj,&data[1])) return TRUE;
                       break;
             case 'E': if(strstrl(from,&data[1]) || strstrl(to,&data[1])) return TRUE;
                       break;
            }
         }
         break;
      }
   }

 return FALSE;
}

bool
Message::readPrev(int method,int order,char *data)
{
 long prevlast=lastIndex;

 bool x = readNext(method,-order,data);

 if(!x) lastIndex = prevlast;

 return x;
}

