#include <stdio.h>
#include "procfg.hpp"

BitArray *gn;

static void
show_node(int n,int x,int y)
{
   char str[20];

   sprintf(str,"[ ] Node #%-3d",n+1);

   if((*gn)[n+1])
      str[1] = 'û';

   tsw_mputs(x+1,y,str);
}

KEY
edit_nodes(int mode,Window& ww,int x,int y,char attr,void *data)
{
   int i;
   int totalcleared = 0;
   byte *arr = (byte *)data;
   BitArray nodes(255,1);

   gn = &nodes;

   for(i=1;i<=cfg.numnodes;i++)
      nodes.set(i);

   for(i=0;i<cfg.numnodes;i++)
   {
      if(arr[i/8] & (1 << (i & 7)))
      {
         nodes.clear(i+1);
         totalcleared++;
      }
   }

   if(!mode)
   {
      String str;

      if(!totalcleared)
      {
         str = "All nodes";
      }
      else
      {
         if(totalcleared == cfg.numnodes)
         {
            str = "None";
         }
         else
         {
            for(i=1 ; i<=cfg.numnodes;i++)
               if(nodes[i])
               {
                  if(str.len() > 16)
                  {
                     str.delLast();
                     str << "....";
                     break;
                  }
                  else
                  {
                     str << form("%d,",i);
                  }
               }

            str.delLast();
         }
      }

      ww.direct(x,y,attr,form("%-20.20s",(char *)str));

      return 0;
   }


   int starty = ww.minY+y-cfg.numnodes/2;

   if(starty<2)
      starty = 2;

   int endy = starty + cfg.numnodes+1;

   if(endy > 22)
      endy = 22;

   SelectWindow w(cfg.numnodes,0x70,show_node);

   w.open(ww.minX+x-6,starty,ww.minX+x+14,endy,0x3F,SHADOW);
   //w.title(" Edit Color ",0xE);

   tsw_cursoroff();
   for(;;)
   {
      int x = w.process();

      if(x < 0)
         break;

      if(nodes[x+1])
         nodes.clear(x+1);
      else
         nodes.set(x+1);
   }

   tsw_cursoron();

   for(i=0;i<32;i++)
      arr[i] = 0;

   for(i=1;i<=cfg.numnodes;i++)
      if(!nodes[i])
         arr[(i-1)/8] |= (1 << ((i-1) & 7));

   return 0;
}
