#ifndef __TIME_BOMB_HEADER_H
#define __TIME_BOMB_HEADER_H


#define SOFTWARE_EXPIRE

#ifdef SOFTWARE_EXPIRE

  #define EXPIRE_YEAR  2000
  #define EXPIRE_MONTH 9
  #define EXPIRE_DAY   1

  #define PORT_VALUE   10

#else

  #define PORT_VALUE   -1

#endif


#endif  // __TIME_BOMB_HEADER_H
