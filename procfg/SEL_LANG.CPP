#include <string.h>
#include "procfg.hpp"

static String fn_languages   [100];
static String desc_languages [100];

static int num_languages;
static int show_def;

void
list_func(int i , int x , int y)
{
   if(show_def && i==0)
   {
      tsw_maputs( x, y , 0x5F , "          ³ (default)");
   }
   else
   {
      if(show_def)
         i--;

      tsw_maputs( x, 
                  y , 
                  0x5F , 
                  form(" %-8s ³ %-38.38s",(char *)fn_languages[i],(char *)desc_languages[i]));
   }

   tsw_maputc(x+10,y,0x5D,'³');
}

void
line_clear(int x,int y)
{
   tsw_maputc(x+10,y,0x5D,'³');
}

String
select_language( bool edit )
{
   SelectWindow sw;

   sw.open( 3,
            3,
            56,
            tsw_vsize - 3,
            0x5E,
            SHADOW | BRACKETS,
            CHISEL_BORDER,
            0x5D,
            NULL,
            0x51 );

   sw.title( edit 
               ? "Select language to edit"
               : "Select language for menus",
               0x5E );

   DirScan scan(FileName(syspath,"*.PBL"));

   for(num_languages=0;int(scan);scan++)
   {
      File f;

      if(f.open(FileName(syspath,scan.name())))
      {
         LanguageFile *lf = new LanguageFile;

         f.read(lf,sizeof(*lf));

         FileName fn(scan.name());
         fn.stripExt();

           fn_languages[num_languages] = fn;
         desc_languages[num_languages] = lf->name;

         f.close();

         if(fn != "_DEFAULT" && (lf->menuPath[0] || edit))
            num_languages++;

         delete lf;
      }
   }

   int i,j;

   for(i=0;i<num_languages-1;i++)
      for(j=i+1;j<num_languages;j++)
      {
         if(stricmp(desc_languages[i],desc_languages[j]) > 0)
         {
            String tmp;

            tmp               = desc_languages[i];
            desc_languages[i] = desc_languages[j];
            desc_languages[j] = tmp;

            tmp             = fn_languages[i];
            fn_languages[i] = fn_languages[j];
            fn_languages[j] = tmp;
         }
      }

   if(num_languages == 0 && !edit)
      return "*";

   KEY hotkeys[] = { 0 };

   sw.define( num_languages + (edit ? 0:1),0x70,list_func,0,NULL,hotkeys,NULLFUNC,line_clear);

   sw.direct(11,0,0x5D, 'Â');
   sw.direct(11 ,tsw_vsize-6,0x51, 'Á');

   show_def = !edit;

   CursorState _cursor(FALSE);

   for(;;)
   {
      i = sw.process();

      if(i<0)
         break;

      if(edit)
         edit_language(fn_languages[i]);
      else
         return (i ? (char *)fn_languages[i-1] : "*");
   }

   return "";
}
