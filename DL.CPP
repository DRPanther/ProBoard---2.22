#define Use_TagList
#define Use_LinkedList

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>
#include "proboard.hpp"

struct InputFile
   {
      int    area;
      String name;

      InputFile();
      InputFile(char *s);
      InputFile(char *s , int area);
   };

const int MAX_DOWNLOADS = 100;

void create_dirlist(int area,char *firstdir,LinkedList<String>& dirlist);
bool is_freefile(char *fname);
bool send_files( protocol& p , LinkedList<DownloadFile>& files ,LinkedList<String>& downloads , LinkedList<String>& uploads);
void read_personal_files( LinkedList<InputFile>& files );
void delete_personal_file( char *name );
void copy_cdrom( LinkedList<DownloadFile> &files , LinkedList<String> &copied);
void delete_copied ( LinkedList<String> &copied);

void process_uploads(protocol& p , char *dir , LinkedList<String>& uploads , bool pvt , bool ask_desc ,  bool no_log ,  bool quiet_dl ,  String extra_log );

bool check_dszlog(protocol& p , LinkedList<String>& downloads , LinkedList<String>& uploads);
bool create_dszctl(protocol& p , LinkedList<DownloadFile>& files);

static void
read_from_file(LinkedList<InputFile>& input_files , char *fn)
{
   TextFile f(fn);

   if(f.opened())
   {
      while(!f.eof())
      {
         String s = f.readLine();

         s.delLast('\n');
         s.trim();

         if(!s.len()) continue;

         input_files.add(InputFile(s));
      }
   }
}


void
download(char *data)
{
   bool                 no_input           = FALSE;
   bool                 any_file           = FALSE;
   bool                 private_download   = FALSE;
   bool                 free_time          = FALSE;
   char                 protocol_key       = '\0';
   bool                 quiet_download     = FALSE;
   bool                 goodbye            = FALSE;
   bool                 ignore_kbytes      = FALSE;
   bool                 no_log             = FALSE;
   bool                 ask_description    = TRUE;
   int                  npara;
   int                  i;
   long                 total_bytes;
   long                 free_bytes;
   String               param[20];
   BitArray             arealist(MAX_FILE_AREAS,1);
   LinkedList<DownloadFile> files;
   LinkedList<InputFile> input_files;
   LinkedList<String>   copied_to_cd;
   protocol             prot;
   FileName             extra_log;

   npara = parse_data(data,param);           // Parse command line

   create_arealist(param,npara,arealist);    // Create list of selected file areas

   adjust_limits();

   for(i=0 ; i<npara ; i++)
   {
      if(param[i][0]=='/')
         switch(toupper(param[i][1]))
         {
            case 'F': if(param[i][2] == '=')
                      {
                        if(param[i][3] == '@')
                           read_from_file(input_files,&param[i][4]);
                        else
                           input_files.add(InputFile(&param[i][3]));
                      }
                      else
                      {
                        input_files.add(InputFile(&param[i][2]));
                      }
                      no_input = TRUE;
            case 'A': any_file = TRUE;
                      break;
            case 'P': private_download = TRUE;
                      no_input         = TRUE;
                      break;
            case 'T': free_time = TRUE;
                      break;
            case 'K': protocol_key = param[i][3];
                      break;
            case 'Q': quiet_download = TRUE;
                      break;
            case 'I': ignore_kbytes = TRUE;
                      break;
            case 'N': no_log = TRUE;
                      break;
            case 'L': extra_log = &param[i][3];
                      break;
            case 'D': ask_description = FALSE;
                      break;
         }
   }

   if(user.uFlags & UFLAG_IGNORE)
      ignore_kbytes = TRUE;

   if(!quiet_download)
   {
      io << "\n\f" << S_DOWNLOAD_TITLE << "\n\n";

      if(timer.online()<download_delay && !(user.uFlags & UFLAG_IGNORE) && !private_download)
      {
         if(showansascrip("DLDELAY") == ANS_NOFILE)
         {
            io << S_DOWNLOAD_DELAY(form("%d",download_delay)) << "\n\n";
         }

         io << S_PRESS_ENTER_TO_CONTINUE;

         return;
      }

      if(!cfg.downloadHours.enabled() && !(user.uFlags & UFLAG_IGNORE) && !private_download)
      {
         io << S_DOWNLOAD_NOT_NOW;
         return;
      }
   }

   if(!user.defaultProtocol || !select_protocol(prot,user.defaultProtocol))
   {
      if(!select_protocol(prot,protocol_key))
         return;
   }
   else
   {
      io << S_PROTOCOL_SELECTED(prot.name);
   }

   if(!any_file && !no_input)
      for(taglist.rewind() ; !taglist.eol() ; taglist++)
      {
         input_files.add( InputFile(taglist.get().name , taglist.get().area) );
      }

   for(input_files.rewind() ; !input_files.eol() ; input_files++)
   {
      if(input_files.get().area)
         arealist.set(input_files.get().area);
   }

   if(!quiet_download && !no_input)
   {
      char s[80];

      io << "\n\n" << S_ENTER_FILES_TO_DOWNLOAD << "\n\n";

      int file_count = 1;

      for( input_files.rewind() ; !input_files.eol() ; input_files++ , file_count++)
      {
         io << S_ENTER_DOWNLOAD_FILENAME(form("%d",file_count));
         io << input_files.get().name << '\n';
      }

      for(;;file_count++)
      {
         io << S_ENTER_DOWNLOAD_FILENAME(form("%d",file_count));

         io.read(s,(any_file ? 60:13),READMODE_UPALL);

         if(!s[0]) break;

         char *tmp = strtok(s," ");

         for(i=0 ; tmp != NULL ; tmp = strtok(NULL," "))
         {
            if(strchr(tmp,'\\') && !any_file) continue;

            input_files.add(InputFile(tmp));
         }

         io << "\n";
      }

      if(!input_files.count())
         return;

      io << '\n';
   }

   if(!quiet_download)
      io << '\n' << S_SEARCHING_FILE_DATABASE << '\xFF';

   if(private_download)
   {
      read_personal_files(input_files);

      if(!input_files.count())
      {
         io << S_NO_PERSONAL_FILES_FOUND << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         return;
      }
   }

   // At this point we have a list of files that have to be download in the
   // linked list called 'input_files'.  If a path name is specified, it
   // should not be looked for in the file areas

   int files_found = 0;

   for( input_files.rewind() ; !input_files.eol() && files_found < MAX_DOWNLOADS; )
   {
      DownloadFile cur_file;

      if(strchr(input_files.get().name , '\\'))   // Explicit path found?
      {
         DirScan scan(input_files.get().name);

         while(int(scan) && files_found < MAX_DOWNLOADS)
         {
            FileName fn(input_files.get().name);

            fn.stripName();
            fn.appendBS();

            cur_file.area = 0;
            cur_file.free = FALSE;
            cur_file.copy = FALSE;
            cur_file.size = scan.size();
            cur_file.name = fn + scan.name();

            files.add(cur_file);
            files_found++;

            scan++;
         }

         input_files.remove();
      }
      else
      {
         input_files++;
      }
   }

   if(input_files.count())
   {
      File fidx;
      FilesIdx idx;
      FileArea fa;

      if(fidx.open(FileName(syspath,"FILESIDX.PB") , fmode_read , cfg.fastmode ? 8192:2048))
      {
         long idx_size = fidx.len() / sizeof(idx);
         long step = idx_size / 50;
         long count = 0;

         if(step <= 1)
            step = 1;

         fidx.seek(0);

         if(!quiet_download)
         {
            if(!ansi_mode && !avatar)
            {
               io << "\n\n";
            }

            io << "\6³    ³    ³    ³    ³    ³    ³    ³    ³    ³    ³";

            if(ansi_mode || avatar)
            {
               io << "\x1b[51D\3";
            }
            else
            {
               io << '\r';
            }
         }

         String firstchars;
         bool wildcard_found = FALSE;

         for( input_files.rewind() ; !input_files.eol() ; input_files++)
         {
            firstchars << char(toupper(input_files.get().name[0]));
         }

         if(strpbrk(firstchars,"*?")) wildcard_found = TRUE;


         while(files_found < MAX_DOWNLOADS)
         {
            if(fidx.read(&idx,sizeof(idx)) != sizeof(idx))
               break;

            if(!((count++) % step) && !quiet_download) io << "±\xFF";

            if(!wildcard_found && !strchr(firstchars,toupper(idx.name[0]))) continue;

            if(!arealist[idx.area]) continue;

            for( input_files.rewind() ; !input_files.eol() && files_found < MAX_DOWNLOADS ; input_files++)
            {
               if(input_files.get().area && idx.area != input_files.get().area) continue;

               if(!matchfile(input_files.get().name,idx.name)) continue;

               if(!fa.read(idx.area) || !check_access(fa.level,fa.flags,fa.flagsNot)) continue;

               LinkedList<String> dirlist;

               create_dirlist(idx.area,fa.filepath,dirlist);

               for( dirlist.rewind() ; !dirlist.eol() && files_found < MAX_DOWNLOADS ; dirlist++)
               {
                  String wildcard = dirlist.get();

                  wildcard << idx.name;

                  DirScan scan(wildcard);

                  while(int(scan) && files_found < MAX_DOWNLOADS)
                  {
                     DownloadFile tmpfile;

                     tmpfile.area = idx.area;
                     tmpfile.size = scan.size();
                     tmpfile.free = FALSE;
                     tmpfile.copy = !!fa.cdrom;
                     tmpfile.name = dirlist.get() + scan.name();

                     files.add(tmpfile);
                     files_found++;

                     scan++;
                  }
               }
            }
         }
      }
      else
      {
         LOG("Error opening FILESIDX.PB");
      }
   }

   total_bytes = 0;
   free_bytes  = 0;

   if(!quiet_download)
   {
      io << '\n';

      if(files.count())
      {
         io << '\n' << S_DOWNLOAD_FILES_FOUND_HEADER << '\n';
      }

      for(files.rewind() ; !files.eol() ; )
      {
         FileArea fa;
         FileName fn;
         String area_name;

         fn = files.get().name;
         fn.stripPath();

         if(files.get().area)
         {
            fa.read(files.get().area);
            area_name = fa.name;
         }
         else
         {
            if(private_download)
               area_name = "Personal Files";
            else
               area_name = "Global";
         }

         io << S_DOWNLOAD_FILE_FOUND_ENTRY((char *)fn,form("%ld",(files.get().size+512L) / 1024L),(char *)area_name);

         files.get().free = (fa.free || is_freefile((char *)fn));

         if((total_bytes - free_bytes + files.get().size + user.kbToday*1024L) > (1024L*download_limit) && !files.get().free)
         {
            io << S_FILE_EXCEEDS_LIMIT(form("%u",download_limit)) << '\n';

            files.remove();
         }
         else
         {
            if(upload_needed > 0 && !files.get().free)
            {
               io << S_UPLOAD_REQUIRED(form("%u",upload_needed)) << '\n';

               files.remove();
            }
            else
            {
               io << S_FILE_FOUND_ACTION_PROMPT;

               char action = wait_language_hotkeys(K_FILE_FOUND_ACTION_PROMPT);

               int prompt_len = language_string_length(S_FILE_FOUND_ACTION_PROMPT);

               io << String('\b',prompt_len) << String(' ',prompt_len) << String('\b',prompt_len);

               switch(action)
               {
                  case '\r':
                  case 0:
                     {
                        io << S_ACTION_DOWNLOAD << ' ';

                        if(files.get().free)
                        {
                           io << S_FREE_FILE;

                           free_bytes += files.get().size;
                        }

                        total_bytes += files.get().size;

                        io << '\n';
                        files++;
                     }
                     break;
                  case 1 :
                     {
                        io << S_ACTION_FILE_SKIPPED << '\n';

                        files.remove();
                     }
                     break;
                  case 2 :
                     {
                        io << S_ACTION_FILE_QUIT << '\n';

                        while(!files.eol()) files.remove();
                     }
                     break;
               }
            }
         }
      }

      if(!files.count())
      {
         io << '\n' << S_NO_FILES_FOUND << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;

         return;
      }

      word estimated = word(total_bytes / ((io.baud ? io.baud:115200L) * (prot.efficiency?prot.efficiency:100)/1000L));

      if(prot.flags & PROT_LOCAL)
         estimated = 0;

      io << '\n' << S_X_FILES_SELECTED(form("%d",files.count()),form("%d",int(total_bytes/1024)),form("%02u:%02u",estimated/60,estimated%60)) << '\n';

      if(!free_time && estimated/60>timer.left())
      {
         io << '\n' << S_NOT_ENOUGH_TIME_LEFT_FOR_DOWNLOAD << "\n\n" <<  S_PRESS_ENTER_TO_CONTINUE;

         return;
      }

      io << '\n' << S_START_DOWNLOAD_PROMPT;

      char k = wait_language_hotkeys(K_START_DOWNLOAD_PROMPT);

      if(k==2)
         return;

      if(k==1)
      {
         io << S_DOWNLOAD_ACTION_GOODBYE;

         goodbye = TRUE;
      }

      io << "\n\n";

      copy_cdrom( files , copied_to_cd);

      io << "\n\n" << S_ACTIVATING_PROTOCOL << '\xFF';
   }

   LinkedList<String> downloads,uploads;

   if(free_time) timer.suspend();

   if(!send_files(prot , files , downloads , uploads))
   {
      delete_copied(copied_to_cd);

      return;
   }

   delete_copied(copied_to_cd);

   if(free_time) timer.restart();

   total_bytes = 0;
   free_bytes  = 0;
   files_found = 0;

   for(downloads.rewind() ; !downloads.eol() ; downloads++)
   {
      for(files.rewind() ; !files.eol() ; )
      {
         FileName fn(files.get().name);

         fn.stripPath();

         if(downloads.get() == fn)
         {
            total_bytes += files.get().size;

            if(files.get().free)
               free_bytes += files.get().size;

            remove_tag(fn,files.get().area);

            if(private_download)
               delete_personal_file(files.get().name);

            files_found++;

            File f;

            if(extra_log.len())
            {
               f.open(extra_log,fmode_copen | fmode_text | fmode_append | fmode_write);

               f.printf("D %d %s %ld %s\n",files.get().area,(char *)files.get().name,files.get().size,files.get().free ? "YES":"NO");
               f.close();
            }

            if(!no_log)
            {
               f.open(FileName(syspath,"DOWNLOAD.LOG"),fmode_write | fmode_append |
                                                       fmode_copen | fmode_text);

               f.printf("%05d %s\n",files.get().area,(char *)files.get().name);
               f.close();

               LOG(1,"Download-%c %s",prot.key,(char *)fn);
            }

            files.remove();
         }
         else
         {
            files++;
         }
      }
   }

   if(!ignore_kbytes)
   {
      user.numDownloads += files_found;
      user.kbDownloaded += int((total_bytes-free_bytes) / 1024L);
      user.kbToday      += int((total_bytes-free_bytes) / 1024L);
   }

   if(!quiet_download)
   {
      io << "\n\f\n" << S_X_FILES_DOWNLOADED_SUCCESSFULLY(form("%d",files_found),form("%ld",total_bytes/1024L)) << "\n\n";

      if(files.count())
         io << S_X_FILES_NOT_DOWNLOADED(form("%d",files.count())) << "\n\n";

      if(goodbye && !files.count() && !uploads.count())
      {
         unlink(prot.logfile);
         unlink(prot.ctlfile);

         io << "\n\n" << S_HANGUP_AFTER_DOWNLOAD_PROMPT << "10";

         for(int i=10;i>0;i--)
         {
            io << form("\b\b%2d\xFF",i);

            char c = toupper(io.readkey());

            if(c==K_HANGUP_AFTER_DOWNLOAD_PROMPT[0]) return;
            if(c==K_HANGUP_AFTER_DOWNLOAD_PROMPT[1]) break;

            sleep(1);
         }

         LOG("Hanging up after download.");

         showansascrip("DLHANGUP");

         exit_proboard();
      }

      if(uploads.count())
         process_uploads(prot,cfg.uploadpath,uploads,private_download , ask_description , no_log , quiet_download , extra_log );

      io << S_PRESS_ENTER_TO_CONTINUE;
   }
}

void
read_personal_files( LinkedList<InputFile>& files )
{
   File f(FN_PVTFILES_PB);

   if(!f.opened()) return;

   f.rewind();

   for(;;)
   {
      _PrivateFile pvt;

      if(f.read(&pvt,sizeof(pvt)) != sizeof(pvt))
         break;

      if(strcmpl(pvt.to,user.name) || !pvt.fname[0]) continue;

      String fname;

      if(!strchr(pvt.fname,'\\'))
         fname = String(cfg.pvtuploadpath) + pvt.fname;
      else
         fname = pvt.fname;

      DirScan scan(fname);

      if(int(scan))
         files.add( InputFile(fname) );
   }
}

void
delete_personal_file( char *name )
{
   File f(FN_PVTFILES_PB,fmode_rw | fmode_excl);

   if(!f.opened())
      return;

   FileName fn;
   FileName fn_full( name );

   _PrivateFile pvt;

   int num_found = 0;

   for(;;)
   {
      if(f.read(&pvt,sizeof(pvt))!=sizeof(pvt)) break;

      if(!pvt.fname[0])
         continue;

      if(!strchr(pvt.fname,'\\'))
         fn = String(cfg.pvtuploadpath) + pvt.fname;
      else
         fn = pvt.fname;

      if(fn == fn_full)
         num_found++;
   }

   f.rewind();

   for(;;)
   {
      if(f.read(&pvt,sizeof(pvt))!=sizeof(pvt)) break;

      if(!pvt.fname[0] || stricmp(pvt.to,user.name))
         continue;

      if(!strchr(pvt.fname,'\\'))
         fn = String(cfg.pvtuploadpath) + pvt.fname;
      else
         fn = pvt.fname;

      if(fn_full == fn)
      {
         pvt.fname[0]=0;

         f.seek(-long(sizeof(pvt)),seek_cur);
         f.write(&pvt,sizeof(pvt));

         if(pvt.attr & PVTFILE_KEEP)
            continue;

         if(num_found < 2)   // Only kill file if not waiting for oher user
            unlink(fn_full);
      }
   }
}

InputFile::InputFile()
{
   area = 0;
}

InputFile::InputFile(char *s)
{
   area = 0;

   if(s[0] == '[')
   {
      area = atoi(&s[1]);

      for(int i=0; s[i] ; i++)
      {
         if(s[i] == ']')
         {
            name = &s[i+1];
            return;
         }
      }
   }

   name = s;
}

InputFile::InputFile(char *s,int a)
{
   name = s;
   area = a;
}
