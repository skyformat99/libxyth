OUTPUT=check
SRC=check.c check_template.c check_context.c\
	check_template_from_xyt.c \
        check_template_from_raw_image.c \
	check_destroy_template.c \
	check_create_context.c \
	check_destroy_context.c \
	check_add_template.c \
	check_remove_template.c \
	check_identify.c

CPPFLAGS=-I../include
CFLAGS=-std=c99 -Wall
LDFLAGS=-lbergamota-1.0 -lcheck -lm -lrt -lpthread -L..

all:
	$(CC) -o $(OUTPUT) $(SRC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

debug: CFLAGS+=-g -O0
debug: all

clean:
	-rm $(OUTPUT)
