
/* ------------------------------------------------------------
 * Filename ............... Template.Hpp
 *
 * General Purpose ........ include file with template class
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

/*--] Classes [----------------------------------] TemplateIO [------------*/

const ShrtT maxTemplates = 256;

_StructType(template_data)
struct template_data
{
   ChrT     desc     [ 61];            // description
   ChrT     comment  [ 81];            // comment
   ChrT     valMsg   [  9];            // message to send when validating
   WrdT     secLevel;                  // security level
   WrdT     subscribe;                 // subscribe period in days
   WrdT     expLevel;                  // expiration level
   WrdT     tbTime;                    // time to add on the timebank
   WrdT     tbKbyte;                   // kbyte to add on the timebank
   WrdT     netCredit;                 // netmail credit
   IntT     logLevel;                  // his/her loglevel
   FlgT     hidden;                    // some UFLAGS_xxx booleans
   FlgT     noTops;
   FlgT     atten;
   FlgT     noKill;
   FlgT     ignDld;
   FlgT     deleted;                   // is this template deleted ?
   ChrT     reserved [99];             // reserved for the future
   PbFlags  valFlagsOn;                // access flags to switch on  (validated)
   PbFlags  valFlagsOff;               // access flags to switch off (validated)
   PbFlags  expFlagsOn;                // access flags to switch on  (expired)
   PbFlags  expFlagsOff;               // access flags to switch off (expired)
};

_ClassType(TemplateIO)
class TemplateIO : public template_data, public MultiRecIO
{
public:
            TemplateIO     (  ChrP path, ChrP fn = "Template.Pb"        );

   FlgT     readRec        (                                            );
   FlgT     writeRec       (                                            );
   ShrtT    recLen         (                                            );
   void     clear          (                                            );
   void     copy           (  TemplateIOP orig                          );

   FlgT     inUse          (                                            );
};

extern TemplateIOP tpl;

/*---------------------------] END OF THE CODE [---------------------------*/
