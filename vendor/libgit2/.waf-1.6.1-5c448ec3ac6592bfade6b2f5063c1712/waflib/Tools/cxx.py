#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

from waflib import TaskGen,Task,Utils
from waflib.Tools import c_preproc
from waflib.Tools.ccroot import link_task,stlink_task
def cxx_hook(self,node):
	return self.create_compiled_task('cxx',node)
TaskGen.extension('.cpp','.cc','.cxx','.C','.c++')(cxx_hook)
if not'.c'in TaskGen.task_gen.mappings:
	TaskGen.task_gen.mappings['.c']=TaskGen.task_gen.mappings['.cpp']
class cxx(Task.Task):
	color='GREEN'
	run_str='${CXX} ${CXXFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CXX_SRC_F}${SRC} ${CXX_TGT_F}${TGT}'
	vars=['CXXDEPS']
	ext_in=['.h']
	scan=c_preproc.scan
class cxxprogram(link_task):
	run_str='${LINK_CXX} ${CXXLNK_SRC_F}${SRC} ${CXXLNK_TGT_F}${TGT[0].abspath()} ${RPATH_ST:RPATH} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${tsk.frameworks()} ${STLIB_MARKER} ${STLIBPATH_ST:STLIBPATH} ${STLIB_ST:STLIB} ${SHLIB_MARKER} ${LIBPATH_ST:LIBPATH} ${LIB_ST:LIB} ${LINKFLAGS}'
	ext_out=['.bin']
	inst_to='${BINDIR}'
	chmod=Utils.O755
class cxxshlib(cxxprogram):
	inst_to='${LIBDIR}'
class cxxstlib(stlink_task):
	pass
