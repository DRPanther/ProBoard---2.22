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
     io << "³\n"
     //io << "\1(c) 1990-1996 Philippe Leybaert \7³\n"
               " ³                                                                        ³\n";
     io << "\7 ³ Copyright (c) 1990-1999  TeleGrafix Communications, Inc.               ³\n";
     io << "\7 ³ All Rights Reserved                                                    ³\n";
     io << "\7 ³ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ³\n"
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
          io << "\7 ³ \7Registered to:\6 " << form("%-36s",registered_user_name() ) << "\7                    ³\n"
                "\7 ³                \6"   << form("%-36s",cfg.bbsname ) << "\7                    ³\n"
                "\7 ³ \7Serial number:\6 " << form("%-48.48s", buf ) << "\7        ³\n"
                "\7 ³                                                                        ³\n";
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

