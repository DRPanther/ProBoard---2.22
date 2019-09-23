#include <string.h>
#include <..\key20\pbkey.hpp>

void decompress_data(byte *input,byte *output,int insize,int outsize);

static void
regis_read()
{
   int i , output_size;
   File f;

   if(!f.open(FileName(syspath,"REGKEY.PB")))
   {
      registered = FALSE;

      return;
   }

   output_size = int(f.len() - 33);

   byte *output_data = new byte[output_size];

   f.seek(33);
   f.read(output_data , output_size);
   f.close();

   for(i = 0 ; i < output_size ; i++)
   {
      output_data[i] ^= fixed_xor_data[i % 100];
   }

   KeyFile *kf = new KeyFile;

   decompress_data(output_data , (byte *)kf , output_size , sizeof(*kf));

   delete [] output_data;

   encrypt((char *)kf->keyData,sizeof(kf->keyData),kf->xorData);

   if(
         kf->keyCrc != crc32(kf->keyData , sizeof(kf->keyData))
      || kf->xorCrc != crc32(kf->xorData , sizeof(kf->xorData))
     )
   {
      registered = FALSE;

      delete kf;

      return;
   }

   if(kf->securityCheck[regSelectedTable] != calc_security(&kf->keyData[0] , regTable1 , regTable2))
   {
      registered = FALSE;

      delete kf;

      return;
   }

   regis = kf->keyData[0];

   delete kf;

   if(
       (regis.expirDate.ok() && regis.expirDate <= Date(TODAY))
     ||
       (regis.expirVersion   && regis.expirVersion <= VER_WORD)
    )
   {
      static int count = 0;

      if(!count++) LOG("Key file expired!");

      registered = FALSE;
      return;
   }

   registered = (!stricmp(regis.sysopName,cfg.sysopname) && !stricmp(regis.bbsName,cfg.bbsname));

   if(VER_BETA < 0xF000 && !regis.beta)
      registered = FALSE;

  //if(VER_BETA < 0xF000 && ....) registered = FALSE;
}
