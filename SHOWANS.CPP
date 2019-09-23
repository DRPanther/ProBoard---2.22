#include "proboard.hpp"




//---------------------  
// Show ANS or ASC file
//---------------------  

void showansasc( char *data )
{
     showansasc( data, NULL );
}



//--------------------------  
// Show ANS, ASC or RIP file
//--------------------------  

char showansascrip( char *data )
{
     if ( ! show_rip( data, FALSE ) )
     {
          return  showansasc( data, NULL );
     }
     else
     {
          return 0;
     }
}



//----------------------------  
// View ANS file, with a pause
//----------------------------  

void view_ans_wait( char *data )
{
     showansasc( data, NULL );


     io << '\n' 
        << S_PRESS_ENTER_TO_CONTINUE;
}



//--------------------------------------------------------  
// Show an AVT, ANS or ASC file with Avatar auto-detection
//--------------------------------------------------------  

char showansasc( char *data,
                 char *hotkeys )
{
     FileName f( cfg.txtpath, data );
     char     k;


     if ( avtplus )
     {
          f.changeExt( "AVP" );

          k  =  io.sendfile( f, hotkeys );


          if ( k != ANS_NOFILE ) 
          {
               return k;
          }
     }


     if ( avatar )
     {
          f.changeExt( "AVT" );

          k  =  io.sendfile( f, hotkeys );


          if ( k != ANS_NOFILE ) 
          {
               return k;
          }
     }


     if ( ansi_mode )
     {
          f.changeExt( "ANS" );

          k  =  io.sendfile( f, hotkeys );


          if ( k != ANS_NOFILE ) 
          {
               return k;
          }
     }


     io.color( COLOR_WHITE );


     f.changeExt( "ASC" );

     k  =  io.sendfile( f, hotkeys );

     return k;
}

