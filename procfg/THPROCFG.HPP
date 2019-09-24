
/* ------------------------------------------------------------
 * Filename ............... ThProCfg.Hpp
 *
 * General Purpose ........ main include file
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

#include <ProBoard.Hpp>
#include "ProCfg.Hpp"

/*-------------------------------------------------------------------------*/

const BytT  nBlack    = 0; const BytT  hBlack    =  8;
const BytT  nBlue     = 1; const BytT  hBlue     =  9;
const BytT  nGreen    = 2; const BytT  hGreen    = 10;
const BytT  nCyan     = 3; const BytT  hCyan     = 11;
const BytT  nRed      = 4; const BytT  hRed      = 12;
const BytT  nMagenta  = 5; const BytT  hMagenta  = 13;
const BytT  nYellow   = 6; const BytT  hYellow   = 14;
const BytT  nWhite    = 7; const BytT  hWhite    = 15;

inline   setFBColor(BytT fg, BytT bg)
{
   return((bg << 4) + fg);
}

/*--] Structures [-------------------------------] userFilter [------------*/

_StructType(userFilter)
struct userFilter
{
   ChrT     name[36];
   ChrT     location[26];
   PbFlags  flagsOn;
   PbFlags  flagsOff;
   WrdT     lowerLevel;
   WrdT     higherLevel;

            userFilter  (                                               )
            {
               clear();
            }

   void     clear       (                                               )
            {
               CLEAR_OBJECT(name);
               CLEAR_OBJECT(location);
               flagsOn     = "";
               flagsOff    = "";
               lowerLevel  = 0;
               higherLevel = WrdT(65535L);
            }
};

/*--] Structures [-------------------------------] globalChange [----------*/

_StructType(globalChange)
struct globalChange
{
   enum what
   {
      chExpFlagsOn,             // expiration flags on
      chExpFlagsOff,            // expiration flags off
      chSecFlags,               // security flags
      chSecLevel,               // security level
      chExpLevel,               // expiration level
      chNetCredit               // netmail credit
   };

   enum who
   {
      chEqualTo,                // equal to specified level
      chLessThan,               // less than specified level
      chGreaterThan,            // greater than specified level
      chAllUsers                // all users
   };

   ShrtT    changeWhat;
   ShrtT    changeWho;
   UShrtT   changeLevel;

   PbFlags  expFlagsOn;
   PbFlags  expFlagsOff;
   PbFlags  secFlags;
   UShrtT   secLevel;
   UShrtT   expLevel;
   LngT     netCredit;

            globalChange   (                                            )
            {
               changeWhat  = chSecLevel;
               changeWho   = chAllUsers;
               changeLevel = 0;

               expFlagsOn  = "";
               expFlagsOff = "";
               secFlags    = "";
               secLevel    = 0;
               expLevel    = 0;
               netCredit   = 0;
            }
};

/*--] Classes [----------------------------------] fileEntry [-------------*/

_ClassType(fileEntry)
class fileEntry : public Object
{
public:
   FileName fname;
   LngT     size;
   Date     date;
   Time     time;

   virtual  ~fileEntry     (                                            )
            {
            }
};

/*--] External variables [-------------------------------------------------*/

extern ChrT        usrFind[80];
extern LngP        userList;
extern UShrtT      userListCount;
extern UShrtT      NumUsers;

/*--] prototypes [-------------------------------] EditUser.Cpp [----------*/
void  clearUserList  (                                                  );
ShrtT fillUserList   (  userFilter& filter                              );
ShrtT getDiskRecNo   (  ShrtT recno                                     );
ShrtT getListRecNo   (  ShrtT recno                                     );
ShrtT filterUser     (                                                  );
ShrtT searchUser     (  ShrtT startRec                                  );
void  listUser       (  IntT recno, IntT x, IntT y                      );
ShrtT pickUser       (  ShrtT curRecNo                                  );

/*--] prototypes [-------------------------------] EditVTpl.Cpp [----------*/
ShrtT pickTemplate   (  ShrtT curRecNo = 0                              );

/*--] prototypes [-------------------------------] ProMisc.Cpp [-----------*/
KEY   enterPwd       (  IntT mode, Window& w, IntT x, IntT y, ChrT attr,
                        AnyP data                                       );

ShrtT askYesNo       (  ChrP title                                      );
void  showHelp       (  ChrP hlp[], char *pTitle=NULL, FlgT center = FALSE                 );

/*--] prototypes [---------------------------------------------------------*/

/*---------------------------] END OF THE CODE [---------------------------*/
