#define Use_TagList
#define Use_LinkedList

#include <stdlib.h>
#include <string.h>
#include "proboard.hpp"

void
edit_taglist(char *)
{
   int i;

   if(taglist.count() == 0)
   {
      io << "\n\n\6No files tagged!  " << S_PRESS_ENTER_TO_CONTINUE;

      return;
   }

   for(;;)
   {
      io << "\n\f" << S_EDIT_TAGGED_TITLE << "\n\n";

      for(i = 1 , taglist.rewind() ; !taglist.eol() ; taglist++,i++)
      {
         FileArea fa;

         fa.read(taglist.get().area);

         io << form("    \3%2d\7 - \6%-12s\7 \2 (%s)\n",i,taglist.get().name,fa.name);
      }

      io << '\n' << S_EDIT_TAGGED_PROMPT;

      char rep = wait_language_hotkeys(K_EDIT_TAGGED_PROMPT);

      if(rep == 2) break;

      if(rep == 0)
      {
         while(taglist.count())
         {
            taglist.rewind();
            taglist.remove();
         }

         io <<        K_EDIT_TAGGED_PROMPT[0] << "\n\n" << S_ALL_FILE_TAGS_CLEARED
            << ' ' << S_PRESS_ENTER_TO_CONTINUE;
      }

      if(rep == 1)
      {
         char s[4];

         io << K_EDIT_TAGGED_PROMPT[1] << "\n\n" << S_ENTER_FILE_TO_UNTAG;

         io.read(s,3,READMODE_DIGITS);

         int n = atoi(s);

         if(n > 0 && n <= taglist.count())
         {
            for(taglist.rewind() , i=1 ; i<n ; i++) taglist++;

            taglist.remove();
         }
      }
   }
}

void
remove_tag(char *name , int area)
{
   for(taglist.rewind() ; !taglist.eol() ; taglist++)
   {
      if(!stricmp(taglist.get().name,name) && taglist.get().area == area)
      {
         taglist.remove();
         break;
      }
   }
}

bool
is_tagged(char *name , int area)
{
   for(taglist.rewind() ; !taglist.eol() ; taglist++)
   {
      if(!stricmp(taglist.get().name,name) && taglist.get().area == area)
         return TRUE;
   }

   return FALSE;
}
