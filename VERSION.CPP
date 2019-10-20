#include <string.h>
#include "proboard.hpp"

void
version_info(char *)
{
     LOG(2,"Version info requested");

     io << "\f\n\7"
     " ������������������������������������������������������������������������Ŀ\n"
     " � ProBoard version " VERSION;

     for(int i=0;i<54-strlen(VERSION);i++) io << ' ';
	 io << "�\n";
	 io << " � \1(c) 2019 John Riley\7                                                    �\n";
	 io << " � \1(c) Originally Developed by Philippe Leybaert\7                          �\n";
               " �                                                                        �\n";
     io << "\7 � Copyright (c) 1990-2019  ProBoard Developmment Ninja Team              �\n";
     io << "\7 � All Rights Reserved                                                    �\n";
     //io << "\7 ������������������������������������������������������������������������ĳ\n"
          "\7 �                                                                        �\n";


     //----------------------------  
     // JDR: REGISTRATION REFERENCE
     //----------------------------  

     if(registered)
     {
          char buf[ 80 ];


          if ( max_node_count == 0 )
          {
               strcpy( buf, 
                       form( buf,
                             "%s (Unlimited nodes)",
                             software_serial_number() ) );
          }
          else
          {
               strcpy( buf,
                       form( "%s (%d nodes maximum)",
                             software_serial_number(),
                             max_node_count ) );
          }
         
     }
     else
     {
          io << " � \6UNREGISTERED EVALUATION VERSION                                        �\n"
                "\7 �                                                                        �\n";
     }


     io << "\7 ������������������������������������������������������������������������ĳ\n";

     io <<   " � JAM(mbp) - Copyright 1993 Joaquim Homrighausen, Andrew Milner,         �\n"
          " �                           Mats Birch, Mats Wallin.                     �\n"
          " �                           ALL RIGHTS RESERVED.                         �\n";
     io << "\7 ��������������������������������������������������������������������������\n\n  "
     << S_PRESS_ENTER_TO_CONTINUE;
}

