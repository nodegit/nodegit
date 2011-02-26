PACKAGE = libgit2
NODEJS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)
NODEBLD = node-waf

BASE = .
LIBPATH = /usr/local/lib:$(BASE)/vendor

all: buildbindings lint

buildbindings:
	$(NODEBLD) build

install:
	$(NODEBLD) install

clean:
	rm -rf ./build
	rm -rf ./vendor/libgit2/build

unittest:
	$(NODEJS) $(BASE)/test/index.js test

lint:
	node ./util/hint-check.js
