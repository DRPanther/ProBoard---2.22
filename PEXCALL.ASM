% MODEL MEM_MOD

CODESEG

;PUBLIC __pex_dummy , __pex_set_pex_ds , __pex_set_pb_ds
PUBLIC _CallPEX

PROC _CallPEX
     ARG p:PTR

     push       bp
     mov        bp,sp

     les        bx,[p]

     mov        ax,[es:bx+2]     ; Get segment of function
     or         ax,ax
     jz         @@no_func

     push       ds
     push       [bp+16]
     push       [bp+14]
     push       [bp+12]
     push       [bp+10]

     mov        ds,[es:bx+4]

     call       [dword ptr es:bx]

     add        sp,8
     pop        ds

@@no_func:
     pop        bp
     ret
ENDP _CallPEX


;PROC __pex_dummy
;     ret
;ENDP __pex_dummy
;
;PROC __pex_set_pex_ds
;     push    bp
;     mov     bp,sp
;     mov     bx,[bp+6]
;     mov     ds,bx
;     pop     bp
;     ret
;ENDP __pex_set_pex_ds
;
;PROC __pex_set_pb_ds
;     push    bp
;     mov     bp,sp
;
;     mov     bx,@Data
;     mov     ds,bx
;     mov     bx,[bp+6]
;     mov     cx,[bp+8]
;     or      bx,bx
;     or      bx,cx
;     jz      @@no_set
;
;     pop     bp
;     ret
;
;@@no_set:
;     xor     ax,ax          ; So PEX_RET returns NOT_HANDLED
;     pop     bp
;     ret
;ENDP __pex_set_pb_ds
;
END
