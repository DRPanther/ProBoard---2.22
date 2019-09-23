#include <string.h>
#include "proboard.hpp"

void
command_stack::parse(char *s)
{
 int l = strlen(s);

 if(!l) return;

 sp = l;
 if(sp>100) sp = 100;

 for(int i=sp-1;i>=0;i--,s++) buf[i]=(*s==';')?'\r':(*s);
}

char
command_stack::getnext()
{
 if(!sp) return 0;
 return buf[--sp];
}

char
command_stack::pollnext()
{
 if(!sp) return 0;
 return buf[sp-1];
}
