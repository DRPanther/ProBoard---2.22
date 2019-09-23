#include <ctype.h>
#include <string.h>
#include "proboard.hpp"

bool
select_protocol(protocol& p , char protocol_key , bool seldef)
{
   String hotkeys;
   String s;

   if(protocol_key == '\0')
   {
      s << S_PROTOCOL_MENU_TITLE << "\n\n";

      for(int i=1 ; p.read(i) ; i++)
      {
         if(p.flags&PROT_ENABLED && !(io.baud>0 && (p.flags&PROT_LOCAL)))
         {
            s << form("\6<\3%c\6>\7 %s\n",p.key,p.name);

            hotkeys << p.key;
         }
         else
         {
            hotkeys << '-';
         }
      }

      s << "\n\6<\3-\6> " << (seldef ? S_NO_DEFAULT_PROTOCOL : S_PROTOCOL_MENU_GOBACK)
        << "\n\n"
        << S_PROTOCOL_MENU_PROMPT;

      hotkeys << '-';

      char k=io.send(s,hotkeys);

      io.show_remote = TRUE;

      if(k==1) return FALSE;

      if(!k) k=io.wait(hotkeys);

      if(k=='-')
         return FALSE;

      for(i=0;i<hotkeys.len();i++)
         if(k==hotkeys[i])
         {
            p.read(i+1);
            break;
         }

      io << "\6" << p.name << '\n';

      rip_reset();

      return TRUE;
   }
   else
   {
      for(int i=1 ; p.read(i) ; i++)
         if(toupper(p.key) == toupper(protocol_key))
            return TRUE;

      return FALSE;
   }
}

void
change_default_protocol(char *)
{
   protocol p;

   if(user.defaultProtocol == '\0' || !select_protocol(p,user.defaultProtocol))
      strcpy(p.name,S_NO_DEFAULT_PROTOCOL);

   io << "\n\f\n" << S_CHANGE_PROTOCOL_TITLE(p.name) << "\n\n";

   if(select_protocol(p,0,TRUE))
      user.defaultProtocol = p.key;
   else
      user.defaultProtocol = 0;
}

bool
protocol::read(int n)
{
   File f;

   if(!f.open(FileName(syspath,"PROTOCOL.PRO"))) return -1;

   f.seek(long(n-1)*sizeof(protocol));

   if(f.read(this,sizeof(protocol))!=sizeof(protocol)) return FALSE;

   return TRUE;
}
