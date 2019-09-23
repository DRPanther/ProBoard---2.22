#include <string.h>
#include <stdlib.h>
#include "proboard.hpp"



static 
void near adjust_parms( char   *s,
                        String *param )
{
     char delimiter;


     strip_linefeed( s );


     for ( int i = 0;  i < 4;  i++ )
     {
          param[ i ].clear();


          while( *s == ' '  && 
                 *s ) 
          {
               s++;
          }


          if ( *s == '\0' )
          {
               break;
          }


          if ( *s == '\"' ) 
          {
               delimiter = *s++;
          }
          else 
          {
               delimiter = ' ';
          }


          for ( int j = 0;  
          
                *s != delimiter  && 
                (*s);
                
                j++ )
          {
               param[ i ]  <<  (*s++);
          }
     }
}



#define NUMCOMMANDS 18



void questionnaire( char *data )
{
     char str[ 300 ];
     char *commands[]  =  { 
                               "ASK" ,
                               "CHANGECOLOR",
                               "OUTPUTANSWER",
                               "POSTINFO",
                               "QUIT",
                               "SETFLAG",
                               "CLEARSCREEN",
                               "DISPLAY",
                               "ENDIF",
                               "IF",
                               "GETCHOICE",
                               "CAPITALISE",
                               "DISPLAYFILE",
                               "EXEC",
                               "LISTANSWER",
                               "MENUCMND",
                               "SETSECURITY",
                               "WAITENTER"
                          };
     String   answers[ 30 ];
     String   param  [ 4  ];
     File     qf;

     FileName qafile( pb_curlang.questPath[ 0 ] 
                         ? pb_curlang.questPath 
                         : syspath,data,
                      ".Q-A" );

     FileName aswfile( data, ".ASW" );
     int      i;
     bool     capitalise = FALSE;


     if ( ! qf.open( qafile,
                     fmode_read | fmode_text ) )
     {
          if ( strcmpl( data, "NEWUSER" ) )
          {
               LOG( "File %s not found!",  (char *) qafile );

               io  <<  "\n\n\1Scriptfile not found! " 
                   <<  S_PRESS_ENTER_TO_CONTINUE;
          }


          return;
     }


     LOG( 2, "Questionnaire: %s", data );


     for ( ; ; )
     {
          if ( ! qf.readLine( str, 299 ) ) 
          {
               break;
          }


          adjust_parms( str, param );
          
          
          for ( i = 0;  i < NUMCOMMANDS;  i++ )
          {
               if ( param[ 0 ]  !=  commands[ i ] )
               {
                    continue;
               }


               switch ( i )
               {
                    case 0: 

                         char s[ 80 ];


                         io.read( s,
                                  atoi( param[ 1 ] ),
                                  capitalise
                                    ? READMODE_UPFIRST
                                    : 0 );

                         answers[ atoi( param[ 2 ] )  -  1 ]  =  s;

                         io << '\n';

                         break;


                    case 1: 
                    
                         io.color( atoi( param[ 1 ] ) ); 

                         break;


                    case 2: 
                    
                         File f( aswfile,
                                 fmode_write     | 
                                   fmode_append  | 
                                   fmode_copen   | 
                                   fmode_text );
                         
                         
                         if ( param[ 2 ][ 0 ] ) 
                         {
                              f  <<  form( "%s%s\n",
                                           (char *) param[ 1 ],
                                           (char *) answers[ atoi( param[ 2 ] )  -  1 ] );
                         }
                         else 
                         {
                              f  <<  form( "%s\n",
                                           (char *) answers[ atoi( param[ 1 ] )  -  1 ] );
                         }

                    
                         break;


                    case 3: 
                    {
                         Date date( TODAY );
                         Time time( NOW   );
                         File f( aswfile, 
                                 fmode_write     | 
                                   fmode_append  | 
                                   fmode_copen   | 
                                   fmode_text );


                         f  <<  form( "*** %s completed questionnaire at %02d:%02d on %02d-%s-%02d ***\n",
                                      user.name,
                                      time[ 0 ],
                                      time[ 1 ],
                                      date[ 0 ],
                                      months_short[ date[ 1 ] ],
                                      date[ 2 ] % 100 );    // Y2K FIXED          
                                   // date[ 2 ] );          // Y2K BUG
                    }
                         break;


                    case 4: 
                    
                         return;


                    case 5: 
                    
                         int flag = -1;


                         strupr( param[ 1 ] );
                         
                         
                         if ( param[ 1 ][ 1 ]  >=  '1'  && 
                              param[ 1 ][ 1 ]  <=  '8' )
                         {
                              switch ( param[ 1 ][ 0 ] )
                              {
                                   case 'A': 
                                   
                                        flag  =  param[ 1 ][ 1 ]  -  '1' + 1;  
                                        break;


                                   case 'B': 
                                   
                                        flag  =  param[ 1 ][ 1 ]  -  '1' + 9;  
                                        break;


                                   case 'C': 
                                   
                                        flag  =  param[ 1 ][ 1 ]  -  '1' + 17; 
                                        break;


                                   case 'D': 
                                   
                                        flag  =  param[ 1 ][ 1 ]  -  '1' + 25; 
                                        break;
                              }
                         }
                         else
                         {
                              if ( param[ 1 ][ 0 ]  >=  'A'  && 
                                   param[ 1 ][ 0 ]  <=  'Z') 
                              {
                                   flag  =  param[ 1 ][ 0 ]  -  'A' + 1;
                              }


                              if ( param[ 1 ][ 0 ]  >=  '1'  && 
                                   param[ 1 ][ 0 ]  <=  '6' ) 
                              {
                                   flag  =  param[ 1 ][ 0 ]  -  '1' + 27;
                              }
                         }


                         if ( flag < 1  || 
                              flag > 32 ) 
                         {
                              break;
                         }


                         if ( param[ 2 ]  ==  "ON" )  
                         {
                              user.aFlags.setflag( flag );
                         }


                         if ( param[ 2 ]  ==  "OFF" ) 
                         {
                              user.aFlags.clearflag( flag );
                         }


                         updatemenu = TRUE;
                    
                         break;


                    case 6: 
                    
                         io.cls();
                         break;


                    case 7: 
                    
                         for ( int i = 0;  i  <  param[ 1 ].len();  i++ ) 
                         {
                              if ( param[ 1 ][ i ]  ==  '|' ) 
                              {
                                   param[ 1 ][ i ]  =  '\n';
                              }
                         }


                         io  <<  param[ 1 ];
                    
                    
                         break;


                    case 9: 
                    
                         if ( answers[ atoi( param[ 1 ] )  -  1 ]  ==  param[ 3 ] ) 
                         {
                              break;
                         }


                         do
                         {
                              if ( ! qf.readLine( str, 299 ) ) 
                              {
                                   return;
                              }


                              adjust_parms( str, param );
                         } 
                         while ( param[ 0 ]  !=  commands[ 8 ] );
                    

                         break;


                    case 10: 
                    
                         char c  =  io.wait( param[ 1 ] );


                         answers[ atoi( param[ 2 ] )  -  1 ]  =  String( c );

                         io << c 
                            << '\n';
                    
                         break;


                    case 11: 

                         if ( param[ 1 ]  ==  "ON" ) 
                         {
                              capitalise = TRUE;
                         }
                         else 
                         {
                              capitalise = FALSE;
                         }


                         break;


                    case 12: 
                    
                         showansasc( param[ 1 ] );

                         break;


                    case 13: 
                    
                         shell( param[ 1 ] );

                         break;


                    case 14: 
                    
                         io  <<  (char *) answers[ atoi( param[ 1 ] )  -  1 ] 
                             << '\n';

                         break;


                    case 15: 

                         ( *( menufunctions[ atoi( param[ 1 ] ) ] ) )  ( param[ 2 ] );
                         
                         break;


                    case 16: 
                    
                         user.level  =  atoi( param[ 1 ] );
                         updatemenu  =  TRUE;

                         break;


                    case 17: 
                    
                         io << "\t\n";

                         break;
               }
          }
     }
}
