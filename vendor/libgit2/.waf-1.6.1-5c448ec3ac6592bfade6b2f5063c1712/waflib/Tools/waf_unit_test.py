#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys
from waflib.TaskGen import feature,after
from waflib import Utils,Task,Logs,Options
testlock=Utils.threading.Lock()
def make_test(self):
	if getattr(self,'link_task',None):
		self.default_install_path=None
		self.create_task('utest',self.link_task.outputs)
class utest(Task.Task):
	color='PINK'
	ext_in=['.bin']
	vars=[]
	def runnable_status(self):
		ret=super(utest,self).runnable_status()
		if ret==Task.SKIP_ME:
			if getattr(Options.options,'all_tests',False):
				return Task.RUN_ME
		return ret
	def run(self):
		status=0
		filename=self.inputs[0].abspath()
		self.ut_exec=getattr(self,'ut_exec',[filename])
		if getattr(self.generator,'ut_fun',None):
			self.generator.ut_fun(self)
		try:
			fu=getattr(self.generator.bld,'all_test_paths')
		except AttributeError:
			fu=os.environ.copy()
			self.generator.bld.all_test_paths=fu
			lst=[]
			for g in self.generator.bld.groups:
				for tg in g:
					if getattr(tg,'link_task',None):
						lst.append(tg.link_task.outputs[0].parent.abspath())
			def add_path(dct,path,var):
				dct[var]=os.pathsep.join(Utils.to_list(path)+[os.environ.get(var,'')])
			if sys.platform=='win32':
				add_path(fu,lst,'PATH')
			elif sys.platform=='darwin':
				add_path(fu,lst,'DYLD_LIBRARY_PATH')
				add_path(fu,lst,'LD_LIBRARY_PATH')
			else:
				add_path(fu,lst,'LD_LIBRARY_PATH')
		cwd=getattr(self.generator,'ut_cwd','')or self.inputs[0].parent.abspath()
		proc=Utils.subprocess.Popen(self.ut_exec,cwd=cwd,env=fu,stderr=Utils.subprocess.PIPE,stdout=Utils.subprocess.PIPE)
		(stdout,stderr)=proc.communicate()
		tup=(filename,proc.returncode,stdout,stderr)
		self.generator.utest_result=tup
		testlock.acquire()
		try:
			bld=self.generator.bld
			Logs.debug("ut: %r",tup)
			try:
				bld.utest_results.append(tup)
			except AttributeError:
				bld.utest_results=[tup]
		finally:
			testlock.release()
def summary(bld):
	lst=getattr(bld,'utest_results',[])
	if lst:
		Logs.pprint('CYAN','execution summary')
		total=len(lst)
		tfail=len([x for x in lst if x[1]])
		Logs.pprint('CYAN','  tests that pass %d/%d'%(total-tfail,total))
		for(f,code,out,err)in lst:
			if not code:
				Logs.pprint('CYAN','    %s'%f)
		Logs.pprint('CYAN','  tests that fail %d/%d'%(tfail,total))
		for(f,code,out,err)in lst:
			if code:
				Logs.pprint('CYAN','    %s'%f)
def options(opt):
	opt.add_option('--alltests',action='store_true',default=False,help='Exec all unit tests',dest='all_tests')

feature('test')(make_test)
after('apply_link')(make_test)