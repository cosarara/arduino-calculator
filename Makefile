
cli: src/eval.c src/float.c cli.c
	gcc -Wall -g -c src/eval.c
	gcc -Wall -g -c src/float.c
	gcc -Wall -g -Isrc -lm eval.o float.o cli.c -o cli

clang: src/eval.c src/float.c cli.c
	clang -Wall -g -c src/eval.c
	clang -Wall -g -c src/float.c
	clang -Wall -g -Isrc -lm eval.o float.o cli.c -o cli

#CC = avr-gcc
#CXX = avr-g++
#
#CFLAGS = -g -0s
#%.o: %.c
#	$(CXX) -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=106 -I/usr/share/arduino/hardware/arduino/cores/arduino -I/usr/share/arduino/hardware/arduino/variants/standard /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp -o /tmp/build1262642884276253155.tmp/HardwareSerial.cpp.o
#arduino: eval.o float.o


