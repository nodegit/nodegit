#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os
from waflib import Utils
from waflib.Tools import ccroot,ar
from waflib.Configure import conf
def find_scc(conf):
	v=conf.env
	cc=None
	if v['CC']:cc=v['CC']
	elif'CC'in conf.environ:cc=conf.environ['CC']
	if not cc:cc=conf.find_program('cc',var='CC')
	if not cc:conf.fatal('suncc was not found')
	try:
		conf.cmd_and_log('%s -flags'%cc)
	except:
		conf.fatal('suncc %r was not found'%cc)
	v['CC']=cc
	v['CC_NAME']='sun'
def scc_common_flags(conf):
	v=conf.env
	v['CC_SRC_F']=''
	v['CC_TGT_F']=['-c','-o','']
	if not v['LINK_CC']:v['LINK_CC']=v['CC']
	v['CCLNK_SRC_F']=''
	v['CCLNK_TGT_F']=['-o','']
	v['CPPPATH_ST']='-I%s'
	v['DEFINES_ST']='-D%s'
	v['LIB_ST']='-l%s'
	v['LIBPATH_ST']='-L%s'
	v['STLIB_ST']='-l%s'
	v['STLIBPATH_ST']='-L%s'
	v['SONAME_ST']='-Wl,-h,%s'
	v['SHLIB_MARKER']='-Bdynamic'
	v['STLIB_MARKER']='-Bstatic'
	v['cprogram_PATTERN']='%s'
	v['CFLAGS_cshlib']=['-Kpic','-DPIC']
	v['LINKFLAGS_cshlib']=['-G']
	v['cshlib_PATTERN']='lib%s.so'
	v['LINKFLAGS_cstlib']=['-Bstatic']
	v['cstlib_PATTERN']='lib%s.a'
configure='''
find_scc
find_ar
scc_common_flags
cc_load_tools
cc_add_flags
link_add_flags
'''

conf(find_scc)
conf(scc_common_flags)