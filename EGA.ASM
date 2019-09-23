% MODEL MEM_MOD

public _set43,_set25,_setvideomode,_getvideomode

 CODESEG

PROC _set43
     push  bp
     push  si
     push  di

     mov   ax,1112h
     xor   bx,bx
     int   10h

     pop   di
     pop   si
     pop   bp
     ret
ENDP _set43

PROC _set25
     push  bp
     push  si
     push  di

     mov   ax,3
     int   10h

     pop   di
     pop   si
     pop   bp
     ret
ENDP _set25

PROC _getvideomode
     push  bp
     push  si
     push  di

     mov   ax,0f00h
     int   10h
     xor   ah,ah

     pop   di
     pop   si
     pop   bp
     ret
ENDP _getvideomode

PROC _setvideomode
     ARG mode:WORD
     push  bp
     mov   bp,sp
     push  si
     push  di

     mov   ax,[mode]
     int   10h

     pop   di
     pop   si
     pop   bp
     ret
ENDP _setvideomode

END

