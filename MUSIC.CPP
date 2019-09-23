#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>
#include <tswin.hpp>
#include "proboard.hpp"

struct music_data { unsigned freq,dur; };

char
playsong(char *name,char *hotkeys,int length)
{
int silent=0,i,j;
music_data *data;
time_t tm;

File fp(FileName(syspath,name,".MUS"),fmode_read | fmode_text);

if(!fp.opened())
  {
  if(!length) return 1;
  tm=time(NULL);

  for(i=0;;i++)
   {
   if(int(time(NULL)-tm)>length) break;

   if(!silent && !noyell_flag) tsw_beep(100*(i%20)+200,40);

   if(KB.hit())
     {
     KEY c=KB.uget();
     if(hotkeys) for(j=0;j<strlen(hotkeys);j++) if(c==hotkeys[j]) return c;
     }
   }
  return 0;
  }

data=new music_data[1000];

for(i=0;i<1000;)
  {
  char str[100];
  if(!fp.readLine(str,99)) break;
  char *p=strtok(str," \n");
  if(!strcmpl(p,"TONE"))
    {
    data[i].freq=atoi(strtok(NULL," \n"));
    data[i++].dur =atoi(strtok(NULL," \n"));
    }
  if(!strcmpl(p,"WAIT"))
    {
    data[i].freq=0;
    data[i++].dur =atoi(strtok(NULL," \n"));
    }
  }

fp.close();

int numnotes=i;
tm=time(NULL);

for(;;)
 {
 for(i=0;i<numnotes;i++)
  {
  if(KB.hit())
     {
     KEY c=KB.uget();
     if(hotkeys) for(j=0;j<strlen(hotkeys);j++) if(c==hotkeys[j]) { nosound(); delete [] data; return c; }
     }
  if(length && int(time(NULL)-tm)>length) { nosound(); delete [] data; return 0; }
  if(!noyell_flag)
    {
     if(data[i].freq)
       {
        sound(data[i].freq);
        msleep(data[i].dur*10);
       }
      else
       {
        nosound();
        msleep(data[i].dur*10);
       }
    }
  }
 if(!length) break;
 }
nosound();

delete [] data;

return 0;
}

