#include <string.h>
#include "proboard.hpp"



void raw_dir( char *data )
{
     char wildcard[ 64 ];


     if ( ! *data )
     {
          io << "\f\n" 
             << S_DIRECTORY_TO_SHOW;

          io.read( wildcard,
                   50,
                   READMODE_UPALL );


          if ( ! wildcard[ 0 ] )
          {
               return;
          }
     }
     else 
     {
          strcpy( wildcard, data );
     }


     append_backspace( wildcard );

     strcat( wildcard, "*.*" );

     LOG( 1, "Raw dir: %s", wildcard );

     io << "\n\n";


     io.enablestop();
     linecounter( 2 );


     DirScan f( wildcard );


     while( int( f ) )
     {
          Date d = f.date();
          Time t = f.time();


          io  <<  form( "\7%-12s \3%7ld  \6%02d-%s-%04d  \2%02d:%02d:%02d\n",
                        f.name(),
                        f.size(),
                        d[ 0 ],
                        months_short[ d[ 1 ] ],
                        d[ 2 ]  +  1900,          // Y2K OKAY!
                        t[ 0 ],
                        t[ 1 ],
                        t[ 2 ] );

          if ( linecounter()  || 
               stopped ) 
          {
               break;
          }


          ++f;
     }


     io << "\n\n" 
        << S_PRESS_ENTER_TO_CONTINUE;
}
