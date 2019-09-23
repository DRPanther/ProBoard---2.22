#include "proboard.hpp"


//**************************************************************************
//
//                       TELEGRAFIX CONFIDENTIAL 
//                       PROTECTED TRADE SECRETS
//
// The contents of this file are confidential materials of TeleGrafix
// Communications, Inc.  Use of this information is subject to the terms and
// conditions set forth in the TeleGrafix software source code licensing
// contract.
//
//**************************************************************************

//##########################################################################
//                                                                         
//    Module file: SERIALIZ.C
//                                                                         
//  Last modified: 12/11/94 (Jeff Reeder)                                   
//      Author(s): Jeff Reeder - 12/11/94                                   
//                                                                         
// -------------------------------------------------------------------------
//                                                                         
//          Copyright (c) 1992-95 TeleGrafix Communications, Inc.          
//                           All Rights Reserved                           
//                                                                         
// -------------------------------------------------------------------------
//                                                                         
//                       REVISION HISTORY INFORMATION                      
//                                                                         
//   Version   Date      Description                                       
//   ------------------------------------------------------------------    
//                                                                         
//
//##########################################################################

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <dos.h>
#include <time.h>
#include <bios.h>

//--------------------------------------------------------------------------

#define UNREG_STRING "(Unregistered copy)"

#define SERIAL_NUMBER_LENGTH 17


#define USERNAME_LEN        31
#define DEFAULT_REGNAME     "Name of Registered Owner here!"

#define SERIAL_NUM_LEN      17
#define DEFAULT_SERIALNO    "<SERIAL NUMBER!>"

#define SYSTEM_CODE_LEN     13
#define DEFAULT_SYSTEM_CODE "TELEGRAFIX\0\0"



//---------------------
// Product Code defines
//---------------------

#define PROBOARD_BBS         "PB"
#define PROBOARD_BBS_NAME    "ProBoard BBS"


//-----------------
// Platform defines
//-----------------

#define PLATFORM_DOS         'D'
#define PLATFORM_DOS_NAME    "MS-DOS"


//---------------
// Origin defines
//---------------

#define ORIGIN_TELEGRFX           '0'
#define ORIGIN_TELEGRFX_NAME      "TeleGrafix Corporate HQ"

#define ORIGIN_TELEGRFX_COMP      '1'
#define ORIGIN_TELEGRFX_NAME_COMP "Free Upgrades"

#define ORIGIN_TELEGRFX_GIF       '2'
#define ORIGIN_TELEGRFX_NAME_GIF  "GIF edition"


//-------------
// Misc defines
//-------------

#define BASE_36          36   // Meganums are base 36
#define START_YEAR     1992   // The year TeleGrafix began
#define MAX_SN_COUNTER 1285   // 1295 is ZZ in meganums, set to 1285 to prevent duplicate serial numbers (says Jeff)
#define MAX_SN_YEARS     35   // Seconds / 2
#define MAX_SN_MONTHS    11   // Seconds / 2
#define MAX_SN_DAYS      31   // Number of 2
#define MAX_SN_HOURS     23   // Seconds / 2
#define MAX_SN_MINS      29   // Minutes / 2
#define MAX_SN_SECS      29   // Seconds / 2

#define MAX_SN_LENGTH    16   // Maximum length of a serial number


static char regname_buf    [ USERNAME_LEN    ]  =  DEFAULT_REGNAME;
static char serial_buf     [ SERIAL_NUM_LEN  ]  =  DEFAULT_SERIALNO;
static char system_code_buf[ SYSTEM_CODE_LEN ]  =  DEFAULT_SYSTEM_CODE;



enum regResults
{
     registrationNone = 0,         // Software is not registered
     registrationBad,              // Illegal registration information
     registrationGood,             // Software is fully registered
};



static 
word crc_table_16[ 256 ] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};



static
word makeCRC16( word  count,
                word  crc,
                byte *buffer )
{
     byte *p = buffer;


     while ( count--  !=  0 )
     {
          crc = word( ( crc << 8 )  ^  crc_table_16[ ( crc >> 8 )  ^  *p++ ] );
     }

     return crc;
}



byte *decryptData( byte  *buf, 
                   short  len,
                   byte   key )
{
     short i;
     byte  c;
     byte  shift = 0;
     byte  iPrime;
     byte  nKey;

     (void) shift;

     iPrime  =  0x17;
     nKey    =  ( byte ) ~key;


     for ( i = 0;  i < len;  i++, key++, iPrime++, nKey++ )
     {
          c   =  ( buf[ i ]  ^  nKey )  ^  iPrime;     // Get encrypted char  


          //--------------------------------------------------------------
          // Now let's do the bit shifting (rolling) to the left.  We used
          // to do this in a loop, but doing it this way avoids the loop
          // overhead and speeds things up at the cost of slightly larger
          // code size.
          //--------------------------------------------------------------
          
          shift  =  ( byte )  ( i & 0x07 );
          
          
          if ( shift != 0 )
          {
               c  =  ( byte )  ( ( c  <<  shift )   |   
                                 ( c  >>  ( 8 - shift ) ) );
          }
          
          
          //-----------------------------------
          // Now let's perform the bit swapping
          //-----------------------------------
          
          c  =  ( byte )  ~(   ( c             >> 7  )  | 
                               ( c             << 7  )  |
                             ( ( c  &  0x40 )  >>  5 )  | 
                             ( ( c  &  0x02 )  <<  5 )  |
                             ( ( c  &  0x20 )  >>  3 )  |
                             ( ( c  &  0x04 )  <<  3 )  |
                             ( ( c  &  0x10 )  >>  1 )  | 
                             ( ( c  &  0x08 )  <<  1 ) );


          buf[ i ]  =  ( byte )  ( ( c ^ key )  ^  0x49 );       // Store real char in buf 
     }


     return( buf );
}



static
byte *encryptData( byte  *buf, 
                   short  len,
                   byte   key )
{
     short i;
     byte  c;
     byte  shift = 0;
     byte  iPrime;
     byte  nKey;

     (void) shift;

     iPrime  =  0x17;
     nKey    =  ( byte ) ~key;


     for ( i = 0;   i  <  ( short ) len;   i++, key++, iPrime++, nKey++ )
     {
          //------------------
          // Get the character
          //------------------

          c   =  ( byte )  ( ( buf[ i ]  ^  key )  ^  0x49 );               


          //-----------------------------------
          // Now let's perform the bit swapping
          //-----------------------------------
          
          c  =  ( byte )  ~(   ( c           >>  7 )  |
                               ( c           <<  7 )  | 
                             ( ( c & 0x40 )  >>  5 )  | 
                             ( ( c & 0x02 )  <<  5 )  | 
                             ( ( c & 0x20 )  >>  3 )  | 
                             ( ( c & 0x04 )  <<  3 )  |
                             ( ( c & 0x10 )  >>  1 )  | 
                             ( ( c & 0x08 )  <<  1 ) );
          
          
          //----------------------------------------------------------------
          // Now let's do the bit shifting (rolling) to the right.  We used
          // to do this in a loop, but doing it this way eliminates the loop
          // code overhead and consequently speeds things up a bit.
          //----------------------------------------------------------------
          
          shift  =  ( byte ) ( i & 0x07 );
          
          
          if ( shift != 0 )
          {
               c  =  ( byte )  ( ( c  >>  shift )  |
                                 ( c  <<  ( 8 - shift ) ) );
          }

          buf[ i ]  =  ( c ^ iPrime )  ^  nKey;
     }


     return( buf );
}



static
int isValidMegaNum( char iChar )
{
     if ( iChar >= '0'  &&
          iChar <= '9' )
     {
          return 1;
     }
     else if ( isalpha( iChar ) )
     {
          return 1;
     }
     else
     {
          return 0;
     }
}



static
word serialNumberDecode( char *sBuf,
                         word *piNodeCount )
{
    char    pstrProductID[ 3 ];
    byte    iPlatformID;
    byte    iOriginID;
    char    buf[ 16 + 1 ];
    char    eBuf[ 16 + 1 ];
    struct  dosdate_t dt;
    struct  dostime_t tm;
    word    yearValue;
    word    minValue;
    word    secValue;
    char    counterBuf[ 8 ];
    char    minuteBuf[ 8 ];
    char    dayBuf[ 8 ];
    char    secBuf[ 8 ];
    char    yearBuf[ 8 ];
    char    hourBuf[ 8 ];
    char    monthBuf[ 8 ];
    char    crcBuf[ 5 ];
    word    oldCRC;
    word    newCRC;
    word    iCounter;

    char    P1;     // Product Code byte 1
    char    P2;     // Product Code byte 2
    char    Pl;     // Platform
    char    Or;     // Origin

    char    N1;     // Numeric Counter byte 1
    char    N2;     // Numeric Counter byte 2

    char    Yr;     // Year
    char    Mo;     // Month
    char    Dy;     // Day
    char    Hr;     // Hour
    char    Mi;     // Minute/2
    char    Se;     // Second/2

    char    R1;     // CRC byte 1
    char    R2;     // CRC byte 2
    char    R3;     // CRC byte 3
    char    R4;     // CRC byte 4


    if ( piNodeCount )
    {
          *piNodeCount = 2;
    }


    if ( strlen( sBuf )  !=  MAX_SN_LENGTH )
    {
        //-----------------------------
        // Illegal serial number length
        //-----------------------------

        return( 0 );
    }

    //------------------------------------------------------
    // Assign buffer fields to our serial number field codes
    //------------------------------------------------------

    R1 = sBuf[  7 ];
    R2 = sBuf[ 14 ];
    R3 = sBuf[  5 ];
    R4 = sBuf[ 11 ];

    P1 = sBuf[  0 ];
    P2 = sBuf[  1 ];
    Pl = sBuf[  2 ];
    Or = sBuf[  3 ];
    N2 = sBuf[  4 ];
    Mi = sBuf[  6 ];
    Dy = sBuf[  8 ];
    Se = sBuf[  9 ];
    Yr = sBuf[ 10 ];
    Hr = sBuf[ 12 ];
    N1 = sBuf[ 13 ];
    Mo = sBuf[ 15 ];

    crcBuf[ 0 ] = R1;
    crcBuf[ 1 ] = R2;
    crcBuf[ 2 ] = R3;
    crcBuf[ 3 ] = R4;
    crcBuf[ 4 ] = '\0';

    //----------------------------------
    // build buffer for encryption check
    //----------------------------------

    buf[ 0 ]    = P1;
    buf[ 1 ]    = P2;
    buf[ 2 ]    = Pl;
    buf[ 3 ]    = Or;
    buf[ 4 ]    = N2;
    buf[ 5 ]    = Mi;
    buf[ 6 ]    = Dy;
    buf[ 7 ]    = Se;
    buf[ 8 ]    = Yr;
    buf[ 9 ]    = Hr;
    buf[ 10 ]   = N1;
    buf[ 11 ]   = Mo;
    buf[ 12 ]   = '\0';


    oldCRC  =  word( strtol( crcBuf, 
                             NULL, 
                             16 ) );


    strcpy( eBuf, buf );

    encryptData( eBuf,  strlen( eBuf ),  0 );


    newCRC  =  makeCRC16( strlen( buf ),  
                          0,  
                          eBuf );


    sprintf( crcBuf,  "%04x",  newCRC );

    strupr( crcBuf );


    if ( toupper( P1 )  !=  'P'  ||
         toupper( P2 )  !=  'B' )
    {
          //---------------------  
          // Invalid product code
          //---------------------  

          return( 0 );
    }

    pstrProductID[ 0 ]  =  P1;      // buf[ 0 ];
    pstrProductID[ 1 ]  =  P2;      // buf[ 1 ];
    pstrProductID[ 2 ]  =  '\0';

    iPlatformID         =  Pl;      // buf[ 2 ];
    iOriginID           =  Or;      // buf[ 3 ];

    counterBuf[ 0 ]     =  N1;      // buf[ 10 ];
    counterBuf[ 1 ]     =  N2;      // buf[ 4 ];
    counterBuf[ 2 ]     =  '\0';

    minuteBuf[ 0 ]      =  Mi;      // buf[ 5 ];
    minuteBuf[ 1 ]      =  '\0';

    dayBuf[ 0 ]         =  Dy;      // buf[ 6 ];
    dayBuf[ 1 ]         =  '\0';

    secBuf[ 0 ]         =  Se;      // buf[ 7 ];
    secBuf[ 1 ]         =  '\0';

    yearBuf[ 0 ]        =  Yr;      // buf[ 8 ];
    yearBuf[ 1 ]        =  '\0';

    hourBuf[ 0 ]        =  Hr;      // buf[ 9 ];
    hourBuf[ 1 ]        =  '\0';

    monthBuf[ 0 ]       =  Mo;      // buf[ 11 ];
    monthBuf[ 1 ]       =  '\0';


    if ( not isxdigit( R1 )  or
         not isxdigit( R2 )  or
         not isxdigit( R3 )  or
         not isxdigit( R4 ) )
    {
        //------------------------------------
        // Illegal CRC value - not hexadecimal
        //------------------------------------

        return( 0 );
    }
    else if ( newCRC != oldCRC )
    {
        //-----------------------
        // CRC values don't match
        //-----------------------

        return( 0 );
    }
    else if ( not isupper( iPlatformID ) )
    {
        //--------------------
        // Illegal platform ID
        //--------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( iOriginID ) )
    {
        //------------------
        // Illegal origin ID
        //------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( counterBuf[ 0 ] )  or
              not isValidMegaNum( counterBuf[ 1 ] ) )
    {
        //----------------------
        // Illegal counter value
        //----------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( yearBuf[ 0 ] ) )
    {
        //-------------------
        // Illegal year value
        //-------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( monthBuf[ 0 ] ) )
    {
        //--------------------
        // Illegal month value
        //--------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( dayBuf[ 0 ] ) )
    {
        //------------------
        // Illegal day value
        //------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( hourBuf[ 0 ] ) )
    {
        //-------------------
        // Illegal hour value
        //-------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( minuteBuf[ 0 ] ) )
    {
        //---------------------
        // Illegal minute value
        //---------------------

        return( 0 );
    }
    else if ( not isValidMegaNum( secBuf[ 0 ] ) )
    {
        //---------------------
        // Illegal second value
        //---------------------

        return( 0 );
    }


    iCounter   =  ( word ) strtol( counterBuf, NULL, BASE_36 );
    minValue   =  ( word ) strtol( minuteBuf,  NULL, BASE_36 );
    dt.day     =           strtol( dayBuf,     NULL, BASE_36 );
    secValue   =  ( word ) strtol( secBuf,     NULL, BASE_36 );
    yearValue  =  ( word ) strtol( yearBuf,    NULL, BASE_36 );
    tm.hour    =           strtol( hourBuf,    NULL, BASE_36 );
    dt.month   =           strtol( monthBuf,   NULL, BASE_36 );


    if ( iCounter > MAX_SN_COUNTER )
    {
        //---------------------------
        // Counter value out of range
        //---------------------------

        return( 0 );
    }
    else if ( yearValue > BASE_36 )
    {
        //------------------
        // year out of range
        //------------------

        return( 0 );
    }
    else if ( dt.month > MAX_SN_MONTHS )
    {
        //----------------------
        // Month second of range
        //----------------------

        return( 0 );
    }
    else if ( dt.day > MAX_SN_DAYS )
    {
        //-----------------
        // Day out of range
        //-----------------

        return( 0 );
    }
    else if ( tm.hour > MAX_SN_HOURS )
    {
        //---------------------
        // Hour second of range
        //---------------------

        return( 0 );
    }
    else if ( minValue > MAX_SN_MINS )
    {
        //---------------------------
        // Minutes value out of range
        //---------------------------

        return( 0 );
    }
    else if ( secValue > MAX_SN_SECS )
    {
        //--------------------
        // second out of range
        //--------------------

        return( 0 );
    }

    dt.month++;
    dt.day++;

    dt.year    =  START_YEAR + yearValue;
    tm.minute  =  minValue * 2;
    tm.second  =  secValue * 2;


    if ( piNodeCount )
    {
          *piNodeCount = iCounter;
    }

    return( 1 );
}



static
regResults registration_info( char *name, 
                              char *serial,
                              char *actcode,
                              word *node_count )
{
     word i;
     word checksum = 0;
     byte nBuf[ 80 ];
     byte sBuf[ 80 ];


     if ( memcmp( system_code_buf, 
                  DEFAULT_SYSTEM_CODE, 
                  SYSTEM_CODE_LEN )  ==  0 )
     {
          //---------------------
          // System Code is blank
          //---------------------

          if ( actcode )
          {
               strcpy( actcode, DEFAULT_SYSTEM_CODE );
          }
     }
     else
     {
          //---------------------------------------------------------
          // Looks like we have a valid system code.  Let's decode it
          //---------------------------------------------------------

          char eBuf[ 80 ];


          memmove( eBuf, 
                   system_code_buf, 
                   SYSTEM_CODE_LEN );

          decryptData( eBuf, 
                       SYSTEM_CODE_LEN, 
                       0x00 );


          if ( actcode )
          {
               //---------------------------------------------------------
               // Now let's copy that system code to our calling parameter
               //---------------------------------------------------------

               sprintf( actcode, 
                        "%-12.12s", 
                        eBuf );
          }
     }


     if ( memcmp( regname_buf, 
                  DEFAULT_REGNAME, 
                  USERNAME_LEN )  ==  0 )
     {
          //-------------------
          // User name is blank
          //-------------------

          return( registrationNone );
     }
     else if ( memcmp( serial_buf, 
                       DEFAULT_SERIALNO, 
                       SERIAL_NUM_LEN )  ==  0 )
     {
          //-----------------------
          // Serial number is blank
          //-----------------------

          return( registrationNone );
     }


     for ( i = 0;  i < USERNAME_LEN;  i++ )
     {
          checksum  +=  regname_buf[ i ];
     }


     checksum  =  checksum & 0x00FF;


     memmove( nBuf, regname_buf, USERNAME_LEN   );
     memmove( sBuf, serial_buf,  SERIAL_NUM_LEN );


     decryptData( nBuf, USERNAME_LEN,   0x00     );
     decryptData( sBuf, SERIAL_NUM_LEN, checksum );


     if ( memchr( sBuf, '\0', SERIAL_NUM_LEN )  ==  NULL )
     {
          //---------------------------------
          // One of 'em isn't null terminated
          //---------------------------------

          return( registrationBad );
     }
     else if ( memchr( nBuf, '\0', USERNAME_LEN )  ==  NULL )
     {
          //---------------------------------
          // One of 'em isn't null terminated
          //---------------------------------

          return( registrationBad );
     }
     else if ( not serialNumberDecode( sBuf, node_count ) )
     {
          //----------------------------------
          // couldn't decode the serial number
          //----------------------------------

          return( registrationBad );
     }
     

     if ( name )
     {
          strcpy( name, nBuf );
     }


     if ( serial )
     {
          serial[ 0 ]   =  sBuf[ 0 ]; 
          serial[ 1 ]   =  sBuf[ 1 ]; 
          serial[ 2 ]   =  sBuf[ 2 ]; 
          serial[ 3 ]   =  sBuf[ 3 ]; 
          serial[ 4 ]   =  '-';
          serial[ 5 ]   =  sBuf[ 4 ]; 
          serial[ 6 ]   =  sBuf[ 5 ]; 
          serial[ 7 ]   =  sBuf[ 6 ]; 
          serial[ 8 ]   =  sBuf[ 7 ]; 
          serial[ 9 ]   =  '-';
          serial[ 10 ]  =  sBuf[ 8 ]; 
          serial[ 11 ]  =  sBuf[ 9 ]; 
          serial[ 12 ]  =  sBuf[ 10 ];
          serial[ 13 ]  =  sBuf[ 11 ];
          serial[ 14 ]  =  '-';
          serial[ 15 ]  =  sBuf[ 12 ];
          serial[ 16 ]  =  sBuf[ 13 ];
          serial[ 17 ]  =  sBuf[ 14 ];
          serial[ 18 ]  =  sBuf[ 15 ];

          serial[ 19 ]  =  sBuf[ 16 ];

          serial[ 20 ]  =  '\0';
     }


     return( registrationGood );
}



static
char *serialFormatted( char *sBuf )
{
     static char tBuf[ 20 ];


     sprintf( tBuf,
              "%-4.4s %-4.4s %-4.4s %-4.4s",
              sBuf,
              sBuf + 4,
              sBuf + 8,
              sBuf + 12 );

     return( tBuf );
}



char *software_serial_number( void )
{
     static char buf[ 50 ];
     regResults result;


     if ( registered )
     {
          result  =  registration_info( NULL, 
                                        buf, 
                                        NULL,
                                        NULL );
          
          
          //-------------------------------------------
          // This is a "shareware" copy of the software
          //-------------------------------------------
          
          if ( result == registrationGood )
          {
               //----------------------------------------------------------
               // We have valid registration information.  Let's return the
               // serial number to the calling function.
               //----------------------------------------------------------
 
               return buf;
               //return( serialFormatted( buf ) );
          }
     }


     //-------------------------------------------------------------
     // We have a version with no serial number.  This means that we
     // have an unregistered copy of the software.
     //-------------------------------------------------------------

     return( UNREG_STRING );
}



char *registered_user_name( void )
{

     regResults result;
     static char buf[ 50 ];



     if ( registered )
     {
          result  =  registration_info( buf, 
                                        NULL, 
                                        NULL,
                                        NULL );
     
     
          //-------------------------------------------
          // This is a "shareware" copy of the software
          //-------------------------------------------
     
          if ( result == registrationGood )
          {
               //----------------------------------------------------------
               // We have valid registration information.  Let's return the
               // user's name to the calling function.
               //----------------------------------------------------------
     
               return( buf );
          }
     }


     //---------------------------------------------------------
     // We have a version with no user name.  This means that we
     // have an unregistered copy of the software.
     //---------------------------------------------------------

     return( UNREG_STRING );
}



char *get_system_code( void )
{
     regResults  result;
     static char buf[ 30 ];


     if ( registered )
     {
          result  =  registration_info( NULL, 
                                        NULL, 
                                        buf,
                                        NULL );
     
     
          //-------------------------------------------
          // This is a "shareware" copy of the software
          //-------------------------------------------
     
          if ( result  ==  registrationGood )
          {
               return( buf );
          }
     }


     return( UNREG_STRING );
}



word get_node_count( void )
{
     regResults result;
     word       iNodeCount = 2;


     if ( registered )
     {
          result  =  registration_info( NULL, 
                                        NULL, 
                                        NULL,
                                        & iNodeCount );
     
     
          //-------------------------------------------
          // This is a "shareware" copy of the software
          //-------------------------------------------
     
          if ( result  ==  registrationGood )
          {
               return iNodeCount;
          }
     }
     

     return 2;
}



word is_version_registered( void )
{
     regResults result;
     char       serial[ 80 ];
     char       name[   80 ];


     result  =  registration_info( name, 
                                   serial, 
                                   NULL,
                                   NULL );


     if ( result == registrationGood )
     {
          //----------------------
          // Fully registered copy
          //----------------------

          return( TRUE );
     }
     else
     {
          //---------------------
          // Unregistered edition
          //---------------------

          return( FALSE );
     }
}



void CHECK_REG( void )
{
     if ( is_version_registered() )
     {
          registered     = TRUE;
          max_node_count = get_node_count();
     }
     else
     {
          registered     = TRUE;
          max_node_count = 255;
     }
}
