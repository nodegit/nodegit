#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

from waflib import Task
from waflib.TaskGen import extension
class bison(Task.Task):
	color='BLUE'
	run_str='${BISON} ${BISONFLAGS} ${SRC[0].abspath()} -o ${TGT[0].name}'
	ext_out=['.h']
def big_bison(self,node):
	has_h='-d'in self.env['BISONFLAGS']
	outs=[]
	if node.name.endswith('.yc'):
		outs.append(node.change_ext('.tab.cc'))
		if has_h:
			outs.append(node.change_ext('.tab.hh'))
	else:
		outs.append(node.change_ext('.tab.c'))
		if has_h:
			outs.append(node.change_ext('.tab.h'))
	tsk=self.create_task('bison',node,outs)
	tsk.cwd=node.parent.get_bld().abspath()
	self.source.append(outs[0])
def configure(conf):
	conf.find_program('bison',var='BISON')
	conf.env.BISONFLAGS=['-d']

extension('.y','.yc','.yy')(big_bison)