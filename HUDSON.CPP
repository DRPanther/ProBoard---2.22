#define Use_MsgBase

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "proboard.hpp"



struct HudsonMsgInfo
{
     int low;
     int high;
     int total;
     int active[ 200 ];
};



struct HudsonMsgIdx
{
     int  num;
     byte area;
};



struct HudsonMsgToIdx
{
     char to[ 36 ];
};



struct HudsonMsgHdr
{
     int   msgNum;
     int   prevMsg;
     int   nextMsg;
     int   tRead;
     word  startRec;
     int   numRecs;
     int   destNet;
     int   destNode;
     int   orgNet;
     int   orgNode;
     char  destZone;
     char  orgZone;
     int   cost;
     byte  msgAttr;
     byte  netAttr;
     byte  area;
     char  postTime[ 6 ];
     char  postDate[ 9 ];
     char  to  [ 36 ];
     char  from[ 36 ];
     char  subj[ 67 ];
     Date  recvDate;
     Time  recvTime;

     void  operator=( Message& );
};



const byte QMSGATTR_DELETED       = 0x01;
const byte QMSGATTR_UNMOVED_NET   = 0x02;
const byte QMSGATTR_NET           = 0x04;
const byte QMSGATTR_PVT           = 0x08;
const byte QMSGATTR_RECEIVED      = 0x10;
const byte QMSGATTR_UNMOVED_ECHO  = 0x20;
const byte QMSGATTR_LOCAL         = 0x40;

const byte QNETATTR_KILL          = 0x01;
const byte QNETATTR_SENT          = 0x02;
const byte QNETATTR_ATTACH        = 0x04;
const byte QNETATTR_CRASH         = 0x08;
const byte QNETATTR_REQ           = 0x10;
const byte QNETATTR_AUD           = 0x20;
const byte QNETATTR_RET           = 0x40;

const int  CACHE_NEXT             = 1;
const int  CACHE_PREV             = 2;
const int  CACHE_START            = 3;



HudsonMsgBase::HudsonMsgBase()
{
     lastMsgIdx    = new HudsonMsgIdx;
     lastReadArray = NULL;
     highArray     = NULL;

     open();
}



HudsonMsgBase::~HudsonMsgBase()
{
     close();

     delete lastMsgIdx;


     if ( highArray )    
     {
          delete [] highArray;
     }


     if ( lastReadArray ) 
     {
          delete [] lastReadArray;
     }
}



void HudsonMsgHdr::operator=( Message &msg )
{
     msg.to[ 35 ]  =  msg.from[ 35 ]
                   =  msg.subj[ 65 ]  =  '\0';         // Safety measure


     msgNum    =  int( msg.id   );
     prevMsg   =  int( msg.prev );
     nextMsg   =  int( msg.next );
     tRead     =  0;
     destNet   =  msg.destNet;
     destZone  =  msg.destZone;
     destNode  =  msg.destNode;
     orgNet    =  msg.origNet;
     orgZone   =  msg.origZone;
     orgNode   =  msg.origNode;
     cost      =  msg.cost;
     area      =  msg.areaNum();

     msgAttr = 0;
     netAttr = 0;


     if (     msg.attr & MSGATTR_ECHOMAIL  && 
          ! ( msg.attr & MSGATTR_SENT  )   &&   
              msg.attr & MSGATTR_LOCAL )
     {
          msgAttr |= QMSGATTR_UNMOVED_ECHO;
     }


     if (     msg.attr & MSGATTR_NETMAIL  && 
          ! ( msg.attr & MSGATTR_SENT  )   &&   
              msg.attr & MSGATTR_LOCAL )
     {
          msgAttr |= QMSGATTR_UNMOVED_NET;
     }


     if ( msg.attr & MSGATTR_DELETED     )   msgAttr |= QMSGATTR_DELETED;
  // if ( msg.attr & MSGATTR_UNSENT_NET  )   msgAttr |= QMSGATTR_UNMOVED_NET;
     if ( msg.attr & MSGATTR_NETMAIL     )   msgAttr |= QMSGATTR_NET;
     if ( msg.attr & MSGATTR_PRIVATE     )   msgAttr |= QMSGATTR_PVT;
     if ( msg.attr & MSGATTR_RECEIVED    )   msgAttr |= QMSGATTR_RECEIVED;
  // if ( msg.attr & MSGATTR_UNSENT_ECHO )   msgAttr |= QMSGATTR_UNMOVED_ECHO;
     if ( msg.attr & MSGATTR_LOCAL       )   msgAttr |= QMSGATTR_LOCAL;

     if ( msg.attr & MSGATTR_KILL        )   netAttr |= QNETATTR_KILL;
     if ( msg.attr & MSGATTR_CRASH       )   netAttr |= QNETATTR_CRASH;
     if ( msg.attr & MSGATTR_SENT        )   netAttr |= QNETATTR_SENT;
     if ( msg.attr & MSGATTR_FILE        )   netAttr |= QNETATTR_ATTACH;
     if ( msg.attr & MSGATTR_FILEREQ     )   netAttr |= QNETATTR_REQ;
     if ( msg.attr & MSGATTR_AUDITREQ    )   netAttr |= QNETATTR_AUD;
     if ( msg.attr & MSGATTR_RECEIPTREQ  )   netAttr |= QNETATTR_RET;


     sprintf( postTime,
              "%02d:%02d",
              msg.postTime[ 0 ],
              msg.postTime[ 1 ] );

     c2pas( postTime );


     sprintf( postDate,
              "%02d-%02d-%02d",
              msg.postDate[ 1 ],
              msg.postDate[ 0 ],
           // msg.postDate[ 2 ] );                // Y2K BUG!
              msg.postDate[ 2 ]  %  100 );        // JDR: Y2K FIXED!

     c2pas( postDate );


     strcpy( to, msg.to );

     c2pas( to );


     strcpy( from, msg.from );

     c2pas( from );


     strcpy( subj, msg.subj );

     c2pas( subj );


     recvDate = msg.recvDate;
     recvTime = msg.recvTime;
}



void Message::operator=( HudsonMsgHdr& mh )
{
     num      = mh.msgNum;
     id       = mh.msgNum;
     prev     = mh.prevMsg;
     next     = mh.nextMsg;
     destNet  = mh.destNet;
     destZone = mh.destZone;
     destNode = mh.destNode;
     origNet  = mh.orgNet;
     origZone = mh.orgZone;
     origNode = mh.orgNode;
     cost     = mh.cost;

     attr = 0;


     if ( mh.msgAttr & QMSGATTR_DELETED      )   attr |= MSGATTR_DELETED;
     if ( mh.msgAttr & QMSGATTR_UNMOVED_NET  )   attr |= MSGATTR_NETMAIL;
     if ( mh.msgAttr & QMSGATTR_NET          )   attr |= MSGATTR_NETMAIL;
     if ( mh.msgAttr & QMSGATTR_PVT          )   attr |= MSGATTR_PRIVATE;
     if ( mh.msgAttr & QMSGATTR_RECEIVED     )   attr |= MSGATTR_RECEIVED;
     if ( mh.msgAttr & QMSGATTR_UNMOVED_ECHO )   attr |= MSGATTR_ECHOMAIL;
     if ( mh.msgAttr & QMSGATTR_LOCAL        )   attr |= MSGATTR_LOCAL;
                                                 
     if ( mh.netAttr & QNETATTR_KILL         )   attr |= MSGATTR_KILL;
     if ( mh.netAttr & QNETATTR_SENT         )   attr |= MSGATTR_SENT;
     if ( mh.netAttr & QNETATTR_ATTACH       )   attr |= MSGATTR_FILE;
     if ( mh.netAttr & QNETATTR_CRASH        )   attr |= MSGATTR_CRASH;
     if ( mh.netAttr & QNETATTR_REQ          )   attr |= MSGATTR_FILEREQ;
     if ( mh.netAttr & QNETATTR_AUD          )   attr |= MSGATTR_AUDITREQ;
     if ( mh.netAttr & QNETATTR_RET          )   attr |= MSGATTR_RECEIPTREQ;


     if ( attr & MSGATTR_LOCAL )
     {
          if ( ! ( mh.msgAttr & QMSGATTR_UNMOVED_ECHO )  && 
               ! ( mh.msgAttr & QMSGATTR_UNMOVED_NET  ) )
          {
               attr |= MSGATTR_SENT;
          }
     }


     setArea( mh.area );


     mh.postTime[ 0 ]  =  min( (byte) mh.postTime[ 0 ],  5 );

     pas2c( mh.postTime );


     postTime[ 0 ]  =  atoi( strtok( mh.postTime, ":" ) );
     postTime[ 1 ]  =  atoi( strtok( NULL,        ":" ) );
     postTime[ 2 ]  =  0;


     mh.postDate[ 0 ]  =  min( (byte) mh.postDate[ 0 ],  8 );

     pas2c( mh.postDate );


     postDate[ 1 ]  =  atoi( strtok( mh.postDate, "-" ) );
     postDate[ 0 ]  =  atoi( strtok( NULL,        "-" ) );
  // postDate[ 2 ]  =  atoi( strtok( NULL,        "-" ) );       // Y2K BUG!
     postDate[ 2 ]  =  atoi( strtok( NULL,        "-" ) ) % 100; // JDR: Y2K FIX!


     mh.to[ 0 ]  =  min( (byte) mh.to[ 0 ],  35 );

     pas2c(  mh.to );
     strcpy( to, mh.to );


     mh.from[ 0 ]  =  min( (byte) mh.from[ 0 ],  35 );

     pas2c(  mh.from );
     strcpy( from, mh.from );


     mh.subj[ 0 ]  =  min( (byte) mh.subj[ 0 ],  65 );

     pas2c(  mh.subj );
     strcpy( subj, mh.subj );


     recvDate = mh.recvDate;
     recvTime = mh.recvTime;
}



bool HudsonMsgBase::open()
{
     FileName  fn_msghdr(   cfg.msgpath, "MSGHDR.BBS"   );
     FileName  fn_msgtxt(   cfg.msgpath, "MSGTXT.BBS"   );
     FileName  fn_msgtoidx( cfg.msgpath, "MSGTOIDX.BBS" );
     FileName  fn_msgidx(   cfg.msgpath, "MSGIDX.BBS"   );
     FileName  fn_msginfo(  cfg.msgpath, "MSGINFO.BBS"  );


     if ( ! f_msghdr  .open( fn_msghdr,   fmode_rw | fmode_copen       )  || 
          ! f_msgtxt  .open( fn_msgtxt,   fmode_rw | fmode_copen       )  || 
          ! f_msgtoidx.open( fn_msgtoidx, fmode_rw | fmode_copen, 2048 )  || 
          ! f_msgidx  .open( fn_msgidx,   fmode_rw | fmode_copen, 1024 )  || 
          ! f_msginfo .open( fn_msginfo,  fmode_rw | fmode_copen ) )
     {
          if ( f_msghdr  .error() )   file_error( fn_msghdr   );
          if ( f_msgtxt  .error() )   file_error( fn_msgtxt   );
          if ( f_msgtoidx.error() )   file_error( fn_msgtoidx );
          if ( f_msgidx  .error() )   file_error( fn_msgidx   );
          if ( f_msginfo .error() )   file_error( fn_msginfo  );

          fatalerror( "Error creating Hudson message base" );

          return FALSE;
     }

     flushCache();

     return TRUE;
}



void HudsonMsgBase::close()
{
     flushCache();

     f_msghdr  .close();
     f_msgtxt  .close();
     f_msgtoidx.close();
     f_msgidx  .close();
     f_msginfo .close();
}



long HudsonMsgBase::highMsg( MsgArea& ma )
{
     readHighArray();

     return  long( highArray[ ma.areaNum - 1 ] );
}



long HudsonMsgBase::lowMsg( MsgArea& )
{
     HudsonMsgInfo mi;

     readInfo( mi );

     return  long( mi.low );
}



long HudsonMsgBase::numMsgs( MsgArea& ma )
{
     HudsonMsgInfo mi;

     readInfo( mi );

     return  long( mi.active[ ma.areaNum - 1 ] );
}



long HudsonMsgBase::totalMsgs()
{
     HudsonMsgInfo mi;

     readInfo( mi );

     return  long( mi.total );
}



void HudsonMsgBase::readInfo( HudsonMsgInfo &mi )
{
     CLEAR_OBJECT( mi );

     f_msginfo.rewind();
     f_msginfo.flush();

     f_msginfo.read( & mi,  
                     sizeof( mi ) );
}



void HudsonMsgBase::writeInfo( HudsonMsgInfo &mi )
{
     f_msginfo.rewind();

     f_msginfo.write( & mi,
                      sizeof( mi ) );

     f_msginfo.flush();
}



long HudsonMsgBase::readMsg( Message &msg,
                             long     num )
{
     HudsonMsgIdx mi;
     int          order = 1;


     f_msgidx.rewind();


     if ( num < 0 )
     {
          order = -1;
          num   = -num;
     }


     long  index  =  findIdx( mi, 
                              int( num ), 
                              msg.areaNum(),
                              order );


     if ( index < 0 ) 
     {
          return -1;
     }


     HudsonMsgHdr hdr;


     f_msghdr.seek( index  *  sizeof( hdr ) );


     if ( f_msghdr.read( & hdr,
                         sizeof( hdr ) )  !=  sizeof( hdr ) ) 
     {
          return -1;
     }


     msg = hdr;     // Conversion applied!

     return msg.id;
}



void HudsonMsgBase::flushCache()
{
     lastReadRecord   = -1;
     lastIndex        = -1;
     lastMsgIdx->num  = -1;
     lastMsgIdx->area = 0;


     if ( highArray != NULL )  
     {
          delete [] highArray;
     }


     if ( lastReadArray != NULL ) 
     {
          delete [] lastReadArray;
     }


     highArray     = NULL;
     lastReadArray = NULL;
}



void HudsonMsgBase::readHighArray()
{
     if ( highArray ) 
     {
          return;
     }


     highArray  =  new int[ 200 ];


     HudsonMsgIdx mi;


     memset( highArray,
             0,
             sizeof( *highArray )  *  200 );

     f_msgidx.seek( 0 );


     for ( ; ; )
     {
          if ( f_msgidx.read( & mi,
                              sizeof( mi ) )  !=  sizeof( mi ) )
          {
               break;
          }


          if ( mi.num   >  0    && 
               mi.area  >  0    && 
               mi.area  <  200  && 
               mi.num   >  highArray[ mi.area - 1 ] )
          {
               highArray[ mi.area - 1 ]  =  mi.num;
          }
     }
}



long HudsonMsgBase::findIdx( HudsonMsgIdx &mi,
                             int           num,
                             int           area,
                             int           order )
{
     long idx = -1;


     if ( order < 0 ) 
     {
          idx  =  f_msgidx.len() / 3;
     }


     if ( lastMsgIdx->num  == num  && 
          lastMsgIdx->area == area )
     {
          mi = *lastMsgIdx;

          return lastIndex;
     }


     if ( order == 0 ) 
     {
          flushCache();
     }


     if ( lastMsgIdx->num > 0 )
     {
          if ( num   > lastMsgIdx->num  && 
               order > 0 )
          {
               idx = lastIndex;
          }


          if ( num   < lastMsgIdx->num  && 
               order < 0 )
          {
               idx = lastIndex;
          }
     }


     if ( order < 0 )
     {
          f_msgidx.disableBuffer();


          for ( idx--;  idx >= 0;  idx-- )
          {
               f_msgidx.seek( idx  *  sizeof( HudsonMsgIdx ) );

               f_msgidx.read( lastMsgIdx, sizeof( *lastMsgIdx ) );


               if ( lastMsgIdx->num < 1 ) 
               {
                    continue;
               }


               if ( lastMsgIdx->num  <= num  && 
                    lastMsgIdx->area == area )
               {
                    lastIndex = idx;
                    mi        = *lastMsgIdx;

                    f_msgidx.enableBuffer();

                    return idx;
               }
          }


          f_msgidx.enableBuffer();
     }
     else      
     {
          //----  
          // >=0
          //----  

          for ( idx++;  ;  idx++ )
          {
               f_msgidx.seek( idx  *  sizeof( HudsonMsgIdx ) );


               if ( f_msgidx.read( lastMsgIdx,
                                   sizeof( *lastMsgIdx ) )  !=  sizeof( *lastMsgIdx ) )
               {
                    break;
               }


               if ( lastMsgIdx->area == area )
               {
                    if ( 
                         lastMsgIdx->num == num  || 
                         (
                           lastMsgIdx->num >= num  && 
                           order           >  0 
                         )
                       )
                    {
                         lastIndex = idx;
                         mi        = *lastMsgIdx;

                         return idx;
                    }
               }
          }
     }


     flushCache();

     return -1;
}



word HudsonMsgBase::readMsgText( Message &msg,
                                 char    *ptr,
                                 long     offset, 
                                 word     size )
{
     HudsonMsgIdx mi;
     HudsonMsgHdr hdr;
     long         index  =  findIdx( mi,
                                     int( msg.id ),
                                     msg.areaNum(),
                                     0 );


     if ( index < 0 ) 
     {
          return 0;
     }


     f_msghdr.seek( index  *  sizeof( HudsonMsgHdr ) );


     if ( f_msghdr.read( & hdr,
                         sizeof( hdr ) )  !=  sizeof( hdr ) ) 
     {
          return 0;
     }


     byte  buf[ 256 ];
     long  rec          =  hdr.startRec  +  offset / 255;
     word  bytescopied  =  0;
     int   buf_index    =  int( offset % 255 );


     for ( ;  
           
           rec          <  ( hdr.startRec + hdr.numRecs )  &&  
           bytescopied  <  size; 

           rec++ )
     {
          f_msgtxt.seek( rec * 256 );
          f_msgtxt.read( buf,  256 );


          int  l  =  int( buf[ 0 ] );

          pas2c( buf );

          l -= buf_index;


          if ( l < 1 ) 
          {
               break;
          }


          if ( l > size-bytescopied ) 
          {
               l  =  size - bytescopied;
          }


          memcpy( ptr + bytescopied, 
                  & buf[ buf_index ],
                  l );

          bytescopied += l;
          buf_index    =  0;
     }


     return bytescopied;
}



word HudsonMsgBase::scanMail( MessageIndex *mi,
                              word          maxmsgs )
{
     f_msgtoidx.rewind();


     int     i;
     int     num_found;
     long    index;
     MsgArea ma;


     for ( index = 0,  i = 0;  i < maxmsgs;  index++ )
     {
          HudsonMsgToIdx h_mti;
          HudsonMsgIdx   h_mi;


          if ( f_msgtoidx.read( & h_mti,
                                sizeof( h_mti ) )  !=  sizeof( h_mti ) )
          {
               break;
          }


          pas2c( h_mti.to );


          if ( stricmp( user.name,  h_mti.to )  && 
               stricmp( user.alias, h_mti.to ) )
          {
               continue;
          }


          f_msgidx.seek( index * 3 );


          if ( f_msgidx.read( & h_mi,
                              sizeof( h_mi ) )  ==  sizeof( h_mi ) )
          {
               mi[ i ].num   =  h_mi.num;
               mi[ i ].area  =  h_mi.area;


               if ( ! ma.read( mi[ i ].area ) )
               {
                    continue;
               }


               if ( ! stricmp( user.alias,
                               h_mti.to )  && 

                    ! ma.flags             && 
                    
                    strcmp( user.name, user.alias ) )
               {
                    continue;
               }


               if ( mi[ i ].num  >  1                               && 
                    user.mailCheckBoards.connected( mi[ i ].area )  && 
                    ma.msgBaseType == MSGBASE_HUDSON                && 
                    (
                      check_access( ma.readLevel,
                                    ma.readFlags,
                                    ma.readFlagsNot )  || 
                                    
                      ma.sysopAccess()
                    )
                  )
               {
                    i++;
               }
          }
     }


     num_found = i;

     bool  *to_all_areas  =  new bool[ 200 ];


     for ( i = 0;  i < 200;  i++ )
     {
          to_all_areas[ i ]  =  FALSE;
     }


     for ( i = 1;  i <= 200  &&  i <= MsgArea::highAreaNum();  i++ )
     {
          if ( ! ma.read( i )  || 
               ! check_access( ma.readLevel,
                               ma.readFlags,
                               ma.readFlagsNot )  || 

               ma.msgBaseType != MSGBASE_HUDSON   || 
               ma.msgType     != MSG_TOALL )
          {
               continue;
          }


          to_all_areas[ i - 1 ]  =  TRUE;
     }


     f_msgidx.rewind();


     while ( num_found < 200 )
     {
          HudsonMsgIdx tmp_mi;


          if ( f_msgidx.read( & tmp_mi,
                              sizeof( tmp_mi ) )  !=  sizeof( tmp_mi ) )
          {
               break;
          }


          if ( tmp_mi.num < 1 ) 
          {
               continue;
          }


          if ( tmp_mi.area < 1    || 
               tmp_mi.area > 200  || 
               ! to_all_areas[ tmp_mi.area - 1 ] )
          {
               continue;
          }


          if ( ! ma.read( tmp_mi.area ) )
          {
               continue;
          }


          if ( tmp_mi.num  >  ma.lastRead( user_recnr ) )
          {
               mi[ num_found ].num   =  tmp_mi.num;
               mi[ num_found ].area  =  tmp_mi.area;

               num_found++;
          }
     }


     delete [] to_all_areas;

     return num_found;
}



long HudsonMsgBase::appendMsg( Message &msg )
{
     File           ft;
     HudsonMsgIdx   midx;
     HudsonMsgToIdx mtidx;
     HudsonMsgInfo  minfo;
     long           startrec;
     long           numrecs;
     long           index;


     if ( msg.areaNum() < 1  || 
          msg.areaNum() > 200 ) 
     {
          return -1;
     }


     readInfo( minfo );


     if ( ! lock() ) 
     {
          return -1;
     }


     msg.id  = ++minfo.high;
     msg.num = msg.id;


     minfo.total++;

     minfo.active[ msg.areaNum() - 1 ]++;


     midx.num   =  int( msg.id );
     midx.area  =  msg.areaNum();


     index  =  word( f_msghdr.len()  /  sizeof( HudsonMsgHdr ) );


     if ( ! ft.open( "MSGTMP" ) )
     {
          unlock();

          return -1;
     }


     startrec  =  f_msgtxt.len() / 256L;

     f_msgtxt.seek( 256L * startrec );


     for ( numrecs = 0;  ;  numrecs++ )
     {
          char x[ 256 ];
          int  l = 0;


          if ( ! numrecs )
          {
               //----------------------------  
               // JDR: REGISTRATION REFERENCE
               //----------------------------  

               sprintf( x,
                        "\1PID: " PID_STRING_HUDSON "%c\r\n" , 
                        registered 
                          ? 'r'
                          : 'u' );

               l  =  strlen( x );
          }


          l  +=  ft.read( & x[ l ],
                          255 - l );

          x[ l ]  =  0;


          if ( l < 1 )
          {
               break;
          }


          c2pas( x );

          f_msgtxt.write( x, 256 );
     }


     ft.close();

     msg.postDate.today();
     msg.postTime.now();


     HudsonMsgHdr hdr;


     hdr = msg;          // Conversion applied


     hdr.startRec  =  word( startrec );
     hdr.numRecs   =  int(  numrecs  );


     f_msghdr.seek(  index  *  sizeof( HudsonMsgHdr ) );
     f_msghdr.write( & hdr,
                     sizeof( hdr ) );


     strcpy( mtidx.to, msg.to );

     c2pas( mtidx.to );


     f_msgtoidx.seek(  index  *  sizeof( HudsonMsgToIdx ) );
     f_msgtoidx.write( & mtidx,
                       sizeof( mtidx ) );

     f_msgidx.seek(  index  *  sizeof( HudsonMsgIdx ) );
     f_msgidx.write( & midx,
                     sizeof( midx ) );


     writeInfo( minfo );

     unlock();

     flushCache();


     return msg.id;
}



bool HudsonMsgBase::updateMsg( Message &msg )
{
     HudsonMsgIdx   midx;
     HudsonMsgToIdx mtidx;
     long           index;


     if ( msg.areaNum() < 1  || 
          msg.areaNum() > 200 ) 
     {
          return FALSE;
     }


     index  =  findIdx( midx,
                        int( msg.id ),
                        msg.areaNum(),
                        0 );


     if ( index < 0 ) 
     {
          return FALSE;
     }


     HudsonMsgHdr hdr;


     f_msghdr.seek( index  *  sizeof( hdr ) );


     if ( f_msghdr.read( & hdr,
                         sizeof( hdr ) )  !=  sizeof( hdr ) )
     {
          return FALSE;
     }


     if ( ! lock() ) 
     {
          return FALSE;
     }

     hdr = msg;


     f_msghdr.seek(  index  *  sizeof( hdr ) );
     f_msghdr.write( & hdr,
                     sizeof( hdr ) );


     strcpy( mtidx.to, msg.to );


     if ( msg.attr & MSGATTR_RECEIVED ) 
     {
          strcpy( mtidx.to, "* Received *" );
     }


     if ( msg.attr & MSGATTR_DELETED )
     {
          strcpy( mtidx.to, "* Deleted *"  );
     }


     c2pas( mtidx.to );


     f_msgtoidx.seek(  index  *  sizeof( HudsonMsgToIdx ) );
     f_msgtoidx.write( & mtidx,
                       sizeof( mtidx ) );

     unlock();

     flushCache();


     return TRUE;
}



bool HudsonMsgBase::deleteMsg( Message &msg )
{
     HudsonMsgIdx   midx;
     HudsonMsgToIdx mtidx;
     HudsonMsgInfo  minfo;
     long           index;


     if ( msg.areaNum() < 1  || 
          msg.areaNum() > 200 ) 
     {
          return FALSE;
     }


     index  =  findIdx( midx,
                        int( msg.id ),
                        msg.areaNum(),
                        0 );


     if ( index < 0 ) 
     {
          return FALSE;
     }


     HudsonMsgHdr hdr;


     f_msghdr.seek( index  *  sizeof( hdr ) );


     if ( f_msghdr.read( & hdr,
                         sizeof( hdr ) )  !=  sizeof( hdr ) )
     {
          return FALSE;
     }


     if ( ! lock() ) 
     {
          return FALSE;
     }


     msg.attr |= MSGATTR_DELETED;
     hdr       = msg;


     f_msghdr.seek(  index  *  sizeof( hdr ) );
     f_msghdr.write( & hdr,
                     sizeof( hdr ) );


     strcpy( mtidx.to, "* Deleted *" );

     c2pas( mtidx.to );


     f_msgtoidx.seek(  index  *  sizeof( HudsonMsgToIdx ) );
     f_msgtoidx.write( & mtidx,
                       sizeof( mtidx ) );


     midx.num = -1;


     f_msgidx.seek(  index * 3 );
     f_msgidx.write( & midx, 3 );

     readInfo( minfo );


     minfo.total--;

     minfo.active[ msg.areaNum() - 1 ]--;


     writeInfo( minfo );

     unlock();

     flushCache();


     return TRUE;
}



bool HudsonMsgBase::lock()
{
     static bool  lock_error = FALSE;


     if ( lock_error ) 
     {
          return FALSE;
     }

     f_msghdr.flush();
     f_msgtxt.flush();
     f_msginfo.flush();
     f_msgtoidx.flush();
     f_msgidx.flush();


     if ( ! cfg.multiline ) 
     {
          return TRUE;
     }


     if ( ! f_msginfo.lock( 407, 1 ) )
     {
          int errno = f_msginfo.error();


          lock_error = TRUE;

          fatalerror( form( "FILE LOCK ERROR (%d - %s)",
                            errno,
                            dos_error_message( errno ) ) );

          return FALSE;
     }


     return TRUE;
}



bool HudsonMsgBase::unlock()
{
     static bool lock_error = FALSE;


     if ( lock_error ) 
     {
          return FALSE;
     }


     f_msghdr.flush();
     f_msgtxt.flush();
     f_msginfo.flush();
     f_msgtoidx.flush();
     f_msgidx.flush();


     if ( ! cfg.multiline )
     {
          return TRUE;
     }


     if ( ! f_msginfo.unlock( 407, 1 ) )
     {
          int errno = f_msginfo.error();


          lock_error = TRUE;

          fatalerror( form( "FILE UNLOCK ERROR (%d - %s)",
                            errno,
                            dos_error_message( errno ) ) );
     }


     return TRUE;
}



long HudsonMsgBase::lastRead( MsgArea &ma,
                              long     rec )
{
     readLastRead( rec );


     if ( ma.areaNum < 1  || 
          ma.areaNum > 200 ) 
     {
          return 0;
     }


     return  long( lastReadArray[ ma.areaNum - 1 ] );
}



void HudsonMsgBase::setLastRead( MsgArea &ma,
                                 long     rec,
                                 long     num )
{
     File f;


     readLastRead( rec );


     if ( ! f.open( FileName( cfg.msgpath,
                              "LASTREAD.BBS" ),
                    fmode_rw | fmode_copen ) )
     {
          return;
     }


     if ( f.len()  <  ( rec + 1 )  *  400 )
     {
          word n  =  0;
          long r  =  f.len() / 400 * 400;

          f.seek( r );


          for( ;  r  <  ( rec + 1 )  *  400;  r += 2 )
          {
               f.write( & n,  2 );
          }
     }


     if ( ma.areaNum < 1  || 
          ma.areaNum > 200 ) 
     {
          return;
     }


     long pos  =  rec * 400  +  ( ma.areaNum - 1 )  *  2;


     f.seek( pos );

     f.write( & num,  2 );


     lastReadArray[ ma.areaNum - 1 ]  =  int( num );
}



void HudsonMsgBase::readLastRead( long recno )
{
     static dword ticks = clockticks();


     if ( clockdiff( ticks )  >  1000 )
     {
          ticks = clockticks();

          flushCache();
     }


     if ( lastReadArray  && 
          recno == lastReadRecord ) 
     {
          return;
     }


     if ( lastReadArray == NULL )
     {
          lastReadArray  =  new int[ 200 ];

          memset( lastReadArray,
                  0,
                  400 );
     }


     lastReadRecord = recno;


     File f;


     if ( ! f.open( FileName( cfg.msgpath,
                              "LASTREAD.BBS" ) ) )
     {
          return;
     }


     f.seek( recno * 400 );


     if ( f.read( lastReadArray, 400 )  !=  400 )
     {
          lastReadRecord = -1;

          memset( lastReadArray,
                  0,
                  400 );
     }
}
