#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import sys
from waflib.Tools import ar,d
from waflib.Configure import conf
def find_gdc(conf):
	conf.find_program('gdc',var='D')
def common_flags_gdc(conf):
	v=conf.env
	v['DFLAGS']=[]
	v['D_SRC_F']=''
	v['D_TGT_F']=['-c','-o','']
	v['D_LINKER']=v['D']
	v['DLNK_SRC_F']=''
	v['DLNK_TGT_F']=['-o','']
	v['DINC_ST']='-I%s'
	v['DSHLIB_MARKER']=v['DSTLIB_MARKER']=''
	v['DSTLIB_ST']=v['DSHLIB_ST']='-l%s'
	v['DSTLIBPATH_ST']=v['DLIBPATH_ST']='-L%s'
	v['LINKFLAGS_dshlib']=['-shared']
	v['DHEADER_ext']='.di'
	v['D_HDR_F']='-fintfc -fintfc-file='
def configure(conf):
	conf.find_gdc()
	conf.load('ar')
	conf.load('d')
	conf.common_flags_gdc()
	conf.d_platform_flags()

conf(find_gdc)
conf(common_flags_gdc)