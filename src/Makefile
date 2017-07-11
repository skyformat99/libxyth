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

OUTPUT=../libxyth-$(VERSION_MAJOR).$(VERSION_MINOR).a
TARGETS=context.o \
        template-common.o \
        template-xyt.o \
        template-raw-image.o \
        common.o \
        identify.o \
        add_remove.o

CPPFLAGS=-I../include -I. -I../include/mindtct \
            -DXYTH_VERSION_MAJOR=$(VERSION_MAJOR)\
            -DXYTH_VERSION_MINOR=$(VERSION_MINOR)
CFLAGS=-fPIC -std=c99 -Wextra

all: $(TARGETS)
	strip --strip-unneeded $(TARGETS)
	$(AR) -rc $(OUTPUT) $(TARGETS) -T ../mindtct/libmindtct.a

debug: CFLAGS+=-g -O0
debug: CPPFLAGS+=-D_XYTH_DEBUG
debug: all

clean:
	-rm $(TARGETS) $(OUTPUT)
