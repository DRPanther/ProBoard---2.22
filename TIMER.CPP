#define Use_MsgBase

#include <stdio.h>
#include <dos.h>
#include <time.h>
#include <tswin.hpp>
#include <string.h>
#include "proboard.hpp"



//**************************************************************************
//
// Update the main console's user display area
//
//     Prototype:  void update_display();
//
//    Parameters:  None
//
//       Returns:  Nothing
//
//       Remarks:
//
// This routine is primarly used to update the status bar.
//
// -------------------------------------------------------------------------
//
//    Created on:  ??/??/???? (Philippe Leybaert)
// Last modified:  08/05/1999 (Jeff Reeder)   Modified to change the color
//                            of the status bar, and fix the DOB Y2K glitch.
//
//**************************************************************************

void update_display()
{
     static int oldmode = 0;

     update_user_window();

     
     if ( user_recnr >= 0 )
     {
          char s[ 100 ];
          
          
          if ( oldmode != display_mode )
          {
               tsw_clearrect( 1,
                              tsw_vsize,
                              tsw_hsize,
                              tsw_vsize,
                              BAR_COLOR );

               oldmode = display_mode;
          }


          tsw_maputs( 1,
                      tsw_vsize,
                      BAR_COLOR,
                      String( ' ', tsw_hsize ) );


          if ( num_yells ) 
          {
               tsw_changeatt( BAR_COLOR | 0x80,
                              1,
                              tsw_vsize,
                              tsw_hsize,
                              tsw_vsize );
          }


          switch ( display_mode )
          {
               case 1: 
               
                    sprintf( s,
                             "%s <%u>",
                             user.name,user.level );

                    tsw_mputs( 2,
                               tsw_vsize,
                               s );

                    if ( sysop_next )
                    {
                         tsw_changeatt( BAR_COLOR | 0x80,
                                        1,
                                        tsw_vsize,
                                        strlen( s )  +  1,
                                        tsw_vsize );
                    }


                    sprintf( s,
                             " [%ld bps]  [Online: %d]  [Left: %d]",
                             io.baud,
                             timer.online(),
                             timer.left() );

                    tsw_mputs( tsw_hsize  -  strlen( s )  -  ( rip_mode 
                                                                 ? 2 
                                                                 : 0 ),
                               tsw_vsize,
                               s );


                    if ( rip_mode )
                    {
                         tsw_maputc( tsw_hsize - 1,
                                     tsw_vsize,
                                     BAR_COLOR | 0x80,
                                     'R' );
                    }

                    break;


               case 2: 
               
                    sprintf( s,
                             "[Handle: %s]",
                             user.alias );

                    tsw_mputs( 2,
                               tsw_vsize,
                               s );

                    sprintf( s, "[Flags: " );

                    user.aFlags.flagstostr( & s[ 8 ] );

                    strcat( s, "]" );

                    tsw_mputs( tsw_hsize  -  strlen( s ),
                               tsw_vsize,
                               s );
                    
                    break;


               case 3: 
               
                    sprintf( s,
                             "[City: %s",
                             user.city );


                    if ( user.country[ 0 ] )
                    {
                         strcat( s, ", " );
                         strcat( s, user.country );
                    }


                    strcat( s, "]" );

                    tsw_mputs( 2,
                               tsw_vsize,
                               s );

                    sprintf( s,
                             "[Data: %s]  [Tel: %s]",
                             user.dataPhone,
                             user.voicePhone );

                    tsw_mputs( tsw_hsize  -  strlen( s ),
                               tsw_vsize,
                               s );
                    
                    break;


               case 4: 
               
                    sprintf( s,
                             "[#Calls: %ld]     [#DL: %ld]     [K DL: %ld]     [#UL: %ld]     [K UL: %ld]",
                             user.timesCalled,
                             user.numDownloads,
                             user.kbDownloaded,
                             user.numUploads,
                             user.kbUploaded );

                    tsw_centerline( tsw_vsize, s );
                    
                    break;


               case 5: 
               
                    sprintf( s,
                             "[%s]",
                             user.comment );

                    tsw_centerline( tsw_vsize, s );
                    
                    break;


               case 6: 
               
                    sprintf( s,
                             "[System calls: %ld]        [#Msgs: %ld]        [#Users: %d]",
                             totalcalls,
                             msgbase.totalMsgs(),
                             num_users );

                    tsw_centerline( tsw_vsize, s );
                    
                    break;


               case 7: 
               
                    sprintf( s, 
                             "[Chat: %s]",
                             page_reason );

                    tsw_centerline( tsw_vsize, s );
                    
                    break;


               case 8: 
               
                    sprintf( s,
                             "[Last Caller: %s]",
                             lastcaller.name );

                    tsw_mputs( 2, 
                               tsw_vsize,
                               s );

                    if ( lastcaller.date[ 0 ]  ==  0  &&
                         lastcaller.date[ 1 ]  ==  0  &&
                         lastcaller.date[ 2 ]  ==  0 )
                    {
                         sprintf( s,
                                  "[%d-%s-%04d] [%02d:%02d-%02d:%02d]",
                                  00,
                                  "???",
                                  0,
                                  lastcaller.timeIn [ 0 ],
                                  lastcaller.timeIn [ 1 ],
                                  lastcaller.timeOut[ 0 ],
                                  lastcaller.timeOut[ 1 ]);
                    }
                    else
                    {
                         sprintf( s,
                                  "[%d-%s-%04d] [%02d:%02d-%02d:%02d]",
                                  lastcaller.date[ 0 ],
                                  months_short[ lastcaller.date[ 1 ] ],
                                  lastcaller.date[ 2 ]  +  1900,
                                  lastcaller.timeIn [ 0 ],
                                  lastcaller.timeIn [ 1 ],
                                  lastcaller.timeOut[ 0 ],
                                  lastcaller.timeOut[ 1 ]);
                    }

                    tsw_mputs( tsw_hsize  -  strlen( s ),
                               tsw_vsize,
                               s );
                    
                    break;


               case 9: 
               
                    sprintf( s,
                             "[DOB: %02d-%s-%04d]   [Time used today: %ld]   [Kbytes DL today: %d]",
                             user.birthDate[ 0 ],
                             months_short[ user.birthDate[ 1 ] ],
                             user.birthDate[ 2 ]  +  1900,
                             long( user.timeUsed + timer.used() ),
                             user.kbToday );

                    tsw_centerline( tsw_vsize, s );
                    
                    break;
          }
     }
     else 
     {
          tsw_selbar( tsw_vsize,
                      1,
                      tsw_hsize,
                      BAR_COLOR );
     }
}



void
usertimer::decrease()
{
 suspendedtime--;
 check();
}

void
usertimer::increase()
{
 suspendedtime++;
 check();
}

usertimer::usertimer()
{
 started = (time_t)0;
 suspended = 0;
 suspendedtime = 0;
 fixedtime = 0;
}

void
usertimer::start(int min)
{
 started       = time(NULL);
 suspended     = FALSE;
 suspendedtime = 0;
 fixedtime     = 0;

 time_limit    = min;
}

void
usertimer::suspend()
{
 if(!suspended) suspend_start = time(NULL);

 suspended++;
}

void
usertimer::restart()
{
 if(!suspended) return;

 suspended--;
 if(!suspended) suspendedtime += int((time(NULL)-suspend_start)/60L);
}

int
usertimer::online()
{
 if(!started) return 0;
         else return (int)((time(NULL)-started)/60L);
}

int
usertimer::left()
{
 int x = time_limit - online() + suspendedtime + fixedtime;

 if(suspended) x += int((time(NULL)-suspend_start)/60L);

 return x;
}

void
usertimer::changeleft(int l)
{
 fixedtime = l - time_limit - suspendedtime + online();
}

int
usertimer::used()
{
 return online() - suspendedtime;
}



void
usertimer::check()
{
static bool warning = FALSE;

if(!started) return;

int l=left();

if(l>2) warning = FALSE;

if(l<0)
  {
   io << '\n' << S_TIME_LIMIT_EXCEEDED << '\xFF';

   LOG("Time limit! User thrown out.");
   sleep(2);
   exit_proboard();
  }

if(l<=2 && !warning)
  {
   io << '\7' << '\7' << '\n' << S_TIME_LIMIT_WARNING("2") << "\n\xFF";

   warning = TRUE;
  }
}

void
usertimer::checkinactivity()
{
static bool warning = FALSE;

if(!io.baud || !cfg.inactivity_time) return;

int inactive=(int)(time(NULL)-inactivity_start);

if(inactive > cfg.inactivity_time+1000)
  {
   clearinactivity();
   return;
  }

if(inactive < cfg.inactivity_time-20)
  {
   warning = FALSE;
   return;
  }

if(inactive >= cfg.inactivity_time)
   {
      io << '\n' << S_INACTIVITY_TIMEOUT(form("%d",cfg.inactivity_time)) << "\n\xFF";

    LOG("Inactivity. User thrown out.");
    sleep(2);
    exit_proboard();
   }

 if(warning) return;

 io << '\7';
 io << '\7' << '\n' << S_INACTIVITY_WARNING("20") << "\n\xFF";

 LOG(2,"Inactivity warning");

 warning = TRUE;
}

void
usertimer::clearinactivity()
{
inactivity_start=time(NULL);
}
