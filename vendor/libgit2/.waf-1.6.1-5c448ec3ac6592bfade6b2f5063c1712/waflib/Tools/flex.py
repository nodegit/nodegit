#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import waflib.TaskGen
def decide_ext(self,node):
	if'cxx'in self.features:
		return['.lex.cc']
	return['.lex.c']
waflib.TaskGen.declare_chain(name='flex',rule='${FLEX} -o${TGT} ${FLEXFLAGS} ${SRC}',ext_in='.l',decider=decide_ext)
def configure(conf):
	conf.find_program('flex',var='FLEX')
