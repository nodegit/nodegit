#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

typos={'feature':'features','sources':'source','targets':'target','include':'includes','export_include':'export_includes','define':'defines','importpath':'includes','installpath':'install_path',}
meths_typos=['__call__','program','shlib','stlib','objects']
from waflib import Logs,Build,Node,Task,TaskGen
import waflib.Tools.ccroot
def replace(m):
	oldcall=getattr(Build.BuildContext,m)
	def call(self,*k,**kw):
		for x in typos:
			if x in kw:
				kw[typos[x]]=kw[x]
				del kw[x]
				Logs.error('typo %r -> %r'%(x,typos[x]))
		return oldcall(self,*k,**kw)
	setattr(Build.BuildContext,m,call)
def enhance_lib():
	for m in meths_typos:
		replace(m)
	old_ant_glob=Node.Node.ant_glob
	def ant_glob(self,*k,**kw):
		for x in k[0].split('/'):
			if x=='..':
				Logs.error("In ant_glob pattern %r: '..' means 'two dots', not 'parent directory'"%k[0])
		return old_ant_glob(self,*k,**kw)
	Node.Node.ant_glob=ant_glob
	old=Task.is_before
	def is_before(t1,t2):
		ret=old(t1,t2)
		if ret and old(t2,t1):
			Logs.error('Contradictory order constraints in classes %r %r'%(t1,t2))
		return ret
	Task.is_before=is_before
	def check_err_features(self):
		lst=self.to_list(self.features)
		if'shlib'in lst:
			Logs.error('feature shlib -> cshlib, dshlib or cxxshlib')
		for x in('c','cxx','d','fc'):
			if not x in lst and lst and lst[0]in[x+y for y in('program','shlib','stlib')]:
				Logs.error('%r features is probably missing %r'%(self,x))
	TaskGen.feature('*')(check_err_features)
def options(opt):
	enhance_lib()
def configure(conf):
	pass
