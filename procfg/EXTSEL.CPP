#ifdef PEX
  #define NO_KEY_DEF
  #include <pb_sdk.h>
#else
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
#endif

#include <tswin.hpp>
#include "extsel.hpp"

#define max(a,b) ( (a)>(b) ? (a) : (b) )
#define min(a,b) ( (a)<(b) ? (a) : (b) )

void
ExtSelectWindow::recalc(int order)
{
   int i,y;
   int height = maxY - minY + 1;

   if(end >= numEntries)
   {
      end = numEntries-1;

      for(i=1,y=height - (*func)(*this,end) ; y>0 ; i++)
      {
         y -= (*func)(*this,end-i);
      }

      start = end - i + 1;

      for(i = start ; y < height; i++)
      {
         offset[i-start] = y;

         y += (*func)(*this,i);
      }
   }
   else
   {
      for(i = start , y = 0; y < height; i++)
      {
         if(i >= numEntries)
         {
            end = numEntries;
            recalc();
            return;
         }
         offset[i-start] = y;

         y += (*func)(*this,i);
      }

      end = i-1;
   }
}

void
ExtSelectWindow::scrollUp(int n)
{
   end++;

   int left = offset[0] + (*func)(*this,start);  // # lines of 1st item on screen

   int new_offset = offset[0];

   int i = n;

   while( i >= left)
   {
      new_offset += (*func)(*this,start);
      start++;
      i -= (*func)(*this,start);
   }

   offset[0] = new_offset - n;

   for(i = start+1 ; i <= end ; i++)
   {
      offset[i-start] = offset[i-start-1] + (*func)(*this,i-1);
   }
}

void
ExtSelectWindow::scrollDown(int)
{
   start--;
   recalc();
}


void
ExtSelectWindow::redraw(int n)
{
   if(n >= 0)
   {
      (*func)(*this,n,offset[n-start] + 1,maxY - minY - offset[n-start] + 1);
   }
   else
   {
      clear();

      for(int i=start; i <= end ; i++)
      {
         (*func)(*this,i,offset[i-start] + 1,maxY - minY - offset[i-start] + 1);
      }
   }
}

void
ExtSelectWindow::keyDown()
{
   if(current >= numEntries-1)
      return;

   previous = current++;

   int height = (maxY-minY+1);

   if(   current == end
      && offset[end-start]+(*func)(*this,current) > height)
   {
      int n = offset[end-start] + (*func)(*this,current) - height;

      tsw_scroll(UP,minX,minY,maxX,maxY,n,attr);

      scrollUp(n);

      redraw(current);
   }

   if(current > end)
   {
      int n = (*func)(*this,current);

      tsw_scroll(UP,minX,minY,maxX,maxY,n,attr);
      scrollUp(n);

      redraw(current);
   }
}

void
ExtSelectWindow::keyUp()
{
}

int
ExtSelectWindow::process()
{
   previous = current;

   if(!end)
   {
      recalc();
      redraw();
   }


   for(;;)
   {
      int ofs;

      if(previous >= start && previous <= end)
      {
         ofs = offset[previous-start];
         tsw_selbar(max(ofs,0)+minY,minX,maxX,attr,(*func)(*this,previous)+((ofs < 0)?ofs:0));
         redraw(previous);
      }

      ofs = offset[current-start];

      tsw_selbar(max(ofs,0)+minY,minX,maxX,barAttr,(*func)(*this,current)+((ofs < 0) ? ofs:0));


      if(action!=NULLFUNC && numEntries) (*action)(current);
      KB.clear();
      KEY k = KB.uget();

      if(hotKeys!=NULL)
      for(int i=0 ; hotKeys[i] ; i++)
         if(k == hotKeys[i])
         {
            hotkey = k;
            return SL_HOTKEY;
         }

      if(k == KEY_ESC)
      {
         return SL_ESC;
      }

      if(k == 13)
      {
         return current;
      }

      switch(k)
      {
         case KEY_DN: keyDown();
                      break;
         case KEY_UP:
            {
               if(current <= 0)
                  break;

               previous = current--;

               if(current == start && offset[0] < 0)
               {
                  tsw_scroll(DOWN,minX,minY,maxX,maxY,-offset[0],attr);
                  recalc();
                  redraw(current);
               }

               if(current < start)
               {
                  int n = (*func)(*this,current);

                  tsw_scroll(DOWN,minX,minY,maxX,maxY,n,attr);
                  scrollDown(n);

                  redraw(current);
               }
            }
            break;

         case KEY_PGUP:
            {
               if(start == 0)
               {
                  previous = current;
                  current = 0;
                  break;
               }

               int n = 0;
               int y = offset[current-start];

               for(int i = start;i>0 && n<(maxY-minY+1);i--)
               {
                  n += (*func)(*this,i);
               }

               start = i;
               recalc();

               current = start;

               for(i=start;i<=end;i++)
               {
                  if(offset[i-start] >= y)
                  {
                     current = i;
                     break;
                  }
               }

               redraw();
            }
            break;

         case KEY_PGDN:
            {
               if(end == numEntries-1)
               {
                  previous = current;
                  current  = end;
                  break;
               }

               int y = offset[current-start];

               start = end+1;
               recalc();

               previous = current;
               current = start;

               for(int i=start;i<=end;i++)
               {
                  if(offset[i-start] >= y)
                  {
                     current = i;
                     break;
                  }
               }

               redraw();
            }
            break;

         case KEY_HOME:
            {
               start = 0;
               current = previous = 0;
               recalc();
               redraw();
            }
            break;

         case KEY_END:
            {
               if(current == numEntries-1)
                  break;

               //start = numEntries-2;
               end = numEntries;
               current = numEntries-1;
               recalc();
               redraw();
            }
            break;
      }
   }
}

void
ExtSelectWindow::show(int n)
{
   start = n;
   current = previous = n;
   recalc();
   redraw();
}

ExtSelectWindow::ExtSelectWindow(int num,ATTR battr,int(*f)(ExtSelectWindow&,int,int,int),KEY *hot,void(*actionf)(int))
{
   barAttr    = battr;
   numEntries = num;
   hotKeys    = hot;
   func       = f;
   action     = actionf;

   current    = 0;
   previous   = 0;
   start      = 0;
   end        = 0;
}

void
ExtSelectWindow::define(int num,ATTR battr,int(*f)(ExtSelectWindow&,int,int,int),KEY *hot,void(*actionf)(int))
{
   barAttr    = battr;
   numEntries = num;
   hotKeys    = hot;
   func       = f;
   action     = actionf;

   current    = 0;
   previous   = 0;
   start      = 0;
   end        = 0;
}

void
ExtSelectWindow::reDefine(int num)
{
}
