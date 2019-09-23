#define Use_LinkedList
#define Use_TagList

#include <string.h>
#include "proboard.hpp"

static
long
find_tagentry(File& f)
{
   char name[36];

   f.rewind();
   for(;;)
   {
      if(f.read(name,36) != 36)
         return -1;

      long p = f.pos();

      if(name[0] && !stricmp(name,user.name))
         return p;

      word n;

      if(f.read(&n,sizeof(n)) != sizeof(n))
         return -1;

      f.seek(long(n) * sizeof(FilesIdx),seek_cur);
   }
}

void
write_taglist()
{
   File f;

   if(!f.open(FileName(syspath,"TAGLIST.PB"),fmode_rw|fmode_copen))
      return;

   long p = find_tagentry(f);

   if(p > 0)
   {
      f.seek(p - 36);
      f.write("",1);        // Invalidate entry
   }

   if(taglist.count())
   {
      f.seek(0,seek_end);

      f.write(user.name,36);

      word n = taglist.count();

      f << n;
      taglist.rewind();

      for(word i=0;i<n;i++,taglist++)
      {
         f.write(&taglist.get(),sizeof(taglist.get()));
      }
   }
}

void
read_taglist()
{
   File f;

   taglist.kill();

   if(!f.open(FileName(syspath,"TAGLIST.PB"),fmode_read))
      return;

   long p = find_tagentry(f);

   if(p < 0)
      return;

   word n;

   f >> n;

   for(word i=0;i<n;i++,taglist++)
   {
      FilesIdx fi;

      if(f.read(&fi,sizeof(fi)) != sizeof(fi))
         break;

      taglist.add(fi);
   }
}
