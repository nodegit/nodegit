#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import sys
if sys.hexversion < 0x020400f0: from sets import Set as set
import copy,re
from waflib import Task,Utils,Logs,Errors
feats=Utils.defaultdict(set)
class task_gen(object):
	mappings={}
	prec=Utils.defaultdict(list)
	def __init__(self,*k,**kw):
		self.source=''
		self.target=''
		self.meths=[]
		self.prec=Utils.defaultdict(list)
		self.mappings={}
		self.features=[]
		self.tasks=[]
		if not'bld'in kw:
			self.env=ConfigSet.ConfigSet()
			self.idx=0
			self.path=None
		else:
			self.bld=kw['bld']
			self.env=self.bld.env.derive()
			self.path=self.bld.path
			try:
				self.idx=self.bld.idx[id(self.path)]=self.bld.idx.get(id(self.path),0)+1
			except AttributeError:
				self.bld.idx={}
				self.idx=self.bld.idx[id(self.path)]=0
		for key,val in kw.items():
			setattr(self,key,val)
	def __str__(self):
		return"<task_gen %r declared in %s>"%(self.name,self.path.abspath())
	def __repr__(self):
		lst=[]
		for x in self.__dict__.keys():
			if x not in['env','bld','compiled_tasks','tasks']:
				lst.append("%s=%r"%(x,repr(getattr(self,x))))
		return"bld(%s) in %s"%(" ".join(lst),self.path.abspath())
	def get_name(self):
		try:
			return self._name
		except AttributeError:
			if isinstance(self.target,list):
				lst=[str(x)for x in self.target]
				name=self._name=','.join(lst)
			else:
				name=self._name=str(self.target)
			return name
	def set_name(self,name):
		self._name=name
	name=property(get_name,set_name)
	def to_list(self,value):
		if isinstance(value,str):return value.split()
		else:return value
	def post(self):
		if getattr(self,'posted',None):
			return False
		self.posted=True
		keys=set(self.meths)
		self.features=Utils.to_list(self.features)
		for x in self.features+['*']:
			st=feats[x]
			if not st:
				Logs.warn('feature %r does not exist - bind at least one method to it'%x)
			keys.update(st)
		prec={}
		prec_tbl=self.prec or task_gen.prec
		for x in prec_tbl:
			if x in keys:
				prec[x]=prec_tbl[x]
		tmp=[]
		for a in keys:
			for x in prec.values():
				if a in x:break
			else:
				tmp.append(a)
		out=[]
		while tmp:
			e=tmp.pop()
			if e in keys:out.append(e)
			try:
				nlst=prec[e]
			except KeyError:
				pass
			else:
				del prec[e]
				for x in nlst:
					for y in prec:
						if x in prec[y]:
							break
					else:
						tmp.append(x)
		if prec:
			raise Errors.WafError('Cycle detected in the method execution %r'%prec)
		out.reverse()
		self.meths=out
		Logs.debug('task_gen: posting %s %d'%(self,id(self)))
		for x in out:
			try:
				v=getattr(self,x)
			except AttributeError:
				raise Errors.WafError('%r is not a valid task generator method'%x)
			Logs.debug('task_gen: -> %s (%d)'%(x,id(self)))
			v()
		Logs.debug('task_gen: posted %s'%self.name)
		return True
	def get_hook(self,node):
		name=node.name
		for k in self.mappings:
			if name.endswith(k):
				return self.mappings[k]
		for k in task_gen.mappings:
			if name.endswith(k):
				return task_gen.mappings[k]
		raise Errors.WafError("File %r has no mapping in %r (did you forget to load a waf tool?)"%(node,task_gen.mappings.keys()))
	def create_task(self,name,src=None,tgt=None):
		task=Task.classes[name](env=self.env.derive(),generator=self)
		if src:
			task.set_inputs(src)
		if tgt:
			task.set_outputs(tgt)
		self.tasks.append(task)
		return task
	def clone(self,env):
		newobj=self.bld()
		for x in self.__dict__:
			if x in['env','bld']:
				continue
			elif x in['path','features']:
				setattr(newobj,x,getattr(self,x))
			else:
				setattr(newobj,x,copy.copy(getattr(self,x)))
		newobj.posted=False
		if isinstance(env,str):
			newobj.env=self.bld.all_envs[env].derive()
		else:
			newobj.env=env.derive()
		return newobj
def declare_chain(name='',rule=None,reentrant=True,color='BLUE',ext_in=[],ext_out=[],before=[],after=[],decider=None,scan=None):
	ext_in=Utils.to_list(ext_in)
	ext_out=Utils.to_list(ext_out)
	cls=Task.task_factory(name,rule,color=color,ext_in=ext_in,ext_out=ext_out,before=before,after=after,scan=scan)
	def x_file(self,node):
		ext=decider and decider(self,node)or cls.ext_out
		if ext_in:
			_ext_in=ext_in[0]
		out_source=[node.change_ext(x,ext_in=_ext_in)for x in ext]
		if reentrant:
			for i in range(reentrant):
				self.source.append(out_source[i])
		tsk=self.create_task(name,node,out_source)
	for x in cls.ext_in:
		task_gen.mappings[x]=x_file
	return x_file
def taskgen_method(func):
	setattr(task_gen,func.__name__,func)
	return func
def feature(*k):
	def deco(func):
		setattr(task_gen,func.__name__,func)
		for name in k:
			feats[name].update([func.__name__])
		return func
	return deco
def before(*k):
	def deco(func):
		setattr(task_gen,func.__name__,func)
		for fun_name in k:
			if not func.__name__ in task_gen.prec[fun_name]:
				task_gen.prec[fun_name].append(func.__name__)
		return func
	return deco
def after(*k):
	def deco(func):
		setattr(task_gen,func.__name__,func)
		for fun_name in k:
			if not fun_name in task_gen.prec[func.__name__]:
				task_gen.prec[func.__name__].append(fun_name)
		return func
	return deco
def extension(*k):
	def deco(func):
		setattr(task_gen,func.__name__,func)
		for x in k:
			task_gen.mappings[x]=func
		return func
	return deco
def to_nodes(self,lst,path=None):
	tmp=[]
	path=path or self.path
	find=path.find_resource
	if isinstance(lst,self.path.__class__):
		lst=[lst]
	for x in Utils.to_list(lst):
		if isinstance(x,str):
			node=find(x)
			if not node:
				raise Errors.WafError("source not found: %r in %r"%(x,path))
		else:
			node=x
		tmp.append(node)
	return tmp
def process_source(self):
	self.source=self.to_nodes(getattr(self,'source',[]))
	for node in self.source:
		self.get_hook(node)(self,node)
def process_rule(self):
	if not getattr(self,'rule',None):
		return
	name=str(getattr(self,'name',None)or self.target or self.rule)
	cls=Task.task_factory(name,self.rule,getattr(self,'vars',[]),shell=getattr(self,'shell',True),color=getattr(self,'color','BLUE'))
	tsk=self.create_task(name)
	if getattr(self,'target',None):
		if isinstance(self.target,str):
			self.target=self.target.split()
		if not isinstance(self.target,list):
			self.target=[self.target]
		for x in self.target:
			if isinstance(x,str):
				tsk.outputs.append(self.path.find_or_declare(x))
			else:
				x.parent.mkdir()
				tsk.outputs.append(x)
		if getattr(self,'install_path',None):
			self.bld.install_files(self.install_path,tsk.outputs)
	if getattr(self,'source',None):
		tsk.inputs=self.to_nodes(self.source)
		self.source=[]
	if getattr(self,'scan',None):
		cls.scan=self.scan
	if getattr(self,'cwd',None):
		tsk.cwd=self.cwd
	if getattr(self,'update_outputs',None)or getattr(self,'on_results',None):
		Task.update_outputs(cls)
	if getattr(self,'always',None):
		Task.always_run(cls)
	for x in['after','before','ext_in','ext_out']:
		setattr(cls,x,getattr(self,x,[]))
def sequence_order(self):
	if self.meths and self.meths[-1]!='sequence_order':
		self.meths.append('sequence_order')
		return
	if getattr(self,'seq_start',None):
		return
	if getattr(self.bld,'prev',None):
		self.bld.prev.post()
		for x in self.bld.prev.tasks:
			for y in self.tasks:
				y.set_run_after(x)
	self.bld.prev=self
re_m4=re.compile('@(\w+)@',re.M)
class subst_pc(Task.Task):
	def run(self):
		code=self.inputs[0].read()
		code=code.replace('%','%%')
		lst=[]
		def repl(match):
			g=match.group
			if g(1):
				lst.append(g(1))
				return"%%(%s)s"%g(1)
			return''
		code=re_m4.sub(repl,code)
		try:
			d=self.generator.dct
		except AttributeError:
			d={}
			for x in lst:
				d[x]=getattr(self.generator,x,'')or self.env.get_flat(x)or self.env.get_flat(x.upper())
				if not d[x]and not getattr(self.generator,'quiet',False):
					raise ValueError('variable %r has no value for %r'%(x,self.outputs))
		self.outputs[0].write(code%d)
		self.generator.bld.raw_deps[self.uid()]=self.dep_vars=lst
		try:delattr(self,'cache_sig')
		except AttributeError:pass
	def sig_vars(self):
		bld=self.generator.bld
		env=self.env
		upd=self.m.update
		vars=self.generator.bld.raw_deps.get(self.uid(),[])
		act_sig=bld.hash_env_vars(env,vars)
		upd(act_sig)
		lst=[getattr(self.generator,x,'')for x in vars]
		upd(Utils.h_list(lst))
		return self.m.digest()
def add_pcfile(self,node):
	tsk=self.create_task('subst_pc',node,node.change_ext('.pc','.pc.in'))
	self.bld.install_files('${PREFIX}/lib/pkgconfig/',tsk.outputs)
class subst(subst_pc):
	pass
def process_subst(self):
	src=self.to_nodes(getattr(self,'source',[]))
	tgt=getattr(self,'target',[])
	if isinstance(tgt,self.path.__class__):
		tgt=[tgt]
	tgt=[isinstance(x,self.path.__class__)and x or self.path.find_or_declare(x)for x in Utils.to_list(tgt)]
	if len(src)!=len(tgt):
		raise Errors.WafError('invalid source or target for %r'%self)
	for x,y in zip(src,tgt):
		if not(x and y):
			raise Errors.WafError('invalid source or target for %r'%self)
		self.create_task('subst',x,y)
	inst_to=getattr(self,'install_path',None)
	if inst_to:
		self.bld.install_files(inst_to,tgt,chmod=getattr(self,'chmod',Utils.O644))
	self.source=[]

taskgen_method(to_nodes)
feature('*')(process_source)
feature('*')(process_rule)
before('process_source')(process_rule)
feature('seq')(sequence_order)
extension('.pc.in')(add_pcfile)
feature('subst')(process_subst)
before('process_source','process_rule')(process_subst)