#define Use_MsgBase

#include <string.h>
#include "proboard.hpp"

const int text_bufsize = 512;



void Message::export( char *fname )
{
     File outf;


     if ( ! outf.open( fname, fmode_create ) ) 
     {
          return;
     }


     outf  <<  form( "Message #%-5ld [%s] ",
                     msgNum(),
                     msgArea->name );


     if ( attr & MSGATTR_PRIVATE ) 
     {
          outf << "[PRIVATE] ";
     }


     if ( msgArea->msgKind == MSG_NET )
     {
          if ( attr & MSGATTR_CRASH )  outf << "[CRASH] ";
          if ( attr & MSGATTR_KILL  )  outf << "[KILL] ";
          if ( attr & MSGATTR_FILE  )  outf << "[FILE] ";
          if ( attr & MSGATTR_SENT  )  outf << "[SENT] ";
     }


     if ( prev )   
     {
          outf  <<  form( "[%ld <ÄÄ] ",
                          msgArea->msgNum( prev ) );
     }


     if ( next )
     {
          outf  <<  form( "[ÄÄ> %ld] ",
                          msgArea->msgNum( next ) );
     }


     outf << "\r\n";


     String org(  from );
     String dest( to   );


     if ( msgArea->msgKind == MSG_NET )
     {
          int pt_from  =  pointNum( 0 );
          int pt_to    =  pointNum( 1 );


          org  <<  form( " (%d:%d/%d",
                         origZone,
                         origNet,
                         origNode );

          dest  <<  form( " (%d:%d/%d",
                          destZone,
                          destNet,
                          destNode );


          if ( pt_from )
          {
               org  <<  form( ".%d",  pointNum( 0 ) );
          }


          if ( pt_to )   
          {
               dest  <<  form( ".%d",  pointNum( 1 ) );
          }


          org  << ')';
          dest << ')';
     }


     outf  <<  form( "From: %-46.46s   Posted: %2d %s %02d  %02d:%02d\r\n",
               (char *) org,postDate[ 0 ],
               months_short[ postDate[ 1 ] ],
               postDate[ 2 ]  %  100,             // Y2K FIXED!
            // postDate[ 2 ],                     // Y2K BUG!
               postTime[ 0 ],
               postTime[ 1 ] );

     outf  <<  form( "To  : %-46.46s ",
                     (char *) dest );


     if ( attr & MSGATTR_RECEIVED )
     {
          outf  <<  form( "Received: %2d %s %02d  %02d:%02d",
                          recvDate[ 0 ],
                          months_short[ recvDate[ 1 ] ],
                          recvDate[ 2 ]  %  100,       // Y2K FIXED!
                       // recvDate[ 2 ],               // Y2K BUG!
                          recvTime[ 0 ],
                          recvTime[ 1 ],
                          recvTime[ 2 ] );
     }


     outf << "\r\n";


     if ( attr & MSGATTR_FILE ) 
     {
          outf  <<  form( "File: %s\r\n\r\n",  subj );
     }
     else 
     {
          outf  <<  form( "Subj: %s\r\n\r\n",  subj );
     }


     outf.close();

     createMsgTextFile( fname, TRUE );
}



void
Message::createMsgTextFile(char *fname , bool append)
{
 File file;

 if(append) file.open(fname , fmode_write | fmode_append);

 if(!file.opened()) file.open(fname,fmode_create);
 if(!file.opened()) return;

 bool inkludge = FALSE;

 int index = 0;

 char line[100];
 memset(line,0,100);

 bool msgdone = FALSE;

 for(long txt_off = 0; !msgdone ;txt_off += 255)
   {
   char r[256];

   int l = msgBase()->readMsgText(*this,r,txt_off,255);

   if(l < 255) msgdone = TRUE;

   for(int j=0;j<l;j++)
     {
      byte x = r[j];

      if(x == '\t') x = ' ';
      if(x == 1)    inkludge=1;
      if(x == '\n' || x == 0x8D || x <= 8) continue;

      if(x == '\r')
        {
         x='\n';

         if(inkludge)
           {
            inkludge = FALSE;

            continue;
           }
        }

      if(inkludge) continue;

      if(x == '\n')
        {
         if(strncmp(line,"SEEN-BY",7)) file << line << "\r\n";

         memset(line,0,100);

         index=0;
        }
       else
        {
         if(x>=' ' || x==0x1B) line[index++] = x;
        }

      if(index>79)
        {
         String wrap;

         wordwrap(line,wrap,79);

         if(strncmp(line,"SEEN-BY",7)) file << line << "\r\n";

         memset(line,0,100);

         strcpy(line,wrap);

         index=strlen(line);
        }
      }
   }

 if(index)
    if(strncmp(line,"SEEN-BY",7))
        file << line << "\r\n";
}


void
Message::createMsgTextString(char *string , word maxsize )
{
 maxsize -= 100;

 string[0] = 0;

 bool inkludge = FALSE;

 int index = 0;

 char line[100];
 memset(line,0,100);

 bool msgdone = FALSE;

 char *r = new char[text_bufsize+1];

 for(long txt_off = 0; !msgdone ;txt_off += text_bufsize)
   {
   int l = msgBase()->readMsgText(*this,r,txt_off,text_bufsize);

   if(l < text_bufsize) msgdone = TRUE;

   for(int j=0;j<l;j++)
     {
      byte x = r[j];

      if(x == '\t') x = ' ';
      if(x == 1)    inkludge=1;
      if(x == '\n' || x == 0x8D || x <= 8) continue;

      if(x == '\r')
        {
         x='\n';

         if(inkludge)
           {
            inkludge = FALSE;

            continue;
           }
        }

      if(inkludge) continue;

      if(x == '\n')
        {
         if(strncmp(line,"SEEN-BY",7))
           {
            if(strlen(string) < maxsize)
              {
               strcat(string,line);
               strcat(string,"\r\n");
              }
           }

         memset(line,0,100);

         index=0;
        }
       else
        {
         if(x>=' ' || x==0x1B) line[index++] = x;
        }

      if(index>79)
        {
         String wrap;

         wordwrap(line,wrap,79);

         if(strncmp(line,"SEEN-BY",7))
           {
            if(strlen(string) < maxsize)
              {
               strcat(string,line);
               strcat(string,"\r\n");
              }
           }

         memset(line,0,100);

         strcpy(line,wrap);

         index=strlen(line);
        }
      }
   }

   if(index)
      if(strncmp(line,"SEEN-BY",7))
      {
         if(strlen(string) < maxsize)
         {
            strcat(string,line);
            strcat(string,"\r\n");
         }
      }

   delete [] r;
}
