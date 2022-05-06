# makfile configuration
COMMON_OBJECTS  = main.o
CPU             = msp430g2553
CFLAGS          = -mmcu=${CPU} -I../h
LDFLAGS		= -L/opt/ti/msp430_gcc/include
#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-as

all:
	(cd timerLib && make install)
	make project.elf


#additional rules for files
project.elf:: ${COMMON_OBJECTS} buzzer.o   lib/libTimer.a
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^



load: project.elf
	msp430loader.sh $^

clean:
	(cd timerLib && make clean)
	rm -rf h lib
	rm -f *.o *.elf
