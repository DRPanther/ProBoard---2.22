#include <tswin.hpp>
#include "proboard.hpp"



#define HELP 1
#define USER 2
#define MACRO 3


static int    status = 0;
static Window win;
static int    WINSIZE;

static 
void near show()
{
     int   maxx        =  SCREEN.maxX;
     word *buf         =  new word[ tsw_vsize * maxx ];
     int   x           =  SCREEN.getX();
     int   cursorline  =  SCREEN.getY();
     int   caplines    =  ( cursorline  >  tsw_vsize  -  ( WINSIZE + 2 ) ) 
                             ?  tsw_vsize  -  ( WINSIZE + 2 ) 
                             :  cursorline;


     tsw_gettext( buf,
                  1,
                  cursorline - caplines + 1,
                  maxx,
                  cursorline + 1 );


     if ( cursorline  >  tsw_vsize  -  ( WINSIZE + 2 ) )
     {
          cursorline  =  tsw_vsize  -  ( WINSIZE + 2 );
     }


     byte  att = SCREEN.attrib();


     SCREEN.attrib( 7 );
     SCREEN.clear();
     SCREEN.change( 1,
                    1,
                    maxx,
                    tsw_vsize  -  ( WINSIZE + 1 ) );


     tsw_puttext( buf,
                  1,
                  1,
                  maxx,
                  caplines );

//     win.open( 1,
//               tsw_vsize - WINSIZE,
//               maxx,
//               tsw_vsize - 1,
//               0x17,
//               0,
//               SINGLE_BORDER );

     win.open( 1,
               tsw_vsize - WINSIZE,
               maxx,
               tsw_vsize - 1,
               0x70,
               0,
               CHISEL_BORDER,
               0x7F,
               NULL,
               0x78 );

     SCREEN.setPos( x, cursorline );
     SCREEN.attrib( att );


     delete [] buf;
}



static 
void near hide()
{
     int maxx = SCREEN.maxX;


     win.close();


     word  *buf  =  new word[ tsw_vsize * maxx ];


     tsw_gettext( buf,
                  1,
                  1,
                  maxx,
                  tsw_vsize  -  ( WINSIZE + 1 ) );


     byte att = SCREEN.attrib();


     SCREEN.saveCursor();

     SCREEN.attrib( 7 );
     SCREEN.change( 1,
                    1,
                    maxx,
                    tsw_vsize - 1 );

     SCREEN.clear();
     SCREEN.restoreCursor();
     SCREEN.attrib( att );


     tsw_puttext( buf,
                  1,
                  1,
                  maxx,
                  tsw_vsize  -  ( WINSIZE + 1 ) );


     delete [] buf;
}



void toggle_userinfo()
{
  // if ( user_recnr < 0 ) 
  // {
  //      return;
  // }
  // 
  // 
  // switch ( status )
  // {
  //      case USER: 
  //      
  //           hide(); 
  // 
  //           status = 0; 
  //           
  //           break;
  // 
  // 
  //      case 0: 
  //      
  //           WINSIZE = 5;
  // 
  //           show();
  // 
  // 
  //      default:
  // 
  //           if ( WINSIZE != 5 ) 
  //           { 
  //                hide(); 
  //                
  //                WINSIZE = 5; 
  //                
  //                show(); 
  //           }
  // 
  // 
  //           win.clear();
  // 
  //           win.attrib( 0x17 );
  // 
  //           win  <<  " Handle   :                      Flags    :\n"
  //                    " # Calls  :                      Last call:\n"
  //                    " Downloads:                      Uploads  :";
  // 
  //           win.attrib( 0x1F );
  // 
  //           win.setpos( 13, 1); 
  //           win << user.alias;
  // 
  // 
  //           win.setpos( 45, 1 ); 
  //           
  //           char s[ 33 ]; 
  //           
  //           user.aflags.flagstostr( s ); 
  //           
  //           win << s;
  // 
  // 
  //           win.setpos( 13, 2 ); 
  //           
  //           win  <<  form( "%d", user.timescalled );
  // 
  // 
  //           win.setpos( 45, 2 ); 
  //           
  //           win  <<  form( "%02d %3s %04d at %02d:%02d:%02d",
  //                          user.lastdate[ 0 ],
  //                          months_short[ user.lastdate[ 1 ] ],
  //                          user.lastdate[ 2 ]  +  1900,       // Y2K FIX!
  //                          user.lasttime[ 0 ],
  //                          user.lasttime[ 1 ],
  //                          user.lasttime[ 2 ] );
  // 
  // 
  //           win.setpos( 13, 3 ); 
  //           
  //           win  <<  form( "%d (%d Kb)",
  //                          user.numdownloads,
  //                          user.kdownloaded );
  // 
  // 
  //           win.setpos( 45, 3 ); 
  //           
  //           win  <<  form( "%d (%d Kb)",
  //                          user.numuploads,
  //                          user.kuploaded );
  // 
  // 
  //           SCREEN.setpos( SCREEN.getx(),
  //                          SCREEN.gety() );
  // 
  //           status = USER;
  //      
  //           break;
  // };
}



void toggle_help()
{
     switch ( status )
     {
          case HELP: 
               
               hide(); 

               status = 0; 
               
               break;


          case 0: 
          
               WINSIZE = 7; 
               
               show();


          default:
          
               if ( WINSIZE != 7 ) 
               { 
                    hide(); 
                    
                    WINSIZE = 7; 
                    
                    show(); 
               }


               win.clear();
               win.attrib( 0x71 );

               win  <<  " [ShF1] Macro Help         [AltM] Edit Current Menu    [AltS] Static\n"
                        " [AltC] Chat               [AltE] Edit User            [AltH] Hang Up\n"
                        " [AltI] Image              [AltJ] DOS Shell            [AltL] Lock Out\n"
                        " [AltR] Reset Chat   [Ctrl-Lt/Rt] Level               [Up/Dn] Timer\n"
                        " [Home] Reset Status     [F2-F10] Status          [PgUp/PgDn] Cycle status";

               SCREEN.setPos( SCREEN.getX(),
                              SCREEN.getY() );

               status = HELP;
          
               break;
  }
}



void toggle_macrohelp()
{
     switch ( status )
     {
          case MACRO: 
          
               hide(); 
               
               status = 0; 
               
               break;


          case 0: 
          
               WINSIZE = 7; 
               
               show();


          default:
          
               if ( WINSIZE != 7 ) 
               { 
                    hide(); 

                    WINSIZE = 7; 
                    
                    show(); 
               }


               win.clear();


               for ( int i = 0;  i < 10;  i++ )
               {
                    win.setPos( ( i / 5 )  *  38  +  2,
                                i % 5  +  1 );
                    

                    //----------------------------  
                    // JDR: REGISTRATION REFERENCE
                    //----------------------------  
                    
                    if ( registered ) 
                    {
                         win  <<  form( "[0;34;47mAF%-2d - [0;31;47m%-27.27s[1;33;47m",
                                        i + 1,
                                        cfg.sysopkeys[ i ] );
                    }
                    else 
                    {
                         win  <<  form( "[0;34;47mAF%-2d - [33;47m%-27.27s[1;33;47m",
                                        i + 1,
                                        "REGISTER NOW!" );
                    }
               }


               SCREEN.setPos( SCREEN.getX(),
                              SCREEN.getY() );

               status = MACRO;
          
          
               break;
          }
}
