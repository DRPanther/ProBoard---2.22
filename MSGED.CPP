#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <tswin.hpp>
#include "proboard.hpp"

#define LINELEN 70

class msglines
   {
   String *line;
   int n;
   void rebuild();
   void enterline(String&,String&);
   void ruler();
  public:
   msglines() { line=new String[60]; n=0; }
   ~msglines() { delete [] line; n=0; }
   String& operator[](int i) { return line[i-1]; }
   int numlines() { return n; }
   void append();
   void save();
   void insert(int l);
   void del(int l1,int l2);
   void edit(int l);
   void replace(int l,char *str1,char *str2);
   void show();
   void show(int l);
   };

void
msged()
{
msglines msg;

unlink("MSGTMP");

if(run_sdkfile("/I MSGED") >= 0) return;

io << "\n\n" << S_MSGED_HEADER << "\n\n";

msg.append();

for(;;)
  {
   io << '\n' << S_MSGED_COMMAND_LIST;
   char k= wait_language_hotkeys(K_MSGED_COMMAND_LIST);

   switch(k)
    {
    case 5  : {
              io << S_MSGED_LIST << '\n';
              msg.show();
              } break;
    case 0  : {
              io << S_MSGED_SAVE;
              msg.save();
              return;
              }
    case 6  : {
              io << S_MSGED_ASK_ABORT;
              if(io.ask(0)) return;
              io << "\n\n";
              } break;
    case 1  : {
              io << S_MSGED_CONTINUE << '\n';
              msg.append();
              } break;
    case 7  : {
              int line=0;
              io << S_MSGED_EDIT_LINE(form("%d",msg.numlines()));

              io.read(line,2);
              io << '\n';
              if(line<1 || line>msg.numlines()) break;
              io << '\n';
              msg.edit(line);
              io << '\n';
              } break;
    case 3  : {
              int line1=0,line2=0;
              io << S_MSGED_DELETE_LINE << "\n\n";
              io << S_MSGED_DELETE_FROM_LINE(form("%d",msg.numlines()));
              io.read(line1,2);
              io << '\n';
              if(line1<1 || line1>msg.numlines()) break;
              io << S_MSGED_DELETE_TO_LINE(form("%d",line1),form("%d",msg.numlines()));
              io.read(line2,2);
              io << '\n';
              if(line2<line1 || line2>msg.numlines()) break;
              msg.del(line1,line2);
              io << '\n' << S_MSGED_X_LINES_DELETED(form("%d",line2-line1+1)) << '\n';
              } break;
    case 4  : {
              int line=0;
              io << S_MSGED_INSERT_LINE(form("%d",msg.numlines()));
              io.read(line,2);
              io << '\n';
              if(line<1 || line>msg.numlines()) break;
              io << '\n';
              msg.insert(line);
              } break;
    case 2  : {
              int line=0;
              char text1[50],text2[50];

              io << S_MSGED_REPLACE_TEXT(form("%d",msg.numlines()));
              io.read(line,2);
              io << '\n';
              if(line<1 || line>msg.numlines()) break;
              io << '\n';
              msg.show(line);
              io << '\n' << S_MSGED_ENTER_TEXT_TO_REPLACE;
              io.read(text1,49);
              if(!text1[0])
                  {
                  io << '\n';
                  break;
                  }
              if(msg[line].find(text1)<0)
                  {
                     io << "\n\n" << S_MSGED_TEXT_NOT_FOUND << '\n';
                     break;
                  }
              io << "\n\n" << S_MSGED_REPLACE_BY;
              io.read(text2,49);
              msg.replace(line,text1,text2);
              io << "\n\n" << S_MSGED_LINE_NOW_READS << "\n\n";
              msg.show(line);
              } break;
    }
  }
}

void
msglines::save()
{
if(n<1) return;

File f("MSGTMP",fmode_create | fmode_text);

for(int i=0;i<n;i++) f << line[i];

if(line[n-1][line[n-1].len()-1]!='\n') f << '\n';
f << '\n';
}

void
msglines::ruler()
{
io << "\n\2 [##] \7[-----------------------------------------------------------------------]\n\n";
}

void
msglines::append()
{
int i=n?n-1:0;

strip_linefeed(line[i]);
String wrap;

ruler();

while(i<60)
  {
  io << form(" \3[\7%2d\3]:\6 ",i+1);
  enterline(line[i],wrap);
  if(line[i][0]=='\n') { line[i]=""; break; }
  i++;
  if(i<60) line[i]=wrap;
  wrap="";
  }
n=i;
}

void
msglines::insert(int l)
{
ruler();

String wrap,tmpline;

while(n<60)
  {
  io << form(" \3[\7%2d\3]:\6 ",l);
  wrap="";
  enterline(tmpline,wrap);
  if(tmpline[0]=='\n') break;
  for(int i=n;i>=l;i--) line[i]=line[i-1];
  line[l-1]=tmpline;
  tmpline=wrap;
  n++;
  l++;
  }
}

void
msglines::del(int l1,int l2)
{
l1--;
l2--;

for(int i=l1;i<n-l2+l1-1;i++) line[i]=line[i+l2-l1+1];
n-=l2-l1+1;
if(!n) line[0]="";
}

void
msglines::edit(int l)
{
String wrap("X");
int LF=0;

if(line[l-1][line[l-1].len()-1]=='\n') LF=1;

strip_linefeed(line[l-1]);

ruler();

io << form(" \3[\7%2d\3]:\6 ",l);

enterline(line[l-1],wrap);

if(!LF) line[l-1][line[l-1].len()-1]=' ';
}

void
msglines::replace(int l,char *s1,char *s2)
{
l--;
int i=line[l].find(s1);
if(i<0) return;
line[l].del(i,strlen(s1));
line[l].insert(i,s2);

rebuild();
}

void
msglines::rebuild()
{
String tmpline[60];

for(int i=0,l=0;i<n;i++)
  {
  String wrap;
  for(int j=0;j<line[i].len();j++)
    {
    tmpline[l] << line[i][j];
    if(line[i][j]=='\n') l++;
    if(tmpline[l].len()>LINELEN)
      {
      wordwrap(tmpline[l],wrap,LINELEN);
      tmpline[++l]=wrap;
      }
    }
  }
for(i=0,n=-1;i<60;i++)
   {
   line[i]=tmpline[i];
   if(n<0) if(!line[i][0]) n=i;
   }
}

void
msglines::show(int l)
{
String s(line[l-1]);
strip_linefeed(s);
io << form("\3[\7%2d\3]:\6 %s\n",l,(char *)s);
}

void
msglines::show()
{
io << '\n';
stopped=0;
linecounter(0);
for(int i=1;i<=n;i++)
  {
  show(i);
  if(linecounter() || stopped) break;
  }
}

void
msglines::enterline(String& s,String& wrap)
{
int nowrap=0;
if(wrap.len()) nowrap=1;
int i;

if(!nowrap) wrap="";

for(i=0;i<s.len();i++) io << s[i];

for(;;)
  {
  char k=io.wait();
  if(k==27 || k==7) continue;
  if(k==9) k=' ';

  switch(k)
    {
    case 8: if(s.len()>0)
              {
              s[s.len()-1]=0;
              io << "\b \b";
              }
             break;
    case 13: s << '\n'; io << '\n'; wrap=""; return;
    default: if(nowrap && s.len()>LINELEN) continue;
             io << k;
             s << (char)k;
    }
  if(s.len()>LINELEN)
    {
    int l=wordwrap(s,wrap,LINELEN);
    for(int i=0;i<l;i++) io << "\b \b";
    io << '\n';
    return;
    }
  }
}

