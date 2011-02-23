PACKAGE = libgit2
NODEJS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)

BASE = .
LIBPATH = /usr/local/lib:$(BASE)/vendor

all: clean build lint

build:
	node-waf configure build

install:
	node-waf install

clean:
	rm -rf ./build
	rm -rf ./vendor/libgit2/build

unittest:
	$(NODEJS) $(BASE)/test/index.js test

lint:
	node ./util/hint-check.js
