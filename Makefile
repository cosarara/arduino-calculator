
CC = gcc

cli: src/eval.c src/float.c src/math_extra.c cli.c
	$(CC) -Wall -g -c src/eval.c
	$(CC) -Wall -g -c src/float.c
	$(CC) -Wall -g -c src/math_extra.c
	$(CC) -Wall -g -Isrc -lm eval.o float.o math_extra.o cli.c -o cli

#CC = avr-gcc
#CXX = avr-g++
#
#CFLAGS = -g -0s
#%.o: %.c
#	$(CXX) -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=106 -I/usr/share/arduino/hardware/arduino/cores/arduino -I/usr/share/arduino/hardware/arduino/variants/standard /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp -o /tmp/build1262642884276253155.tmp/HardwareSerial.cpp.o
#arduino: eval.o float.o


