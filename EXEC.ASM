;-------------------------------------------------------------------------
;  Exec function with swapping to disk.    (c) 1990,1991 Philippe Leybaert
;-------------------------------------------------------------------------

% MODEL MEM_MOD

LOCALS @@

PUBLIC _swapshell

EXTRN __psp : WORD

EXTRN _shell_windowed  : WORD
EXTRN _shell_updateline: WORD
EXTRN _shell_swap      : WORD
EXTRN _shell_swapname  : PTR
EXTRN _use_ems         : BYTE
EXTRN _tsw_vsize       : WORD
EXTRN _tsw_hsize       : WORD

EXTRN _tsw_videobase   : FAR PTR

  CODESEG _TEXT

numpara     dw (?)             ; number of paragraphs reserved by compiler
savedpara   dw (?)             ; number of paragraphs to keep
swapsize    dw (?)             ; size of swapfile (# paragraphs)
swapseg     dw (?)             ; segment of start of swapregion
main_ss     dw (?)             ; saved for swapshell
main_sp     dw (?)             ; function (compiler stack saved here)
old_ss      dw (?)             ; saved for
old_sp      dw (?)             ; exec function
saved_psp   dw (?)             ; psp of my program
retval      dw (?)             ; return value of process
swapmode    dw (?)             ; swap code/data?
ems_handle  dw (?)             ; EMS swapspace handle
num_cols    dw (?)             ; # screen columns
num_lines   dw (?)             ; # screen lines
use_ems     db (?)             ; Use EMS?

data_seg    dw (?)

prgname     db 128 DUP (0)
swapname    db 13 DUP (0)
old_dir     db '\'
            db 69 DUP (0)
old_disk    dw 0

displayline dw 132 dup (?)

  EVEN

cur_x       dw 0
cur_y       dw 0

LABEL old21 DWORD
old21_off   dw 0
old21_seg   dw 0

LABEL old1c DWORD
old1c_off   dw 0
old1c_seg   dw 0

LABEL videoptr DWORD
video_off   dw 0
video_seg   dw 0

            dw 99 DUP (?)        ; new stack (100 words)
newstackptr dw 0

parablock   dw 0
            dw offset comlen
            dw @Code
            dd 0FFFFFFFFh
            dd 0FFFFFFFFh
comlen      db 0
comline     db 127 dup (0)

PROC exec NEAR

     push   bx
     push   cx
     push   dx
     push   di
     push   si
     push   bp
     push   ds
     push   es

     mov    [cs:old_ss],ss       ; save stack
     mov    [cs:old_sp],sp       ;   (exec function destroys ss an sp)

     push   cs                   ; es <- cs
     pop    es

     mov    dx,offset prgname    ; get program name
     mov    bx,offset parablock  ; get parameter block
     mov    ax,4B00h             ;
     int    21h                  ; call DOS EXEC function

     cli
     mov    ss,[cs:old_ss]       ; restore stack pointer
     mov    sp,[cs:old_sp]
     sti

     mov    ah,4Dh
     int    21h
     xor    ah,ah
     mov    [cs:retval],ax

     pop    es
     pop    ds
     pop    bp
     pop    si
     pop    di
     pop    dx
     pop    cx
     pop    bx

     ret
ENDP exec

PROC shrink_exec NEAR  ; ES: segment , BX: # para's to allocate
     assume  ds:@Code

     cmp    [swapmode],0
     je     @@noshrink

     mov    ax,4a00h                  ; shrink memory size to minimum
     int    21h                       ; (only this module remains)

@@noshrink:
     call   near exec                 ; shell

     mov    ah,0Eh                    ;
     mov    dl,[byte old_disk]        ; Restore current disk
     int    21h                       ;

     mov    ah,3Bh                    ;
     mov    dx,offset old_dir         ; Restore current directory
     int    21h                       ;

     cmp    [swapmode],0
     je     @@noexpand

     mov    ax,[saved_psp]
     mov    es,ax
     mov    bx,[numpara]
     mov    ax,4A00h
     int    21h                       ; give back memory we stole

     push   ds                       ; save data segment

     mov    cx,[swapsize]
     mov    ax,[swapseg]
     mov    ds,ax

     call   read_para

     pop    ds                       ; restore data segment

@@noexpand:
     ret
ENDP shrink_exec

PROC read_para NEAR ; read cx paragraphs to ds:0
     push   ds
     push   ax
     push   bx
     push   cx
     push   dx
     push   si
     push   di
     push   es

     cmp    [cs:use_ems],0
     je     @@use_file

     mov    ah,40h
     int    67h
     or     ah,ah
     jnz    @@use_file

     mov    dx,[cs:ems_handle]

     mov    ah,41h
     int    67h

     push   ds
     pop    es
     mov    ds,bx                  ; es = page frame
     xor    bx,bx                  ; bx = logical page index

@@ems_loop:
     cmp    cx,0400h
     jb     @@ems_remainder

     push   cx

     mov    ax,4400h
     int    67h                  ; map page bx to phys.page 0

     mov    cx,02000h
     cld
     xor    si,si
     xor    di,di
     rep movsw                   ; transfer data to page frame
     inc    bx

     pop    cx
     sub    cx,0400h
     mov    ax,es
     add    ax,0400h
     mov    es,ax
     jmp    short @@ems_loop

@@ems_remainder:
     mov    ax,4400h
     int    67h                  ; map page bx to phys.page 0

     shl    cx,1                 ; Convert # paragraphs -> #words
     shl    cx,1
     shl    cx,1

     cld
     xor    si,si
     xor    di,di
     rep movsw                   ; transfer data to page frame

     mov    ah,45h
     int    67h                  ; Free EMS


     jmp short @@quit

@@use_file:
     push   ds

     mov    ax,cs
     mov    ds,ax
     assume ds:@Code

     mov    ax,3D00h
     mov    dx,offset swapname
     int    21h                      ; open swap-file
     mov    bx,ax

     pop    ds

@@loopke:
     cmp    cx,0F00h
     jb     @@remainder

     push   cx

     mov    cx,0F000h
     xor    dx,dx
     mov    ax,3F00h
     int    21h

     pop    cx
     sub    cx,0F00h
     mov    ax,ds
     add    ax,0F00h
     mov    ds,ax
     jmp    short @@loopke

@@remainder:
     shl    cx,1
     shl    cx,1
     shl    cx,1
     shl    cx,1
     xor    dx,dx
     mov    ax,3F00h
     int    21h

     mov    ax,3E00h
     int    21h                      ; close file

     mov    ax,cs
     mov    ds,ax
     mov    dx,offset swapname
     mov    ax,4100h
     int    21h

@@quit:
     pop    es
     pop    di
     pop    si
     pop    dx
     pop    cx
     pop    bx
     pop    ax
     pop    ds
     ret
ENDP read_para

PROC disp NEAR
     push   cx
     push   si
     push   di
     push   bp

     cmp    al,0ah
     jne    no_lf

     mov    cx,[cs:num_lines]
     dec    cx

     inc    [cs:cur_y]
     cmp    [cs:cur_y],cx      ; At bottom of screen?
     jne    upd_cursor
     dec    [cs:cur_y]         ; Yep, scroll up
     jmp short scroll_up

no_lf:
     cmp    al,0dh
     jne    no_cr

     mov    [cs:cur_x],0
     jmp short upd_cursor

no_cr:
     cmp    al,8
     jne    no_bs

     cmp    [cs:cur_x],0
     je     @@ok
     dec    [cs:cur_x]
     jmp short upd_cursor

no_bs:
     mov    ah,0eh
     xor    bx,bx
     int    10h

     mov    cx,[cs:num_cols]

     inc    [cs:cur_x]
     cmp    [cs:cur_x],cx       ; At right of window?
     jne    @@ok
     mov    [cs:cur_x],0        ; Yep, linefeed
     inc    [cs:cur_y]

     mov    cx,[cs:num_lines]
     dec    cx

     cmp    [cs:cur_y],cx       ; At bottom of screen
     jne    upd_cursor
     dec    [cs:cur_y]

scroll_up:
     mov    ax,0601h
     mov    cx,0000h
     mov    dl,4Fh
     mov    dh,[byte cs:num_lines]
     dec    dh
     dec    dh
     mov    bh,7
     int    10h

upd_cursor:
     mov    ah,2
     mov    bh,0
     mov    dh,[byte cs:cur_y]
     mov    dl,[byte cs:cur_x]
     int    10h
@@ok:
     pop    bp
     pop    di
     pop    si
     pop    cx
     ret
ENDP disp


PROC new21
     cmp    ah,2
     je     catch
     cmp    ah,6
     jne    test_9
     cmp    dl,0ffh
     jne    catch
test_9:
     cmp    ah,9
     je     catch
     cmp    ah,40h
     jne    nocatch
     cmp    bx,1
     je     catch
     cmp    bx,2
     je     catch

nocatch:
     jmp    [cs:old21]

catch:
     push   ax
     push   bx
     push   cx
     push   dx
     push   si
     push   di
     push   bp
     push   ds
     push   es

     cmp    ah,2
     jne    no_2

     mov    al,dl
     call   disp
     jmp short done_catch

no_2:
     cmp    ah,6
     jne    no_6
     mov    al,dl
     call   disp
     jmp short done_catch

no_6:
     cmp    ah,9
     jne    no_9

     mov    si,dx

@@l: mov    al,[ds:si]
     cmp    al,'$'
     je     done_catch
     call   disp
     inc    si
     jmp short @@l

no_9:
     mov   si,dx

@@l2:
     or    cx,cx
     jz    ret_40

     mov   al,[ds:si]
     call  disp
     inc   si
     dec   cx
     jmp short @@l2

ret_40:
     pop   es
     pop   ds
     pop   bp
     pop   di
     pop   si
     pop   dx
     pop   cx
     pop   bx
     pop   ax
     mov   ax,cx
     clc
     iret

done_catch:
     pop   es
     pop   ds
     pop   bp
     pop   di
     pop   si
     pop   dx
     pop   cx
     pop   bx
     pop   ax
     iret
ENDP new21

timer_val dw 0

PROC new1c
     push  ax
     inc   [cs:timer_val]
     mov   ax,[cs:timer_val]
     and   ax,7
     jnz @@nodisp

     push  cx
     push  si
     push  di
     push  bp
     push  ds
     push  es

     push  cs
     pop   ds
     assume ds:@Code

     les   di,[videoptr]
     mov   si,offset displayline
     cld
     mov   cx,80
     rep movsw

     pop   es
     pop   ds
     pop   bp
     pop   di
     pop   si
     pop   cx

@@nodisp:
     pop   ax

     jmp   [cs:old1c]
ENDP new1c

startswap:                           ; all data/code past this point can be
                                     ; swapped (who cares!)

PROC _swapshell
     ARG    command:PTR
     push   bp
     mov    bp,sp
     push   ds
     push   si
     push   di

     mov    ax,ds
     mov    [cs:data_seg],ax

     mov    ax,[_tsw_vsize]
     mov    [cs:num_lines],ax

     mov    ax,[_tsw_hsize]
     mov    [cs:num_cols],ax

     mov    ax,[_shell_swap]
     mov    [cs:swapmode],ax

     mov    al,[_use_ems]
     mov    [cs:use_ems],al

     call   near install_isr

     mov    ax,[__psp]                ; store psp for future use
     mov    [cs:saved_psp],ax

     mov    si,offset _shell_swapname
     push   cs
     pop    es
     mov    di,offset swapname
     mov    cx,13
     cld
     rep    movsb

     lds    si,[command]
     mov    di,offset prgname
@@lp:
     mov    al,[si]
     cmp    al,' '
     je     @@qt
     or     al,al
     je     @@qt
     mov    [cs:di],al
     inc    si
     inc    di
     jmp short @@lp
@@qt:
     mov    [byte cs:di],0
     mov    al,[si]
     inc    si
     cmp    al,' '
     je     @@qt

     dec    si

     mov    cx,0
     mov    di,offset comline
@@lp2:
     mov    al,[si]
     or     al,al
     je     @@qt2
     mov    [cs:di],al
     inc    si
     inc    di
     inc    cx
     jmp short @@lp2
@@qt2:
     mov    [byte cs:di],13
     mov    di,offset comlen
     mov    [cs:di],cl

     mov    ax,@Code                  ; ds = code segment
     mov    ds,ax
     assume ds:@Code                  ; tell Tasm where ds points to

     mov    [main_ss],ss              ; save compiler stack
     mov    [main_sp],sp

     mov    ax,cs
     cli
     mov    ss,ax                     ; set my own stack
     mov    sp,offset newstackptr
     sti

     mov    ax,[saved_psp]            ; get psp address
     dec    ax                        ; get address of MCB
     mov    es,ax
     mov    ax,[es:3]                 ; get number of paragraphs reserved
     mov    [numpara],ax              ; and store it

     mov    bx,offset startswap       ; we can swap from this point
     mov    cl,4
     shr    bx,cl                     ; convert to # paragraphs
     add    bx,@Code
     mov    ax,[saved_psp]            ; how many paragraphs
     sub    bx,ax                     ; must remain?
     add    bx,2                      ; 2 extra paragraphs (safe!)
     mov    es,ax                     ; es <- psp

     mov    [savedpara],bx            ; store it
     push   bx                        ; push number of paragraphs
     push   es                        ; push psp address

     mov    cx,[numpara]              ; get number of paragraphs reserved
     sub    cx,bx                     ; calculate # paragraphs to save

     mov    [swapsize],cx             ; store for later use
     mov    ax,es                     ; get psp
     add    ax,bx                     ; calculate address for swap-start
     mov    [swapseg],ax              ; store for later use

     push   ds                        ; save data segment

     cmp    [swapmode],0
     je     @@nowrite

     mov    ds,ax                     ; set ds to region to save
     call   near write_para           ; save file

@@nowrite:
     pop    ds

     mov    ah,19h                    ;
     int    21h                       ; Save current disk
     mov    [byte old_disk],al        ;

     mov    si,offset old_dir+1       ;
     mov    dl,0                      ; Save current directory
     mov    ah,47h                    ;
     int    21h                       ;

     pop    es
     pop    bx

     call   near shrink_exec

     cli
     mov    ss,[main_ss]             ; restore compiler stack
     mov    sp,[main_sp]
     sti

     call   near remove_isr

     pop    di
     pop    si
     pop    ds
     pop    bp
     mov    ax,[cs:retval]              ; set return value
     ret
ENDP _swapshell

PROC write_para NEAR ; write cx paragraphs from ds:0
     push   ds
     push   ax
     push   bx
     push   cx
     push   dx
     push   si
     push   di
     push   es

     cmp    [cs:use_ems],0
     je     @@use_file

     mov    ah,40h
     int    67h
     or     ah,ah
     jnz    @@use_file

     push   cx
     mov    bx,cx
     mov    cl,10
     shr    bx,cl
     inc    bx
     pop    cx

     mov    ah,43h
     int    67h
     or     ah,ah
     jnz    @@use_file
     mov    [cs:ems_handle],dx

     mov    ah,41h
     int    67h
     mov    es,bx                  ; es = page frame
     xor    bx,bx                  ; bx = logical page index

@@ems_loop:
     cmp    cx,0400h
     jb     @@ems_remainder

     push   cx

     mov    ax,4400h
     int    67h                  ; map page bx to phys.page 0

     mov    cx,02000h
     cld
     xor    si,si
     xor    di,di
     rep movsw                   ; transfer data to page frame
     inc    bx

     pop    cx
     sub    cx,0400h
     mov    ax,ds
     add    ax,0400h
     mov    ds,ax
     jmp    short @@ems_loop

@@ems_remainder:
     mov    ax,4400h
     int    67h                  ; map page bx to phys.page 0

     shl    cx,1                 ; Convert # paragraphs -> #words
     shl    cx,1
     shl    cx,1

     cld
     xor    si,si
     xor    di,di
     rep movsw                   ; transfer data to page frame

     jmp short @@quit


@@use_file:
     mov    [cs:use_ems],0

     push   ds
     push   cx

     push   cs
     pop    ds

     mov    dx,offset swapname
     mov    ax,3c00h                  ; Create
     xor    cx,cx                     ;     swap-file
     int    21h

     pop    cx
     pop    ds
     mov    bx,ax

@@loopke:
     cmp    cx,0F00h
     jb     @@remainder

     push   cx

     mov    cx,0F000h
     xor    dx,dx
     mov    ax,4000h
     int    21h

     pop    cx
     sub    cx,0F00h
     mov    ax,ds
     add    ax,0F00h
     mov    ds,ax
     jmp    short @@loopke

@@remainder:
     shl    cx,1
     shl    cx,1
     shl    cx,1
     shl    cx,1
     xor    dx,dx
     mov    ax,4000h
     int    21h

     mov    ax,3E00h
     int    21h

@@quit:
     pop    es
     pop    di
     pop    si
     pop    dx
     pop    cx
     pop    bx
     pop    ax
     pop    ds
     ret
ENDP write_para


PROC install_isr NEAR
     push   ax
     push   bx
     push   cx
     push   dx
     push   ds
     push   es
     push   si
     push   di
     push   bp

     mov    ax,[cs:data_seg]
     mov    ds,ax

     cmp    [_shell_windowed],0
     je     @@nowindow

     mov    ax,3521h
     int    21h
     mov    ax,es
     mov    [cs:old21_seg],ax
     mov    [cs:old21_off],bx
     mov    ax,2521h
     mov    dx,offset new21
     push   ds
     push   cs
     pop    ds
     int    21h
     pop    ds

     mov    [cs:cur_x],0
     mov    [cs:cur_y],0

     mov    ax,0600h
     mov    cx,0
     mov    dl,4Fh
     mov    dh,[byte cs:num_lines]
     dec    dh
     dec    dh
     mov    bh,7
     int    10h
     mov    dx,0
     mov    bh,0
     mov    ah,2
     int    10h

@@nowindow:
     cmp    [_shell_updateline],0
     je     @@notupdated

     mov    ax,351Ch
     int    21h
     mov    ax,es
     mov    [cs:old1c_seg],ax
     mov    [cs:old1c_off],bx
     mov    ax,251Ch
     mov    dx,offset new1c
     push   ds
     push   cs
     pop    ds
     int    21h
     pop    ds

     push   ds
     mov    ax,[_shell_updateline]
     dec    ax
     mov    cx,[cs:num_cols]
     shl    cx,1
     mul    cx

     lds    si,[_tsw_videobase]
     add    si,ax

     mov    ax,ds
     mov    [cs:video_seg],ax
     mov    [cs:video_off],si

     mov    ax,[cs:num_lines]
     dec    ax
     mov    cx,[cs:num_cols]
     shl    cx,1
     mul    cx
     mov    si,ax                 ; SI = (num_lines-1) * num_cols * 2

     push   cs
     pop    es
     mov    di,offset displayline
     mov    cx,[cs:num_cols]
     cld
     rep movsw

     pop     ds

@@notupdated:
     cmp    [_shell_windowed],0
     jne    @@done

     mov    ax,0600h
     mov    cx,0
     mov    dl,[byte cs:num_cols]
     dec    dl
     mov    dh,[byte cs:num_lines]
     dec    dh
     mov    bh,7
     int    10h
     mov    dx,0
     mov    bh,0
     mov    ah,2
     int    10h

@@done:
     pop    bp
     pop    di
     pop    si
     pop    es
     pop    ds
     pop    dx
     pop    cx
     pop    bx
     pop    ax
     ret
ENDP install_isr

PROC remove_isr NEAR
     push   ds
     mov    ax,[cs:data_seg]
     mov    ds,ax

     cmp    [_shell_windowed],0
     je     @@nowindowrestore

     push   ds
     mov    ax,2521h
     lds    dx,[cs:old21]
     int    21h
     pop    ds

@@nowindowrestore:
     cmp    [_shell_updateline],0
     je     @@noupdaterestore

     mov    ax,251Ch
     lds    dx,[cs:old1c]
     int    21h

@@noupdaterestore:
     pop    ds
     ret
ENDP remove_isr

ENDS

END

