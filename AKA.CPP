#include <string.h>
#include "proboard.hpp"

int
aka::read(int n)
{
   File f(FileName(syspath,"AKA.PRO"));

   if(!f.opened())
   {
      CLEAR_OBJECT(*this);

      return -1;
   }

   CLEAR_OBJECT(*this);

   f.seek(long(n)*sizeof(aka));

   if(f.read(this,sizeof(aka)) != sizeof(aka))
   {
      CLEAR_OBJECT(*this);

      return -1;
   }
   return 0;
}

