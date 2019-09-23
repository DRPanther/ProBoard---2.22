#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "proboard.hpp"

LanguageFile    pb_curlang;
LanguageString *pb_lang;
int pb_num_lang = 0;

static void
KillLanguage()
{
   if(pb_lang)
   {
      delete [] pb_lang;
   }

   pb_lang     = NULL;
   pb_num_lang = 0;
}

bool
ReadLanguage( char *langname )
{
   if(langname == NULL)
   {
      KillLanguage();

      return FALSE;
   }

   FileName fn(syspath , langname , ".PBL");
   File f;

   if(!f.open(fn , fmode_read , 4096))
   {
      LOG("ERROR: Unable to read language file %s",(char *)fn);

      return FALSE;
   }

   if(f.read(&pb_curlang , sizeof(LanguageFile)) != sizeof(LanguageFile))
   {
      LOG("Error reading language file <%s>" , (char *)fn);

      return FALSE;
   }

   if(strlen(pb_curlang.menuPath) < 3)
      pb_curlang.menuPath[0] = '\0';

   if(strlen(pb_curlang.txtPath) < 3)
      pb_curlang.txtPath[0] = '\0';

   if(strlen(pb_curlang.questPath) < 3)
      pb_curlang.questPath[0] = '\0';

   if(pb_curlang.menuPath[0])
      append_backspace(pb_curlang.menuPath);

   if(pb_curlang.txtPath[0])
      append_backspace(pb_curlang.txtPath);

   if(pb_curlang.questPath[0])
      append_backspace(pb_curlang.questPath);

   KillLanguage();

   f >> pb_num_lang;

   pb_lang = new LanguageString[pb_num_lang];

   for(int i=0;i<pb_num_lang;i++)
   {
      if(!pb_lang[i].read(f)) break;

      String str;

      if(!(pb_lang[i].flags & LANG_NOCOLOR))
         str << char(26) << char(pb_lang[i].color);

      char *s = pb_lang[i].s;

      bool high = FALSE;

      bool akrol_found = FALSE;
      bool pex_found   = FALSE;
      bool ans_found   = FALSE;
      bool last_was_ansi=FALSE;

      String ans_pex_name;

      for(;*s;s++)
      {
         last_was_ansi = FALSE;

         if(akrol_found)
         {
            if(*s != 'p' && *s != 'a')
            {
               str << '@' << *s;
            }
            else
            {
               if(*s == 'p')
                  pex_found = TRUE;
               else
                  ans_found = TRUE;

               ans_pex_name = "";
            }

            akrol_found = FALSE;

            continue;
         }

         if(pex_found || ans_found)
         {
            if(*s == '@')
            {
               str << (pex_found ? char(24):char(25)) << char(ans_pex_name.len()) << ans_pex_name;

               pex_found = ans_found = FALSE;

               last_was_ansi = TRUE;
            }
            else
            {
               ans_pex_name << *s;
            }

            continue;
         }

         if(*s == '^')
         {
            high = !high;

            if(high)
               str << char(26) << char(pb_lang[i].highlight);
            else
               str << char(26) << char(pb_lang[i].color);
         }
         else
         {
            if(*s == '\\')
            {
               char c = *(++s);

               if(c == '\\' || c == '^')
               {
                  str << c;

                  continue;
               }
               else
               {
                  byte col = 0;

                  if(isxdigit(c))
                  {
                     for( int i = 0; i < 2 ; i++ , s++)
                     {
                        c = *s;

                        if(isdigit(c))
                           col = col * 16 + c - '0';
                        else
                           col = col * 16 + toupper(c) - 'A' + 10;
                     }

                     str << char(26) << char(col);

                     s--;

                     continue;
                  }

                  if(c == 'H' || c == 'L')
                  {
                     switch(*(++s))
                     {
                        case 'B': col = 1;
                                 break;
                        case 'G': col = 2;
                                 break;
                        case 'C': col = 3;
                                 break;
                        case 'R': col = 4;
                                 break;
                        case 'P': col = 5;
                                 break;
                        case 'Y': col = 6;
                                 break;
                        case 'W': col = 7;
                                 break;
                     }

                     if(c == 'H') col |= 8;
                  }

                  str << char(26) << char(col);

                  continue;
               }
            }

            if(*s == '@')
            {
               akrol_found = TRUE;
               continue;
            }

            str << (*s);
         }
      }

#ifdef DEBUG
      if(str.find("\x11")>0)
         LOG("s = '%s'",(char *)str);
#endif

      if(pb_lang[i].flags & LANG_PROMPT)
      {
         if(!last_was_ansi)
            str << ' ';

         str << char(26) << char(pb_lang[i].promptcolor);
      }

      delete [] pb_lang[i].s;

      pb_lang[i].len = str.len();
      pb_lang[i].s   = new char[ pb_lang[i].len + 1 ];

      strcpy(pb_lang[i].s , str);
   }

   f.close();

   S_PRESS_ENTER_TO_CONTINUE = _S_PRESS_ENTER_TO_CONTINUE;
   S_PRESS_ENTER_TO_CONTINUE << '\t';

   return TRUE;
}

char
wait_language_hotkeys(char *hk, bool enter)
{
   String hotkeys;

   hotkeys = hk;
   if(enter)
      hotkeys << '\r';

   char k = io.wait(hotkeys);

   k = hotkeys.find(String(char(k)));

   if(hotkeys[k] == '\r')
      return '\r';
   else
      return k;
}

int
language_string_length( char *str )
{
   int l = 0;

   for( ; *str ; str++)
   {
      char c = *str;

      if(c == char(26))
      {
         str++;
         continue;
      }

      l++;
   }

   return l;
}
