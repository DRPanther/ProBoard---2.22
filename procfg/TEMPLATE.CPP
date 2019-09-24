
/* ------------------------------------------------------------
 * Filename ............... Template.Cpp
 *
 * General Purpose ........ Template IO functions
 * ------------------------------------------------------------
 * First date ............. 6 november 1993
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

#include <string.h>

#define Uses_MultiRecIO
#include <ProBoard.Hpp>
#include "Template.Hpp"

/*--] Code [-------------------------------------] TemplateIO class [------*/

/*
 * Routine   : TemplateIO()
 * Purpose   : The constructor
 * ------------------------------------------------------------------------
 * Parameters: ProBoard pex path, filename
 * Return    : None
 *
 */

TemplateIO::TemplateIO(ChrP path, ChrP fn)
{
   setFname(path, fn);
}

/*
 * Routine   : readRec()
 * Purpose   : read a record from the current file
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : TRUE if OK, FALSE if NOT
 *
 */

FlgT TemplateIO::readRec()
{
   return(File::read(this, recLen()) == recLen());
}

/*
 * Routine   : writeRec()
 * Purpose   : write a record to the current file
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : TRUE if OK, FALSE if NOT
 *
 */

FlgT TemplateIO::writeRec()
{
   return(File::write(this, recLen()) == recLen());
}

/*
 * Routine   : recLen()
 * Purpose   : return the record length
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : record length
 *
 */

ShrtT TemplateIO::recLen()
{
   return(SIZ(template_data));
}

/*
 * Routine   : clear()
 * Purpose   : clear the current record
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : None
 *
 */

void TemplateIO::clear()
{
   memset(this, 0, recLen());
}

/*
 * Routine   : inUse()
 * Purpose   : check if a record is in use
 * ------------------------------------------------------------------------
 * Parameters: None
 * Return    : TRUE if in use, FALSE if NOT
 *
 */

FlgT TemplateIO::inUse()
{
   return((strlen(desc) > 0) ? TRUE : FALSE);
}

/*
 * Routine   : copy()
 * Purpose   : Copy the data from another object (of the same type !)
 * ------------------------------------------------------------------------
 * Parameters: pointer to the other object
 * Return    : None
 *
 */

void TemplateIO::copy(TemplateIOP orig)
{
   memcpy(this, orig, recLen());
}

/*---------------------------] END OF THE CODE [---------------------------*/
