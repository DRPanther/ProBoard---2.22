#define Use_MsgBase

#include <ctype.h>
#include <io.h>
#include <dos.h>
#include <string.h>
#include "proboard.hpp"



void mailcheck( char *data )
{
     MsgArea  ma;
     String   param[ 10 ];
     bool     checkmail   =  TRUE;
     bool     checkfiles  =  TRUE;
     int      num_para    =  parse_data( data, param );


     for ( int i = 0;  i < num_para;  i++ )
     {
          if ( param[ i ][ 0 ]  ==  '/' )
          {
               switch ( toupper( param[ i ][ 1 ] ) )
               {
                    case 'M': 
                    
                         checkfiles = FALSE;
                         break;


                    case 'F': 
                    
                         checkmail  = FALSE;
                         break;
               }
          }
     }


     if ( checkmail )
     {
       // if ( rip_mode )
       // {
       //      rip_start_dialog( S_CHECKING_MAILBOX, 2 );
       //      io.fullcolor( 0x1F );
       // 
       //      io.show_remote = FALSE;
       // }
       

          io  <<  "\n\7" 
              <<  S_CHECKING_MAILBOX 
              <<  char( 0xFF );

          io.show_remote = TRUE;

          
          int           found      =  0;
          MessageIndex *msgsfound  =  new MessageIndex[ 200 ];


          found  =  msgbase.scanMail( msgsfound, 200 );

       
       // if ( rip_mode )
       // {
       //      rip_end_dialog();
       //      rip_reset();
       // }

          
          if ( ! found )
          {
               io << "\n\n" 
                  << S_MAILBOX_EMPTY 
                  << '\xFF';

               msleep( 1500 );

               io << '\n';
          }
          else
          {
               io << "\n\n" 
                  << S_MAIL_FOUND_TITLE 
                  << "\n\n";
          }


          if ( found )
          {
               markedmsglist.clear();


               for ( i = 1;  i <= MsgArea::highAreaNum();  i++ )
               {
                    int on1line = 0;
                    
                    
                    if ( ! ma.read( i ) ) 
                    {
                         continue;
                    }


                    for ( int j = 0;  j < found;  j++ )
                    {
                         if ( msgsfound[ j ].area  ==  i )
                         {
                              if ( ! on1line )
                              {
                                   if ( ! check_access( ma.readLevel,
                                                        ma.readFlags,
                                                        ma.readFlagsNot )  && 
                                                        
                                        ! ma.sysopAccess() )
                                   {
                                        LOG( "Mail found in inaccessible area (%d)!!",
                                             i );

                                        break;
                                   }


                                   io  <<  form( "\6%-33.33s: \3", ma.name );
                              }


                              if ( on1line++ == 6 )
                              {
                                   io  <<  form( "\n%-35s", "" );

                                   on1line = 1;
                              }


                              io  <<  form( "#%-5d ",
                                            ma.msgNum( msgsfound[ j ].num ) );

                              markedmsglist.add( msgsfound[ j ] );
                         }
                    }


                    if ( on1line ) 
                    {
                         io << "\n\n";
                    }
               }


               for ( ; ; )
               {
                    io << S_MAIL_FOUND_ACTION_PROMPT;


                    char k  =  wait_language_hotkeys( K_MAIL_FOUND_ACTION_PROMPT );


                    switch ( k )
                    {
                         case '\r':
                         case 0:
                         
                              io  <<  K_MAIL_FOUND_ACTION_PROMPT[ 0 ] 
                                  <<  '\n';

                              readmsg( "0 /N" );
                              
                              break;


                         case 1:
                         
                              io  <<  K_MAIL_FOUND_ACTION_PROMPT[ 1 ] 
                                  << '\n';

                              qscan_msg( "0 /N" );
                              
                              break;


                         default:
                         
                              io  <<  K_MAIL_FOUND_ACTION_PROMPT[ 2 ]
                                  <<  "\n\n" 
                                  <<  S_MESSAGES_HAVE_BEEN_MARKED
                                  <<  "\n\n" 
                                  <<  S_PRESS_ENTER_TO_CONTINUE;

                              break;
                    }


                    io << '\n';


                    if ( k != 1 ) 
                    {
                         break;
                    }
               }
          }


          delete [] msgsfound;
     }



     if ( checkfiles )
     {
          File f;
          int  found = 0;


          if ( ! checkmail )
          {
               io << '\n' 
                  << S_CHECKING_FOR_PERSONAL_FILES 
                  << '\xFF';
          }


          if ( f.open( FN_PVTFILES_PB ) )
          {
               _PrivateFile pvtfil;


               for ( ; ; )
               {
                    if ( f.read( & pvtfil,
                                 sizeof( pvtfil ) )  !=  sizeof( pvtfil ) ) 
                    {
                         break;
                    }


                    if ( strcmpl( pvtfil.to, user.name )  ||  
                         ! pvtfil.fname[ 0 ] ) 
                    {
                         continue;
                    }


                    FileName fn;


                    if ( ! strchr( pvtfil.fname, '\\' ) )
                    {
                         fn  =  String( cfg.pvtuploadpath )  +  pvtfil.fname;
                    }
                    else
                    {
                         fn  =  pvtfil.fname;
                    }


                    if ( access( fn, 0 ) ) 
                    {
                         continue;
                    }


                    fn.stripPath();


                    if ( ! found )
                    {
                         io << "\n\n" 
                            << S_PERSONAL_FILES_FOUND 
                            << "\n\n";
                    }


                    io  <<  form( "     \7* \3%-12s \7-\2 %02d %3s %4d\3 \7- ",
                                  (char *) fn,pvtfil.date[ 0 ],
                                  months_short[ pvtfil.date[ 1 ] ],
                                  pvtfil.date[ 2 ]  +  1900 );        // Y2K OKAY

                    io  <<  S_PERSONAL_FILE_FROM( pvtfil.from ) 
                        <<  '\n';


                    found++;
               }


               f.close();
               
               
               if ( found )
               {
                    io << '\n' 
                       << S_DOWNLOAD_NOW;


                    if ( io.ask( 0 ) )
                    {
                         download( "/P" );
                    }

                 // io << '\n' 
                 //    << S_PRESS_ENTER_TO_CONTINUE;
               }
          }


          if ( ! found  && 
               ! checkmail )
          {
               io << "\n\n" 
                  << S_NO_PERSONAL_FILES_FOUND 
                  << '\xFF';
               
               msleep( 1500 );
               
               io << '\n';
          }
     }
}
