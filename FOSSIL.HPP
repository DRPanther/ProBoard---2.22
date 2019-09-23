typedef unsigned FOS_STATUS;

#define FSTAT_BUFEMPTY 0x4000   // output buffer empty
#define FSTAT_NOTFULL  0x2000   // output buffer NOT full
#define FSTAT_OVERRUN  0x0200   // input buffer overrun
#define FSTAT_AVAIL    0x0100   // input available
#define FSTAT_CARRIER  0x0080   // carrier detected

extern "C"
  {
   FOS_STATUS fos_setbps      (int port,byte baudcode); // set baudrate
   FOS_STATUS fos_status      (int port);               // get fossil-status
   void       fos_init        (int port);               // initialize fossil
   void       fos_deinit      (int port);               // deinitialize fossil
   void       fos_setdtr      (int port,char dtr);      // set DTR on(1)/off(0)
   void       fos_purgeoutput (int port);               // purge output buffer
   void       fos_purgeinput  (int port);               // purge input buffer
   void       fos_flowctl     (int port,byte ctl);      // set flow control (CTS/RTS)
   bool       fos_sendnw      (int port,byte ch);       // send char without wait (1=sent,0=not sent)
   FOS_STATUS fos_send        (int port,byte ch);       // send char with wait
   int        fos_getchnw     (int port);               // get char without wait (-1 when none available)
   byte       fos_getch       (int port);               // get char with wait
   byte       fos_break       (int port,bool mode);     // Start/Stop sending break
   word       fos_sendblock   (int port,byte *buf,word size);
   word       fos_readblock   (int port,byte *buf,word size);
  }

#define fos_byteready(port)   (fos_status(port) & FSTAT_AVAIL)
#define fos_outputempty(port) (fos_status(port) & FSTAT_BUFEMPTY)
#define fos_carrier(port)     (fos_status(port) & FSTAT_CARRIER)

#define status_byteready(stat)   ((stat) & FSTAT_AVAIL)
#define status_outputempty(stat) ((stat) & FSTAT_BUFEMPTY)
#define status_carrier(stat)     ((stat) & FSTAT_CARRIER)

/*
static void
fos_sendbreak(int port)
{
 fos_break(port,TRUE);

 unsigned long ticks = clockticks();
 while(clockdiff(ticks)<2) {}

 fos_break(port,FALSE);
}
*/
