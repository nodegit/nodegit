PACKAGE = libgit2
NODEJS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)
NODEBLD = node-waf

BASE = .
LIBPATH = /usr/local/lib:$(BASE)/vendor
NODE_LIB_PATH = ~/.node_libraries
INSTALL_PATH = $(NODE_LIB_PATH)/nodegit2

all: buildbindings

buildbindings:
	$(NODEBLD) build

install:
	mkdir -p $(INSTALL_PATH)
	mkdir -p $(INSTALL_PATH)/build/default
	mkdir -p $(INSTALL_PATH)/lib

	cp -f $(BASE)/build/default/nodegit2.node $(INSTALL_PATH)/build/default/nodegit2.node
	cp -f $(BASE)/lib/* $(INSTALL_PATH)/lib/
	cp -f $(BASE)/package.json $(INSTALL_PATH)/

uninstall:
	rm -rf $(INSTALL_PATH)

clean:
	rm -rf $(BASE)/build
	rm -rf $(BASE)/vendor/libgit2/build

unittest:
	$(NODEJS) $(BASE)/test/index.js test

lint:
	$(NODEJS) $(BASE)/util/hint-check.js
