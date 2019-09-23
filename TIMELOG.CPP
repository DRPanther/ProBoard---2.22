#include <string.h>
#include "proboard.hpp"

void
timelog::read()
{
memset(this,0,sizeof(timelog));
numdays=1;

File f(FN_TIMELOG_PRO);

if(f.opened()) f.read(this,sizeof(timelog));

if(!daysactive) daysactive = numdays;
}

void
timelog::update()
{
memset(this,0,sizeof(timelog));

File f(FN_TIMELOG_PRO,fmode_rw | fmode_excl | fmode_copen);

f.read(this,sizeof(timelog));

if(!daysactive) daysactive = numdays;

if(lastdate!=Date(TODAY))
  {
   if(cfg.usage_days && numdays>=cfg.usage_days)
     {
      for(int i=0;i<24;i++) hours[i] -= long(numdays-cfg.usage_days+1)*hours[i]/numdays;
      for(i=0;i<7;i++)      days [i] -= long(numdays-cfg.usage_days+1)*days [i]/numdays;
      numdays = cfg.usage_days;
     }
    else numdays++;

   daysactive++;

   lastdate.today();
  }

hours[login_time[0]%24]+=timer.online();
days[login_date.weekDay()%7]+=timer.online();
weeks[login_date.weekNum()%53]+=timer.online();

if(io.baud) totalcalls++;

f.rewind();
f.write(this,sizeof(timelog));
}
