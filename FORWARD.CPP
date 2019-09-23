#define Use_MsgBase

#include <string.h>
#include "proboard.hpp"

void
Message::forward()
{
     int     areanum = 0;
     MsgArea ma;
     char    newto[ 36 ];


     if ( msgArea->msgKind == MSG_LOCAL ) 
     {
          io << "\n\n"
             << S_FORWARD_IN_SAME_AREA;
     }


     if ( msgArea->msgKind != MSG_LOCAL  || 
          ! io.ask( TRUE ) )
     {
          io << "\n\f\n";

          linecounter( 0 );

          set_msgarea( "* -N /L" );

          io << '\n' 
             << S_SELECT_MSG_AREA_TO_FORWARD_MESSAGE_IN;

          io.read( areanum, 5 );


          if ( areanum < 1 ) 
          {
               return;
          }
     }
     else 
     {
          areanum = areaNum();
     }


     if ( ! ma.read( areanum ) )
     {
          io << "\n\n" 
             << S_UNKNOWN_MESSAGE_AREA
             << "\n\n" 
             << S_PRESS_ENTER_TO_CONTINUE;

          return;
     }


     for ( ; ; )
     {
          User tuser;


          io << "\n\n" 
             << S_FORWARD_MESSAGE_TO;

          io.read( newto,
                   35,
                   READMODE_UPFIRST );


          if ( ! newto[ 0 ] )
          {
               return;
          }


          if ( ! strcmpl( newto, "Sysop" ) )
          {
               strcpy( newto, msgArea->sysop );
          }


          if ( ma.msgKind != MSG_LOCAL  || 
               tuser.search( newto ) )
          {
               break;
          }


          io << "\n\n" 
             << S_USER_NOT_LOCATED 
             << '\n';
     }


     io << "\n\n" 
        << S_FORWARDING_MESSAGE;


     File tmpf;


     if ( ! tmpf.open( "MSGTMP", fmode_create ) )
     {
          return;
     }


     tmpf << "----------------------------------------------------------------------\r\n";
     
     tmpf << form( " ** Original message to     : %s\r\n\r\n", 
                   to );

     tmpf << form( " ** Original post date/time : %d-%s-%02d  %02d:%02d\r\n\r\n",
                   postDate[ 0 ],
                   months_short[ postDate[ 1 ] ],
                   postDate[ 2 ] % 100,      // Y2K FIX: JDR
                // postDate[ 2 ],            // Y2K BUG!  FIXED
                   postTime[ 0 ],
                   postTime[ 1 ] );

     tmpf << form( " ** Forwarded by %s using ProBoard v" VERSION "\r\n",
                   user.name);

     tmpf << "----------------------------------------------------------------------\r\n\r\n";


     tmpf.close();

     createMsgTextFile( "MSGTMP", TRUE );


     if ( post_message( from,
                        newto,
                        subj,
                        areanum,
                        ( attr & MSGATTR_PRIVATE )
                          ? TRUE 
                          : FALSE )  <  0 )
     {
          io << S_SAVING_MESSAGE_ERROR;
     }


     io << "\n\n" 
        << S_PRESS_ENTER_TO_CONTINUE;
}

