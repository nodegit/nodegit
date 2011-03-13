#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys
from waflib.Tools import ccroot,ar
from waflib.Configure import conf
def find_xlcxx(conf):
	cxx=conf.find_program(['xlc++_r','xlc++'],var='CXX')
	cxx=conf.cmd_to_list(cxx)
	conf.env.CXX_NAME='xlc++'
	conf.env.CXX=cxx
def xlcxx_common_flags(conf):
	v=conf.env
	v['CXX_SRC_F']=''
	v['CXX_TGT_F']=['-c','-o','']
	if not v['LINK_CXX']:v['LINK_CXX']=v['CXX']
	v['CXXLNK_SRC_F']=''
	v['CXXLNK_TGT_F']=['-o','']
	v['CPPPATH_ST']='-I%s'
	v['DEFINES_ST']='-D%s'
	v['LIB_ST']='-l%s'
	v['LIBPATH_ST']='-L%s'
	v['STLIB_ST']='-l%s'
	v['STLIBPATH_ST']='-L%s'
	v['RPATH_ST']='-Wl,-rpath,%s'
	v['SONAME_ST']=''
	v['SHLIB_MARKER']=''
	v['STLIB_MARKER']=''
	v['LINKFLAGS_cxxprogram']=['-Wl,-brtl']
	v['cxxprogram_PATTERN']='%s'
	v['CXXFLAGS_cxxshlib']=['-fPIC']
	v['LINKFLAGS_cxxshlib']=['-G','-Wl,-brtl,-bexpfull']
	v['cxxshlib_PATTERN']='lib%s.so'
	v['LINKFLAGS_cxxstlib']=''
	v['cxxstlib_PATTERN']='lib%s.a'
configure='''
find_xlcxx
find_ar
xlcxx_common_flags
cxx_load_tools
cxx_add_flags
link_add_flags
'''

conf(find_xlcxx)
conf(xlcxx_common_flags)