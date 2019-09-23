#define Use_MsgBase

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "proboard.hpp"



void User::list( char *s,
                 bool  aliases )
{
     struct User tuser;


     io << S_USER_LIST_HEADER 
        << '\n';


     linecounter( 2 );

     io.enablestop();
     
     
     for ( int rec = 0;  ;  rec++ )
     {
          if ( ! tuser.read( rec ) )
          {
               break;
          }


          if ( 
               tuser.uFlags  &  ( 
                                  UFLAG_DELETED  | 
                                  UFLAG_HIDDEN 
                                )
             )
          {
               continue;
          }


          if ( s[ 0 ]  && 
               fuzzy_search( s,
                             ( aliases 
                                 ? tuser.alias
                                 : tuser.name ),

                             75 )  <  0 ) 
          {
               continue;
          }


          if ( ! strcmpl( ( aliases 
                              ? tuser.alias
                              : tuser.name ),
                              
                          cfg.sysopname )  && 
                          
               cfg.discrete )
          {
               continue;
          }


          String id;


          for ( int i = 0;  i < num_limits;  i++ )
          {
               if ( limit[ i ].level  ==  tuser.level ) 
               {
                    id  =  limit[ i ].id;
               }
          }


          if ( io.send( form( "\3%-30.30s \6%-18.18s  \2%02d %s %02d, %02d:%02d \6%5ld \7%s\n",
                              ( aliases 
                                  ? tuser.alias
                                  : tuser.name ),
                              tuser.city,
                              tuser.lastDate[ 0 ],
                              months_short[ tuser.lastDate[ 1 ] ],
                              tuser.lastDate[ 2 ]  %  100,    // Y2K FIXED!
                           // tuser.lastDate[ 2 ],            // Y2K BUG!
                              tuser.lastTime[ 0 ],
                              tuser.lastTime[ 1 ],
                              tuser.timesCalled,
                              (char *) id ) ) ) 
          {
               break;
          }


          if ( linecounter() ) 
          {
               break;
          }
     }
}
