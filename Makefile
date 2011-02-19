PACKAGE = libgit2
NODEJS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)

BASE = .
LIBPATH = /usr/local/lib:$(BASE)/vendor

all:
	node-waf configure build

unittest:
	$(NODEJS) $(BASE)/test/index.js test

clean:
	rm -rf ./build
	rm -rf ./vendor/libgit2/build
