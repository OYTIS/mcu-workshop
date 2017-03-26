TARGET = blink.elf

SOURCES = src/main.c \
	  src/startup_gcc.c

OBJECTS=$(SOURCES:src/%.c=obj/%.o)
LISTINGS=$(SOURCES:src/%.c=lst/%.lst)

CFLAGS = -Iinc -mcpu=cortex-m4 -mthumb -O0 -g
LDFLAGS = -T ./blink.ld

all: ${TARGET}

${TARGET}: ${OBJECTS}
	arm-eabi-ld -o $@ $^ ${LDFLAGS}

obj/%.o: src/%.c
	arm-eabi-gcc -c -o $@ $< ${CFLAGS}

list: ${LISTINGS}

lst/%.lst: src/%.c
	arm-eabi-gcc -c -Wa,-a,-ad -Iinc -mcpu=cortex-m4 -mthumb -Os $< > $@

clean:
	rm -f ${OBJECTS} ${TARGET} ${LISTINGS}
