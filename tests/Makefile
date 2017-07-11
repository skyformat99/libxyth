# Copyright 2011-2017 Rodrigo Dias Correa
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

OUTPUT=check
SRC=check.c check_template.c check_context.c\
	check_template_from_xyt.c \
	check_destroy_template.c \
	check_create_context.c \
	check_destroy_context.c \
	check_add_template.c \
	check_remove_template.c \
	check_identify.c

CPPFLAGS=-I../include
CFLAGS=-std=c99 -Wall
LDFLAGS=-lxyth-1.0 -lcheck -lm -lrt -lpthread -L..

all:
	$(CC) -o $(OUTPUT) $(SRC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

debug: CFLAGS+=-g -O0
debug: all

clean:
	-rm $(OUTPUT)
