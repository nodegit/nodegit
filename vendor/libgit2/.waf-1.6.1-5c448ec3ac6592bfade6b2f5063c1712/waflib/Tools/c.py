#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

from waflib import TaskGen,Task,Utils
from waflib.Tools import c_preproc
from waflib.Tools.ccroot import link_task,stlink_task
def c_hook(self,node):
	return self.create_compiled_task('c',node)
class c(Task.Task):
	color='GREEN'
	run_str='${CC} ${CFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC} ${CC_TGT_F}${TGT}'
	vars=['CCDEPS']
	ext_in=['.h']
	scan=c_preproc.scan
Task.classes['cc']=cc=c
class cprogram(link_task):
	run_str='${LINK_CC} ${CCLNK_SRC_F}${SRC} ${CCLNK_TGT_F}${TGT[0].abspath()} ${RPATH_ST:RPATH} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${tsk.frameworks()} ${STLIB_MARKER} ${STLIBPATH_ST:STLIBPATH} ${STLIB_ST:STLIB} ${SHLIB_MARKER} ${LIBPATH_ST:LIBPATH} ${LIB_ST:LIB} ${LINKFLAGS}'
	ext_out=['.bin']
	inst_to='${BINDIR}'
	chmod=Utils.O755
class cshlib(cprogram):
	inst_to='${LIBDIR}'
class cstlib(stlink_task):
	inst_to='${LIBDIR}'

TaskGen.extension('.c')(c_hook)