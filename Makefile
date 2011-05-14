NODE_JS = $(if $(shell test -f /usr/bin/nodejs && echo "true"),nodejs,node)
NODE_BLD = node-waf
NODE_LIB_PATH = ~/.node_libraries

BASE = .
INSTALL_PATH = $(NODE_LIB_PATH)/nodegit

NATURAL_DOCS_PATH = $(BASE)/vendor/naturaldocs/

all: build

update: clean config build uninstall install

config:
	@@$(BASE)/configure

build:
	@@$(NODE_BLD) build

debug:
	@@$(NODE_BLD) debug

install:
	@@mkdir -p $(INSTALL_PATH)
	@@mkdir -p $(INSTALL_PATH)/build/default
	@@mkdir -p $(INSTALL_PATH)/lib

	@@cp -f $(BASE)/build/default/nodegit.node $(INSTALL_PATH)/build/default/nodegit.node
	@@cp -f $(BASE)/lib/* $(INSTALL_PATH)/lib/
	@@cp -f $(BASE)/package.json $(INSTALL_PATH)/

	@@echo "Installed to $(INSTALL_PATH)"

uninstall:
	@@rm -rf $(INSTALL_PATH)
	@@echo "Uninstalled from $(INSTALL_PATH)"

clean:
	@@rm -rf $(BASE)/build/
	@@rm -rf $(BASE)/vendor/libgit2/build/

test:
	@@$(NODE_JS) $(BASE)/test/index.js test

lint:
	@@$(NODE_JS) $(BASE)/util/hint-check.js

doc:
	@@$(NATURAL_DOCS_PATH)/NaturalDocs -i $(BASE)/include -o HTML $(BASE)/doc -p $(BASE)/doc -s Theme

.PHONY: test build doc
