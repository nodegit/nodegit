#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import sys
if sys.hexversion < 0x020400f0: from sets import Set as set
import sys
from waflib import ConfigSet,Logs,Options,Scripting,Task,Build,Configure,Node,Runner,TaskGen,Utils,Errors,Context
sys.modules['Environment']=ConfigSet
ConfigSet.Environment=ConfigSet.ConfigSet
sys.modules['Logs']=Logs
sys.modules['Options']=Options
sys.modules['Scripting']=Scripting
sys.modules['Task']=Task
sys.modules['Build']=Build
sys.modules['Configure']=Configure
sys.modules['Node']=Node
sys.modules['Runner']=Runner
sys.modules['TaskGen']=TaskGen
sys.modules['Utils']=Utils
from waflib.Tools import c_preproc
sys.modules['preproc']=c_preproc
from waflib.Tools import c_config
sys.modules['config_c']=c_config
ConfigSet.ConfigSet.copy=ConfigSet.ConfigSet.derive
ConfigSet.ConfigSet.set_variant=Utils.nada
Build.BuildContext.add_subdirs=Build.BuildContext.recurse
Build.BuildContext.new_task_gen=Build.BuildContext.__call__
Build.BuildContext.is_install=0
Node.Node.relpath_gen=Node.Node.path_from
def name_to_obj(self,s,env=None):
	Logs.warn('compat: change "name_to_obj(name, env)" by "get_tgen_by_name(name)"')
	return self.get_tgen_by_name(s)
Build.BuildContext.name_to_obj=name_to_obj
def env_of_name(self,name):
	try:
		return self.all_envs[name]
	except KeyError:
		Logs.error('no such environment: '+name)
		return None
Build.BuildContext.env_of_name=env_of_name
def set_env_name(self,name,env):
	self.all_envs[name]=env
	return env
Configure.ConfigurationContext.set_env_name=set_env_name
def retrieve(self,name,fromenv=None):
	try:
		env=self.all_envs[name]
	except KeyError:
		env=ConfigSet.ConfigSet()
		self.prepare_env(env)
		self.all_envs[name]=env
	else:
		if fromenv:Logs.warn("The environment %s may have been configured already"%name)
	return env
Configure.ConfigurationContext.retrieve=retrieve
Configure.ConfigurationContext.sub_config=Configure.ConfigurationContext.recurse
Configure.ConfigurationContext.check_tool=Configure.ConfigurationContext.load
Configure.conftest=Configure.conf
Configure.ConfigurationError=Errors.ConfigurationError
Options.OptionsContext.sub_options=Options.OptionsContext.recurse
Options.OptionsContext.tool_options=Context.Context.load
Options.Handler=Options.OptionsContext
Task.simple_task_type=Task.task_type_from_func=Task.task_factory
Task.TaskBase.classes=Task.classes
def setitem(self,key,value):
	if key.startswith('CCFLAGS'):
		key=key[1:]
	self.table[key]=value
ConfigSet.ConfigSet.__setitem__=setitem
def old_importpaths(self):
	if getattr(self,'importpaths',[]):
		self.includes=self.importpaths
from waflib import Context
eld=Context.load_tool
def load_tool(*k,**kw):
	ret=eld(*k,**kw)
	if'set_options'in ret.__dict__:
		Logs.warn('compat: rename "set_options" to options')
		ret.options=ret.set_options
	if'detect'in ret.__dict__:
		Logs.warn('compat: rename "detect" to "configure"')
		ret.configure=ret.detect
	return ret
Context.load_tool=load_tool
rev=Context.load_module
def load_module(path):
	ret=rev(path)
	if'set_options'in ret.__dict__:
		Logs.warn('compat: rename "set_options" to "options" (%r)'%path)
		ret.options=ret.set_options
	if'srcdir'in ret.__dict__:
		Logs.warn('compat: rename "srcdir" to "top" (%r)'%path)
		ret.top=ret.srcdir
	if'blddir'in ret.__dict__:
		Logs.warn('compat: rename "blddir" to "out" (%r)'%path)
		ret.out=ret.blddir
	return ret
Context.load_module=load_module
old_post=TaskGen.task_gen.post
def post(self):
	self.features=self.to_list(self.features)
	if'cc'in self.features:
		Logs.warn('compat: the feature cc does not exist anymore (use "c")')
		self.features.remove('cc')
		self.features.append('c')
	if'cstaticlib'in self.features:
		Logs.warn('compat: the feature cstaticlib does not exist anymore (use "cstlib" or "cxxstlib")')
		self.features.remove('cstaticlib')
		self.features.append(('cxx'in self.features)and'cxxstlib'or'cstlib')
	if getattr(self,'ccflags',None):
		Logs.warn('compat: "ccflags" was renamed to "cflags"')
		self.cflags=self.ccflags
	return old_post(self)
TaskGen.task_gen.post=post
def waf_version(*k,**kw):
	Logs.warn('wrong version (waf_version was removed in waf 1.6)')
Utils.waf_version=waf_version
import os
def apply_uselib_local(self):
	env=self.env
	from waflib.Tools.ccroot import stlink_task
	self.uselib=self.to_list(getattr(self,'uselib',[]))
	self.includes=self.to_list(getattr(self,'includes',[]))
	names=self.to_list(getattr(self,'uselib_local',[]))
	get=self.bld.get_tgen_by_name
	seen=set([])
	tmp=Utils.deque(names)
	if tmp:
		Logs.warn('"uselib_local" is deprecated, replace by "use"')
	while tmp:
		lib_name=tmp.popleft()
		if lib_name in seen:
			continue
		y=get(lib_name)
		y.post()
		seen.add(lib_name)
		if getattr(y,'uselib_local',None):
			for x in self.to_list(getattr(y,'uselib_local',[])):
				obj=get(x)
				obj.post()
				if getattr(obj,'link_task',None):
					if not isinstance(obj.link_task,stlink_task):
						tmp.append(x)
		if getattr(y,'link_task',None):
			link_name=y.target[y.target.rfind(os.sep)+1:]
			if isinstance(y.link_task,stlink_task):
				env.append_value('STLIB',[link_name])
			else:
				env.append_value('LIB',[link_name])
			self.link_task.set_run_after(y.link_task)
			self.link_task.dep_nodes+=y.link_task.outputs
			tmp_path=y.link_task.outputs[0].parent.bldpath()
			if not tmp_path in env['LIBPATH']:
				env.prepend_value('LIBPATH',[tmp_path])
		for v in self.to_list(getattr(y,'uselib',[])):
			if not env['STLIB_'+v]:
				if not v in self.uselib:
					self.uselib.insert(0,v)
		if getattr(y,'export_includes',None):
			self.includes.extend(y.to_incnodes(y.export_includes))
def apply_objdeps(self):
	names=getattr(self,'add_objects',[])
	if not names:
		return
	names=self.to_list(names)
	get=self.bld.get_tgen_by_name
	seen=[]
	while names:
		x=names[0]
		if x in seen:
			names=names[1:]
			continue
		y=get(x)
		if getattr(y,'add_objects',None):
			added=0
			lst=y.to_list(y.add_objects)
			lst.reverse()
			for u in lst:
				if u in seen:continue
				added=1
				names=[u]+names
			if added:continue
		y.post()
		seen.append(x)
		for t in getattr(y,'compiled_tasks',[]):
			self.link_task.inputs.extend(t.outputs)
def process_obj_files(self):
	if not hasattr(self,'obj_files'):
		return
	for x in self.obj_files:
		node=self.path.find_resource(x)
		self.link_task.inputs.append(node)
def add_obj_file(self,file):
	if not hasattr(self,'obj_files'):self.obj_files=[]
	if not'process_obj_files'in self.meths:self.meths.append('process_obj_files')
	self.obj_files.append(file)
old_define=Configure.ConfigurationContext.__dict__['define']
def define(self,key,val,quote=True):
	old_define(self,key,val,quote)
	if key.startswith('HAVE_'):
		self.env[key]=1
old_undefine=Configure.ConfigurationContext.__dict__['undefine']
def undefine(self,key):
	old_undefine(self,key)
	if key.startswith('HAVE_'):
		self.env[key]=0
def set_incdirs(self,val):
	Logs.warn('compat: change "export_incdirs" by "export_includes"')
	self.export_includes=val
TaskGen.task_gen.export_incdirs=property(None,set_incdirs)

TaskGen.feature('d')(old_importpaths)
TaskGen.before('apply_incpaths')(old_importpaths)
TaskGen.feature('c','cxx','d')(apply_uselib_local)
TaskGen.before('apply_incpaths','propagate_uselib_vars')(apply_uselib_local)
TaskGen.after('apply_link','process_source')(apply_uselib_local)
TaskGen.feature('cprogram','cxxprogram','cstlib','cxxstlib','cshlib','cxxshlib','dprogram','dstlib','dshlib')(apply_objdeps)
TaskGen.after('apply_link')(apply_objdeps)
TaskGen.after('apply_link')(process_obj_files)
TaskGen.taskgen_method(add_obj_file)
Configure.conf(define)
Configure.conf(undefine)