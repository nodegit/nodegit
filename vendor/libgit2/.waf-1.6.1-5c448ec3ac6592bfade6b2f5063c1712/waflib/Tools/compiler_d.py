#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys,imp,types
from waflib import Utils,Configure,Options,Logs
def configure(conf):
	for compiler in conf.options.dcheck.split(','):
		conf.env.stash()
		conf.start_msg('Checking for %r (d compiler)'%compiler)
		try:
			conf.load(compiler)
		except conf.errors.ConfigurationError ,e:
			conf.env.revert()
			conf.end_msg(False)
			Logs.debug('compiler_cxx: %r'%e)
		else:
			if conf.env.D:
				conf.end_msg(True)
				conf.env['COMPILER_D']=compiler
				conf.env.D_COMPILER=conf.env.D
				break
			conf.end_msg(False)
	else:
		conf.fatal('no suitable d compiler was found')
def options(opt):
	d_compiler_opts=opt.add_option_group('D Compiler Options')
	d_compiler_opts.add_option('--check-d-compiler',default='gdc,dmd',action='store',help='check for the compiler [Default:gdc,dmd]',dest='dcheck')
	for d_compiler in['gdc','dmd']:
		opt.load('%s'%d_compiler)
