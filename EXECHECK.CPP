#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "proboard.hpp"

#pragma warn -rch

const long CHECKFREQ = 50;

struct
  {
   char string[15];
   dword checksum;
  } execheck = { "#`&$%#$%@%&^%E",0L };

void
check_exe(char */*fname*/)
{
/*
 File f;
 bool first_time = FALSE;
 dword checksum = 0L;
 long offset;

// #ifndef RELEASE
  return;
// #endif

#if __OVERLAY__
 return;
#endif

 if(!execheck.checksum)
   {
    int c;

    if(!f.open(fname,fmode_rw,512)) exit(ERRLVL_FATALERR);

    first_time = TRUE;

    for(;;)
      {
       c = f.readByte();
       if(c<0) exit(ERRLVL_FATALERR);
       if(char(c) == execheck.string[0])
         {
          char buf[14];
          f.read(buf,13);
          buf[13] = '\0';
          if(!strcmp(buf,&execheck.string[1]))
            {
             offset = f.pos() + 1;
             break;
            }
          f.seek(-12,seek_cur);
         }
      }
   }

SCRAMBLE();

if(clockticks()%CHECKFREQ && !first_time) return;

if(!first_time) if(!f.open(fname,fmode_read,1024)) exit(ERRLVL_FATALERR);

f.rewind();

for(;;)
  {
   int c = f.readByte();
   if(c<0) break;
   checksum += byte(c);

   SCRAMBLE();
  }

if(first_time)
  {
   f.seek(offset);
   f.write(&checksum,4);
  }
 else
  {
   byte *p = (byte *)&execheck.checksum;

   for(int i=0;i<sizeof(execheck.checksum);i++) checksum -= p[i];

   SCRAMBLE();

   if(checksum != execheck.checksum)
     {
      LOG("PROBOARD.EXE CRC ERROR");
      exit(ERRLVL_FATALERR);
     }
  }
*/
}
