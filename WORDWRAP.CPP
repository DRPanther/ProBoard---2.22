#include <string.h>
#include "proboard.hpp"

int
wordwrap(char *s,String& str,int max)
{
   int spacepos=-1;

   if(strlen(s)<=max) return 0;

   for(int i=0;i<strlen(s) && i<max;i++)
      if(s[i]==' ' || s[i]=='-')
         spacepos = i;

   if(spacepos<0)
   {
      str    = &s[max];
      s[max] = 0;
   }
   else
   {
      str = &s[spacepos+1];
      s[spacepos + 1]=0;
   }

   return str.len();
}

