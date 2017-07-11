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
