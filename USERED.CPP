#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <tswin.hpp>
#include "proboard.hpp"



void usereditor( char * )
{
     User  rec;
     int   cur_user;
     char *loglevels[]  =  { 
                                "Friend", 
                                "Normal", 
                                "Suspicious",
                                "Dangerous" };


     for ( cur_user = num_users ; ; )
     {
          rec.read(cur_user-1);
  
          io  <<  form( "\f\n\6User record \1%d",
                        cur_user );


          if ( rec.uFlags & UFLAG_DELETED ) 
          {
               io << "\7  [DELETED]";
          }


          if ( rec.uFlags & UFLAG_NOKILL ) 
          {
               io << "\7  [NOKILL]";
          }


          io  <<  "\n\n";

          io  <<  form( "\6UserName : \3%s\n\n"
                        "\6City     : \3%s\n"
                        "\6Password : \3%s\n\n",
                        rec.name,
                        rec.city,
                        rec.passWord );

          io  <<  form( "\6Level    : \3%-5u  "
                        "\6Loglevel : \3%s\n\n"
                        "\6# Calls  : \3%-5ld  "
                        "\6Last Call: \3%02d-%s-%04d at %02d:%02d:%02d\n",
                        rec.level,
                        loglevels[ rec.logLevel ],
                        rec.timesCalled,
                        rec.lastDate[ 0 ],
                        months_short[ rec.lastDate[ 1 ] ],
                        rec.lastDate[ 2 ]  +  1900,
                        rec.lastTime[ 0 ],
                        rec.lastTime[ 1 ],
                        rec.lastTime[ 2 ] );

          io  <<  "\n\n\6<\3+/-\6> <\3F\6>irst <\3L\6>ast Le<\3v\6>el <\3Q\6>uit <\3S\6>earch <\3D\6>elete <\3N\6>oKill ";


          char k  =  io.wait( "S+-FLVQDN\r" );


          switch ( k )
          {
               case 'Q': 
               
                    return;


               case 'D': 
               
                    rec.uFlags ^= UFLAG_DELETED;

                    rec.write( FALSE );
                    
                    break;


               case 'N': 
               
                    rec.uFlags ^= UFLAG_NOKILL;

                    rec.write( FALSE );

                    break;


               case '+': 
               
                    if ( cur_user < num_users ) 
                    {
                         cur_user++; 
                    }
                    
                    break;


               case '-': 
               
                    if ( cur_user > 1 )
                    {
                         cur_user--; 
                    }
                    
                    break;


               case 'F': 
               
                    cur_user = 1;
                    
                    break;


               case 'L': 
               
                    cur_user = num_users;
                    
                    break;


               case 'V': 
               
                    io << "\n\nNew level: \3";

                    io.read( int( rec.level ) );

                    rec.write( FALSE );

                    break;


               case 'S': 
               {
                    char s[ 36 ];
                    int  i;


                    io << "\n\nSearch for string: \3";

                    io.read( s, 
                             36,
                             READMODE_UPFIRST );

                    if ( ! s[ 0 ] ) 
                    {
                         break;
                    }


                    io << "\7\n\nSearching...";


                    for ( i = 0;  i < num_users;  i++ )
                    {
                         rec.read( i );


                         if ( strstrl( rec.name, s ) )
                         { 
                              cur_user  =  i + 1; 
                              
                              break; 
                         }
                    }


                    if ( i == num_users ) 
                    {
                         cur_user = 1;
                    }
               } 
                    break;
          }
     }
}



void edit_user()
{
     user.write( FALSE );

     shell( String( form( "*Q*N*SPROCFG.EXE UE %d",
                    user_recnr + 1 ) ) );

     user.read( user_recnr );
}

