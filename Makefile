include config.mk

SRC = field.c life.c preprocess_id.c rules.c
OBJ = ${SRC:.c=.o}

all: options life

options:
	@echo life build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

life: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f life ${OBJ}

.PHONY: all options clean
