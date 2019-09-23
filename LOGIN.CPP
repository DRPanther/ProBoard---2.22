#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dos.h>
#include <tswin.hpp>
#include "proboard.hpp"


static bool newuser( char *username );

static bool check_trashcan( char *username );



//-------------------------------  
// 0 -> ok   -1 -> User violation
//-------------------------------  

int login()   
{
     char username[ 36 ];
     char password[ 16 ];
     int  i;
     bool systempwd_ok    = FALSE;
     bool login_pex_found = FALSE;

   
     for ( ; ; )
     {
          if ( ! io.baud  && 
               cfg.allowquicklogin ) 
          {
               tsw_showfooter( "Press [Enter] to login as sysop", BAR_COLOR );
          }


          bool ansi  =  user.uFlags & UFLAG_ANSI;


          user.newUser();

          strcpy( user.language, cfg.defaultLanguage );

          user.logLevel = 1;


          if ( ansi ) 
          {
               user.uFlags |= UFLAG_ANSI;
          }


          //----------------------------  
          // JDR: REGISTRATION REFERENCE
          //----------------------------  

          if ( ! registered )
          {
               char *s = "\rProBoard v" VERSION " [UNREGISTERED]\n";


            // LOG( "crc = %08lX",  crc32( s ) );


              /* if ( crc32( s )  !=  0x17D65243L ) 
               {
                    exit_proboard();
               } */


               io << s;
          }


          if ( cfg.usesystempwd    && 
               cfg.systempwd[ 0 ]  && 
               ! systempwd_ok )
          {
               showansascrip( "BEFOREPW" );


               for ( int i = 0;  i < cfg.max_passinput;  i++ )
               {
                    char pwd[ 16 ];


                    io << S_ENTER_SYSTEM_PASSWORD;

                    io.read( pwd,
                             15,
                             READMODE_PWD );

                    io << "\n\n\xFF";


                    if ( pwd[ 0 ] )
                    {
                         if ( ! stricmp( pwd, cfg.systempwd ) )  
                         {
                              break;
                         }


                         io << S_WRONG_PASSWORD 
                            << "\n\n";

                         LOG( "Wrong system password: '%s'", pwd );
                    }
               }


               if ( i == cfg.max_passinput )
               {
                    LOG( "Exceeded maximum retries." );

                    io << "\n\n" 
                       << S_HANGING_UP 
                       << "\n\xFF";

                    LOG( "Hanging up" );

                    sleep( 2 );

                    exit_proboard();
               }


               systempwd_ok = TRUE;

               showansascrip( "AFTERPW" );
          }


          user_recnr = -1;


          if ( run_sdkfile( form( "/I _LOGIN %lu", 
                                  (dword) username ) )  >=  0 )
          {
               login_pex_found = TRUE;
          }


          if ( user_recnr >= 0 )
          {
               break;
          }


          if ( ! login_pex_found )
          {
               io << '\r' 
                  << S_ENTER_YOUR_NAME;


               int saved_x;
               int saved_y;


               SCREEN.getPos( saved_x, saved_y );

               io.read( username,
                        35,
                        READMODE_UPFIRST );


               if ( strchr( username, ';' ) ) 
               {
                    *strchr( username, ';' )  =  '\0';
               }


               for ( i = 0;  username[ i ];  i++ )
               {
                    if ( username[ i ]  ==  '\xFF' )
                    {
                         username[ i ]  =  ' ';
                    }
               }


               strip_trailing( username );
               strip_leading(  username );

               tsw_showfooter( "", BAR_COLOR );


               if ( ! username[ 0 ] )
               {
                    if ( ! io.baud )
                    {
                         strcpy( username, cfg.sysopname );

                         SCREEN.setPos( saved_x, saved_y );

                         SCREEN.attrib( ( SCREEN.attrib() & 0xF )  |  0x10 );
                         
                         
                         io << username;


                         for ( i = 0;  i  <  35 - strlen( username );  i++ )
                         {
                              io << ' ';
                         }
                    }
                    else
                    {
                         io << "\n\n\xFF";

                         continue;
                    }
               }


               if ( check_trashcan( username ) )
               {
                    LOG( "Trashcan name: \"%s\"", username );

                    io << "\n\n";
                    
                    
                    if ( showansascrip( "TRASHCAN" )  ==  ANS_NOFILE )
                    {
                         io << S_ILLEGAL_NAME 
                            << "\n\n";
                    }


                    continue;
               }


               rip_reset();
               rip_textmode();


               io << "\n\n\7" 
                  << S_SCANNING_USER_FILE 
                  << char( 0xFF );


               bool found  =  user.search( username );


               while ( found  && 
                       ( user.uFlags & UFLAG_DELETED ) )
               {
                    found = user.searchNext();
               }


               user_recnr  =  found 
                                ?  int( user.record ) 
                                :  -1;

         
               if ( ! found  && 
                    cfg.allowalias )
               {
                    found  =  user.search( username,
                                           TRUE,
                                           FALSE,
                                           TRUE );

                    user_recnr  =  found 
                                     ?  int( user.record ) 
                                     :  -1;
               }


               if ( user_recnr < 0      && 
                    ! cfg.allowoneword  && 
                    ! strchr( username, ' ' ) )
               {
                    LOG( "Illegal name: \"%s\"", username );

                    io << "\n\n" 
                       << S_WRONG_NAME_FORMAT 
                       << "\n\n";


                    continue;
               }
          }


          login_date.today();
          login_time.now();


          if ( user_recnr < 0 )
          {
               LOG( "User not located : %s", username );

               run_sdkfile( "/I NEWUSER" );

               showansascrip( "NEWUSER" );


               io << "\n\n" 
                  << S_ARE_YOU_A_NEW_USER( username );

         
               if ( io.ask( 0 ) )
               {
                    /*if ( ! registered  &&
                         num_users >= MAX_UNREG_USERS )
                    {
                         io << "\n\n"
                            << "This unregistered version of ProBoard cannot create any more new accounts\n"
                            << "\n\n";

                         continue;
                    }*/


                    user_recnr  =  newuser( username )  
                                     ?  int( user.record ) 
                                     :  ( -1 );

                    return 0;
               }
               else
               {
                    io << "\n\n";

                    LOG( "Name misspelled (%s).", username );


                    continue;
               }
          }
          else
          {
               break;
          }
     }


     read_taglist();

     login_date.today();
     login_time.now();


     if ( user.language[ 0 ] )
     {
          if ( ! ReadLanguage( user.language ) )
          {
               if ( ! ReadLanguage( cfg.defaultLanguage ) )
               {
                    fatalerror( "Default language not found" );
               }
          }


          readconfig();
     }


     if ( ! user.lastNewFilesCheck.ok() )
     {
          user.lastNewFilesCheck.today();
     }


     if ( user.lastDate  !=  Date( TODAY )  || 
          ( user.uFlags & UFLAG_GUEST ) )
     {
          user.timeUsed = 0;
          user.kbToday  = 0;
     }


     if ( user.level < min_level )
     {
          LOG( "User \"%s\" not allowed to logon", user.name );


          if ( showansascrip( "NOACCESS" )  ==  ANS_NOFILE )
          {
               io << "\7\n\n" 
                  << S_NO_ACCESS_ON_THIS_NODE 
                  << "\n\xFF";
          }


          sleep( 2 );

          exit_proboard();
     }


     if ( ( user.uFlags & UFLAG_LOCALONLY )  && 
          io.baud )
     {
          LOG( "User \"%s\" not allowed to logon from remote", user.name );

          io << "\7\n\n" 
             << S_NO_ACCESS_ON_THIS_NODE 
             << "\n\xFF";

          sleep( 2 );

          exit_proboard();
     }


     update_display();

     LOG( "User logged in: %s", user.name );


     if ( ! login_pex_found )
     {
          for ( i = 0;  io.baud  ||  ! cfg.allowquicklogin;  i++ )
          {
               io << "\n\n";


               if ( ! i )
               {
                    showansascrip( "BEFOREPW" );
               }


               io << S_ENTER_PASSWORD;

               io.read( password,
                        15,
                        READMODE_PWD );


               if ( user.passWord[ 0 ]  ==  '\0' )
               {
                    if ( RaCrc( password )  ==  user.passWordCRC )
                    {
                         break;
                    }
               }
               else
               {
                    if ( ! stricmp( user.passWord, password ) )
                    {
                         break;
                    }
               }


               LOG( "Wrong password: %s", password );

               io << "\n\n" 
                  << S_WRONG_PASSWORD;


               if ( i  ==  cfg.max_passinput - 1 )
               {
                    LOG( "User exceeded maximum password tries" );

                    
                    if ( cfg.pwdmessages  && 
                         ! ( user.uFlags & UFLAG_GUEST ) )
                    {
                         LOG( "Posting message to user & sysop" );

                         write_msgtmp( form( "Someone failed to login using your name on %d %s %d, at %02d:%02d\r\n",
                                             login_date[ 0 ],
                                             months_short[ login_date[ 1 ] ],
                                             login_date[ 2 ] + 1900,  // Y2K OK
                                             login_time[ 0 ],
                                             login_time[ 1 ] ) );

                         post_message( "ProBoard Security Manager",
                                       user.name,
                                       "Password violation",
                                       cfg.securityboard,
                                       TRUE);

                         write_msgtmp( form( "Someone failed to login as '%s' on %d %s %d, at %02d:%02d\r\n",
                                             user.name,
                                             login_date[ 0 ],
                                             months_short[ login_date[ 1 ] ], 
                                             login_date[ 2 ] + 1900,  // Y2K OK
                                             login_time[ 0 ],
                                             login_time[ 1 ] ) );

                         post_message( "ProBoard Security Manager",
                                       cfg.sysopname,
                                       "Password violation",
                                       cfg.securityboard,
                                       TRUE );
                    }


                    io << "\n\n" 
                       << S_LEAVE_MESSAGE_TO_SYSOP;

                    
                    if ( io.ask() )
                    {
                         io << '\n';

                         writemsg( form( "%d /T=Sysop",
                                         cfg.securityboard ) );
                    }


                    io << "\n\n" 
                       << S_HANGING_UP 
                       << "\n\xFF";

                    LOG( "Hanging up" );

                    sleep( 2 );

                    user_recnr = -1;


                    return -1;
               }
          }


          showansascrip( "AFTERPW" );
     }


     io << '\n';


     if ( cfg.asksex  && 
          ! user.sex )
     {
          io << '\n' 
             << S_ASK_SEX;


          char k  =  wait_language_hotkeys( K_ASK_SEX, FALSE );


          user.sex  =  k + 1;


          io  <<  ( k ? S_FEMALE
                      : S_MALE 
                  );

          io  <<  '\n';
     }


     if ( cfg.askBirthDay == 1  && 
          ! user.birthDate.ok() )
     {
          do
          {
               io << '\n' 
                  << S_ASK_BIRTHDATE( date_formats_long[ user.dateFormat ] );

               InputDate( user.birthDate, user.dateFormat );
          }
          while ( ! user.birthDate.ok() );


          io << '\n';
     }


     password_ok = TRUE;


     if ( ! cfg.allowansi ) 
     {
          user.uFlags &= ~UFLAG_ANSI;
     }


     if ( user.expDate[ 0 ] )
     {
          int ex  =  user.expDate  -  Date( TODAY );


          if ( ex <= 0 )
          {
               if ( user.expLevel < user.level )
               {
                    word oldlevel = user.level;
                    user.level    = user.expLevel;


                    for ( int f = 1;  f <= 32;  f++ )
                    {
                         if ( user.expFlagsOn.getflag( f ) )
                         {
                              user.aFlags.setflag( f );
                         }
                    }


                    for ( f = 1;  f <= 32;  f++ )
                    {
                         if ( user.expFlagsOff.getflag( f ) )
                         {
                              user.aFlags.clearflag( f );
                         }
                    }


                    run_sdkfile( form( "/I EXPIRED %u", oldlevel ) );


                    if ( showansascrip( "EXPIRED" )  ==  ANS_NOFILE )
                    {
                         io << '\n' 
                            << S_LEVEL_EXPIRED 
                            << "\n\n" 
                            << S_PRESS_ENTER_TO_CONTINUE;
                    }
               }
          }
          else
          {
               if ( ex <= 30 )
               {
                    if ( showansascrip( "EXP_WARN" )  ==  ANS_NOFILE )
                    {
                         io << '\n' 
                            << S_LEVEL_WILL_EXPIRE( form( "%d", ex ) ) 
                            << "\n\n";

                         io << S_PRESS_ENTER_TO_CONTINUE;
                    }
               }
          }
     }


     if ( user.level < 1 )
     {
          LOG( "Access denied (Level 0 - Locked out)" );

          io << '\n' 
             << S_LOCKED_OUT 
             << "\n\n" 
             << S_HANGING_UP 
             << '\xFF';

          sleep( 2 );

          exit_proboard();
     }


     if ( user.fileArea <= 0 ) 
     {
          user.fileArea = 1;
     }


     if ( user.msgArea  <= 0 ) 
     {
          user.msgArea  = 1;
     }


     if ( cfg.discrete  && 
          ! strcmpl( user.name, cfg.sysopname ) ) 
     {
          user.uFlags |= UFLAG_HIDDEN;
     }


     if ( user.uFlags & UFLAG_ATTEN )
     {
          if ( playsong( "ATTEN", 
                         "\x1b", 
                         0 )  ==  1 )
          {
               for ( i = 0;  i < 5;  i++ )
               {
                    sound( word( 1000  +  i * 400 ) );

                    msleep( 100 );
               }


               nosound();
          }
     }


     return 0;
}



//------------------------------  
// returns record number of user
//------------------------------  

static 
bool newuser( char *username )  
{
     if ( ! cfg.newuserlevel )
     {
          if ( showansascrip( "PRIVATE" )  ==  ANS_NOFILE )
          {
               io << "\n\n"
                  << S_PRIVATE_SYSTEM  <<  "\n\n"
                  << S_HANGING_UP      <<  '\xFF';
          }


          sleep( 2 );

          LOG( "Private System - New User rejected" );

          exit_proboard();
     }


     if ( cfg.newuserlevel < min_level )
     {
          if ( showansascrip( "NOACCESS" )  ==  ANS_NOFILE )
          {
               io << "\n\n" 
                  << S_NOT_FOR_NEW_USERS 
                  << "\n\xFF";
          }


          sleep( 2 );

          LOG( "New user rejected" );

          exit_proboard();
     }


     bool tmp_ansi = ansi_mode;


     user.newUser();
     
     strcpy( user.language, cfg.defaultLanguage );


     if ( tmp_ansi )
     {
          user.uFlags |= UFLAG_ANSI;
     }


     if ( cfg.europe )
     {
          user.dateFormat = 2;
     }


     strcpy( user.name, username );

     LOG( "Creating new user." );

     run_sdkfile( "/I NEWUSER1" );

     showansascrip( "NEWUSER1" );


     for ( ; ; )
     {
          int     num_languages = 0;
          DirScan scan( FileName( syspath, "*.PBL" ) );


          for ( ;  int( scan );  scan++, num_languages++ )
          {
               if ( String(scan.name()) == "_DEFAULT.PBL")
               {
                    num_languages--;
               }
          }


          if ( num_languages > 1 )
          {
               io << "\n\n";

               select_language();
          }


          if ( cfg.allowansi )
          {
               io << "\n\n" 
                  << S_ASK_ANSI;


               if ( io.ask( 1 ) )
               {
                    user.uFlags |= UFLAG_ANSI;
               }
          }


          if ( cfg.allowavatar )
          {
               io << "\n\n" 
                  << S_ASK_AVATAR;


               if ( io.ask( 0 ) )
               {
                    user.uFlags |= UFLAG_AVATAR;
               }


               if ( avatar )
               {
                    io << "\n\n" 
                       << S_ASK_AVATAR_PLUS;


                    if ( io.ask( 0 ) )
                    {
                         user.uFlags |= UFLAG_AVTPLUS;
                    }
               }
          }


          if ( ansi_mode )
          {
               io << "\n\n" 
                  << S_ASK_FSED;


               if ( io.ask( 1 ) )
               {
                    user.uFlags |= UFLAG_FSED;
               }
          }


          io << "\n\n" 
             << S_ASK_CLS;


          if ( io.ask( 1 ) )
          {
               user.uFlags |= UFLAG_CLEAR;
          }


          io << "\n\n" 
             << S_ASK_PAUSE;


          if ( io.ask( 1 ) )
          {
               user.uFlags |= UFLAG_PAUSE;
          }


          io << "\n\n" 
             << S_ASK_SCREENLENGTH( "24" );


          char length[ 3 ];


          io.read( length,
                   2,
                   READMODE_DIGITS );

          user.screenLength  =  atoi( length );
          
          
          if ( user.screenLength < 5 )
          {
               user.screenLength = 24;
          }


          do
          {
               do
               {
                    io << "\n\n" 
                       << S_ASK_CITY;

                    io.read( user.city, 25 );
               }
               while ( strlen( user.city )  <  3 );


               if ( cfg.askstate )
               {
                    io << "\n\n" 
                       << S_ASK_STATE;

                    io.read( user.state, 25 );
               }
          }
          while ( cfg.askstate  && 
                  strlen( user.state )  <  2 );


          if ( cfg.askcountry )
          {
               do
               {
                    io << "\n\n" 
                       << S_ASK_COUNTRY;

                    io.read( user.country, 25 );
               }
               while ( strlen( user.country )  <  2 );
          }


          if ( cfg.askaddress )
          {
               io << "\n\n" 
                  << S_ASK_MAILING_ADDRESS 
                  << "\n\n";

               io.read( user.address1, 50 );

               io << '\n';

               io.read( user.address2, 50 );

               io << '\n';

               io.read( user.address3, 50 );
          }


          if ( cfg.askvoicephone )
          {
               do
               {
                    io << "\n\n" 
                       << S_ASK_VOICE_PHONE;

                    io.read( user.voicePhone, 15 );
               }
               while ( strlen( user.voicePhone )  <  6 );
          }


          if ( cfg.askdataphone )
          {
               io << "\n\n" 
                  << S_ASK_DATA_PHONE;

               io.read( user.dataPhone, 15 );
          }


          if ( cfg.askfaxphone )
          {
               io << "\n\n" 
                  << S_ASK_FAX_PHONE;

               io.read( user.faxPhone, 15 );
          }


          if ( cfg.askdateformat )
          {
               io << "\n\n";

               change_dateformat();
          }


          if ( cfg.asksex )
          {
               io << "\n\n" 
                  << S_ASK_SEX;


               char k  =  wait_language_hotkeys( K_ASK_SEX, FALSE );


               user.sex  =  k + 1;

               io  <<  (
                         k ? S_FEMALE
                           : S_MALE
                       );
          }


          if ( cfg.askBirthDay )
          {
               do
               {
                    io << "\n\n" 
                       << S_ASK_BIRTHDATE( date_formats_long[ user.dateFormat ] );

                    InputDate( user.birthDate, user.dateFormat );


                    if ( ! user.birthDate.ok() )
                    {
                         if ( cfg.askBirthDay > 1 )
                         {
                              user.birthDate.set( 0, 0, 0 );
                              
                              break;
                         }
                         else 
                         {
                              continue;
                         }
                    }
               }
               while ( ! user.birthDate.ok() );
          }


          if ( cfg.allowalias )
          {
               for ( ; ; )
               {
                    user.alias[ 0 ]  =  0;

                    io << "\n\n" 
                       << S_ASK_ALIAS;

                    io.read( user.alias,
                             30,
                             READMODE_UPFIRST );

        
                    if ( ! user.alias[ 0 ] )
                    {
                         break;
                    }


                    User tmpuser;
                    
                    
                    if ( tmpuser.search( user.alias )  || 
                         tmpuser.search( user.alias,
                                         TRUE,
                                         FALSE,
                                         TRUE ) )
                    {
                         io << "\n\n" 
                            << S_ALIAS_BEING_USED;
                         
                         continue;
                    }
                    else 
                    {
                         break;
                    }
               }
          }


          char pass[ 16 ];


          for ( ; ; )
          {
               io << "\n\n" 
                  << S_ASK_PASSWORD( form( "%d", cfg.min_passlength ), "15" );

               io.read( user.passWord,
                        15,
                        READMODE_PWD );
      
      
               if ( strlen( user.passWord )  <  cfg.min_passlength )
               {
                    io << "\n\n" 
                       << S_ILLEGAL_PASSWORD;

                    continue;
               }


               io << "\n\n" 
                  << S_ASK_PASSWORD_AGAIN;

               io.read( pass,
                        15,
                        READMODE_PWD );

      
               if ( ! strcmpl( pass, user.passWord ) )
               {
                    break;
               }
          }


          password_ok = TRUE;

          user.level    = cfg.newuserlevel;
          user.logLevel = cfg.newuserloglevel;
          user.aFlags   = cfg.newuserflags;


          io << "\n\n\n" 
             << S_ASK_EVERYTHING_CORRECT;

          
          if ( io.ask( 1 ) )
          {
               break;
          }
          else
          {
               LOG( "Missed some questions, reentering..." );
          }
     }


     user.uFlags   |= UFLAG_HOTKEYS;
     user.fileArea  = 1;
     user.msgArea   = 1;


     io << '\n';

     LOG( "New user created." );

     run_sdkfile( "/I NEWUSER2" );

     showansascrip( "NEWUSER2" );
     questionnaire( "NEWUSER"  );

     user.lastDate.today();
     user.lastTime.now();
     user.firstDate.today();
     user.lastNewFilesCheck.today();


     return  user.write( TRUE );
}



static 
bool check_trashcan( char *username )
{
     File f;

   
     if ( ! f.open( FileName( syspath, "TRASHCAN.CTL" ),
                              fmode_read | fmode_text ) ) 
     {
          return FALSE;
     }


     for ( ; ; )
     {
          char s[ 80 ];


          if ( ! f.readLine( s, 79 ) )
          {
               return FALSE;
          }


          strip_linefeed( s );


          if ( ! strcmpl( s, username ) )
          {
               return TRUE;
          }
     }
}



void show_intro()
{
     char *notregstr = " [UNREGISTERED] Ä Node %d\nCopyright (c) 1990-1999  TeleGrafix Communications, Inc.  All Rights Reserved\n";
//   char *notregstr = " [UNREGISTERED] Ä Node %d Ä Copyright (c) 1990,99  TeleGrafix Communications\n\n";
//   char *notregstr = " [UNREGISTERED]  ÄÄ Node %d ÄÄ   (c) 1990-1997  Philippe Leybaert\n\n";
     long  checksum  = 0;
     int   i;


     for ( i = 0;  i  <  strlen( notregstr );  i++ )
     {
          checksum  +=  notregstr[ i ];
     }

     //-------------------------------------------  
     // Magic registration/copyright futzing check
     //-------------------------------------------  

     if ( checksum != 8235 ) 
     {
          exit_proboard();
     }


     io << "\x1b[5n\x1b[!\r";

     io << "\fProBoard v" VERSION;


     //----------------------------  
     // JDR: REGISTRATION REFERENCE
     //----------------------------  

     if ( registered )
     {
          int l  =  38  -  strlen( VERSION );
          char *pUserName;


          pUserName  =  registered_user_name();


          if (  cfg.numnodes > 1 )
          {
               io  <<  form( "  ÄÄ Node %d ÄÄ   ", node_number );
          }
          else 
          {
               l += 17;
          }


          for ( i = 0;  i  <  l - strlen( pUserName );  i++ )
          {
               io << ' ';
          }


          checksum = 0;

          //char *regtostr = "Registered to %s\n\n";
		  char* regtostr = "                      Freeware\n\n";


          /*for ( i = 0;  i  <  strlen( regtostr );  i++ )
          {
               checksum  +=  regtostr[ i ];
          }


          if ( checksum != 1501 ) 
          {
               exit_proboard();
          } */


          //io  <<  form( regtostr, pUserName )  <<  char( 0xFF );
		  io << regtostr << char(0xFF);
     }
     else
     {
          io  <<  form( notregstr, node_number )  <<  char( 0xFF );

          
          if ( io.baud ) 
          {
               sleep( 2 );
          }
     }


  // io  <<  form( "Checksum=%ld\n", checksum );


     String response;
     dword  ticks = clockticks();


     for ( ;  clockdiff( ticks )  <  30  &&  io.baud; )
     {
          char k = io.readkey();


          if ( ! k )  
          {
               continue;
          }


          response << k;


          if ( strstr( response, "RIPSCRIP" ) )
          {
               rip_detected = TRUE;
          }


          if ( strstr( response, "\x1b[" ) )
          {
               user.uFlags |= UFLAG_ANSI;
          }


          if ( rip_detected  &&  
               ( user.uFlags & UFLAG_ANSI ) ) 
          {
               break;
          }
   }


     if ( rip_detected ) 
     {
          for ( ticks = clockticks();  
                clockdiff( ticks )  <  18  && 
                io.baud;
              ) 
          {
               io.readkey();
          }
     }


     if ( cfg.disableRIP ) 
     {
          rip_detected = FALSE;
     }


     if ( rip_detected ) 
     {
          LOG( "*** RIP DETECTED ***" );
     }


     if ( ! io.baud )
     {
          user.uFlags |= UFLAG_ANSI;
     }


     rip_send_icons();


     if ( ! show_rip( form( "INTRO%d", node_number ) ) )
     {
          if ( ! show_rip( "INTRO" ) )
          {
               if ( showansasc( form( "INTRO%d", node_number ),
                                NULL )  ==  ANS_NOFILE )
               {
                    if ( showansasc( "INTRO", NULL )  ==  ANS_NOFILE )
                    {
                         showansasc( "LOGO" );
                    }
               }
          }
          else
          {
               rip_text_window( 0,
                                21,
                                79,
                                24,
                                2 );
          }
     }
}
