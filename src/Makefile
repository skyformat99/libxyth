OUTPUT=../libbergamota-$(VERSION_MAJOR).$(VERSION_MINOR).a
TARGETS=context.o \
        template-common.o \
        template-xyt.o \
        template-raw-image.o \
        common.o \
        identify.o \
        add_remove.o

CPPFLAGS=-I../include -I. -I../include/mindtct -DBGM_VERSION_MAJOR=$(VERSION_MAJOR) -DBGM_VERSION_MINOR=$(VERSION_MINOR)
CFLAGS=-fPIC -std=c99 -Wextra

all: $(TARGETS)
	strip --strip-unneeded $(TARGETS)
	$(AR) -rc $(OUTPUT) $(TARGETS) -T ../mindtct/libmindtct.a

debug: CFLAGS+=-g -O0
debug: CPPFLAGS+=-D_BGM_DEBUG
debug: all

clean:
	-rm $(TARGETS) $(OUTPUT)
