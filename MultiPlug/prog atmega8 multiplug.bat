set avrdudePath=C:\Users\sebastien\Desktop\arduino-1.0.6\hardware/tools/avr/bin/avrdude
set avrdudeConfPath=C:\Users\sebastien\Desktop\arduino-1.0.6\hardware/tools/avr/etc/avrdude.conf
set args=-v -patmega8 -cstk500v1 -P\\.\COM4 -b19200
set lfuse=c4
set hfuse=d9
set hexPath=C:\TEMP/MultiPlug.ino.hex

%avrdudePath% -C%avrdudeConfPath% %args% -U lfuse:w:0x%lfuse%:m -U hfuse:w:0x%hfuse%:m -Uflash:w:%hexPath%:i
pause