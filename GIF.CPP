#include <string.h>
#include "proboard.hpp"

struct gif_header
   {
      char  sign[6];
      word  xres;
      word  yres;
      word  colorbits : 3;
      sword dummy     : 5;
   };

void
view_gif(char *data)
{
   char gifname[13];

   io << "\f\n" << S_SHOW_GIF_INFO_TITLE_AND_PROMPT;

   io.read(gifname,12,READMODE_UPALL);
   if(!gifname[0]) return;

   String param[40];
   BitArray arealist(MAX_FILE_AREAS,1);

   int npara = parse_data(data,param);

   create_arealist(param,npara,arealist);

   FileArea fa;

   io << form("\n\n\7 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ¿\n"
                    " ³\3 %-13.13s\7³\3 %-11.11s\7³\3 %-7.7s\7³\n"
                    " ÆÍÍÍÍÍÍÍÍÍÍÍÍÍÍØÍÍÍÍÍÍÍÍÍÍÍÍØÍÍÍÍÍÍÍÍµ\n",
               S_SHOW_GIF_FILENAME   ,
               S_SHOW_GIF_RESOLUTION ,
               S_SHOW_GIF_COLORS     );

   linecounter(2);
   stopped=0;
   io.enablestop();

   for(int i=1;i<=FileArea::highAreaNum();i++)
   {
      if(!arealist[i] || !fa.read(i) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;

      String fname(fa.filepath);
      fname << gifname;

      DirScan f(fname);

      while(int(f))
      {
         File fil;

         if(fil.open(FileName(fa.filepath,f.name())))
         {
            gif_header gif;
            fil.read(&gif,sizeof(gif));

            if(!strncmp(gif.sign,"GIF",3))
            {
               io << form(" ³ \3%-12s\7 ³",f.name());
               io << form("\6 %4d x %3d \7³\2   %3d  \7³\n",gif.xres,gif.yres,1 << (gif.colorbits+1));

               if(stopped || linecounter())
               {
                  i = FileArea::highAreaNum();

                  break;
               }
            }
         }

         ++f;
      }
   }

   if(!stopped)
      io << " ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍ¾\n\n"
         << S_PRESS_ENTER_TO_CONTINUE;
}

