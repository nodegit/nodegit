#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

from waflib import TaskGen,Utils
TaskGen.declare_chain(name='luac',rule='${LUAC} -s -o ${TGT} ${SRC}',ext_in='.lua',ext_out='.luac',reentrant=False)
def init_lua(self):
	self.default_chmod=Utils.O755
def configure(conf):
	conf.find_program('luac',var='LUAC')

TaskGen.feature('lua')(init_lua)