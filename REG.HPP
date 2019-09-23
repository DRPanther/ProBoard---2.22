#ifndef __REGISTRATION_HEADER_HPP
#define __REGISTRATION_HEADER_HPP


char *serialFormatted( char *sBuf );

char *software_serial_number( void );
char *registered_user_name(   void );
char *get_system_code(        void );
word  get_node_count(         void );
word  is_version_registered(  void );

void CHECK_REG( void );


#endif    // __REGISTRATION_HEADER_HPP
