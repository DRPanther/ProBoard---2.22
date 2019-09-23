#include <string.h>
#include "proboard.hpp"

void
bulletin(char *data)
{
   char fname[9];

   for(int i=0;data[i]==' ';i++) {}

   char *s = &data[i];
   for(;data[i] && data[i]!=' ';i++) {}
   data[i] = '\0';

   String prompt;

   for(i++;data[i]==' ';i++) {}

   if(data[i])
      prompt = &data[i];
   else
      prompt = S_ENTER_BULLETIN;

   for(;;)
   {
      strcpy(fname,s);

      io << '\n';

      showansasc(fname);

      io << "\n\7" << prompt;

      io.read(&fname[strlen(fname)],8-strlen(fname),READMODE_UPALL);

      if(strlen(fname) <= strlen(s)) break;

      if(showansasc(fname,NULL) == ANS_NOFILE)
         io << "\n\n" << S_FILE_NOT_FOUND << ' ' << S_PRESS_ENTER_TO_CONTINUE;
      else
         io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
   }
}

