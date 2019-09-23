#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>




#define TWO_DIGIT_CROSSOVER 10



//**************************************************************************
//
// Make sure a year value is ProBoard Y2K compliant (i.e., 1900 + 0-127)
//
//     Prototype:  int NormalizeYear( int iYearVal );
//
//    Parameters:  iYearVal ... Input year value to adjust
//
//       Returns:  The "normalized" year value that is Y2K compliant
//
//       Remarks:
//
// The year value that this routine returns is between 0 and 127, to
// indicate the years 1900 through 2027.  2027 will only be returned if the
// macro TWO_DIGIT_CROSSOVER is set to 28.  Right now, it's set to 10, which
// will result in a return value of 0 to 109, indicating 1900 - 2009.
//
// We use this macro to allow the programmer to customize the cutoff of two-
// digit year values to suit their application's needs.
// 
// This routine performs quite a number of checks on the year value.  If the
// year value is negative, it converts it to positive and goes from there.
//
// If the value is a two-digit value, it checks the crossover value to
// figure out what century it's in.  If it's a three-digit value, it
// considers it a freaky value and tries the best it can (it MOD's the value
// by 100, and treats the final date as a two-year value).
//
// We also check for typos here.  We only consider a simple typo of the year
// 3XXX.  If we find a year like this, we'll treat it as a 2XXX year.
// Anything else is treated verbatim as a legitimate 4-digit year.  Anything
// after 2000+TWO_DIGIT_CROSSOVER-1 wraps over to the 1900 century.
//
// -------------------------------------------------------------------------
//
//    Created on:  06/17/99 (Jeff Reeder)
// Last modified:  06/17/99 (Jeff Reeder)   Created
//
//**************************************************************************

int NormalizeYear( int iYearVal )
{
     //----------------------------------------------------------
     // First, check for really outrageous year values.  This
     // means years from 100..1899 and 2000+TWO_DIGIT_CROSSOVER..
     //----------------------------------------------------------

     if ( iYearVal < 0 )
     {
          //---------------------------------------
          // We somehow have a negative year value.
          // Convert it to a positive value.
          //---------------------------------------

          iYearVal *= -1;
     }


     //---------------------------------  
     // Check for outlandish year values
     //---------------------------------  

     if ( iYearVal  >= 3000  &&
          iYearVal  <  3000 + TWO_DIGIT_CROSSOVER )
     {
          //-------------------------------------------------------  
          // User must have hit 3 when they meant 2 - subtract 1000
          //-------------------------------------------------------  

          iYearVal -= 1000;
     }


     if ( 
          iYearVal  >= 100  &&
          (
            iYearVal  <   1900  ||
            iYearVal  >=  2000 + TWO_DIGIT_CROSSOVER 
          )
        )
     {
          //------------------------------------------------
          // The year is more than two digits, and it's not
          // in the range of 1900 - 2000+TWO_DIGIT_CROSSOVER
          //
          // This is a weird input ... do our best by
          // converting it to a two-digit year and proceed.
          //------------------------------------------------

          iYearVal %= 100;
     }
     

     //--------------------------------------------  
     // Now we can proceed with year normaliziation
     //--------------------------------------------  

     if ( iYearVal < 100 )
     {
          //-----------  
          // Short year
          //-----------  

          if ( iYearVal < TWO_DIGIT_CROSSOVER )
          {
               //--------------------------------------------  
               // Should be 2000...2000+TWO_DIGIT_CROSSOVER-1
               //--------------------------------------------  

               iYearVal += 100;
          }
          else
          {
               //-----------------------------------------------
               // This year must be in the 1900's - The value is
               // already in the proper value range.  Do nothing
               //-----------------------------------------------
          }
     }
     else 
     {
          //---------------------------------------------------  
          // The year must be 1900...2000+TWO_DIGIT_CROSSOVER-1
          //---------------------------------------------------  

          if ( iYearVal  <  2000 + TWO_DIGIT_CROSSOVER )
          {
               //-----------------------------  
               // Year is between 1900 .. 1999
               //-----------------------------  
               
               iYearVal -= 1900;
          }
     }    


     return iYearVal;
}



void main( void )
{
     for ( ; ; )
     {
          char buf[ 256 ];


          printf( "\nEnter a year value (ENTER=quit): " );

          gets( buf );


          if ( buf[ 0 ]  ==  '\0' )
          {
               break;
          }


          if ( isdigit( buf[ 0 ] )  ||
               (
                 buf[ 0 ]  ==  '-'  &&
                 isdigit( buf[ 1 ] ) 
               )
             )
          {
               int val1;
               int val2;


               val1  =  atoi( buf );
               val2  =  NormalizeYear( val1 );


               printf( "Buffer=%-10.10s  Year=%-7d  NormalizedYear=%-4d  RealYear=%d\n",
                       buf,
                       val1,
                       val2,
                       val2 + 1900 );
          }
          else
          {
               printf( "\nInvalid year value\n\n" );
          }
     }
}
