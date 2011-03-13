#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys,re
from waflib import TaskGen,Task
from waflib.TaskGen import extension
def rc_file(self,node):
	obj_ext='.rc.o'
	if self.env['WINRC_TGT_F']=='/fo':
		obj_ext='.res'
	rctask=self.create_task('winrc',node,node.change_ext(obj_ext))
	self.compiled_tasks.append(rctask)
class winrc(Task.Task):
	run_str='${WINRC} ${WINRCFLAGS} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${WINRC_TGT_F} ${TGT} ${WINRC_SRC_F} ${SRC}'
	color='BLUE'
def configure(conf):
	v=conf.env
	v['WINRC_TGT_F']='-o'
	v['WINRC_SRC_F']='-i'
	if not conf.env.WINRC:
		if v.CC_NAME=='msvc':
			winrc=conf.find_program('RC',var='WINRC',path_list=v['PATH'])
			v['WINRC_TGT_F']='/fo'
			v['WINRC_SRC_F']=''
		else:
			winrc=conf.find_program('windres',var='WINRC',path_list=v['PATH'])
	if not conf.env.WINRC:
		conf.fatal('winrc was not found!')
	v['WINRCFLAGS']=[]

extension('.rc')(rc_file)