#include <string.h>
#include "proboard.hpp"

void
version_info(char *)
{
     LOG(2,"Version info requested");

     io << "\f\n\7"
     " ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n"
     " ³ ProBoard version " VERSION;

     for(int i=0;i<54-strlen(VERSION);i++) io << ' ';
	 io << "³\n";
	 io << " ³ \1(c) 2019 John Riley\7                                                    ³\n";
	 io << " ³ \1(c) Originally Developed by Philippe Leybaert\7                          ³\n";
               " ³                                                                        ³\n";
     io << "\7 ³ Copyright (c) 1990-2019  ProBoard Developmment Ninja Team              ³\n";
     io << "\7 ³ All Rights Reserved                                                    ³\n";
     //io << "\7 ³ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ³\n"
          "\7 ³                                                                        ³\n";


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
          io << " ³ \6UNREGISTERED EVALUATION VERSION                                        ³\n"
                "\7 ³                                                                        ³\n";
     }


     io << "\7 ³ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ³\n";

     io <<   " ³ JAM(mbp) - Copyright 1993 Joaquim Homrighausen, Andrew Milner,         ³\n"
          " ³                           Mats Birch, Mats Wallin.                     ³\n"
          " ³                           ALL RIGHTS RESERVED.                         ³\n";
     io << "\7 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n  "
     << S_PRESS_ENTER_TO_CONTINUE;
}

