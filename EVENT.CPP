#include "proboard.hpp"

int
event::minutesleft()
{
   Date today(TODAY);
   Time now(NOW);

   int nowminutes=now[0]*60+now[1];

   int min=10080;

   if(enabled)
      for(int i=0;i<7;i++)
      {
         int day = today.weekDay()+i;

         if(day>6) day-=7;

         if(!(days&(1<<day))) continue;

         int m=start[0]*60+start[1]+i*60*24;

         if((m+duration+1) <= nowminutes)
            m += 10080;

         m -= nowminutes;

         if(m < min)
            min = m;
      }

   return min;
}

int
event::nextday()
{
   Date today(TODAY);
   Time now(NOW);

   int nowminutes=now[0]*60+now[1];

   int min=10080;

   int retday=0;

   if(enabled)
      for(int i=0;i<7;i++)
      {
         int day = today.weekDay()+i;

         if(day>6) day -= 7;

         if(!(days&(1<<day))) continue;

         int m = start[0]*60 + start[1] + i*60*24;

         if(m<=nowminutes)
            m += 10080;

         m -= nowminutes;

         if(m<min)
         {
            min    = m;
            retday = day;
         }
      }

   return retday;
}

