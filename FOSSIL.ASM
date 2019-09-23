% MODEL MEM_MOD

PUBLIC _fos_setbps
PUBLIC _fos_init
PUBLIC _fos_deinit
PUBLIC _fos_purgeoutput
PUBLIC _fos_purgeinput
PUBLIC _fos_status
PUBLIC _fos_setdtr
PUBLIC _fos_send
PUBLIC _fos_sendnw
PUBLIC _fos_flowctl
PUBLIC _fos_getch
PUBLIC _fos_getchnw
PUBLIC _fos_break
PUBLIC _fos_sendblock
PUBLIC _fos_readblock

   CODESEG

nofossil_err DB 'FOSSIL driver not installed!',13,10,'$'

EVEN

PROC _fos_setbps
         arg  port:WORD,baud:BYTE
         push    bp
         mov     bp,sp
         mov     ah,0
         mov     al,[baud]
         mov     dx,[port]
         int     14h
         pop     bp
         ret
ENDP _fos_setbps

PROC _fos_status
         arg  port:WORD
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     ah,3
         int     14h
         pop     bp
         ret
ENDP _fos_status

PROC _fos_init
         arg port:WORD
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     ah,4
         int     14h
         cmp     ax,1954h
         je      foss_ok
         push    cs
         pop     ds
         mov     dx,offset nofossil_err
         mov     ah,9
         int     21h
         mov     ax,4cffh
         int     21h
foss_ok: pop     bp
         ret
ENDP _fos_init

PROC _fos_deinit
         arg port:WORD
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     ah,5
         int     14h
         pop     bp
         ret
ENDP _fos_deinit

PROC _fos_setdtr
         arg port:WORD,state:BYTE
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     al,[state]
         mov     ah,6
         int     14h
         pop     bp
         ret
ENDP _fos_setdtr

PROC _fos_purgeoutput
         arg port:WORD
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     ah,9
         int     14h
         pop     bp
         ret
ENDP _fos_purgeoutput

PROC _fos_purgeinput
         arg port:WORD
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     ah,0Ah
         int     14h
         pop     bp
         ret
ENDP _fos_purgeinput

PROC _fos_sendnw
         arg port:WORD,char:BYTE
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     al,[char]
         mov     ah,0Bh
         int     14h
         pop     bp
         ret
ENDP _fos_sendnw

PROC _fos_send
         arg port:WORD,char:BYTE
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     al,[char]
         mov     ah,1
         int     14h
         pop     bp
         ret
ENDP _fos_send


PROC _fos_flowctl
         arg port:WORD,ctl:BYTE
         push    bp
         mov     bp,sp
         mov     dx,[port]
         mov     al,[ctl]
         mov     ah,0Fh
         int     14h
         pop     bp
         ret
ENDP _fos_flowctl

PROC _fos_getchnw
        arg  port:WORD
        push  bp
        mov   bp,sp
        mov   dx,[port]
        mov   ax,0c00h
        int   14h
        cmp   ax,0FFFFh
        je    @@exit
        mov   dx,[port]
        mov   ax,200h
        int   14h
        xor   ah,ah
@@exit: pop   bp
        ret
ENDP _fos_getchnw

PROC _fos_getch
        arg  port:WORD
        push  bp
        mov   bp,sp
        mov   dx,[port]
        mov   ax,200h
        int   14h
        xor   ah,ah
        pop   bp
        ret
ENDP _fos_getch

PROC _fos_sendblock
         arg port:WORD,block:PTR,numbytes:WORD
         push    bp
         mov     bp,sp
         push    di
         mov     dx,[port]
         mov     cx,[numbytes]
if @DataSize
         les     di,[block]
else
         push    ds
         pop     es
         mov     di,[block]
endif
         mov     ah,19h
         int     14h
         pop     di
         pop     bp
         ret
ENDP _fos_sendblock

PROC _fos_readblock
         arg port:WORD,block:PTR,numbytes:WORD
         push    bp
         mov     bp,sp
         push    di
         mov     dx,[port]
         mov     cx,[numbytes]
if @DataSize
         les     di,[block]
else
         push    ds
         pop     es
         mov     di,[block]
endif
         mov     ah,18h
         int     14h
         pop     di
         pop     bp
         ret
ENDP _fos_readblock

PROC _fos_break
        arg  port:WORD, mode:BYTE
        push  bp
        mov   bp,sp
        mov   dx,[port]
        mov   ah,1ah
        cmp   [mode],0
        je    @@stop
        mov   al,1
        jmp   @@exit
@@stop: xor   al,al
@@exit: int   14h
        pop   bp
        ret
ENDP _fos_break


ENDS

END

