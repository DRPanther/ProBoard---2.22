template.hpp 1: 
template.hpp 2: 
template.hpp 3: 
template.hpp 4: 
template.hpp 5: 
template.hpp 6: 
template.hpp 7: 
template.hpp 8: 
template.hpp 9: 
template.hpp 10: 
template.hpp 11: 
template.hpp 12: 
template.hpp 13: 
template.hpp 14: 
template.hpp 15: 
template.hpp 16: 
template.hpp 17: 
template.hpp 18: 
template.hpp 19: 
template.hpp 20: 
template.hpp 21: 
template.hpp 22: 
template.hpp 23: 
template.hpp 24: 
template.hpp 25: const ShrtT maxTemplates = 256;
template.hpp 26: 
template.hpp 27: _StructType(template_data)
template.hpp 28: struct template_data
template.hpp 29: {
template.hpp 30: ChrT     desc     [ 61];            
template.hpp 31: ChrT     comment  [ 81];            
template.hpp 32: ChrT     valMsg   [  9];            
template.hpp 33: WrdT     secLevel;                  
template.hpp 34: WrdT     subscribe;                 
template.hpp 35: WrdT     expLevel;                  
template.hpp 36: WrdT     tbTime;                    
template.hpp 37: WrdT     tbKbyte;                   
template.hpp 38: WrdT     netCredit;                 
template.hpp 39: IntT     logLevel;                  
template.hpp 40: FlgT     hidden;                    
template.hpp 41: FlgT     noTops;
template.hpp 42: FlgT     atten;
template.hpp 43: FlgT     noKill;
template.hpp 44: FlgT     ignDld;
template.hpp 45: FlgT     deleted;                   
template.hpp 46: ChrT     reserved [99];             
template.hpp 47: PbFlags  valFlagsOn;                
template.hpp 48: PbFlags  valFlagsOff;               
template.hpp 49: PbFlags  expFlagsOn;                
template.hpp 50: PbFlags  expFlagsOff;               
template.hpp 51: };
template.hpp 52: 
template.hpp 53: _ClassType(TemplateIO)
template.hpp 54: class TemplateIO : public template_data, public MultiRecIO
template.hpp 55: {
template.hpp 56: public:
template.hpp 57: TemplateIO     (  ChrP path, ChrP fn = "Template.Pb"        );
template.hpp 58: 
template.hpp 59: FlgT     readRec        (                                            );
template.hpp 60: FlgT     writeRec       (                                            );
template.hpp 61: ShrtT    recLen         (                                            );
template.hpp 62: void     clear          (                                            );
template.hpp 63: void     copy           (  TemplateIOP orig                          );
template.hpp 64: 
template.hpp 65: FlgT     inUse          (                                            );
template.hpp 66: };
template.hpp 67: 
template.hpp 68: extern TemplateIOP tpl;
template.hpp 69: 
template.hpp 70: 
template.hpp 71: 
template.hpp 72: 