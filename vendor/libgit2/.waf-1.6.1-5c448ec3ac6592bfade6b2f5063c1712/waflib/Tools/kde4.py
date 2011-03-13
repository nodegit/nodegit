#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys,re
from waflib import Options,TaskGen,Task,Utils
from waflib.TaskGen import feature,after
def init_msgfmt(self):
	try:
		self.install_path
	except:
		self.install_path='${KDE4_LOCALE_INSTALL_DIR}'
def apply_msgfmt(self):
	for lang in self.to_list(self.langs):
		node=self.path.find_resource(lang+'.po')
		task=self.create_task('msgfmt',node,node.change_ext('.mo'))
		if not self.bld.is_install:continue
		langname=lang.split('/')
		langname=langname[-1]
		task.install_path=self.install_path+os.sep+langname+os.sep+'LC_MESSAGES'
		task.filename=getattr(self,'appname','set_your_appname')+'.mo'
		task.chmod=self.chmod
def configure(self):
	kdeconfig=self.find_program('kde4-config')
	prefix=self.cmd_and_log('%s --prefix'%kdeconfig).strip()
	fname='%s/share/apps/cmake/modules/KDELibsDependencies.cmake'%prefix
	try:os.stat(fname)
	except OSError:
		fname='%s/share/kde4/apps/cmake/modules/KDELibsDependencies.cmake'%prefix
		try:os.stat(fname)
		except OSError:self.fatal('could not open %s'%fname)
	try:
		txt=Utils.readf(fname)
	except(OSError,IOError):
		self.fatal('could not read %s'%fname)
	txt=txt.replace('\\\n','\n')
	fu=re.compile('#(.*)\n')
	txt=fu.sub('',txt)
	setregexp=re.compile('([sS][eE][tT]\s*\()\s*([^\s]+)\s+\"([^"]+)\"\)')
	found=setregexp.findall(txt)
	for(_,key,val)in found:
		self.env[key]=val
	self.env['LIB_KDECORE']=['kdecore']
	self.env['LIB_KDEUI']=['kdeui']
	self.env['LIB_KIO']=['kio']
	self.env['LIB_KHTML']=['khtml']
	self.env['LIB_KPARTS']=['kparts']
	self.env['LIBPATH_KDECORE']=[self.env['KDE4_LIB_INSTALL_DIR']]
	self.env['INCLUDES_KDECORE']=[self.env['KDE4_INCLUDE_INSTALL_DIR']]
	self.env.append_value('INCLUDES_KDECORE',[self.env['KDE4_INCLUDE_INSTALL_DIR']+os.sep+'KDE'])
	self.find_program('msgfmt',var='MSGFMT')
class msgfmt(Task.Task):
	color='BLUE'
	run_str='${MSGFMT} ${SRC} -o ${TGT}'

feature('msgfmt')(init_msgfmt)
feature('msgfmt')(apply_msgfmt)
after('init_msgfmt')(apply_msgfmt)