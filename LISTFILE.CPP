#define Use_LinkedList
#define Use_TagList

#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <stdlib.h>
#include <tswin.hpp>
#include "proboard.hpp"

static char *valid="ABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890#$";

static void near getfname(char *s,char *fn);
static bool near displayline(char *line,FileArea& fa);

const int MAX_TAGS = 52;

LinkedList<FilesIdx> taglist;

FilesIdx local_tags[MAX_TAGS];
int      num_local_tags;

static   int remember_area = 0;
static  long remember_pos  = -1;

static int last_tag = 0;

char tag_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890*#$!@%+=&?|></\~^";

static bool eol = FALSE;

static int
filelist_more()
{
   int rep;
   int i;

   byte oldcolor=SCREEN.attrib();

   for(;;)
   {
      /*
      if(rip_mode)
      {
         rip_show_more_tag();
         io.show_remote = FALSE;
      }
      */

      if(eol)
         io << S_END_OF_LIST_TAG;
      else
         io << S_MORE_TAG;

      if(eol)
         rep = wait_language_hotkeys(K_END_OF_LIST_TAG);
      else
         rep = wait_language_hotkeys(K_MORE_TAG);

      io << '\r' << String(' ',79) << '\r';

      if(rep == '\r') rep = 0;

      if(rep == (eol ? 1 : 3))
      {
         io.show_remote = TRUE;

         String tagged;

         io << '\r' << S_ENTER_FILE_TAGS;

         byte tagcharcol = SCREEN.attrib();

         for(;;)
         {
            char c = toupper(io.wait());

            if(c == '\r') break;

            if(c == '\b' && tagged.len())
            {
               io << "\b\b  \b\b";

               tagged.delLast();
            }

            if(tagged.len() >= 26) continue;

            for(i=0; tag_chars[i] ; i++)
            {
               if(c == tag_chars[i]) break;
            }

            if(tag_chars[i] && i < num_local_tags)
            {
               io << c << "\7,";
               io.fullcolor(tagcharcol);
               tagged << c;
            }
         }

         for(i=0;i<tagged.len();i++)
         {
            for(int j=0; tag_chars[j] ; j++)
            {
               if(tagged[i] == tag_chars[j]) break;
            }

            if(!tag_chars[j]) continue;

            if(j+1 > num_local_tags) continue;

            if(!is_tagged(local_tags[j].name , local_tags[j].area))
               taglist.add(local_tags[j]);
         }

         io << '\r' << S_X_FILES_TAGGED(form("%d",taglist.count()));

         io.wait('\r');

         io << "\7\r" << String(' ',language_string_length(S_X_FILES_TAGGED(form("%d",taglist.count())))) << '\r';

         //io << "\7\r                                                                \r";

         //if(rip_mode) io.show_remote = FALSE;
      }

      if(rep == (eol ? 2:4))
      {
         io.show_remote = TRUE;

         edit_taglist();

         io << "\f\7";

         rep = 0;

         //if(rip_mode) io.show_remote = FALSE;
      }

      if(eol)
      {
         if(rep == 0) break;
      }
      else
      {
         if(rep >= 0 && rep <= 2) break;
      }

   }

   io.show_remote = TRUE;

   io.fullcolor(oldcolor);

   /*
   if(rip_mode)
   {
      rip_clear_status();
      rip_show_enter();
   }
   */

   return rep;
}

void
clear_tags()
{
   last_tag       = 0;
   num_local_tags = 0;
}

static bool
is_whitespace(char c)
{
   return (c==' ' || c=='\t');
}

static char
first_nonblank( char *line )
{
   char f =*line;

   if(is_whitespace(*line))
   {
      for(int i=0 ; line[i] != '\0' && is_whitespace(line[i]) ; i++) {}

      f = line[i];
   }

   return f;
}

static int
display_areaname(int areanum, char *areaname , int prevsize)
{
   io << form("\7%3d - %*s\r\xFF",areanum,-prevsize,areaname);

   return strlen(areaname);
}

void
list_files(char *data)
{
   FileArea fa;
   int areanum;

   io << "\f\n" << S_FILELIST_PRESS_S_TO_STOP << "\n\n\xFF";

   linecounter(2);
   io.enablestop();

   if(toupper(data[0])=='X')
      areanum = user.fileArea;
   else
      areanum = atoi(data);

   if(!fa.read(areanum)) return;

   LOG(2,"Files listed (%s)",fa.name);

   File tf;
   if(!tf.open(fa.listpath,fmode_read | fmode_text))
   {
      LOG("Can't open file list %s",fa.listpath);
      return;
   }

   eol = FALSE;
   io.more_func = filelist_more;
   clear_tags();

   char *line = new char[500];

   bool interrupted = FALSE;

   remember_area = fa.areaNum;
   remember_pos  = 0;

   while(tf.readLine(line,499))
   {
      strip_linefeed(line);

      if(!displayline(line,fa))
      {
         interrupted = TRUE;
         break;
      }

      remember_pos = tf.pos();
   }

   io.more_func = NULLFUNC;

   delete [] line;

   if(!interrupted)
   {
      if(num_local_tags)
      {
         eol = TRUE;
         filelist_more();
      }

      //io << '\n' << S_END_OF_FILE_LISTING << ' ' << S_PRESS_ENTER_TO_CONTINUE;
   }
}

void
keyword_search(char *data)
{
   FileArea fa;
   char item[41];

   io << "\f\n" << S_KEYWORD_SEARCH_TITLE_AND_PROMPT;

   io.read(item,40,READMODE_UPALL);

   if(!*item) return;

   String buffer[10];

   int pointer = 0,
       found   = 0,
       group   = 0;

   String param[40];
   BitArray arealist(MAX_FILE_AREAS,1);

   int npara=parse_data(data,param);

   LOG(2,"Keyword search (%s)",item);

   io << "\n\f\n" << S_SEARCHING_FOR_KEYWORD(item) << "\n\n";

   create_arealist(param,npara,arealist);

   for(int i = 0;i<npara;i++)
   {
      if(param[i] == "/FG")
         group           = user.msgGroup;

      if(!strnicmp(param[i],"/FG=",4))
         group           = atoi(&param[i][4]);
   }

   linecounter(2);
   io.enablestop();
   File tf;

   char *line = new char[500];

   eol = FALSE;
   io.more_func = filelist_more;
   clear_tags();

   bool interrupted = FALSE;
   int prev_arealen = 1;

   for(int z=1 ; z<=FileArea::highAreaNum() && !interrupted; z++)
   {
      bool linefeed = TRUE;

      if(!arealist[z] || !fa.read(z) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;

      if(group && !fa.allGroups)
      {
         for(int g=0;g<4;g++)
         {
            if(fa.groups[g] == group)
               break;
         }

         if(g==4)
            continue;
      }

      prev_arealen = display_areaname(z,fa.name,prev_arealen);

      if(stopped) break;

      tf.close();
      if(!tf.open(fa.listpath,fmode_read | fmode_text))
      {
         LOG("Can't open file list %s",fa.listpath);
         continue;
      }

      while(tf.readLine(line,499) && !interrupted)
      {
         strip_linefeed(line);
         if(strchr(valid,toupper(*line)) && *line)
         {
            found=0;
            pointer=0;
            buffer[0]=line;
            if(strstrl(line,item))
            {
               found = TRUE;

               if(linefeed)
               {
                  io << '\n';

                  if(linecounter())
                  {
                     interrupted = TRUE;
                     break;
                  }
               }

               linefeed = FALSE;

               if(!displayline(line,fa))
               {
                  interrupted = TRUE;
                  break;
               }
            }
         }
         else
         {
            char fnb = first_nonblank(line);

            if(fnb && strchr("+!;|",fnb))
            {
               if(pointer<9)
                  pointer++;
               else
                  pointer=1;

               buffer[pointer]=line;

               if(found)
               {
                  if(linefeed)
                  {
                     io << '\n';

                     if(linecounter())
                     {
                        interrupted = TRUE;
                        break;
                     }
                  }

                  linefeed = FALSE;

                  if(!displayline(line,fa))
                  {
                     interrupted = TRUE;
                     break;
                  }
               }
               else
               {
                  if(strstrl(line,item))
                  {
                     for(int i=0;i<=pointer && !interrupted;i++)
                     {
                        if(linefeed)
                        {
                           io << '\n';

                           if(linecounter())
                           {
                              interrupted = TRUE;
                              break;
                           }
                        }

                        linefeed = FALSE;

                        if(!displayline(buffer[i],fa))
                        {
                           interrupted = TRUE;
                           break;
                        }
                     }

                     found = TRUE;
                  }
               }
            }
         }
      }
   }

   delete [] line;

   io.more_func = NULLFUNC;

   if(num_local_tags && !interrupted)
   {
      eol = TRUE;
      filelist_more();
   }
}



#define MAX_NEWFILES 200



void new_files( char *data )
{
     int      group = 0;
     FileArea fa;
     Date     cmpdate;
     char     dateinput[ 9 ];
     String   param[ 40 ];
     BitArray arealist( MAX_FILE_AREAS, 1 );
     
     struct   newfile
     {
          char name[ 13 ];
     }
          *newf;

     int npara  =  parse_data( data, param );


     for ( int i = 0;  i < npara;  i++ )
     {
          if ( param[ i ]  ==  "/FG" )
          {
               group  =  user.msgGroup;
          }


          if ( ! strnicmp( param[ i ],  
                           "/FG=",  
                           4 ) )
          {
               group  =  atoi( & param[ i ][ 4 ] );
          }
     }


     LOG( 2, "New files listed" );


     io << "\f\n" 
        << S_NEW_FILES_SEARCH_TITLE_AND_PROMPT( date_formats_long[ user.dateFormat ] );

     InputDate( cmpdate, user.dateFormat );


     if ( ! cmpdate.ok() )
     {
          cmpdate         = user.lastNewFilesCheck;
          newFilesChecked = TRUE;
     }


     io << "\n\f\n"
        << S_SEARCHING_FOR_NEW_FILES( form( "%d %s %4d",
                                            cmpdate[ 0 ],
                                            months_short[ cmpdate[ 1 ] ],
                                            cmpdate[ 2 ]  +  1900 ) )

                                            // Y2K LOOKS OKAY FOR 4-DIGIT
        << "\n\n\xFF";


     create_arealist( param,
                      npara,
                      arealist );


     newf  =  new newfile[MAX_NEWFILES];


     linecounter( 2 );

     io.enablestop();


     eol          = FALSE;
     io.more_func = filelist_more;

     clear_tags();


     char *line          =  new char[ 500 ];
     bool  interrupted   =  FALSE;
     int   prev_arealen  =  1;


     for ( int z = 1;  
           
           z <= FileArea::highAreaNum()  &&  
           ! interrupted;  
           
           z++ )
     {
          bool linefeed = TRUE;


          if ( ! arealist[ z ]  || 
               ! fa.read( z )   || 
               ! check_access( fa.level,
                               fa.flags,
                               fa.flagsNot ) )
          {
               continue;
          }


          if ( group  && 
               ! fa.allGroups )
          {
               for ( int g = 0;  g < 4;  g++ )
               {
                    if ( fa.groups[ g ]  ==  group )
                    {
                         break;
                    }
               }              


               if ( g == 4 )
               {
                    continue;
               }
          }


          prev_arealen  =  display_areaname( z,
                                             fa.name,
                                             prev_arealen );

          if ( stopped )
          {
               interrupted = TRUE;
               
               break;
          }


          DirScan find( FileName( fa.filepath, "*.*" ) );


          for ( int num_new = 0;  
          
                int( find )  &&  
                num_new < MAX_NEWFILES;
                
                find++ )
          {
               if ( find.date() >= cmpdate )
               {
                    strcpy( newf[ num_new++ ].name,  find.name() );
               }
          }


          if ( ! num_new ) 
          {
               continue;
          }


          File tf;


          if ( ! tf.open( fa.listpath,
                          fmode_read | fmode_text ) )
          {
               LOG( "Can't open file list %s",  fa.listpath );

               continue;
          }


          for ( bool found = FALSE;  tf.readLine( line, 499 ); )
          {
               strip_linefeed( line );


               if ( strchr( valid,  toupper( *line ) )  && 
                    *line )
               {
                    char file[ 13 ];


                    found = FALSE;


                    getfname( line, file );

                    strupr( file );


                    for ( int i = 0;  i < num_new;  i++ )
                    {
                         if ( ! strcmp( newf[ i ].name,  file ))
                         {
                              break;
                         }
                    }


                    if ( i < num_new )
                    {
                         found = TRUE;


                         if ( linefeed )
                         {
                              io << '\n';


                              if ( linecounter() )
                              {
                                   interrupted = TRUE;

                                   break;
                              }
                         }
                    
                    
                         linefeed = FALSE;
                    
                    
                         if ( ! displayline( line, fa ) )
                         {
                              interrupted = TRUE;

                              break;
                         }
                    }
               }
               else
               {
                    char fnb  =  first_nonblank( line );


                    if ( fnb    && 
                         found  && 
                         strchr( "+|!;", fnb ) )
                    {
                         if ( linefeed )
                         {
                              io << '\n';


                              if ( linecounter() )
                              {
                                   interrupted = TRUE;

                                   break;
                              }
                         }
                    
                    
                         linefeed = FALSE;

                    
                         if ( ! displayline( line, fa ) )
                         {
                              interrupted = TRUE;

                              break;
                         }
                    }
               }
          }
     }


     delete [] line;
     delete [] newf;


     io.more_func = NULLFUNC;


     if ( ! interrupted )
     {
          if ( num_local_tags )
          {
               eol = TRUE;

               filelist_more();
          }

       // io << '\n' 
       //    << S_END_OF_FILE_SEARCH 
       //    << ' ' 
       //    << S_PRESS_ENTER_TO_CONTINUE;
     }
}


void
filename_search(char *data)
{
   int group = 0;
   char search[13];

   io << "\f\n" << S_FILENAME_SEARCH_TITLE_AND_PROMPT;

   io.read(search,12,READMODE_UPALL);

   if(!*search) return;

   FileArea fa;
   String param[40];
   BitArray arealist(MAX_FILE_AREAS,1);

   int npara = parse_data(data,param);

   for(int i = 0;i<npara;i++)
   {
      if(param[i] == "/FG")
         group           = user.msgGroup;

      if(!strnicmp(param[i],"/FG=",4))
         group           = atoi(&param[i][4]);
   }

   LOG(2,"Filename search (%s)",search);

   io << "\n\f\n" << S_SEARCHING_FOR_FILENAME(search) << "\n\n\xFF";

   create_arealist(param,npara,arealist);

   linecounter(2);
   io.enablestop();

   char *line = new char[500];

   bool interrupted = FALSE;
   int prev_arealen = 1;

   eol = FALSE;
   io.more_func = filelist_more;
   clear_tags();

   for(int z=1 ; z<=FileArea::highAreaNum() && !interrupted ; z++)
   {
      bool linefeed = TRUE;

      if(!arealist[z] || !fa.read(z) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;

      if(group && !fa.allGroups)
      {
         for(int g=0;g<4;g++)
         {
            if(fa.groups[g] == group)
               break;
         }

         if(g==4)
            continue;
      }

      prev_arealen = display_areaname(z,fa.name,prev_arealen);
      //io << form("\7%3d - %s\n\xFF",z,fa.name);

      File tf;

      if(!tf.open(fa.listpath,fmode_read | fmode_text))
      {
         LOG("Can't open file list %s",fa.listpath);
         continue;
      }

      if(stopped)
      {
         interrupted = TRUE;
         break;
      }

      for(bool found = FALSE ; tf.readLine(line,499) ; )
      {
         strip_linefeed(line);
         if(strchr(valid,toupper(*line))  && *line)
         {
            found=0;
            char file[13];
            getfname(line,file);
            if(!matchfile(search,file)) continue;

            found = TRUE;

            if(linefeed)
            {
               io << '\n';

               if(linecounter())
               {
                  interrupted = TRUE;
                  break;
               }
            }
            linefeed = FALSE;

            if(!displayline(line,fa))
            {
               interrupted = TRUE;
               break;
            }
         }
         else
         {
            char fnb = first_nonblank(line);

            if(fnb && found && strchr("+|!;",fnb))
            {
               if(linefeed)
               {
                  io << '\n';

                  if(linecounter())
                  {
                     interrupted = TRUE;
                     break;
                  }
               }

               linefeed = FALSE;

               if(!displayline(line,fa))
               {
                  interrupted = TRUE;
                  break;
               }
            }
         }
      }
   }

   delete [] line;

   io.more_func = NULLFUNC;

   if(!interrupted)
   {
      if(num_local_tags)
      {
         eol = TRUE;
         filelist_more();
      }

      //io << '\n' << S_END_OF_FILE_SEARCH << ' ' << S_PRESS_ENTER_TO_CONTINUE;
   }
}

static void near
getfname(char *s,char *fn)
{
   for(int i=0 ; i<12 && *s!=' ' && *s ; i++) *fn++ = *s++;
   *fn = '\0';
}

void
split_line( char *s , String& fname , Date& date , long& size , int& counter , String& desc , FileArea& fa)
{
   char *prev_s;
   String token;

   size = -1;
   date.set(0,0,0);

   fname = "";
   while(!is_whitespace(*s) && *s) fname << (*s++);

   prev_s = s;

   while(*s)
   {
      token.clear();

      prev_s = s;

      while(is_whitespace(*s) && *s) s++;                   // Skip whitespace
      while(!is_whitespace(*s) && *s) token << (*s++);

      bool date_del_found = FALSE;
      bool non_num_found  = FALSE;

      for(int j=0;token[j];j++)
      {
         char c = token[j];

         if(strchr("/-.",c))
            date_del_found = TRUE;
         else
            if(!isdigit(c))
               non_num_found = TRUE;
      }

      if(non_num_found)
         break;

      if(date_del_found)
         parse_date(date,token,fa.dateFormat);
      else
         size = atol(token);

      if(!*s)
         prev_s = s;
   }

   s = prev_s;

   while(is_whitespace(*s) && *s) s++;                   // Skip whitespace

   desc = s;

   if(size<0 || !date.ok())
   {
      DirScan f(FileName(fa.filepath,fname));

      if(!int(f))
      {
         size = 0;
         date(0,0,0);
      }
      else
      {
         if(!date.ok())
            date = f.date();

         if(size < 0)
            size = f.size();
      }
   }

   counter = 0;

   bool in_counter = FALSE;
   String counter_string;

   for(int i=0 ; desc[i] ; i++)
   {
      if(desc[i] == ']')
      {
         i++;
         while(is_whitespace(desc[i]) && desc[i]) i++;

         if(!desc[i]) i--;

         desc.del(0,i);

         counter = atoi(counter_string);
         break;
      }

      if(in_counter)
      {
         if(isdigit(desc[i]))
            counter_string << desc[i];
         else
            break;
      }

      if(desc[i] == '[')
         in_counter = TRUE;
   }
}

static void
RemoveDangerousChars(String& s)
{
   if(strpbrk(s,"\x09\x18\x19"))
   {
      int i;

      while((i = s.find("\x09")) >= 0)
         s.del(i,1);
      while((i = s.find("\x18")) >= 0)
         s.del(i,1);
      while((i = s.find("\x19")) >= 0)
         s.del(i,1);
   }
}



static 
bool near displayline( char     *line,
                       FileArea &fa )
{
     String str;
     String wrap;
     String wrapcolor;
     int    left_margin;


     if ( 
          (
            strchr( valid,  toupper( *line ) )  || 
            (
              *line=='!'  && 
              strchr( valid,  toupper( line[ 1 ] ) )
            )
          )
          && *line )
     {
          Date   date;
          long   size;
          int    counter;
          String fname;
          String desc;


          split_line( line, 
                      fname, 
                      date, 
                      size, 
                      counter, 
                      desc, 
                      fa );

          RemoveDangerousChars( desc );


          str  =  fullcolor_string( cfg.listColorTag );

          local_tags[ last_tag ].area  =  fa.areaNum;


          strcpy( local_tags[ last_tag ].name,  fname );
          strupr( local_tags[ last_tag ].name );


          if ( size )
          {
               str  <<  tag_chars[ last_tag ] 
                    <<  ' ';


               if ( last_tag >= num_local_tags ) 
               {
                    num_local_tags  =  last_tag + 1;
               }


               last_tag  =  ( ++last_tag )  %  
                            min( MAX_TAGS,
                                 (
                                  /* rip_mode 
                                    ? rip_screenlines
                                    : */
                                         user.screenLength 
                                 )
                               );
          }
          else
          {
               str << "  ";
          }


          str  <<  fullcolor_string( cfg.listColorFileName );

          str  <<  form( "%-12s ",  (char *) fname );


          if ( size )
          {
               str  <<  fullcolor_string( cfg.listColorDate );

               str  <<  date.format( date_formats_long[ user.dateFormat ] );


            // form( "%02d/%02d/%02d",
            //       date[ ( cfg.europe ? 0 : 1 ) ],
            //       date[ ( cfg.europe ? 1 : 0 ) ],
            //       date[ 2 ] % 100 );                // Y2K FIXED!
            //       date[ 2 ] );                      // Y2K BUG!


               str  <<  fullcolor_string( cfg.listColorSize );


               if ( ( size + 512L )  /  1024L  >  1000L )
               {
                    str  <<  form( "%2ld.%1ldM ",
                                   ( size + 512L )  /    1024L / 1024L,
                                 ( ( size + 512L )  %  ( 1024L * 1024L ) )  /  
                                     1024L * 10L / 1024L );
               }
               else
               {
                    if ( size >= 1000L )
                    {
                         str  <<  form( "%4ldk ", 
                                        ( size + 512L )  /  1024L );
                    }
                    else
                    {
                         str  <<  form( "%4ldb ",  size );
                    }
               }
          }
          else
          {
               str  <<  fullcolor_string( cfg.listColorMissing );
               str  <<  "  <MISSING>   ";
          }


          if ( ! cfg.hideFileCounter )
          {
               str  <<  fullcolor_string( cfg.listColorCounter );
               str  <<  form( "[%02d]",  counter );


               if ( counter < 100 )
               {
                    str << ' ';
               }
          }


          wrapcolor  =  fullcolor_string( cfg.listColorDesc );
          
          
          str << wrapcolor;
          io  << str;


          if ( stopped ) 
          {
               return FALSE;
          }


          left_margin  =  34  -  5 * cfg.hideFileCounter;

          str = desc;
     }
     else
     {
          char fnb  =  first_nonblank( line );


          if ( fnb  && 
               strchr( "+|", fnb ) )
          {
               line  =  strchr( line, fnb );
          }


          switch ( *line )
          {
               case '|':
               case '+':

                    line++;

                    while ( is_whitespace( *line )  && 
                            *line != '\0' )
                    {
                         line++;
                    }


                    left_margin  =  34  -  5 * cfg.hideFileCounter;
                    wrapcolor    =  fullcolor_string( cfg.listColorDesc );

                    io.fullcolor( cfg.listColorDesc );

                    io  <<  String( ' ', left_margin );
               
                    break;


               case '!':
               
                    left_margin = 0;

                    io.fullcolor( cfg.listColorDesc );

                    wrapcolor  =  fullcolor_string( cfg.listColorDesc );

                    line++;
               
                    break;


               case ';':
               
                    left_margin = 0;

                    io.fullcolor( cfg.listColorDesc );

                    wrapcolor  =  fullcolor_string( cfg.listColorDesc );

                    line++;
               
                    break;


               default :
               
                    left_margin = 0;

                    io.fullcolor( cfg.listColorSeperator );

                    wrapcolor  =  fullcolor_string( cfg.listColorSeperator );
               
                    break;
          }


          str = line;

          RemoveDangerousChars( str );
     }


     str.replace( "\t", " " );


     if ( stopped ) 
     {
          return FALSE;
     }


     for ( ; ; )
     {
          int l  =  wordwrap( str,
                              wrap,
                              79 - left_margin );

          str << '\n';
          io  << str;


          if ( linecounter()  || 
               stopped ) 
          {
               return FALSE;
          }


          if ( ! l ) 
          {
               break;
          }


          io  <<  wrapcolor 
              <<  String( ' ', left_margin );

          if ( stopped ) 
          {
               return FALSE;
          }


          str = wrap;
     }


     io.flush();


     return TRUE;
}


