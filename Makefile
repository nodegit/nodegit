PACKAGE = libgit2
NODEJS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)

unittest:
	$(NODEJS) ./test/index.js test
