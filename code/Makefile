TARGET = blink.elf

SOURCES = src/main.c \
	  src/startup_gcc.c

OBJECTS=$(SOURCES:src/%.c=obj/%.o)

CFLAGS = -Iinc -mcpu=cortex-m4 -mthumb -O0 -g
LDFLAGS = -T ./blinky.ld

all: ${TARGET}

${TARGET}: ${OBJECTS}
	arm-eabi-ld -o $@ $^ ${LDFLAGS}

obj/%.o: src/%.c
	arm-eabi-gcc -c -o $@ $< ${CFLAGS}

clean:
	rm -f ${OBJECTS} ${TARGET}
