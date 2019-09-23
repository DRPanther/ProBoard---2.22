#define Use_LinkedList

#include <dir.h>
#include <ctype.h>
#include <dos.h>
#include <stdlib.h>
#include "proboard.hpp"

const word copy_bufsize = 16384;

bool
copy_file( char *s , char *d )
{
   File fi,fo;

   if(!fi.open(s , fmode_read))   return FALSE;

   if(dos_getdiskfreespace(toupper(d[0])-'A'+1) < (fi.len() + copy_bufsize))
      return FALSE;

   if(!fo.open(d , fmode_create)) return FALSE;

   byte *buf = new byte[ copy_bufsize ];

   for(;;)
   {
      word copied = fi.read( buf, copy_bufsize );

      if(!copied) break;

      fo.write( buf , copied);

      if(copied != copy_bufsize) break;
   }

   delete [] buf;

   return TRUE;
}

void
copy_cdrom( LinkedList<DownloadFile> &files , LinkedList<String> &copied)
{
   FileName copydir;

   while(copied.count())
   {
      copied.rewind();
      copied.remove();
   }

   if(getenv("CDTEMP") != NULL)
   {
      copydir = getenv("CDTEMP");

      if(copydir[1] != ':')
      {
         LOG("Invalid path specified in CDTEMP!");
         return;
      }

      copydir.upperCase();
   }
   else
   {
      copydir = String(mypath) + "CD_TEMP";
   }

   copydir.delLast('\\');

   if(copydir.len() > 2)
      mkdir(copydir);

   copydir.appendBS();

   for( files.rewind() ; !files.eol() ; files++)
   {
      if(files.get().copy)
      {
         FileName source,dest;

         source = files.get().name;
         source.stripPath();

         int len = language_string_length(S_COPYING_FROM_CDROM((char *)source));

         io << S_COPYING_FROM_CDROM((char *)source) << '\xFF';

         dest   = copydir + source;
         source = files.get().name;

         if(!copy_file( source , dest ))
         {
            LOG( "Error copying %s to %s" , (char *)source , (char *)dest);
            io << S_ERROR_COPYING_FROM_CDROM << '\n';
         }
         else
         {
            files.get().name = dest;
            copied.add(String(dest));

            io << String('\b',len) << String(' ',len) << String('\b',len);
         }
      }
   }

}

void
delete_copied(LinkedList<String> &copied)
{
   for( copied.rewind() ; !copied.eol() ; copied++)
   {
      unlink(copied.get());
   }

   while(copied.count())
   {
      copied.rewind();
      copied.remove();
   }
}
