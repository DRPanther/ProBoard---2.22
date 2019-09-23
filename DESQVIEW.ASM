
% MODEL MEM_MOD

public _DV_timeslice
public _DV_start_critical
public _DV_end_critical

extrn  _desqview : WORD

 CODESEG

PROC API near
     push ax
     cmp  [_desqview],0
     je no
     mov  ax,101Ah
     int  15h
     mov  ax,bx
     int  15h
     mov  ax,1025h
     int  15h
     jmp short end_api
no:  
     mov  ax,1680h
     int  2fh
end_api:
     pop  ax
     ret
ENDP API

PROC _DV_timeslice
     mov  bx,1000h
     call API
     ret
ENDP _DV_timeslice

PROC _DV_start_critical
     mov  bx,101Bh
     call API
     ret
ENDP _DV_start_critical

PROC _DV_end_critical
     mov  bx,101Bh
     call API
     ret
ENDP _DV_end_critical

 END

