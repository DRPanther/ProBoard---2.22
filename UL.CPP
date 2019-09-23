#define Use_LinkedList

#include <dir.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <io.h>
#include <ctype.h>
#include "proboard.hpp"

static void MoveFailedFile(FileName full_fn);

static bool file_in_index(FileName fn);
       bool check_dszlog(protocol& p , LinkedList<String>& downloads , LinkedList<String>& uploads);

void process_uploads(protocol& p , char *dir , LinkedList<String>& uploads , bool pvt , bool ask_desc ,  bool no_log ,  bool quiet_dl ,  String extra_log );

static char oldpath[70]="";

static char env_buf[80];

static void
pushdir(char *p)
{
   char path[65];
   unsigned disk;

   strcpy(path,p);
   getcwd(oldpath,69);
   disk=toupper(path[0])-'A'+1;

   _dos_setdrive(disk,&disk);

   if(path[strlen(path)-1]=='\\')
      path[strlen(path)-1]=0;

   chdir(&path[2]);
}

static void
popdir()
{
   unsigned disk = toupper(oldpath[0])-'A'+1;
   _dos_setdrive(disk,&disk);
   chdir(&oldpath[2]);
}


void
upload(char *data)
{
   bool     private_download = FALSE;
   bool     no_input         = FALSE;
   bool     quiet_download   = FALSE;
   bool     no_log           = FALSE;
   bool     ask_description  = TRUE;
   char     protocol_key     = '\0';
   char     filename[13]     = "";
   FileName extra_log;
   protocol p;
   int      i,npara;
   String   param[20];
   char     uploadpath[80];

   adjust_limits();

   strcpy(uploadpath , cfg.uploadpath);

   npara = parse_data(data,param);           // Parse command line

   for(i=0 ; i<npara ; i++)
   {
      if(param[i][0]=='/')
         switch(toupper(param[i][1]))
         {
            case 'F': strncpy(filename,&param[i][3],12);
                      filename[12] = '\0';
                      no_input         = TRUE;
                      break;
            case 'P': private_download = TRUE;
                      no_input         = TRUE;
                      strcpy(uploadpath , cfg.pvtuploadpath);
                      break;
            case 'K': protocol_key = param[i][3];
                      break;
            case 'Q': quiet_download = TRUE;
                      ask_description = FALSE;
                      break;
            case 'N': no_log = TRUE;
                      break;
            case 'L': extra_log = &param[i][3];
                      break;
            case 'D': ask_description = FALSE;
                      break;
         }
      else
      {
         strcpy(uploadpath , param[i]);
      }
   }

   append_backspace(uploadpath);
   strupr(uploadpath);

   if(!no_log)
      LOG(2,"Upload menu");

   if(!quiet_download)
      io << "\n\f" << S_UPLOAD_TITLE << "\n\n";

   if(dos_getdiskfreespace(uploadpath[0]-'A'+1)<(1024L*cfg.uploadspace))
   {
      io << "\n\n" << S_NOT_ENOUGH_FREE_DISK_SPACE << "\n\n"
         << S_PRESS_ENTER_TO_CONTINUE;

      return;
   }

   if(timer.left()<10 && time_removed>0 && !quiet_download)
   {
      io << S_NO_UPLOAD_DUE_TO_EVENT << "\n\n"
         << S_PRESS_ENTER_TO_CONTINUE;

      return;
   }

   if(!select_protocol(p,protocol_key)) return;

   if(!(p.flags&PROT_BATCH) && !no_input)
   {
      io << "\n\n" << S_FILE_TO_UPLOAD;

      io.read(filename,12,READMODE_UPALL);

      if(!*filename || strchr(filename,':') || strchr(filename,'\\'))
         return;

      io << '\n';
   }

   String command;

   if(p.ulcom[0] != '@')
      command << "*N*Q";

   for(i=0;i<strlen(p.ulcom);i++)
      if(p.ulcom[i] != '#')
      {
         command << p.ulcom[i];
      }
      else
      {
         command << uploadpath;

         if(!(p.flags&PROT_BATCH)) command << filename;
      }

   if(!quiet_download)
   {
      io << '\n' << S_UPLOAD_START_PROMPT;

      char k = wait_language_hotkeys(K_UPLOAD_START_PROMPT);

      if(k==1) return;

      io << "\n\n" << S_READY_TO_UPLOAD_FILES << '\xFF';
   }

   sprintf(env_buf,"DSZLOG=%s%s",mypath,p.logfile);

   putenv(env_buf);

   timer.suspend();

   pushdir(uploadpath);

   if(command[0]=='@')
   {
      run_sdkfile(replace_stringvars(&command[1]));
   }
   else
   {
      shell(replace_stringvars(command));
   }

   popdir();

   timer.restart();

   LinkedList<String> downloads , uploads;

   check_dszlog(p , downloads , uploads);

   if(!uploads.count())
   {
      if(!quiet_download)
      {
         io << "\n\n" << S_NO_FILES_RECEIVED << "\n\n"
            << S_PRESS_ENTER_TO_CONTINUE;
      }

      return;
   }

   process_uploads(p,uploadpath,uploads,private_download,ask_description,no_log,quiet_download,extra_log);

   if(!quiet_download)
      io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
}


void
process_uploads(protocol& p , char *dir , LinkedList<String>& uploads , bool pvt ,
                                                                        bool ask_desc ,
                                                                        bool no_log ,
                                                                        bool quiet_dl ,
                                                                        String extra_log
                                                                       )
{
   long total_bytes = 0;
   int  total_files = 0;
   String uploadpath(dir);

   bool dupes_found = FALSE;

   if(!quiet_dl)
      io << "\n\n" << S_CHECKING_UPLOADS << "\n\xFF";

   for( uploads.rewind() ; !uploads.eol() ; )
   {
      FileName fn( uploadpath , uploads.get() );

      fn.upperCase();

      FileName fullfn(fn);

      DirScan scan(fullfn);

      if(!int(scan))
      {
         uploads.remove();

         continue;
      }

      fn.stripPath();

      if(!quiet_dl)
         io << '\n' << S_FILE_RECEIVED((char *)fn) << '\xFF';

      Date date(TODAY);
      Time time(NOW);

      File ul_log;

      if(!no_log)
      {
         ul_log.open(FileName(syspath,"UPLOAD.LOG"),fmode_text | fmode_append | fmode_write | fmode_copen);

         ul_log.printf("%02d-%s-%02d %02d:%02d:%02d Upload of %-13s by %s\n",date[0],
                                                   months_short[date[1]],
                                                   date[2],
                                                   time[0],time[1],time[2],
                                                   (char *)fn,user.name);
         ul_log.close();
      }

      if(extra_log.len())
      {
         ul_log.open(extra_log,fmode_text | fmode_append | fmode_write | fmode_copen);
         ul_log.printf("U 0 %s %ld NO\n",(char *)fullfn,scan.size());
         ul_log.close();
      }

      if(!pvt && cfg.checkdupes && file_in_index(fn) && !quiet_dl)
      {
         dupes_found = TRUE;

         if(cfg.killdupes)
         {
            unlink(fullfn);

            io << S_DUPLICATE_UPLOAD_REMOVED;
         }
         else
         {
            io << S_DUPLICATE_UPLOAD;
         }

         LOG(1,"Upload-%c %s (DUPE)",p.key,(char *)fn);

         uploads.remove();
      }
      else
      {
         if(!no_log)
         {
            if(pvt)
               LOG(1,"Upload-%c %s (Personal)",p.key,(char *)fn);
            else
               LOG(1,"Upload-%c %s",p.key,(char *)fn);
         }

         total_files++;
         total_bytes += scan.size();

         uploads++;
      }
   }

   if(cfg.virScanCommand[0])
   {
      LinkedList<FileName> failed_files;

      io << "\n\n" << S_SCANNING_UPLOADS;

      for(uploads.rewind() ; !uploads.eol(); )
      {
         FileName curfile(uploadpath,uploads.get());

         DirScan scan(curfile);

         String cmd     = String("*Q*N") + cfg.virScanCommand;

         cmd.replace("@<FILE>@",curfile);

         shell(replace_stringvars(cmd));

         if(   (cfg.virScanType && !access(cfg.virScanSemaphore,0))
            || (!cfg.virScanType && returnvalue)
           )
         {
            LOG(1,"File '%s' rejected by upload scanner",(char *)curfile);

            if(cfg.virScanFailedAction == VIRSCAN_ACTION_DELETE)
            {
               unlink(curfile);
            }

            if(cfg.virScanFailedAction == VIRSCAN_ACTION_MOVE)
            {
               MoveFailedFile(curfile);
            }

            uploads.remove();

            failed_files.add(curfile);

            total_files--;
            total_bytes -= scan.size();
         }
         else
         {
            uploads++;
         }
      }

      if(failed_files.count())
      {
         io << "\n\n" << S_FOLLOWING_FILES_REJECTED << "\n\n";

         for(failed_files.rewind() ; !failed_files.eol() ; failed_files++)
         {
            FileName fn = failed_files.get();

            fn.stripPath();

            io << "     " << (char *)fn << '\n';
         }

         io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
      }
   }


   if(total_files)
   {
      if(!pvt)
      {
         user.numUploads += total_files;
         user.kbUploaded += int(total_bytes/1024L);

         if(!quiet_dl)
            io << "\n\n" << S_X_FILES_UPLOADED(form("%d",total_files),form("%ld",total_bytes/1024L)) << '\n';
      }
   }
   else
   {
      if(!quiet_dl)
         io << "\n\n" << S_NO_FILES_UPLOADED << '\n';

      return;
   }

   if(!ask_desc) return;

   if(dupes_found && !quiet_dl) showansascrip("DUPESFND");

   if(!pvt && uploads.count())
      io << '\n'
         << S_PLEASE_ENTER_UPLOAD_DESCRIPTIONS
         << "\n\n";

   for(uploads.rewind() ; !uploads.eol(); uploads++)
   {
      io.clearinput();

      if(pvt)
      {
         User tmpuser;
         char username[36];

         for(;;)
         {
            io << '\n' << S_ENTER_USERNAME_FOR_PERSONAL_FILE((char *)uploads.get());
            io.read(username,35,READMODE_UPFIRST);

            if(!strcmpl(username,"Sysop")) strcpy(username,cfg.sysopname);
            if(tmpuser.search(username)) break;
            io << "\n\n" << S_USER_NOT_LOCATED << '\n';
         }

         File f(FN_PVTFILES_PB,fmode_rw | fmode_copen);
         if(!f.opened())
         {
            LOG("Unable to create PVTFILES.PB");
            continue;
         }

         _PrivateFile pvt;

         for(int rec=0;;rec++)
         {
            if(f.read(&pvt,sizeof(pvt))!=sizeof(pvt)) break;

            if(!pvt.fname[0]) break;
         }
         strcpy(pvt.fname,uploads.get());
         strcpy(pvt.to,username);
         strcpy(pvt.from,user.name);
         pvt.date.today();
         f.seek(long(rec)*sizeof(pvt));
         f.write(&pvt,sizeof(pvt));
      }
      else
      {
         FileName fbbs;
         char descr[41];

         io << '\n' << S_ENTER_DESCRIPTION_LINE_1((char *)uploads.get());

         io.clearinput();
         io.read(descr,40);

         if(descr[0]!='/')
            fbbs(uploadpath,"FILES.BBS");
         else
            fbbs(uploadpath,"FILES.PVT");

         File tf(fbbs,fmode_write | fmode_append | fmode_text | fmode_copen);
         tf << form("%-12s %s\n",(char *)uploads.get(),((descr[0]=='/') ? (&descr[1]) : descr));
         while(descr[0])
         {
            io << '\n' << S_ENTER_DESCRIPTION_LINE_X;
            io.read(descr,40);
            if(descr[0]) tf << '+' << ((descr[0]=='/') ? (&descr[1]) : descr) << '\n';
         }

         if(cfg.addUploaderName)
            tf << '+' << "[Uploader: " << user.name << ']\n';
      }
   }

   io << '\n';
}


bool
file_in_index(FileName fn)
{
   if(cfg.ignore_ext) fn.stripExt();

   File idx;

   if(idx.open(FileName(syspath,"FILESIDX.PB") , fmode_read , cfg.fastmode ? 8192:2048))
   {
      FilesIdx fidx;

      for(;;)
      {
         if(idx.read(&fidx,sizeof(fidx)) != sizeof(fidx)) break;

         if(cfg.ignore_ext)
            if(strchr(fidx.name,'.'))
               *strchr(fidx.name,'.') = '\0';

         if(!stricmp(fidx.name,fn)) return TRUE;
      }
   }

   return FALSE;
}

bool
check_dszlog(protocol& p , LinkedList<String>& downloads , LinkedList<String>& uploads)
{
   File dszlog;
   int i;
   char s[200];

   if(dszlog.open(p.logfile,fmode_read | fmode_text))
   {

      while(dszlog.readLine(s,199))
      {
         bool found_upload   = FALSE;
         bool found_download = FALSE;

         strip_linefeed(s);

         char *ptr=strtok(s," ");

         while(ptr)
         {
            if(!strcmp(ptr,p.dlkeyword))
            {
               found_download = TRUE;
               break;
            }
            if(!strcmp(ptr,p.ulkeyword))
            {
               found_upload = TRUE;
               break;
            }

            ptr = strtok(NULL," ");
         }

         if(ptr == NULL) continue;

         for(i=0;i<p.wordnr && ptr!=NULL;i++) ptr = strtok(NULL," ");

         if(ptr==NULL) continue;

         strip_path(ptr);
         strupr(ptr);

         if(found_download)
            downloads.add( String(ptr) );

         if(found_upload)
            uploads.add( String(ptr) );
      }

      dszlog.close();

      unlink(p.ctlfile);
      unlink(p.logfile);

      return TRUE;
   }

   unlink(p.ctlfile);
   unlink(p.logfile);

   return FALSE;
}

bool
create_dszctl(protocol& p , LinkedList<DownloadFile>& files)
{
   File tf;

   if(!tf.open(p.ctlfile,fmode_create | fmode_text))
      return FALSE;

   for(files.rewind() ; !files.eol() ; files++)
   {
      if(p.flags&PROT_BIM)
      {
         tf << form("U       %-80s%-160s",(char *) files.get().name,EMPTY_STRING);
      }
      else
      {
         for(int j=0;j<strlen(p.dlstring);j++)
            if(p.dlstring[j]=='#')
               tf << (char *)files.get().name;
            else
               tf << p.dlstring[j];

         tf << '\n';
      }
   }

   files.rewind();

   return TRUE;
}

bool
send_files( protocol& p , LinkedList<DownloadFile>& files , LinkedList<String>& downloads , LinkedList<String>& uploads )
{
   if(!strchr(p.dlcom,'#'))
      if(!create_dszctl(p , files)) return FALSE;

   String command;

   if(p.dlcom[0] != '@')
      command << "*N*Q";

   files.rewind();

   for(int i=0;i<strlen(p.dlcom);i++)
      if(p.dlcom[i] != '#')
         command << p.dlcom[i];
      else
         command << files.get().name;

   sprintf(env_buf,"DSZLOG=%s%s",mypath,p.logfile);

   putenv(env_buf);

   if(command[0]=='@')
      run_sdkfile(replace_stringvars(&command[1]));
   else
      shell(replace_stringvars(command));

   return check_dszlog(p , downloads , uploads);
}

void
create_dirlist(int area,char *firstdir,LinkedList<String>& dirlist)
{
   File f(FileName(syspath,form("FA_%d.CTL",area)),fmode_read | fmode_text);

   dirlist.rewind();

   dirlist.add(String(firstdir));

   for(;;)
   {
      char str[128];

      if(!f.readLine(str,127)) break;

      append_backspace(str);

      dirlist.add(String(str));
   }
}

bool
is_freefile(char *fname)
{
   File fp(FileName(syspath,"FREEFILE.CTL"),fmode_read | fmode_text);

   if(!fp.opened()) return FALSE;

   for(;;)
   {
      char s[100];

      if(!fp.readLine(s,99)) return FALSE;

      strip_linefeed(s);
      strip_leading(s);
      strip_trailing(s);

      if(matchfile(s,fname)) return TRUE;
   }
}

static void
MoveFailedFile(FileName full_fn)
{
   FileArea fa;
   FileName fn = full_fn;

   fn.stripPath();

   if(fa.read(cfg.virScanFailedArea))
   {
      ts_CopyFile(full_fn,fa.filepath,4096);
      unlink(full_fn);

      File f(fa.listpath,fmode_rw|fmode_append|fmode_copen);
      f.printf("%-12s <FAILED> Uploaded by %s\n",(char *)fn,user.name);
   }
}
