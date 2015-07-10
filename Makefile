.PHONY:src tests check
VER_MAJOR=1
VER_MINOR=0

all: tests

src:
	$(MAKE) -C src $(DEBUG) VERSION_MAJOR=$(VER_MAJOR) VERSION_MINOR=$(VER_MINOR)

tests: src
	$(MAKE) -C tests $(DEBUG)

debug: DEBUG=debug
debug: all

check: tests
	@echo "================================================================================"
	tests/check
	@echo "================================================================================"

clean:
	$(MAKE) -C src clean VERSION_MAJOR=$(VER_MAJOR) VERSION_MINOR=$(VER_MINOR)
	$(MAKE) -C tests clean
