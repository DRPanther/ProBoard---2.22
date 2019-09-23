#include <ctype.h>
#include <stdlib.h>
#include "proboard.hpp"

void
tops(char *data)
{
     char *headers[14];

     headers[ 0] = S_TOPS_TOP_CALLERS               ;
     headers[ 1] = S_TOPS_TOP_CALLERS_UNIT          ;
     headers[ 2] = S_TOPS_TOP_DOWNLOADERS_KB        ;
     headers[ 3] = S_TOPS_TOP_DOWNLOADERS_KB_UNIT   ;
     headers[ 4] = S_TOPS_TOP_DOWNLOADERS_TIMES     ;
     headers[ 5] = S_TOPS_TOP_DOWNLOADERS_TIMES_UNIT;
     headers[ 6] = S_TOPS_TOP_UPLOADERS_KB          ;
     headers[ 7] = S_TOPS_TOP_UPLOADERS_KB_UNIT     ;
     headers[ 8] = S_TOPS_TOP_UPLOADERS_TIMES       ;
     headers[ 9] = S_TOPS_TOP_UPLOADERS_TIMES_UNIT  ;
     headers[10] = S_TOPS_TOP_MSGWRITERS            ;
     headers[11] = S_TOPS_TOP_MSGWRITERS_UNIT       ;
     headers[12] = S_TOPS_TOP_ONLINE                ;
     headers[13] = S_TOPS_TOP_ONLINE_UNIT           ;

     topentry *tp=new topentry[20];

     int type=0;
     switch(toupper(data[0]))
     {
     case 'C': type=0; break;
     case 'K': type=1; break;
     case 'T': type=2; break;
     case 'U': type=3; break;
     case 'F': type=4; break;
     case 'M': type=5; break;
     case 'O': type=6; break;
     default : return;
     }
     int num=atoi(&data[1]);
     if(!num) num=10;


     //----------------------------  
     // JDR: REGISTRATION REFERENCE
     //----------------------------  

     if(!registered) num=5;

     File f(FileName(syspath,"TOPS.PB"));
     if(!f.opened()) return;

     f.seek(long(type)*sizeof(topentry)*20);
     f.read(tp,sizeof(topentry)*20);

     linecounter(3);
     io.enablestop();
     stopped=0;

     LOG(2,"Tops requested : %s",headers[type*2]);

     io << "\n\f\7�������������������������������������������ķ\n";
     io << form("�\3 Pos \7�\6 %-26s\7�\6 %7s \7�\n",headers[type*2],headers[type*2+1]);
     io << "�������������������������������������������Ķ\n";
     for(int i=0;i<num;i++)
     {
     if(stopped || !tp[i].n) break;
     io << form("� \6%3d \7�\3 %-25.25s \7�\5",i+1,tp[i].name);
     if(type==6) io << form("%3ldh %02ld' \7�\n",tp[i].n/60L,tp[i].n%60L);
          else    io << form(" %6lu  \7�\n",tp[i].n);
     if(linecounter()) break;
     }

     if(!stopped) io << "�������������������������������������������Ľ\n"
                    "\n" << S_PRESS_ENTER_TO_CONTINUE;

     delete [] tp;

     io.disablestop();
}

