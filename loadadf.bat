@echo off
rem
rem Loading ADF for COM1
rem
rem    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄCOM-port number, COM1-COM127.
rem    ³   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄAddress in hex of COM-port, 0-FFFF.
rem    ³   ³  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄIRQ number of COM-port, 0-15.
rem    ³   ³  ³      ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄLocked baud-rate, 1-115200.
rem    ³   ³  ³      ³     ÚÄÄÄÄÄÄÄÄÄÄSize of receive buffer, 256-16384.
rem    ³   ³  ³      ³     ³     ÚÄÄÄÄSize of transmit buffer, 256-16384.
rem    ³   ³  ³      ³     ³     ³  ÚÄReceiver FIFO trigger level, 1, 4, 8, 14.
rem    ³   ³  ³      ³     ³     ³  ³
adf COM1 3F8  4  57600  4096  4096  8
rem                        ³        ³
rem    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ        ³
rem    ³               ÚÄÄÄÄÄÄÄÄÄÄÄÄÙ   Advanced options:
rem    ³               ³
rem 4096,3584,3072 16650,16,8    3  11
rem         ³    ³        ³ ³    ³   ³
rem         ³    ³        ³ ³    ³   ÀÄ Modem Control Register.
rem         ³    ³        ³ ³    ÀÄÄÄÄÄ Line Control Register.
rem         ³    ³        ³ ÀÄÄÄÄÄÄÄÄÄÄ 16650 Transmit FIFO trigger level:
rem         ³    ³        ³             8, 16, 24 or 30.
rem         ³    ³        ÀÄÄÄÄÄÄÄÄÄÄÄÄ 16650 Receive FIFO trigger level:
rem         ³    ³                      8, 16, 24 or 28.
rem         ³    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Flow control continue or restart.
rem         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Flow control hold or stop.
rem
