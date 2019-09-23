#include <string.h>
#include "proboard.hpp"

const int MAX_LANGUAGES = 20;

void
select_language(char *data)
{
   if(data != NULL)
      io << "\n\f\n";

   io << S_SELECT_LANGUAGE_TITLE << "\n\n";

   FileName *languages = new FileName[20];

   DirScan scan(FileName(syspath,"*.PBL"));

   for(int i = 0; int(scan) ; scan++ , i<MAX_LANGUAGES)
   {
      if(String(scan.name()) == "_DEFAULT.PBL")
         continue;

      File f(FileName(syspath,scan.name()));

      if(f.opened())
      {
         LanguageFile lf;

         if(f.read(&lf , sizeof(LanguageFile)) != sizeof(LanguageFile))
            continue;

         if(!check_access(lf.level,lf.flags))
            continue;

         if(!lf.avail)
            continue;

         io << form("\7 %3d. \6%s\n",i+1,lf.name);

         languages[i] = scan.name();
         languages[i++].stripExt();
      }
   }

   io << '\n' << S_SELECT_LANGUAGE;

   int num=0;

   io.read(num,2);

   if(num == 0)
      return;

   if(num >= 1 && num <= i)
   {
      if(data)
         io << "\n\n" << S_READING_LANGUAGE_FILE;

      if(!ReadLanguage( languages[num-1] ))
         if(!ReadLanguage( cfg.defaultLanguage ))
            fatalerror("Error reading default language file");

      strcpy(user.language,languages[num-1]);

      readconfig();
   }
   else
   {
      if(num)
         io << "\n\n" << S_INVALID_ENTRY;
   }

   if(num && data)
      io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

   delete [] languages;
}
