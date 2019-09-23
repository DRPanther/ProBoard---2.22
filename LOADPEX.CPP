#define Use_TagList
#define Use_LinkedList
#define Use_Handlers
#define Use_MsgBase

#include <ctype.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <dir.h>
#include <dos.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <stdarg.h>
#include <string.h>
#include <tswin.hpp>
#include <msdos.hpp>

#include "proboard.hpp"
#include "fossil.hpp"

static long _cdecl
hook_l_mul(long,long v1,long v2)
{
 return v1*v2;
}

static long _cdecl
hook_l_div(long,long v1,long v2)
{
 return v1/v2;
}

static long _cdecl
hook_l_mod(long,long v1,long v2)
{
 return v1%v2;
}

static unsigned long _cdecl
hook_ul_div(long,unsigned long v1,unsigned long v2)
{
 return v1/v2;
}

static unsigned long _cdecl
hook_ul_mod(long,unsigned long v1,unsigned long v2)
{
 return v1%v2;
}

static long _cdecl
hook_l_shl(long,long v,unsigned shift)
{
 return v << shift;
}

static long _cdecl
hook_l_shr(long,long v,unsigned shift)
{
 return v >> shift;
}

static unsigned long _cdecl
hook_ul_shl(long,unsigned long v,unsigned shift)
{
 return v << shift;
}

static unsigned long _cdecl
hook_ul_shr(long,unsigned long v,unsigned shift)
{
 return v >> shift;
}

static FILE * _cdecl
hook_fopen(long,char *a,char *b)
{
 bool shared = TRUE;

 if(strchr(b,'*'))
   {
    *strchr(b,'*') = '\0';

    shared = FALSE;
   }

 FILE *fp;

 if(shared) fp = _fsopen(a,b,0x40);
       else fp = fopen(a,b);

 return fp;
}

static FILE * _cdecl
hook_freopen(long,char *a,char *b,FILE *c)
{
 return freopen(a,b,c);
}

static int _cdecl
hook_fseek(long,FILE *a,long b,int c)
{
 return fseek(a,b,c);
}

static long _cdecl
hook_ftell(long,FILE *a)
{
 return ftell(a);
}

static char * _cdecl
hook_fgets(long,char *a,int b,FILE *c)
{
 return fgets(a,b,c);
}

static int _cdecl
hook_fgetc(long,FILE *a)
{
 return fgetc(a);
}

static int _cdecl
hook_fflush(long,FILE *a)
{
 return fflush(a);
}

static int _cdecl
hook_fclose(long,FILE *a)
{
 return fclose(a);
}

static int _cdecl
hook_fputs(long,char *a,FILE *b)
{
 return fputs(a,b);
}

static int _cdecl
hook_getc(long,FILE *a)
{
 return getc(a);
}

static int _cdecl
hook_getchar()
{
 return io.wait();
}

static char * _cdecl
hook_gets(long,char *a)
{
 io.read(a,79,READMODE_NOFIELD);

 return a;
}

static int _cdecl
hook_fputc(long,int a,FILE *b)
{
 return fputc(a,b);
}

static int _cdecl
hook_putc(long,int a,FILE * b)
{
 return putc(a,b);
}

static int _cdecl
hook_putchar(long,int a)
{
 io << char(a) << char(255);

 return a;
}

static int _cdecl
hook_puts(long,char *a)
{
 io << a << "\n\xFF";

 return 1;
}

static size_t _cdecl
hook_fread(long,void *a,size_t b,size_t c,FILE *d)
{
 size_t r = fread(a,b,c,d);

 return r;
}

static size_t _cdecl
hook_fwrite(long,void *a,size_t b,size_t c,FILE *d)
{
 return fwrite(a,b,c,d);
}

static int _cdecl
hook_printf(long,char *a ...)
{
 va_list x;
 va_start(x,a);

 char s[256];

 int l=vsprintf(s,a,x);

 if(l>0)
   io << s << char(0xFF);

 return l;
}

static int _cdecl
hook_fprintf(long,FILE *a,char *b ...)
{
 va_list x;
 va_start(x,b);

 return vfprintf(a,b,x);
}

static int _cdecl
hook_vfprintf(long,FILE *a,char *b,va_list c)
{
 return vfprintf(a,b,c);
}

static int _cdecl
hook_vprintf(long,char *a,va_list b)
{
 char s[256];

 int l = vsprintf(s,a,b);

 if(l>0)
   io << s << char(0xFF);

 return l;
}

static int _cdecl
hook_sprintf(long,char *a,char *b ...)
{
 va_list x;
 va_start(x,b);

 return vsprintf(a,b,x);
}

static int _cdecl
hook_vsprintf(long,char *a,char *b,va_list c)
{
 return vsprintf(a,b,c);
}

static void _cdecl
hook_setbuf(long,FILE *a,char *b)
{
 setbuf(a,b);
}

static int _cdecl
hook_setvbuf(long,FILE *a,char *b,int c,size_t d)
{
 return setvbuf(a,b,c,d);
}

static int _cdecl
hook_remove(long,char *a)
{
 return remove(a);
}

static int _cdecl
hook_rename(long,char *a,char *b)
{
 return rename(a,b);
}

static void _cdecl
hook_rewind(long,FILE *a)
{
 rewind(a);
}

static void _cdecl
hook_clearerr(long,FILE *a)
{
 clearerr(a);
}

static int _cdecl
hook_feof(long,FILE *a)
{
 return feof(a);
}

static int _cdecl
hook_isalnum(long,int a)
{
 return isalnum(a);
}

static int _cdecl
hook_isalpha(long,int a)
{
 return isalpha(a);
}

static int _cdecl
hook_iscntrl(long,int a)
{
 return iscntrl(a);
}

static int _cdecl
hook_isdigit(long,int a)
{
 return isdigit(a);
}

static int _cdecl
hook_isgraph(long,int a)
{
 return isgraph(a);
}

static int _cdecl
hook_islower(long,int a)
{
 return islower(a);
}

static int _cdecl
hook_isprint(long,int a)
{
 return isprint(a);
}

static int _cdecl
hook_ispunct(long,int a)
{
 return ispunct(a);
}

static int _cdecl
hook_isspace(long,int a)
{
 return isspace(a);
}

static int _cdecl
hook_isupper(long,int a)
{
 return isupper(a);
}

static int _cdecl
hook_isxdigit(long,int a)
{
 return isxdigit(a);
}

static int _cdecl
hook_toupper(long,int a)
{
 return toupper(a);
}

static int _cdecl
hook_tolower(long,int a)
{
 return tolower(a);
}

static int _cdecl
hook_int86(long,int a,union REGS *b,union REGS *c)
{
 return int86(a,b,c);
}

static int _cdecl
hook_int86x(long,int a,union REGS *b,union REGS *c,struct SREGS *d)
{
 return int86x(a,b,c,d);
}

static int _cdecl
hook_intdos(long,union REGS *a,union REGS *b)
{
 return intdos(a,b);
}

static int _cdecl
hook_intdosx(long,union REGS *a,union REGS *b,struct SREGS *c)
{
 return intdosx(a,b,c);
}

static int _cdecl
hook_dos_findfirst(long,char *a, unsigned b, struct find_t *c)
{
 return _dos_findfirst(a,b,c);
}

static int _cdecl
hook_dos_findnext(long,struct find_t *a)
{
 return _dos_findnext(a);
}

static int _cdecl
hook_read(long,int a,void *b,unsigned c)
{
 return msdos_read(a,b,c);
}

static int _cdecl
hook_write(long,int a,void *b,unsigned c)
{
 return msdos_write(a,b,c);
}

static int _cdecl
hook_open(long,char *a,int b)
{
 int fh = msdos_open(a,b);

 return fh;
}

static int _cdecl
hook_creat(long,char *a,int b)
{
 return creat(a,b);
}

static void _cdecl
hook_close(long,int a)
{
 msdos_close(a);
}

static int _cdecl
hook_unlink(long,char *a)
{
 return unlink(a);
}

static int _cdecl
hook_chsize(long,int a,long b)
{
 return chsize(a,b);
}

static int _cdecl
hook_dup(long,int a)
{
 return dup(a);
}

static int _cdecl
hook_dup2(long,int a,int b)
{
 return dup2(a,b);
}

static long _cdecl
hook_lseek(long,int a,long b,int c)
{
 return lseek(a,b,c);
}

static int _cdecl
hook_access(long,char *a,int b)
{
 return access(a,b);
}

static long _cdecl
hook_filesize(long,char *)
{
 return -1;
}

static long _cdecl
hook_filelength(long,int a)
{
 return filelength(a);
}

static int _cdecl
hook_isatty(long,int a)
{
 return isatty(a);
}

static int _cdecl
hook_mkdir(long,char *a)
{
 return mkdir(a);
}

static int _cdecl
hook_chdir(long,char *a)
{
 return chdir(a);
}

static int _cdecl
hook_rmdir(long,char *a)
{
 return rmdir(a);
}

static int _cdecl
hook_atoi(long,char *a)
{
 return atoi(a);
}

static long _cdecl
hook_atol(long,char *a)
{
 return atol(a);
}

static long _cdecl
hook_strtol(long,char *a,char **b,int c)
{
 return strtol(a,b,c);
}

static int _cdecl
hook_rand(long)
{
 return rand();
}

static void _cdecl
hook_srand(long,unsigned a)
{
 srand(a);
}

static void * _cdecl
hook_calloc(long,size_t a,size_t b)
{
 return calloc(a,b);
}

static void _cdecl
hook_free(long,void *a)
{
 free(a);
}

static void * _cdecl
hook_malloc(long,size_t a)
{
 return malloc(a);
}

static void * _cdecl
hook_realloc(long,void *a,size_t b)
{
 return realloc(a,b);
}

static char * _cdecl
hook_getenv(long,char *a)
{
 return getenv(a);
}

static int cdecl
hook_putenv(long,char *a)
{
 return putenv(a);
}

static int _cdecl
hook_abs(long,int a)
{
 return abs(a);
}

static long _cdecl
hook_labs(long,long a)
{
 return labs(a);
}

static void * _cdecl
hook_memcpy(long,void *a,void *b,size_t c)
{
 return memcpy(a,b,c);
}

static void * _cdecl
hook_memmove(long,void *a,void *b,size_t c)
{
 return memmove(a,b,c);
}

static char * _cdecl
hook_strcpy(long,char *a,char *b)
{
 return strcpy(a,b);
}

static char * _cdecl
hook_strncpy(long,char *a,char *b,size_t c)
{
 return strncpy(a,b,c);
}

static char * _cdecl
hook_strcat(long,char *a,char *b)
{
 return strcat(a,b);
}

static char * _cdecl
hook_strncat(long,char *a,char *b,size_t c)
{
 return strncat(a,b,c);
}

static int _cdecl
hook_memcmp(long,void *a,void *b,size_t c)
{
 return memcmp(a,b,c);
}

static int _cdecl
hook_strcmp(long,char *a,char *b)
{
 return strcmp(a,b);
}

static int _cdecl
hook_strncmp(long,char *a,char *b,size_t c)
{
 return strncmp(a,b,c);
}

static void * _cdecl
hook_memchr(long,void *a,int b,size_t c)
{
 return memchr(a,b,c);
}

static char * _cdecl
hook_strchr(long,char *a,int b)
{
 return strchr(a,b);
}

static size_t _cdecl
hook_strcspn(long,char *a,char *b)
{
 return strcspn(a,b);
}

static char * _cdecl
hook_strpbrk(long,char *a,char *b)
{
 return strpbrk(a,b);
}

static char * _cdecl
hook_strrchr(long,char *a,int b)
{
 return strrchr(a,b);
}

static size_t _cdecl
hook_strspn(long,char *a,char *b)
{
 return strspn(a,b);
}

static char * _cdecl
hook_strstr(long,char *a,char *b)
{
 return strstr(a,b);
}

static char * _cdecl
hook_strtok(long,char *a,char *b)
{
 return strtok(a,b);
}

static void * _cdecl
hook_memset(long,void *a,int b,size_t c)
{
 return memset(a,b,c);
}

static size_t _cdecl
hook_strlen(long,char *a)
{
 return strlen(a);
}

static int _cdecl
hook_memicmp(long,void *a,void *b,size_t c)
{
 return memicmp(a,b,c);
}

static char * _cdecl
hook_stpcpy(long,char *a,char *b)
{
 return stpcpy(a,b);
}

static int _cdecl
hook_strcmpl(long,char *a,char *b)
{
 return strcmpl(a,b);
}

static int _cdecl
hook_strnicmp(long,char *a, char *b, size_t c)
{
 return strnicmp(a,b,c);
}

static char * _cdecl
hook_strdup(long,char *a)
{
 return strdup(a);
}

static char * _cdecl
hook_strlwr(long,char *a)
{
 return strlwr(a);
}

static char * _cdecl
hook_strupr(long,char *a)
{
 return strupr(a);
}

static char * _cdecl
hook_strnset(long,char *a,int b,size_t c)
{
 return strnset(a,b,c);
}

static char * _cdecl
hook_strrev(long,char *a)
{
 return strrev(a);
}

static char * _cdecl
hook_strset(long,char *a,int b)
{
 return strset(a,b);
}

static void _cdecl
hook_swab(long,char *a,char *b,size_t c)
{
 swab(a,b,c);
}

static clock_t _cdecl
hook_clock(long)
{
 return clock();
}

static time_t _cdecl
hook_time(long,time_t *a)
{
 return time(a);
}

static time_t _cdecl
hook_mktime(long,struct tm *a)
{
 return mktime(a);
}

static char * _cdecl
hook_asctime(long,struct tm *a)
{
 return asctime(a);
}

static char * _cdecl
hook_ctime(long,time_t *a)
{
 return ctime(a);
}

static struct tm * _cdecl
hook_localtime(long,time_t *a)
{
 return localtime(a);
}

static struct tm * _cdecl
hook_gmtime(long,time_t *a)
{
 return gmtime(a);
}

static size_t _cdecl
hook_strftime(long,char *a,size_t b,char *c,struct tm *d)
{
 return strftime(a,b,c,d);
}

static void _cdecl
hook_sleep(long,time_t a)
{
 sleep(int(a));
}

static void _cdecl
hook_usleep(long,unsigned long a)
{
#ifdef __ZTC
 usleep(a);
#else
 msleep(int(a/1000));
#endif
}

static void _cdecl
hook_msleep(long,unsigned long a)
{
 msleep(int(a));
}

static long _cdecl
hook_MemAvail()
{
 return farcoreleft();
}

static char * _cdecl
hook_form(long,char *s ...)
{
 static int index = 0;
 static char buffer[1024];
 va_list v;

 if(index >= 512) index = 0;

 va_start(v,s);

 char *ret = &buffer[index];

 index += vsprintf(&buffer[index],s,v) + 1;

 if(index >= 1024) _exit(EXIT_FAILURE);

 return ret;
}

static void _cdecl
hook_SetColor(long,char color)
{
 char s[2] = { 0,0 };

 s[0] = color;

 io << s << char(0xFF);
}

static void _cdecl
hook_MenuFunction(long,int func,char *data)
{
 (*(menufunctions[func]))(data);
}

static char _cdecl
hook_WaitKey()
{
 return io.wait();
}

static char _cdecl
hook_WaitKeys(long,char *s)
{
 return io.wait(s);
}

static void _cdecl
hook_Input(long,char *s,int len,int method)
{
 io.read(s,len,method);
}

static void _cdecl
hook_HangUp(long)
{
 io.flush();
 exit_proboard();
}

static void _cdecl
hook_Log(long,int loglevel,char *string ...)
{
 char s[100];
 va_list x;
 va_start(x,string);

 vsprintf(s,string,x);

 LOG(loglevel,s);
}

static int _cdecl
hook_PostMessage(long,char *from,char *to,char *subj,int area,bool pvt)
{
 return (int)post_message(from,to,subj,area,pvt);
}

static int _cdecl
hook_PostNetmail(long,char *from,char *to,char *subj,int area,unsigned *address,bool attach,bool crash,bool kill)
{
 return (int)post_netmail_message(from,to,subj,area,address[0],address[1],address[2],address[3],crash,attach,kill);
}

static bool _cdecl
hook_ReadMsgArea(long,int areanum,_MsgArea *ma)
{
 MsgArea local_area;

 memcpy(&local_area , ma , sizeof(_MsgArea));

 bool r = local_area.read(areanum);

 memcpy(ma , &local_area , sizeof(_MsgArea));

 return r;
}

static int _cdecl
hook_NumMsgAreas(long)
{
 return MsgArea::highAreaNum();
}

static int _cdecl
hook_ReadFileArea(long,int area,FileArea *fa)
{
 return (fa->read(area)) ? 0 : (-1);
}

static int _cdecl
hook_NumFileAreas(long)
{
 return FileArea::highAreaNum();
}

static void _cdecl
hook_MsgEd()
{
 edit_message();
}

static void _cdecl
hook_AddTime(long,int minutes)
{
 int i;

 if(minutes<0) for(i=0;i<-minutes;i++) timer.decrease();
         else  for(i=0;i< minutes;i++) timer.increase();
}

static int _cdecl
hook_TimeLeft(long)
{
 return timer.left();
}

static int _cdecl
hook_TimeOnline(long)
{
 return timer.online();
}

static int _cdecl
hook_CheckAccess(long,word level,AccessFlags flags)
{
 return check_access(level,flags);
}

static void _cdecl
hook_SuspendTimer()
{
 timer.suspend();
}

static void _cdecl
hook_RestartTimer()
{
 timer.restart();
}

#define HANDLER_SYSOPKEY         6
#define HANDLER_HANGUP           7

static int _cdecl
hook_InstallHandler(long,int handler,void (*function)(void),unsigned ds)
{
   PEXhandle h;

   h.func = function;
   h.ds   = ds;

   switch(handler)
   {
      case HANDLER_SYSOPKEY:  sysopkey_handlers.insert(h);
                              break;
      case HANDLER_HANGUP:    hangup_handlers.insert(h);
                              break;
   }
// pex_handlers[handler].func = function;
// pex_handlers[handler].ds   = ds;

 return 0;
}

static void _cdecl
hook_RemoveHandler(long,int handler,void (*function)(void),unsigned ds)
{
   LinkedList<PEXhandle> *ll;

   switch(handler)
   {
      case HANDLER_SYSOPKEY:  ll = &sysopkey_handlers;
                              break;
      case HANDLER_HANGUP:    ll = &hangup_handlers;
                              break;
   }

   for(ll->rewind() ; !ll->eol() ; )
   {
      if(ll->get().ds == ds && ll->get().func == function)
      {
         ll->remove();
      }
      else
      {
         (*ll)++;
      }
   }
}

static bool _cdecl
hook_ReadHandler(long,int /*handler*/,PEXhandle * /*ph*/)
{
//    memcpy(ph , &pex_handlers[handler] , sizeof(PEXhandle));
//
//    return (ph->func ? TRUE : FALSE);

   return FALSE;
}

static void _cdecl
hook_RestoreHandler(long,int /*handler*/,PEXhandle * /*ph*/)
{
//    memcpy(&pex_handlers[handler] , ph , sizeof(PEXhandle));
}

static void _cdecl
hook_EnableStop()
{
 io.enablestop();
}

static void _cdecl
hook_DisableStop()
{
 io.enablestop();
}

static bool _cdecl
hook_Stopped()
{
 return stopped;
}

static bool _cdecl
hook_IO_SendByte(long,byte x)
{
 return fos_sendnw(io.port,x);
}

static int _cdecl
hook_IO_ReadByte()
{
 return fos_getchnw(io.port);
}

static bool _cdecl
hook_IO_Carrier()
{
 return fos_carrier(io.port);
}

static bool _cdecl
hook_IO_ByteReady()
{
 return fos_byteready(io.port);
}

static bool _cdecl
hook_IO_BufEmpty()
{
 return fos_outputempty(io.port);
}

static void _cdecl
hook_IO_StartBreak()
{
 fos_break(io.port,TRUE);
}

static void _cdecl
hook_IO_StopBreak()
{
 fos_break(io.port,FALSE);
}

static void _cdecl
hook_IO_DTR(long,bool mode)
{
 fos_setdtr(io.port,mode);
}

static word _cdecl
hook_IO_SendBlock(long,byte *buf,word size)
{
   return fos_sendblock(io.port,buf,size);
}

static word _cdecl
hook_IO_ReadBlock(long,byte *buf,word size)
{
   return fos_readblock(io.port,buf,size);
}


static 
void _cdecl hook_PostInfo( long,
                           char *fname )
{
     File  fp( FileName( syspath,
                         fname,
                         ".ASW" ),
               fmode_write     | 
                 fmode_text    |
                 fmode_append  | 
                 fmode_copen );


     if ( ! fp.opened() ) 
     {
          return;
     }


     Date date( TODAY );
     Time time( NOW   );


     fp.printf( "*** %s completed questionnaire at %02d:%02d on %02d-%s-%02d ***\n",
                user.name,
                time[ 0 ],
                time[ 1 ],
                date[ 0 ],
                months_short[ date[ 1 ] ],
                date[ 2 ] % 100 );           // Y2K BUG FIXED!
             // date[ 2 ] );                 // Y2K BUG!
}

static char _cdecl
hook_ShowHotkeyFile(long,char *fname,char *hotkeys)
{
 return io.sendfile(fname,hotkeys);
}

static char _cdecl
hook_ShowHotkeyANSIFile(long,char *fname,char *hotkeys)
{
 return showansasc(fname,hotkeys);
}

static char _cdecl
hook_PlayMusic(long,char *fname,char *hotkeys)
{
 return playsong(fname,hotkeys,0);
}

static void _cdecl
hook_AdjustTime()
{
 adjust_limits();
}

static bool _cdecl
hook_ReadUser(long,User *rec,long recnr)
{
 return rec->read(recnr);
}

static void _cdecl
hook_WriteUser(long,User *rec)
{
   bool append = FALSE;

   if(rec->record < 0)
      append = TRUE;

   rec->write(append);

   num_users = int(User::numUsers());
}

static bool _cdecl
hook_SetUser(long,long r)
{
   if(user.read(r))
   {
      user_recnr = r;
      return TRUE;
   }
   else
   {
      return FALSE;
   }

}

void *operator new(size_t,void *p) { return p; }

static bool _cdecl
hook_ReadMessage(long,Message *msg,long msgnum,int area)
{
   Message *local_msg = new (msg) Message;

   local_msg->setArea(area);

   bool r = local_msg->read(msgnum);

   area = local_msg->areaNum();

   local_msg->Message::~Message();

   msg->area = area;

   return r;
}


static void _cdecl
hook_WriteMSGTMP(long,char *text)
{
 File f;
 f.open("MSGTMP",fmode_create);
 f << text;
}

static void _cdecl
hook_AppendMSGTMP(long,char *text)
{
 File f;
 if(!f.open("MSGTMP",fmode_write | fmode_copen | fmode_append)) return;
 f << text;
}

static void _cdecl
hook_ShowMessage(long,Message *msg)
{
 msg->show();
}

static void _cdecl
hook_CreateMessageText(long,Message *msg)
{
   int area = msg->area;

   msg->msgArea = new MsgArea;

   msg->setArea(area);

   msg->createMsgTextFile("MSGTMP",FALSE);

   delete msg->msgArea;

   msg->area = area;
}

static void _cdecl
hook_CreateMessageTextString(long,Message *msg,char *s,int maxsize)
{
#ifdef DEBUG_PEX
   LOG("DEBUG: <Entry %ld> CreateMessageTextString( ... maxsize = %d)" , clockticks() , maxsize);
#endif
   int area = msg->area;

   msg->msgArea = new MsgArea;

   msg->setArea(area);

   msg->createMsgTextString(s,maxsize);

   delete msg->msgArea;

   msg->area = area;

#ifdef DEBUG_PEX
   LOG("DEBUG: <Exit %ld> CreateMessageTextString()", clockticks());
#endif
}

static bool _cdecl
hook_FirstMessage(long,Message *msg,int area,int order,long first)
{
#ifdef DEBUG_PEX
   LOG("DEBUG: <Entry %ld> FirstMessage( ... , area = %d , order = %d , first = %ld)" , clockticks() , area,order,first);
#endif
   msg->clear();

   msg->msgArea = new MsgArea;

   msg->setArea(area);

   bool r = msg->readFirst(READMSG_ALL,order,first,"");

   area = msg->areaNum();

   delete msg->msgArea;

   msg->area = area;

#ifdef DEBUG_PEX
   LOG("DEBUG: <Exit %ld> FirstMessage() = %s", clockticks() , r?"TRUE":"FALSE");
#endif

   return r;
}

static bool _cdecl
hook_NextMessage(long,Message *msg,int area,int order)
{
#ifdef DEBUG_PEX
   LOG("DEBUG: <Entry %ld> NextMessage( ... , area = %d , order = %d )" , clockticks() , area,order);
#endif
   msg->msgArea = new MsgArea;

   msg->setArea(area);

   bool r = msg->readNext(READMSG_ALL,order,"");

   area = msg->areaNum();

   delete msg->msgArea;

   msg->area = area;

#ifdef DEBUG_PEX
   LOG("DEBUG: <Exit %ld> NextMessage() = %s", clockticks() , r?"TRUE":"FALSE");
#endif
   return r;
}

static void _cdecl
hook_DeleteMessage(long,Message *msg)
{
   int area = msg->area;

   msg->msgArea = new MsgArea;

   msg->setArea(area);

   msg->remove();

   delete msg->msgArea;

   msg->area = area;
}

static void _cdecl
hook_MarkMessage(long,int area,long msgnum)
{
 MsgArea ma;

 if(!ma.read(area)) return;

 markedmsglist.add(MessageIndex(area,msgnum));
}

static long _cdecl
hook_GetLastRead(long,int area,long userrec)
{
 MsgArea ma;
#ifdef DEBUG_PEX

 LOG("DEBUG: <Entry %ld> GetLastRead( area = %d , recno = %ld )" , clockticks() , area,userrec);

 if(!ma.read(area))
 {
   LOG("SDK: Area not found");
   return -1;
 }

 long n = ma.msgNum(ma.lastRead(userrec));

 LOG("DEBUG: <Exit %ld> GetLastRead() = %ld" , clockticks() , n);

 return n;
#else
 if(!ma.read(area)) return -1;

 return ma.lastRead(userrec);
#endif
}

static void _cdecl
hook_SetLastRead(long,int area,long userrec,long num)
{
 MsgArea ma;
#ifdef DEBUG_PEX
 LOG("DEBUG: SetLastRead( area = %d , recno = %ld , num = %ld )",area,userrec,num);
#endif
 if(!ma.read(area)) return;

 ma.setLastRead(userrec,num);
}

long
hook_MsgId( long , int area , long num)
{
   MsgArea ma;

   if(!ma.read(area)) return -1;

   return ma.msgId(num);
}

long
hook_MsgNum( long , int area , long id)
{
   MsgArea ma;

   if(!ma.read(area)) return -1;

   return ma.msgNum(id);
}

long
hook_HighMsg(long,int area)
{
   MsgArea ma;

   if(!ma.read(area)) return -1;

   return ma.highMsg();
}

long
hook_NumMsgs(long,int area)
{
   MsgArea ma;

   if(!ma.read(area)) return -1;

   return ma.numMsgs();
}

static void _cdecl
hook_UnMarkAllMessages()
{
 markedmsglist.clear();
}

static void _cdecl
hook_ReadMarkedMessages()
{
 readmsg("0 /N");
}

static void _cdecl
hook_ListMarkedMessages()
{
 qscan_msg("0 /N");
}

static bool _cdecl
hook_Ask(long,bool def)
{
 return (io.ask(def) ? TRUE : FALSE);
}

static char _cdecl
hook_PeekChar()
{
 return io.readkey();
}

static KEY _cdecl
hook_ScanKey()
{
 if(KB.hit()) return KB.get();
         else return 0;
}

static void _cdecl
hook_InitLineCounter()
{
 linecounter(0);
}

static bool _cdecl
hook_LineCounter()
{
 return linecounter() ? FALSE : TRUE;
}

static int _cdecl
hook_FuzzySearch(long,char *text,char *search,int degree)
{
 return fuzzy_search(search,text,degree);
}

static bool _cdecl
hook_ExternalInput()
{
 return io.external ? TRUE:FALSE;
}

static void _cdecl
hook_GotoXY(long,int x,int y)
{
 io.move(x,y);
 io.flush();
}

static void _cdecl
hook_SetFullColor(long,byte x)
{
 io.fullcolor(x);
 io.flush();
}

static int _cdecl
hook_ErrorLevel()
{
 return returnvalue;
}

static void _cdecl
hook_ClrEol()
{
 io.clreol();
 io.flush();
}

static bool _cdecl
hook_GetIniVar(long,char *a,char *b,char *c,int d)
{
 return get_ini_var(a,b,c,d);
}

static bool _cdecl
hook_SetIniVar(long,char *a,char *b,char *c)
{
 return set_ini_var(a,b,c);
}

static void _cdecl
hook_ParseStringVars(long,char *input , char *output , int maxlen)
{
 String s = replace_stringvars(input);

 strncpy(output , s , maxlen);
 output[maxlen] = '\0';
}

static bool _cdecl
hook_LocalDisplay(long,bool a)
{
   bool r = io.show_local;

   io.show_local = a;

   return r;
}

static bool _cdecl
hook_ShowRIPscrip(long,char *a)
{
   return show_rip(a,FALSE);
}

static bool _cdecl
hook_RIP(long)
{
   return rip_mode;
}

static bool _cdecl
hook_RemoteDisplay(long,bool a)
{
   bool r = io.show_remote;

   io.show_remote = a;

   return r;
}

static int _cdecl
hook_TimeUntilEvent(long)
{
   return (event_due - timer.online());
}

static void _cdecl
hook_ResetInactivity(long)
{
   timer.clearinactivity();
}

typedef struct
   {
      char name[13];
      word area;
   } SDK_TAGGED_FILE;

static int _cdecl
hook_GetTaggedFiles(long,SDK_TAGGED_FILE *array,int max)
{
   int n = 0;

   for(taglist.rewind(); !taglist.eol() && n < max ; n++,taglist++)
   {
      strncpy(array[n].name,taglist.get().name,12);
      array[n].name[12] = '\0';
      array[n].area = taglist.get().area;
   }

   return n;
}

bool
hook_PutTaggedFiles(long,SDK_TAGGED_FILE *array,int n)
{
   taglist.kill();

   for(int i=0;i<n;i++)
   {
      FilesIdx idx;

      strncpy(idx.name,array[i].name,12);
      idx.name[12] = '\0';
      idx.area = array[i].area;

      taglist.add(idx);
   }

   return TRUE;
}

bool
hook_AddTaggedFile(long,SDK_TAGGED_FILE *tag)
{
   if(!is_tagged(tag->name,tag->area))
   {
      FilesIdx idx;

      strncpy(idx.name,tag->name,12);
      idx.name[12] = '\0';
      idx.area = tag->area;

      taglist.add(idx);

      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

bool
hook_RemoveTaggedFile(SDK_TAGGED_FILE *tag)
{
   if(is_tagged(tag->name,tag->area))
   {
      for(taglist.rewind() ; !taglist.eol() ; taglist++)
      {
         if(!stricmp(tag->name,taglist.get().name) && tag->area == taglist.get().area)
         {
            taglist.remove();

            return TRUE;
         }
      }
   }

   return FALSE;
}

void
hook_ClearTaggedFiles(long)
{
   taglist.kill();
}


bool
hook_IsTagged(long,SDK_TAGGED_FILE *tag)
{
   return is_tagged(tag->name,tag->area);
}


//-------------------------  
// THESE WERE ADDED IN V2.2
//-------------------------  

static
int _cdecl hook_getdisk( long )
{
     return getdisk();
}

static
int _cdecl hook_getcurdir( long,
                           int   drive, 
                           char *directory )
{
     return getcurdir( drive, directory );
}

static
unsigned _cdecl hook_dos_getftime( long,
                                   int       fd, 
                                   unsigned *date, 
                                   unsigned *time )
{
     return _dos_getftime( fd, date, time );
}

static
unsigned _cdecl hook_dos_setftime( long,
                                   int      fd, 
                                   unsigned date, 
                                   unsigned time )
{
     return _dos_setftime( fd, date, time );
}

static
int _cdecl hook_fileno( long,
                        FILE *a )
{
     return fileno( a );
}


/******* END OF HOOKS ********************************************************/

typedef void _far *DataHook;
typedef void _far *FuncHook;

typedef int (_far _cdecl *MainFunc)(int,char *[]);

struct ExeHeader
  {
   unsigned Signature;
   unsigned PartPage;
   unsigned PageCount;
   unsigned NumRelocItems;
   unsigned HeaderSize;
   unsigned MinMem;
   unsigned MaxMem;
   unsigned SS;
   unsigned SP;
   unsigned Checksum;
   unsigned IP;
   unsigned CS;
   unsigned FirstReloc;
   unsigned Overlay;
  };

struct PexHdr
  {
   unsigned host_DS;
   MainFunc entry;
   void _far *functable;
   void _far *datatable;
   unsigned Version;
  };

struct PexRes
  {
   String name;
   MainFunc func;
   DataHook data;
   void *p;
   void (_far _cdecl *constr)(void);
   void (_far _cdecl *destr )(void);
  };

struct DataStruct
  {
   DataHook user;
   DataHook login_date;
   DataHook login_time;
   DataHook user_recnr;
   DataHook limit;
   DataHook num_limits;
   DataHook net_entered;
   DataHook echo_entered;
   DataHook num_users;
   DataHook node_number;
   DataHook curmenu;
   DataHook user_firstname;
   DataHook lastuser;
   DataHook mypath;
   DataHook syspath;
   DataHook cfg;
   DataHook version;
   DataHook baud;
   DataHook beta;
   DataHook page_reason;
   DataHook num_yells;
  };

inline void
init_values(DataStruct *p)
{
   p->user              = (DataHook) &user;
   p->login_date        = (DataHook) &login_date;
   p->login_time        = (DataHook) &login_time;
   p->user_recnr        = (DataHook) (long) user_recnr;
   p->limit             = (DataHook) &limit;
   p->num_limits        = (DataHook) (long) num_limits;
   p->net_entered       = (DataHook) (long) net_entered;
   p->echo_entered      = (DataHook) (long) echo_entered;
   p->num_users         = (DataHook) (long) num_users;
   p->node_number       = (DataHook) (long) node_number;
   p->curmenu           = (DataHook) &curmenu[0];
   p->user_firstname    = (DataHook) &user_firstname[0];
   p->lastuser          = (DataHook) &lastcaller.name[0];
   p->mypath            = (DataHook) &mypath[0];
   p->syspath           = (DataHook) &syspath[0];
   p->cfg               = (DataHook) &cfg;
   p->version           = (DataHook) (long) VER_WORD;
   p->baud              = (DataHook) (long) io.baud;
   p->beta              = (DataHook) (long) VER_BETA;
   p->page_reason       = (DataHook) page_reason;
   p->num_yells         = (DataHook) (long) &num_yells;
}

inline void
update_values(DataStruct *)
{
}

static bool _near load_pex(char *pexname);

static PexRes res_pex[100];
static bool internal;

static bool _near
is_resident(char *name)
{
 for(int i=0;i<100;i++) if(res_pex[i].name == name) return TRUE;

 return FALSE;
}

static void _near
add_resident(PexRes &x)
{
 for(int i=0;i<100;i++)
   if(res_pex[i].name[0] == '\0')
     {
      res_pex[i].name   = x.name;
      res_pex[i].func   = x.func;
      res_pex[i].data   = x.data;
      res_pex[i].p      = x.p;
      res_pex[i].constr = x.constr;
      res_pex[i].destr  = x.destr;

      return;
     }
}

static void _near
remove_resident(char *name)
{
 for(int i=0;i<100;i++)
   if(res_pex[i].name == name)
     {
      word ds = FP_SEG(res_pex[i].data);

      for( sysopkey_handlers.rewind() ; !sysopkey_handlers.eol() ; )
         if(sysopkey_handlers.get().ds == ds)
         {
            sysopkey_handlers.remove();
         }
         else
         {
            sysopkey_handlers++;
         }

      for( hangup_handlers.rewind() ; !hangup_handlers.eol() ; )
         if(hangup_handlers.get().ds == ds)
         {
            hangup_handlers.remove();
         }
         else
         {
            hangup_handlers++;
         }

      if(res_pex[i].destr) (*res_pex[i].destr)();  // Call static destructors

      farfree(res_pex[i].p);

      res_pex[i].name.clear();
      res_pex[i].func   = NULLFUNC;
      res_pex[i].data   = NULL;
      res_pex[i].p      = NULL;
      res_pex[i].constr = NULL;
      res_pex[i].destr  = NULL;

      return;
     }
}

static int _near
run_resident(char *name,int argc,char *argv[])
{
 for(int i=0;i<100;i++)
   if(res_pex[i].name == name)
     {
      init_values((DataStruct *)res_pex[i].data);
      int r = (*res_pex[i].func)(argc,argv);
      update_values((DataStruct *)res_pex[i].data);

      return r;
     }

 return -1;
}


int
run_sdkfile(char *data)
{
 internal = FALSE;

 String param[20];
 String tmps;
 FileName pexfullname;

 int npara = parse_data(data,param);

 for(int i=0;i<npara;i++)
   {
    if(param[i][0] != '/') break;

    param[i].upperCase();
    switch(param[i][1])
      {
       case 'I': internal = TRUE;
                 break;
      }
   }

 param[i].trim();

 if(strchr(param[i],'\\'))
 {
   if(param[i][0] != '\\' && param[i][1] != ':')
   {
      pexfullname = cfg.pexpath;
      pexfullname << param[i];
   }
   else
   {
      pexfullname = param[i];
   }
 }
 else
 {
   pexfullname = cfg.pexpath;
   pexfullname << param[i];
 }

 pexfullname.upperCase();

 if(load_pex(pexfullname) == FALSE) return -1;

 char *argv[10] = { pexfullname, param[i+1], param[i+2], param[i+3], param[i+4],
                    param[i+5], param[i+6], param[i+7], param[i+8], param[i+9] };

 int retval = run_resident(pexfullname,npara-i,argv);

 if(retval!=-1) remove_resident(pexfullname);

 return 0;
}



static 
bool _near load_pex( char *pexname )
{
 if(is_resident(pexname)) return TRUE;

 PexRes res;
 FileName fn(pexname);
 fn.changeExt("PEX");

 File f;

 if(!f.open(fn , fmode_read , 1024))
   {
    if(!internal) LOG("Can't read file <%s>",(char *)fn);
    return FALSE;
   }

 ExeHeader exehdr;

 f.read(&exehdr,sizeof(exehdr));

 long l = 512L*exehdr.PageCount-16L*exehdr.HeaderSize+exehdr.PartPage-(exehdr.PartPage ? 512:0);
 long allocsize = long(exehdr.SS+2) << 4L;

 char *ptr;

 if((ptr = (char *)farmalloc(allocsize))==NULL)
   {
    LOG("PEX file <%s> : Not enough memory",pexname);
    return FALSE;
   }
  else
   {
    byte huge *hptr = (byte huge *)ptr;

    for(long x = 0 ; x < allocsize ; x++) hptr[x] = '\0';
   }

 res.p = ptr;

 ptr = (char *)normalize_ptr(ptr);          // Normalize pointer
 ptr = (char *)MK_FP(FP_SEG(ptr)+1,0);      // Make zero-offset pointer

 f.seek(16L*exehdr.HeaderSize);

 {
  char * p = ptr;

  for(;;)
    {
     int n;

     if(!l) break;

     if(l<30000) n = f.read(p,unsigned(l));
            else n = f.read(p,30000);

     l -= n;
     p = (char *)normalize_ptr(p+n);
    }
 }

 if(exehdr.NumRelocItems)
   {
    f.seek(exehdr.FirstReloc);
    word loadseg = FP_SEG(ptr);

    for(int i=0;i<exehdr.NumRelocItems;i++)
      {
       struct {
          word off,
               seg;
         } r;

       f.read(&r,4);                             // Read reloc-item
       word *p = (word *)MK_FP(loadseg+r.seg,r.off);
       *p += loadseg;                            // Do fixup
      }
   }

 ptr = (char *)MK_FP(exehdr.CS+FP_SEG(ptr),exehdr.IP);
 ptr -= 16;

 if(*((unsigned *)ptr) != 0x4250)
   {
    LOG("Illegal PEX-file <%s>",(char *)fn);
    return FALSE;
   }

 if(((PexHdr *)ptr)->Version>VER_NUM)
   {
    LOG("PEX file <%s> needs ProBoard v%d.%d",(char *)fn,((PexHdr *)ptr)->Version/100,((PexHdr *)ptr)->Version%100);
    return FALSE;
   }

 if(((PexHdr *)ptr)->Version < 200)
   {
    LOG("PEX file <%s> compiled with old SDK (v%d.%d), please upgrade!",(char *)fn,((PexHdr *)ptr)->Version/100,((PexHdr *)ptr)->Version%100);
    return FALSE;
   }

 if(((PexHdr *)ptr)->Version >= 129)
   {
    res.constr = (void (_far _cdecl *)(void)) (*(long *)(ptr - 4));
    res.destr  = (void (_far _cdecl *)(void)) (*(long *)(ptr - 8));
   }
  else
   {
    res.constr = NULL;
    res.destr  = NULL;
   }

 SCRAMBLE();

 ((PexHdr *)ptr)->host_DS = _DS;

 FuncHook *fptr = (FuncHook *)(((PexHdr *)ptr)->functable);

 *(fptr++) = (FuncHook) hook_l_mul;
 *(fptr++) = (FuncHook) hook_l_div;
 *(fptr++) = (FuncHook) hook_l_mod;
 *(fptr++) = (FuncHook) hook_ul_div;
 *(fptr++) = (FuncHook) hook_ul_mod;
 *(fptr++) = (FuncHook) hook_l_shl;
 *(fptr++) = (FuncHook) hook_l_shr;
 *(fptr++) = (FuncHook) hook_ul_shl;
 *(fptr++) = (FuncHook) hook_ul_shr;
 *(fptr++) = (FuncHook) hook_fopen;
 *(fptr++) = (FuncHook) hook_freopen;
 *(fptr++) = (FuncHook) hook_fseek;
 *(fptr++) = (FuncHook) hook_ftell;
 *(fptr++) = (FuncHook) hook_fgets;
 *(fptr++) = (FuncHook) hook_fgetc;
 *(fptr++) = (FuncHook) hook_fflush;
 *(fptr++) = (FuncHook) hook_fclose;
 *(fptr++) = (FuncHook) hook_fputs;
 *(fptr++) = (FuncHook) hook_getc;
 *(fptr++) = (FuncHook) hook_getchar;
 *(fptr++) = (FuncHook) hook_gets;
 *(fptr++) = (FuncHook) hook_fputc;
 *(fptr++) = (FuncHook) hook_putc;
 *(fptr++) = (FuncHook) hook_putchar;
 *(fptr++) = (FuncHook) hook_puts;
 *(fptr++) = (FuncHook) hook_fread;
 *(fptr++) = (FuncHook) hook_fwrite;
 *(fptr++) = (FuncHook) hook_printf;
 *(fptr++) = (FuncHook) hook_fprintf;
 *(fptr++) = (FuncHook) hook_vfprintf;
 *(fptr++) = (FuncHook) hook_vprintf;
 *(fptr++) = (FuncHook) hook_sprintf;
 *(fptr++) = (FuncHook) hook_vsprintf;
 *(fptr++) = (FuncHook) hook_setbuf;
 *(fptr++) = (FuncHook) hook_setvbuf;
 *(fptr++) = (FuncHook) hook_rename;
 *(fptr++) = (FuncHook) hook_rewind;
 *(fptr++) = (FuncHook) hook_clearerr;
 *(fptr++) = (FuncHook) hook_feof;
 *(fptr++) = (FuncHook) hook_isalnum;
 *(fptr++) = (FuncHook) hook_isalpha;
 *(fptr++) = (FuncHook) hook_iscntrl;
 *(fptr++) = (FuncHook) hook_isdigit;
 *(fptr++) = (FuncHook) hook_isgraph;
 *(fptr++) = (FuncHook) hook_islower;
 *(fptr++) = (FuncHook) hook_isprint;
 *(fptr++) = (FuncHook) hook_ispunct;
 *(fptr++) = (FuncHook) hook_isspace;
 *(fptr++) = (FuncHook) hook_isupper;
 *(fptr++) = (FuncHook) hook_isxdigit;
 *(fptr++) = (FuncHook) hook_toupper;
 *(fptr++) = (FuncHook) hook_tolower;
 *(fptr++) = (FuncHook) hook_int86;
 *(fptr++) = (FuncHook) hook_int86x;
 *(fptr++) = (FuncHook) hook_intdos;
 *(fptr++) = (FuncHook) hook_intdosx;
 *(fptr++) = (FuncHook) hook_dos_findfirst;
 *(fptr++) = (FuncHook) hook_dos_findnext;
 *(fptr++) = (FuncHook) hook_read;
 *(fptr++) = (FuncHook) hook_write;
 *(fptr++) = (FuncHook) hook_open;
 *(fptr++) = (FuncHook) hook_creat;
 *(fptr++) = (FuncHook) hook_close;
 *(fptr++) = (FuncHook) hook_unlink;
 *(fptr++) = (FuncHook) hook_remove;
 *(fptr++) = (FuncHook) hook_chsize;
 *(fptr++) = (FuncHook) hook_dup;
 *(fptr++) = (FuncHook) hook_dup2;
 *(fptr++) = (FuncHook) hook_lseek;
 *(fptr++) = (FuncHook) hook_access;
 *(fptr++) = (FuncHook) hook_filesize;
 *(fptr++) = (FuncHook) hook_filelength;
 *(fptr++) = (FuncHook) hook_isatty;
 *(fptr++) = (FuncHook) hook_atoi;
 *(fptr++) = (FuncHook) hook_atol;
 *(fptr++) = (FuncHook) hook_strtol;
 *(fptr++) = (FuncHook) hook_rand;
 *(fptr++) = (FuncHook) hook_srand;
 *(fptr++) = (FuncHook) hook_calloc;
 *(fptr++) = (FuncHook) hook_free;
 *(fptr++) = (FuncHook) hook_malloc;
 *(fptr++) = (FuncHook) hook_realloc;
 *(fptr++) = (FuncHook) hook_getenv;
 *(fptr++) = (FuncHook) NULL;              // hook_bsearch;
 *(fptr++) = (FuncHook) NULL;              // hook_qsort;
 *(fptr++) = (FuncHook) hook_abs;
 *(fptr++) = (FuncHook) hook_labs;
 *(fptr++) = (FuncHook) hook_memcpy;
 *(fptr++) = (FuncHook) hook_memmove;
 *(fptr++) = (FuncHook) hook_strcpy;
 *(fptr++) = (FuncHook) hook_strncpy;
 *(fptr++) = (FuncHook) hook_strcat;
 *(fptr++) = (FuncHook) hook_strncat;
 *(fptr++) = (FuncHook) hook_memcmp;
 *(fptr++) = (FuncHook) hook_strcmp;
 *(fptr++) = (FuncHook) hook_strncmp;
 *(fptr++) = (FuncHook) hook_memchr;
 *(fptr++) = (FuncHook) hook_strchr;
 *(fptr++) = (FuncHook) hook_strcspn;
 *(fptr++) = (FuncHook) hook_strpbrk;
 *(fptr++) = (FuncHook) hook_strrchr;
 *(fptr++) = (FuncHook) hook_strspn;
 *(fptr++) = (FuncHook) hook_strstr;
 *(fptr++) = (FuncHook) hook_strtok;
 *(fptr++) = (FuncHook) hook_memset;
 *(fptr++) = (FuncHook) hook_strlen;
 *(fptr++) = (FuncHook) hook_memicmp;
 *(fptr++) = (FuncHook) hook_stpcpy;
 *(fptr++) = (FuncHook) hook_strcmpl;
 *(fptr++) = (FuncHook) hook_strnicmp;
 *(fptr++) = (FuncHook) hook_strdup;
 *(fptr++) = (FuncHook) hook_strlwr;
 *(fptr++) = (FuncHook) hook_strupr;
 *(fptr++) = (FuncHook) hook_strnset;
 *(fptr++) = (FuncHook) hook_strrev;
 *(fptr++) = (FuncHook) hook_strset;
 *(fptr++) = (FuncHook) hook_swab;
 *(fptr++) = (FuncHook) hook_clock;
 *(fptr++) = (FuncHook) hook_time;
 *(fptr++) = (FuncHook) hook_mktime;
 *(fptr++) = (FuncHook) hook_asctime;
 *(fptr++) = (FuncHook) hook_ctime;
 *(fptr++) = (FuncHook) hook_localtime;
 *(fptr++) = (FuncHook) hook_gmtime;
 *(fptr++) = (FuncHook) hook_strftime;
 *(fptr++) = (FuncHook) hook_sleep;
 *(fptr++) = (FuncHook) hook_usleep;
 *(fptr++) = (FuncHook) hook_msleep;
 *(fptr++) = (FuncHook) hook_form;
 *(fptr++) = (FuncHook) hook_MemAvail;
 *(fptr++) = (FuncHook) hook_SetColor;
 *(fptr++) = (FuncHook) hook_MenuFunction;
 *(fptr++) = (FuncHook) hook_WaitKey;
 *(fptr++) = (FuncHook) hook_WaitKeys;
 *(fptr++) = (FuncHook) hook_Input;
 *(fptr++) = (FuncHook) hook_Log;
 *(fptr++) = (FuncHook) hook_HangUp;
 *(fptr++) = (FuncHook) hook_PostMessage;
 *(fptr++) = (FuncHook) hook_ReadMsgArea;
 *(fptr++) = (FuncHook) hook_MsgEd;
 *(fptr++) = (FuncHook) hook_AddTime;
 *(fptr++) = (FuncHook) hook_TimeLeft;
 *(fptr++) = (FuncHook) hook_TimeOnline;
 *(fptr++) = (FuncHook) hook_CheckAccess;
 *(fptr++) = (FuncHook) hook_SuspendTimer;
 *(fptr++) = (FuncHook) hook_RestartTimer;
 *(fptr++) = (FuncHook) hook_InstallHandler;
 *(fptr++) = (FuncHook) hook_EnableStop;
 *(fptr++) = (FuncHook) hook_DisableStop;
 *(fptr++) = (FuncHook) hook_Stopped;
 *(fptr++) = (FuncHook) hook_IO_SendByte;
 *(fptr++) = (FuncHook) hook_IO_ReadByte;
 *(fptr++) = (FuncHook) hook_IO_Carrier;
 *(fptr++) = (FuncHook) hook_IO_ByteReady;
 *(fptr++) = (FuncHook) hook_IO_BufEmpty;
 *(fptr++) = (FuncHook) hook_IO_StartBreak;
 *(fptr++) = (FuncHook) hook_IO_StopBreak;
 *(fptr++) = (FuncHook) hook_IO_DTR;
 *(fptr++) = (FuncHook) hook_PostInfo;
 *(fptr++) = (FuncHook) hook_ShowHotkeyFile;
 *(fptr++) = (FuncHook) hook_ShowHotkeyANSIFile;
 *(fptr++) = (FuncHook) hook_PlayMusic;
 *(fptr++) = (FuncHook) hook_AdjustTime;
 *(fptr++) = (FuncHook) hook_ReadUser;
 *(fptr++) = (FuncHook) hook_WriteUser;
 *(fptr++) = (FuncHook) hook_ReadMessage;
 *(fptr++) = (FuncHook) hook_WriteMSGTMP;
 *(fptr++) = (FuncHook) hook_AppendMSGTMP;
 *(fptr++) = (FuncHook) hook_ShowMessage;
 *(fptr++) = (FuncHook) hook_CreateMessageText;
 *(fptr++) = (FuncHook) hook_FirstMessage;
 *(fptr++) = (FuncHook) hook_NextMessage;
 *(fptr++) = (FuncHook) hook_DeleteMessage;
 *(fptr++) = (FuncHook) hook_PostNetmail;
 *(fptr++) = (FuncHook) hook_MarkMessage;
 *(fptr++) = (FuncHook) hook_ReadMarkedMessages;
 *(fptr++) = (FuncHook) hook_ListMarkedMessages;
 *(fptr++) = (FuncHook) hook_UnMarkAllMessages;
 *(fptr++) = (FuncHook) hook_Ask;
 *(fptr++) = (FuncHook) hook_PeekChar;
 *(fptr++) = (FuncHook) hook_ScanKey;
 *(fptr++) = (FuncHook) hook_InitLineCounter;
 *(fptr++) = (FuncHook) hook_LineCounter;
 *(fptr++) = (FuncHook) hook_ReadFileArea;
 *(fptr++) = (FuncHook) hook_CreateMessageTextString;
 *(fptr++) = (FuncHook) hook_FuzzySearch;
 *(fptr++) = (FuncHook) hook_RemoveHandler;
 *(fptr++) = (FuncHook) hook_ExternalInput;
 *(fptr++) = (FuncHook) hook_GotoXY;
 *(fptr++) = (FuncHook) hook_SetFullColor;
 *(fptr++) = (FuncHook) hook_putenv;
 *(fptr++) = (FuncHook) hook_ErrorLevel;
 *(fptr++) = (FuncHook) hook_ClrEol;
 *(fptr++) = (FuncHook) hook_GetIniVar;
 *(fptr++) = (FuncHook) hook_SetIniVar;
 *(fptr++) = (FuncHook) hook_NumMsgAreas;
 *(fptr++) = (FuncHook) hook_NumFileAreas;
 *(fptr++) = (FuncHook) hook_GetLastRead;
 *(fptr++) = (FuncHook) hook_SetLastRead;
 *(fptr++) = (FuncHook) hook_ReadHandler;
 *(fptr++) = (FuncHook) hook_RestoreHandler;
 *(fptr++) = (FuncHook) hook_ParseStringVars;
 *(fptr++) = (FuncHook) hook_LocalDisplay;
 *(fptr++) = (FuncHook) hook_ShowRIPscrip;
 *(fptr++) = (FuncHook) hook_RIP;
 *(fptr++) = (FuncHook) hook_RemoteDisplay;
 *(fptr++) = (FuncHook) hook_MsgNum;
 *(fptr++) = (FuncHook) hook_MsgId;
 *(fptr++) = (FuncHook) hook_HighMsg;
 *(fptr++) = (FuncHook) hook_NumMsgs;
 *(fptr++) = (FuncHook) hook_TimeUntilEvent;
 *(fptr++) = (FuncHook) hook_ResetInactivity;
 *(fptr++) = (FuncHook) hook_mkdir;
 *(fptr++) = (FuncHook) hook_rmdir;
 *(fptr++) = (FuncHook) hook_chdir;
 *(fptr++) = (FuncHook) hook_GetTaggedFiles;
 *(fptr++) = (FuncHook) hook_PutTaggedFiles;
 *(fptr++) = (FuncHook) hook_AddTaggedFile;
 *(fptr++) = (FuncHook) hook_RemoveTaggedFile;
 *(fptr++) = (FuncHook) hook_ClearTaggedFiles;
 *(fptr++) = (FuncHook) hook_IsTagged;
 *(fptr++) = (FuncHook) hook_SetUser;
 *(fptr++) = (FuncHook) hook_IO_SendBlock;
 *(fptr++) = (FuncHook) hook_IO_ReadBlock;


 //-------------------  
 // New, added in v2.2
 //-------------------  

 *(fptr++) = (FuncHook) hook_getdisk;
 *(fptr++) = (FuncHook) hook_getcurdir;
 *(fptr++) = (FuncHook) hook_dos_getftime;
 *(fptr++) = (FuncHook) hook_dos_setftime;
 *(fptr++) = (FuncHook) hook_fileno;

 res.name = pexname;
 res.func = ((PexHdr *)ptr)->entry;
 res.data = (DataStruct *)(((PexHdr *)ptr)->datatable);

 add_resident(res);

 init_values((DataStruct *)res.data);

 if(res.constr) (*res.constr)();

 return TRUE;
}

