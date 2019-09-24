#include <stdio.h>
#include <string.h>
#include <tswin.hpp>
#include "procfg.hpp"



const int MAX_GROUPS = 255;



struct AreaGroup : _AreaGroup
{
     static File f;
     static int file_count;
     static String fName;


  public:
     AreaGroup();
     ~AreaGroup();

     static void setFname(char *n)
          {
          fName = n;
          }

     bool read(int);
     bool write(int);
     bool clear(int);

     int  numGroups();
     void truncate();
};



int    AreaGroup::file_count = 0;
File   AreaGroup::f;
String AreaGroup::fName;



AreaGroup::AreaGroup()
{
     if(file_count <= 0)
     {
          f.open(FileName(syspath,fName),fmode_rw | fmode_copen);
          file_count = 0;
     }

     file_count++;
}



AreaGroup::~AreaGroup()
{
     if(file_count <= 1)
     {
          f.close();
          file_count = 1;
     }


     file_count--;
}



bool AreaGroup::read(int rec)
{
     f.seek(long(rec-1) * sizeof(_AreaGroup));
     return (f.read(this,sizeof(*this)) == sizeof(*this)) ? TRUE : FALSE;
}



bool AreaGroup::write(int rec)
{
     f.seek(long(rec-1) * sizeof(_AreaGroup));
     return (f.write(this,sizeof(*this)) == sizeof(*this)) ? TRUE : FALSE;
}



bool AreaGroup::clear(int rec)
{
     CLEAR_OBJECT(*this);

     return write(rec);
}



int AreaGroup::numGroups()
{
     return f.len() / sizeof(_AreaGroup);
}



void AreaGroup::truncate()
{
     AreaGroup fa;
     int i = numGroups();

     for(;i>0;i--)
     {
          if(!fa.read(i)) continue;


          if(fa.name[0])
          {
               f.seek(long(i) * sizeof(_AreaGroup));
               f.cut();
               break;
          }
     }


     if(!i)
     {
          f.seek(0);
          f.cut();
     }
}



static 
void farea_list_func(int i,int x,int y)
{
     AreaGroup fa;
     char *ptr;

     if(!fa.read(i+1) || !fa.name[0]) 
     {
          ptr = form("%5d   תתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתתת",i+1);
     }
     else    
     {
          ptr = form("%5d   %s",i+1,fa.name);
     }


     if(strlen(ptr)>43) 
     {
          ptr[43]=0;
     }


     tsw_maputs( x,      y,  0x3F,  ptr );
     tsw_maputc( x + 6,  y,  0x3F,  '³' );
}



static 
void helpfunction(int i)
{
     char *txt[]  =  {
                       "Name/description of this group",
                       "Level needed to access this group",
                       "Flags needed to access this group"
                     };

     tsw_showfooter(txt[i], BAR_COLOR);
}



void edit_groups( bool filegroups )
{
     Field frm[]  =  
     {
          { FRM_STRING  , 0 , 0,0 , 0         , 79 , 45, 17,2  },
          { FRM_UNSIGNED, 0 , 0,0 , 0         , 5  , 6 , 17,4  },
          { FRM_FUNCTION, 0 , 0,flagonoff_select ,0, 26 , 26, 17,5 }
     };


     AreaGroup::setFname(filegroups ? "FGROUPS.PB" : "MGROUPS.PB");

     AreaGroup fa;

     int num_areas = fa.numGroups();

     KEY hotkeys[]={ '1','2','3','4','5','6','7','8','9',KEY_INS, KEY_DEL , NULL };

     SelectWindow sw;

 
     sw.open( 3,
              3,
              50,
              tsw_vsize - 3,
              0x30,
              SHADOW | BRACKETS,
              CHISEL_BORDER,
              0x3F,
              NULL,
              0x38 );

     sw.title( filegroups ? "Select File Area Group"
                          : "Select Message Area Group",
               0x30 );

     sw.define(num_areas,0x1F,farea_list_func,0,NULL,hotkeys);

     sw.direct(7, 0,             0x3F, 'ֲ');
     sw.direct(7, tsw_vsize - 6, 0x38, 'ֱ');


again:


     for(;;)
     {
          int alias;
         
          tsw_showfooter( "\x1<Ins>\x1 - Add Area      \x1<Del>\x1 - Delete Area      \x1<Enter>\x1 - Edit Area", 
                          BAR_NORM,
                          BAR_HIGH );
         
          int choice=sw.process();
         
         
          if(choice==SL_ESC) 
          {
               break;
          }


          if(choice==SL_HOTKEY)
          {
               switch ( sw.hotkey )
               {
                    case KEY_DEL:
                    {
                         if(!fa.read(sw.current+1))
                         {
                              tsw_beep();
                              continue;
                         }


                         if(fa.name[0])
                         {
                              if(ask_save() == 0)
                              {
                                   fa.clear(sw.current+1);
                              }
                         }

                         
                         continue;
                    }


                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': 
                    
                         KB.push((KEY)(char)sw.hotkey);


                    case KEY_INS:
                    {
                         int newareanum = num_areas+1;

                         Window w( 10,
                                   5,
                                   40,
                                   7,
                                   0x70,
                                   SHADOW,
                                   CHISEL_BORDER,
                                   0x7F,
                                   NULL,
                                   0x71 );


                         w.open();
                         
                         w << " New group number: ";
                         
                         tsw_cursoron();
                         w.attrib(0x1E);
                         w.scan(newareanum,5,6);
                         tsw_cursoroff();

                         if ( newareanum > MAX_GROUPS  || 
                              newareanum < 1 ) 
                         {
                              continue;
                         }


                         if ( newareanum > num_areas )
                         {
                              for ( int i  =  num_areas + 1;
                                    i <= newareanum;
                                    i++ )
                              {
                                   fa.clear( i );
                              }


                              num_areas = newareanum;

                              sw.redefine(num_areas);
                         }


                         choice = newareanum - 1;

                         sw.show(choice);
                    }
               }
          }


          if ( ! fa.read( choice + 1 ) )
          {
               tsw_beep();
               continue;
          }


          unsigned crcvalue = crc(&fa,sizeof(fa));


          if ( ! fa.name[ 0 ] )
          {
               memset(&fa,0,sizeof(fa));
               fa.level = 1;
          }


          AccessFlags *flags[2];

          flags[0] = &fa.flags;
          flags[1] = &fa.flagsNot;

          frm[0].value = fa.name;
          frm[1].value = &fa.level;
          frm[2].value = flags;


          FormWindow w( 8,
                        6,
                        73,
                        13,
                        0x70,
                        SHADOW | BRACKETS,
                        CHISEL_BORDER,
                        0x7F,
                        NULL,
                        0x78 );

          w.define( frm,
                    3,
                    0x71,
                    0x1E,
                    NULL,
                    helpfunction );

          w.open();

          w.title( form( filegroups 
                           ? " File Area Group #%d " 
                           : " Message Area Group #%d ",
                         choice + 1 ),
                   0x70 );

          w.attrib( 0x70 );

          w << "\n     Area name:\n\n"
               "  Access Level:\n"
               "  Access Flags:";


          tsw_cursoron();
          w.process();
          tsw_cursoroff();

          
          if ( crcvalue != crc( & fa,  sizeof( fa ) )  && 
               fa.name[ 0 ] )
          {
               if ( ask_save() == 0)
               {
                  fa.write(choice+1);
               }
          }
     }


     if(fa.read(num_areas))
     {
          if( ! fa.name[ 0 ] )
          {
               fa.truncate();
          }
     }
}



void edit_fgroups()
{
     edit_groups(TRUE);
}



void edit_mgroups()
{
     edit_groups(FALSE);
}

