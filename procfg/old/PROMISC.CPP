
/* ------------------------------------------------------------
 * Filename ............... ProMisc.Cpp
 *
 * General Purpose ........ some miscellaneous functions
 * ------------------------------------------------------------
 * First date ............. 15 dec 1993
 *
 * First in version ....... 2.00
 *
 * Written by ............. Alain Schellinck
 * ------------------------------------------------------------
 * Revisions:
 * ----------
 *
 *   Date   | By |                  Purpose                   |
 * ---------+----+--------------------------------------------+
 *          |    |                                            |
 *          |    |                                            |
 *          |    |                                            |
 */

/*-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include "ThProCfg.Hpp"

/*-------------------------------------------------------------------------*/

/*
 * Routine   : enterPwd
 * Purpose   : let the user enter a password
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

KEY
enterPwd(IntT mode, Window& w, IntT x, IntT y, ChrT attr, AnyP data)
{
   User *user = (User *) data;
   tsw_cursoroff();

   if(mode == TRUE)
   {
      Window w(10,  5, 40, 7, setFBColor(hWhite, nMagenta));
      w.open();
      w << " Password: ";

      w.attrib(setFBColor(hYellow, nMagenta));

      tsw_cursoron();

      ChrT buffer[16];

      if(user->passWord[0] == '\0')
      {
         buffer[0] = '\0';

         w.scan(buffer, 15, 16);

         if(buffer[0])
         {
            strcpy(user->passWord , buffer);

            user->passWordCRC = RaCrc(user->passWord);
         }
      }
      else
      {
         strcpy(buffer, user->passWord);

         w.scan(buffer, 15, 16);
         tsw_cursoroff();

         strcpy(user->passWord, buffer);

         user->passWordCRC = RaCrc(user->passWord);
      }
   }
   else
   {
      if(user->passWord[0] == '\0')
      {
         w.direct(x,y,attr,String(cfg.pwdchar,16));
      }
      else
      {
         String str(user->passWord);

         if(cfg.hidePassword == TRUE)
            str = String(cfg.pwdchar, strlen(user->passWord));

         str.justify(String::left, 16);
         w.direct(x, y, attr, str);
      }
   }

   tsw_cursoron();

   return 0;
}

/*
 * Routine   : askYesNo()
 * Purpose   : ask YES or NO
 * ------------------------------------------------------------------------
 * Parameters: Title
 * Return    : -1 = ESC, TRUE = YES, FALSE = NO
 *
 */

ShrtT askYesNo(ChrP title)
{
   ShrtT ret_val = -1;

   Window w(16, 9, 65, 16, setFBColor(hWhite, nBlack), SHADOW|CENTERED_X);
   w.open();

   w.centerLine(2, title, setFBColor(hYellow, nBlack));

   menu_item yesNoMenu[]=
   {
      { 'Y', 18,18, "                      YES     "  },
      { 'N', 17,17, "                      NO      "  },
      {  0 , 0,0, NULL                                }
   };

   PopMenu menu(yesNoMenu, setFBColor(nBlack, nWhite), setFBColor(hWhite, nBlack));
   menu.open(18, 13, 63, 14, setFBColor(nWhite, nBlack), NOBORDER|CENTERED_X);

   tsw_cursoroff();
   ShrtT choice = menu.process();

   if(choice > 0)
      ret_val = 2 - choice;

   return(ret_val);
}



//**************************************************************************
//
// Calculate the raw string width of a help line, without color codes
//
//     Prototype:  int CalcHelpWidth( char *pData );
//
//    Parameters:  pData ... Help data to calculate the length of
//
//       Returns:  The length, without any color codes
//
//       Remarks:
//
// This routine takes a help line, and calculates how long it is without any
// color codes.  A color code is a three byte sequence starting with 0x01,
// then two hex-encoded digits (i.e., "71" would indicate blue on a gray
// background.)
//
// -------------------------------------------------------------------------
//
//    Created on:  08/06/99 (Jeff Reeder)
// Last modified:  08/06/99 (Jeff Reeder)   Created
//
//**************************************************************************

int CalcHelpWidth( char *pData )
{
     int cLength = 0;


     while (  pData  &&
             *pData )
     {
          if ( *pData == 0x01 )
          {
               pData++;


               if ( *pData  &&
                    isxdigit( *pData ) )
               {
                    pData++;


                    if ( *pData  &&
                         isxdigit( *pData ) )
                    {
                         pData++;
                    }    
               }
               
          }
          else
          {
               cLength++;
               pData++;
          }    
     }


     return cLength;
}



//**************************************************************************
//
// Converts a single hex character to a numerical value from 0-15
//
//     Prototype:  int HexCharToInt( char iChar );
//
//    Parameters:  iChar ... Hex value to convert
//
//       Returns:  A value from 0 through 15.
//
//       Remarks:
//
// If the character value is not a valid hex digit, then a value of 0 is
// returned.
//
// -------------------------------------------------------------------------
//
//    Created on:  08/06/99 (Jeff Reeder)
// Last modified:  08/06/99 (Jeff Reeder)   Created
//
//**************************************************************************

int HexCharToInt( char iChar )
{
     if ( isxdigit( iChar ) )
     {
          if ( iChar >= '0'  &&
               iChar <= '9' )
          {
               return  iChar - '0';
          }    
          else 
          {
               return  toupper( iChar )  -  'A'  +  10;
          }
     }
     else
     {
          //------------------  
          // Invalid hex digit
          //------------------  

          return 0;
     }    
}



//**************************************************************************
//
// 
//
//     Prototype:  void helpShowLine( Window &w, int iLine, char *pText );
//
//    Parameters:  w        ... Window to show line of help text in
//                 iLine    ... Line number in window to receive the text
//                 iDefAttr ... Default color attribute
//                 pText    ... Text to show
//
//       Returns:  Nothing
//
//       Remarks:
//
// This routine displays a single line of text to the help window.  It
// properly handles all color codes, and displays data appropriately.
//
// If the help line doesn't contain any color codes, then it is displayed in
// the default color attribute <iDefAttr>.
//
// -------------------------------------------------------------------------
//
//    Created on:  08/06/99 (Jeff Reeder)
// Last modified:  08/06/99 (Jeff Reeder)   Created
//
//**************************************************************************

static
void helpShowLine( Window &w,
                   int     iLine,
                   ATTR    iDefAttr,
                   char   *pText )
{
     int  iPos      =  0;
     int  cLength   =  strlen( pText );
     ATTR iCurAttr  =  iDefAttr;


     for ( int i = 0;  i < cLength;  i++ )
     {
          if ( pText[ i ]  ==  0x01 )
          {
               //------------------------  
               // Looks like a color code
               //------------------------  

               if ( isxdigit( pText[ i + 1 ] )  &&
                    isxdigit( pText[ i + 2 ] ) )
               {
                    //----------------------------  
                    // We have a valid color code!
                    //----------------------------  

                    int iBack;
                    int iFore;


                    //-----------------------------------------  
                    // Get the foreground and background colors
                    //-----------------------------------------  

                    iBack  =  HexCharToInt( pText[ i + 1 ] );
                    iFore  =  HexCharToInt( pText[ i + 2 ] );


                    iCurAttr  =  ( ( iBack & 0x0F )  <<  4 )  |
                                   ( iFore & 0x0F );

                    i += 2;
               }
          }
          else
          {
               tsw_maputc( w.minX + iPos,
                           w.minY + iLine - 1,
                           iCurAttr,
                           pText[ i ] );

               iPos++;
          }
     }
}



//**************************************************************************
//
// Display a help message
//
//     Prototype:  void showHelp( ChrP hlp[], char *pTitle, FlgT center );
//
//    Parameters:  hlp    ... An array of help lines to display
//                 pTitle ... Help window title
//                 center ... TRUE to center each line, FALSE otherwise
//
//       Returns:  Nothing
//
//       Remarks:
//
// 
//
// -------------------------------------------------------------------------
//
//    Created on:  ??/??/???? (Philippe Leybaert)
// Last modified:  08/06/1999 (Jeff Reeder)   Modified to support custom
//                            color codes in individual lines in the message
//
//**************************************************************************

void showHelp( ChrP  hlp[], 
               char *pTitle,
               FlgT  center )
{
     int bHasCodes = 0;


     tsw_cursoroff();


     ShrtT lines = 0;
     ShrtT width = 0;
     
     
     while ( hlp[ lines ]  !=  NULL )
     {
          if ( strchr( hlp[ lines ], 0x01 ) )
          {
               bHasCodes = 1;
          }


          width  =  max( width,  
                         CalcHelpWidth( hlp[ lines ] )  +  ( center * 4 ) );

          lines++;
     }


     ShrtT hFree  =  tsw_hsize  -  ( width + 3 );
     ShrtT vFree  =  tsw_vsize  -  ( lines + 1 );
     ShrtT ux     =  hFree / 2;
     ShrtT uy     =  vFree / 2;


     Window w( ux, 
               uy, 
               ux + width + 3, 
               uy + lines + 1, 
               0x5F,
               ( pTitle  &&  *pTitle ) 
                  ? ( SHADOW | BRACKETS )
                  : SHADOW,
               CHISEL_BORDER,
               0x5D,
               NULL,
               0x50 );


     w.open();


     if ( pTitle  &&
          *pTitle )
     {
          w.title( pTitle, 0x5F );
     }


     for ( ShrtT cnt = 0;  cnt < lines;  cnt++ )
     {
          if ( bHasCodes )
          {
               helpShowLine( w,
                             cnt + 1,    
                             0x5F,
                             hlp[ cnt ] );
          }
          else
          {
               String txt( hlp[ cnt ] );


               if ( center == TRUE )
               {
                    txt.justify( String::centered, width );
                    txt.rtrim();
               }
               
               
               w  <<  ChrP( txt );
               
               
               if ( 
                    ( cnt + 1  !=  lines )  && 
                    ( txt.len() < width ) 
                  )
               {
                    w << "\n";
               }
          }
     }


     tsw_showfooter( "Press any key to continue.", BAR_NORM );


     KB.get();
}

/*
 * Routine   : showAbout()
 * Purpose   : show the about screen
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

void showAbout()
{
   LCL ChrP aboutText[] =
   {
      "                                                                ",
      "               5EProBoard v" VERSION " Configuration Utility",
      "                                                                ",
      "    Copyright (c) 1990-1999  TeleGrafix Communications, Inc.    ",
      "                      All Rights Reserved                        ",
      "                                                                ",
      "              5DPortions written by Alain Schellinck             ",
      "                                                                ",
      NULL
   };


   showHelp( aboutText, "About ProBoard" );
}

/*---------------------------] END OF THE CODE [---------------------------*/
