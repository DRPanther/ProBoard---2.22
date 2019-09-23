#include <ctype.h>
#include <tswin.hpp>
#include "proboard.hpp"



//----------------------------------------------------------------
//
// This table identifies where the slashes should be in the date
// strings.  This supports each of the international date formats:
//
//        MM/DD/YYYY
//        YYYY/MM/DD
//        DD/MM/YYYY
//   
//----------------------------------------------------------------

static
const char piSlashColumns[ 3 ] [ 2 ]  =
{
     { 2, 5 },
     { 4, 7 },
     { 2, 5 },
};



//**************************************************************************
//
// Input a date value from the user
//
//     Prototype:  void InputDate( Date &d );
//
//    Parameters:  d           ... Date variable to fill-in
//                 iDateFormat ... Format of date to be entered
//
//       Returns:  None
//
//       Remarks:
//
// This code has to be changed to support the various international formats:
//
//        MM/DD/YYYY
//        YYYY/MM/DD
//        DD/MM/YYYY
//
// -------------------------------------------------------------------------
//
//    Created on:  ??/??/?? (Philippe Leybaert)
// Last modified:  06/17/99 (Jeff Reeder)   
//
//**************************************************************************

void InputDate( Date &d,
                byte  iDateFormat )               // JDR: Added 
{
     char s[ MAX_DATE_FIELD_SIZE + 1 ];
     byte k;


     //-------------------------------------------  
     // Show a MAX_DATE_FIELD_SIZE-character field
     //-------------------------------------------  

     io.drawfield( MAX_DATE_FIELD_SIZE );


     int i = 0;
     

     //------------------------------  
     // Keep looping until we're done
     //------------------------------  
     
     loop
     {
          //-------------------------------------  
          // Wait until the user enters something
          //-------------------------------------  

          k = io.wait();


          //-----------------  
          // What do we have?
          //-----------------  

          if ( 
               ( 
                 ! isdigit( k )  and
                 k != CR         and
                 k != BS
               )
               or  k > 0x7F      // This check isn't needed, but better safe
             )
          {
               //----------------------------------------------  
               // If it's not a digit, CR or a BS, we ignore it
               //----------------------------------------------  

               continue;
          }


          if ( k == BS )
          {
               //----------------------------------  
               // We got a backspace (BS) character
               //----------------------------------  

               if ( i > 0 )
               {
                    //-------------------------------------  
                    // We have characters to backspace over
                    //-------------------------------------  

                    i--;
                    

                    //-------------------------------------------
                    // Erase the previous character - destructive
                    // backspace just to be on the cautious side.
                    //-------------------------------------------

                    io << "\b \b";
                    
                    
                    if ( i  ==  piSlashColumns[ iDateFormat ][ 0 ]  or
                         i  ==  piSlashColumns[ iDateFormat ][ 1 ] )
                    {
                         //-----------------------------------------  
                         // We ran over a slash/delimiter.  Erase it
                         //-----------------------------------------  

                         i--;

                         io << "\b \b";
                    }
               }


               continue;
          }


          if ( k == CR )
          {
               if ( i == 0 )
               {
                    //-------------------------------------
                    // We haven't entered anything.  Choose
                    // the default date, Jan 1, 1900
                    //-------------------------------------

                    d.set( 0, 0, 0 );
            
                    break;
               }

          
               if ( i != MAX_DATE_FIELD_SIZE )
               {
                    //----------------------------------------------------  
                    // We haven't maxed out our field size.  Ignore the CR
                    //----------------------------------------------------  

                    continue;
               }
               else
               {
                    //-----------------------------------------
                    // We have filled our date field.  Let's
                    // parse the date information appropriately
                    //-----------------------------------------

                    parse_date( d,
                                s,
                                user.dateFormat );
                    
                    
                    if ( d.ok() )
                    {
                         //-------------------------------------------------  
                         // The date has legitimate values.  Alright to quit
                         //-------------------------------------------------  

                         break;
                    }
                    else
                    {
                         //--------------------------------------------  
                         // The data was bogus.  Keep reading more data
                         //--------------------------------------------  

                         continue;
                    }
               }
          }


          if ( i == MAX_DATE_FIELD_SIZE )
          {
               //-----------------------------------------------  
               // We're at the maximum field size.  Ignore input
               //-----------------------------------------------  

               continue;
          }


          //-----------------------------------------------------------  
          // Assign the current character at the current input position
          //-----------------------------------------------------------  

          s[ i++ ]  =  k;
          

          //-----------------------------  
          // Output the current character
          //-----------------------------  

          io << k;


          if ( i  ==  piSlashColumns[ iDateFormat ][ 0 ]  or
               i  ==  piSlashColumns[ iDateFormat ][ 1 ] )
          {
               //---------------------------------------------------
               // We've moved to a slash position.  Insert the slash
               // automatically, and move to the next digit location
               //---------------------------------------------------

               s[ i++ ]  =  '/';

               io << '/';
          }
     }


     //------------------------------------------------------------
     // We're all done with our date.  Output a carriage return and
     // make sure our buffer is NUL terminated before continuing.
     //------------------------------------------------------------

     io << '\r';

     s[ i ]  =  '\0';


     if ( avatar )
     {
          //---------------------------------------------------------------  
          // We're in Avatar mode.  Reset our screen attributes accordingly
          //---------------------------------------------------------------  

          io << char( 22 ) 
             << char( 1 ) 
             << char( SCREEN.attrib()  &  0x0F );
     }
     else if ( ansi_mode )  
     {
          //-------------------------------------------------------------  
          // We're in ANSI mode.  Make sure we reset the background color
          //-------------------------------------------------------------  

          io << "\x1b[40m";
     }


     //-------------------------------------  
     // Flush all output to the user's modem
     //-------------------------------------  

     io.flush();
}
