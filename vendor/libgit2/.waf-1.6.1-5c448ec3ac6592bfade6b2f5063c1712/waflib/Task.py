#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import sys
if sys.hexversion < 0x020400f0: from sets import Set as set
import os,shutil,re,tempfile
from waflib import Utils,Logs,Errors
NOT_RUN=0
MISSING=1
CRASHED=2
EXCEPTION=3
SKIPPED=8
SUCCESS=9
ASK_LATER=-1
SKIP_ME=-2
RUN_ME=-3
COMPILE_TEMPLATE_SHELL='''
def f(tsk):
	env = tsk.env
	gen = tsk.generator
	bld = gen.bld
	wd = getattr(tsk, 'cwd', None)
	p = env.get_flat
	tsk.last_cmd = cmd = \'\'\' %s \'\'\' % s
	return tsk.exec_command(cmd, cwd=wd, env=env.env or None)
'''
COMPILE_TEMPLATE_NOSHELL='''
def f(tsk):
	env = tsk.env
	gen = tsk.generator
	bld = gen.bld
	wd = getattr(tsk, 'cwd', None)
	def to_list(xx):
		if isinstance(xx, str): return [xx]
		return xx
	tsk.last_cmd = lst = []
	%s
	lst = [x for x in lst if x]
	return tsk.exec_command(lst, cwd=wd, env=env.env or None)
'''
def cache_outputs(cls):
	m1=cls.run
	def run(self):
		bld=self.generator.bld
		if bld.cache_global and not bld.nocache:
			if self.can_retrieve_cache():
				return 0
		return m1(self)
	cls.run=run
	m2=cls.post_run
	def post_run(self):
		bld=self.generator.bld
		ret=m2(self)
		if bld.cache_global and not bld.nocache:
			self.put_files_cache()
		return ret
	cls.post_run=post_run
	return cls
classes={}
class store_task_type(type):
	def __init__(cls,name,bases,dict):
		super(store_task_type,cls).__init__(name,bases,dict)
		name=cls.__name__
		if name.endswith('_task'):
			name=name.replace('_task','')
		if name!='evil'and name!='TaskBase':
			global classes
			if getattr(cls,'run_str',None):
				(f,dvars)=compile_fun(cls.run_str,cls.shell)
				cls.hcode=cls.run_str
				cls.run_str=None
				cls.run=f
				cls.vars.extend(dvars)
			elif getattr(cls,'run',None)and not'hcode'in cls.__dict__:
				cls.hcode=Utils.h_fun(cls.run)
			if not getattr(cls,'nocache',None):
				cls=cache_outputs(cls)
			classes[name]=cls
evil=store_task_type('evil',(object,),{})
class TaskBase(evil):
	color="GREEN"
	ext_in=[]
	ext_out=[]
	before=[]
	after=[]
	hcode=''
	def __init__(self,*k,**kw):
		self.hasrun=NOT_RUN
		try:
			self.generator=kw['generator']
		except KeyError:
			self.generator=self
	def __repr__(self):
		return'\n\t{task %r: %s %s}'%(self.__class__.__name__,id(self),str(getattr(self,"fun","")))
	def __str__(self):
		if hasattr(self,'fun'):
			return'executing: %s\n'%self.fun.__name__
		return self.__class__.__name__+'\n'
	def __hash__(self):
		return id(self)
	def exec_command(self,cmd,**kw):
		bld=self.generator.bld
		try:
			if not kw.get('cwd',None):
				kw['cwd']=bld.cwd
		except AttributeError:
			bld.cwd=kw['cwd']=bld.variant_dir
		return bld.exec_command(cmd,**kw)
	def runnable_status(self):
		return RUN_ME
	def process(self):
		m=self.master
		if m.stop:
			m.out.put(self)
			return
		self.generator.bld.returned_tasks.append(self)
		self.log_display(self.generator.bld)
		try:
			ret=self.run()
		except Exception ,e:
			self.err_msg=Utils.ex_stack()
			self.hasrun=EXCEPTION
			m.error_handler(self)
			m.out.put(self)
			return
		if ret:
			self.err_code=ret
			self.hasrun=CRASHED
		else:
			try:
				self.post_run()
			except Errors.WafError:
				pass
			except Exception:
				self.err_msg=Utils.ex_stack()
				self.hasrun=EXCEPTION
			else:
				self.hasrun=SUCCESS
		if self.hasrun!=SUCCESS:
			m.error_handler(self)
		m.out.put(self)
	def run(self):
		if hasattr(self,'fun'):
			return self.fun(self)
		return 0
	def post_run(self):
		pass
	def log_display(self,bld):
		bld.to_log(self.display())
	def display(self):
		col1=Logs.colors(self.color)
		col2=Logs.colors.NORMAL
		if self.generator.bld.progress_bar==1:
			return self.generator.bld.progress_line(len(self.generator.bld.returned_tasks),self.position[1],col1,col2)
		if self.generator.bld.progress_bar==2:
			ela=str(self.generator.bld.timer)
			try:
				ins=','.join([n.name for n in self.inputs])
			except AttributeError:
				ins=''
			try:
				outs=','.join([n.name for n in self.outputs])
			except AttributeError:
				outs=''
			return'|Total %s|Current %s|Inputs %s|Outputs %s|Time %s|\n'%(self.position[1],len(self.generator.bld.returned_tasks),ins,outs,ela)
		s=str(self)
		if not s:
			return None
		total=self.position[1]
		n=len(str(total))
		fs='[%%%dd/%%%dd] %%s%%s%%s'%(n,n)
		return fs%(len(self.generator.bld.returned_tasks),self.position[1],col1,s,col2)
	def attr(self,att,default=None):
		ret=getattr(self,att,self)
		if ret is self:return getattr(self.__class__,att,default)
		return ret
	def hash_constraints(self):
		cls=self.__class__
		tup=(str(cls.before),str(cls.after),str(cls.ext_in),str(cls.ext_out),cls.__name__)
		h=hash(tup)
		return h
	def format_error(self):
		msg=getattr(self,'last_cmd','')
		if getattr(self,"err_msg",None):
			return self.err_msg
		elif self.hasrun==CRASHED:
			try:
				return' -> task failed (exit status %r): %r\n%r'%(self.err_code,self,msg)
			except AttributeError:
				return' -> task failed: %r\n%r'%(self,msg)
		elif self.hasrun==MISSING:
			return' -> missing files: %r\n%r'%(self,msg)
		else:
			return'?'
class Task(TaskBase):
	vars=[]
	shell=False
	def __init__(self,*k,**kw):
		TaskBase.__init__(self,*k,**kw)
		self.env=kw['env']
		self.inputs=[]
		self.outputs=[]
		self.dep_nodes=[]
		self.run_after=set([])
	def __str__(self):
		env=self.env
		src_str=' '.join([a.nice_path(env)for a in self.inputs])
		tgt_str=' '.join([a.nice_path(env)for a in self.outputs])
		if self.outputs:sep=' -> '
		else:sep=''
		return'%s: %s%s%s\n'%(self.__class__.__name__.replace('_task',''),src_str,sep,tgt_str)
	def __repr__(self):
		return"".join(['\n\t{task %r: '%id(self),self.__class__.__name__," ",",".join([x.name for x in self.inputs])," -> ",",".join([x.name for x in self.outputs]),'}'])
	def uid(self):
		try:
			return self.uid_
		except AttributeError:
			m=Utils.md5()
			up=m.update
			up(self.__class__.__name__)
			for x in self.inputs+self.outputs:
				up(x.abspath())
			self.uid_=m.digest()
			return self.uid_
	def set_inputs(self,inp):
		if isinstance(inp,list):self.inputs+=inp
		else:self.inputs.append(inp)
	def set_outputs(self,out):
		if isinstance(out,list):self.outputs+=out
		else:self.outputs.append(out)
	def set_run_after(self,task):
		assert isinstance(task,TaskBase)
		self.run_after.add(task)
	def signature(self):
		try:return self.cache_sig
		except AttributeError:pass
		self.m=Utils.md5()
		self.m.update(self.hcode)
		self.sig_explicit_deps()
		self.sig_vars()
		if self.scan:
			try:
				imp_sig=self.sig_implicit_deps()
			except Errors.TaskRescan:
				return self.signature()
		ret=self.cache_sig=self.m.digest()
		return ret
	def runnable_status(self):
		for t in self.run_after:
			if not t.hasrun:
				return ASK_LATER
		env=self.env
		bld=self.generator.bld
		try:
			new_sig=self.signature()
		except Errors.TaskNotReady:
			return ASK_LATER
		key=self.uid()
		try:
			prev_sig=bld.task_sigs[key]
		except KeyError:
			Logs.debug("task: task %r must run as it was never run before or the task code changed"%self)
			return RUN_ME
		for node in self.outputs:
			try:
				if node.sig!=new_sig:
					return RUN_ME
			except AttributeError:
				Logs.debug("task: task %r must run as the output nodes do not exist"%self)
				return RUN_ME
		if new_sig!=prev_sig:
			return RUN_ME
		return SKIP_ME
	def post_run(self):
		bld=self.generator.bld
		env=self.env
		sig=self.signature()
		for node in self.outputs:
			try:
				os.stat(node.abspath())
			except OSError:
				self.hasrun=MISSING
				self.err_msg='-> missing file: %r'%node.abspath()
				raise Errors.WafError(self.err_msg)
			node.sig=sig
		bld.task_sigs[self.uid()]=self.cache_sig
	def sig_explicit_deps(self):
		bld=self.generator.bld
		upd=self.m.update
		for x in self.inputs+self.dep_nodes:
			try:
				upd(x.get_bld_sig())
			except(AttributeError,TypeError):
				raise Errors.WafError('Missing node signature for %r (required by %r)'%(x,self))
		if bld.deps_man:
			additional_deps=bld.deps_man
			for x in self.inputs+self.outputs:
				try:
					d=additional_deps[id(x)]
				except KeyError:
					continue
				for v in d:
					if isinstance(v,bld.root.__class__):
						try:
							v=v.get_bld_sig()
						except AttributeError:
							raise Errors.WafError('Missing node signature for %r (required by %r)'%(v,self))
					elif hasattr(v,'__call__'):
						v=v()
					upd(v)
		return self.m.digest()
	def sig_vars(self):
		bld=self.generator.bld
		env=self.env
		upd=self.m.update
		act_sig=bld.hash_env_vars(env,self.__class__.vars)
		upd(act_sig)
		dep_vars=getattr(self,'dep_vars',None)
		if dep_vars:
			upd(bld.hash_env_vars(env,dep_vars))
		return self.m.digest()
	scan=None
	def sig_implicit_deps(self):
		bld=self.generator.bld
		key=self.uid()
		prev=bld.task_sigs.get((key,'imp'),[])
		if prev:
			try:
				if prev==self.compute_sig_implicit_deps():
					return prev
			except IOError:
				pass
			del bld.task_sigs[(key,'imp')]
			raise Errors.TaskRescan('rescan')
		(nodes,names)=self.scan()
		if Logs.verbose:
			Logs.debug('deps: scanner for %s returned %s %s'%(str(self),str(nodes),str(names)))
		bld.node_deps[key]=nodes
		bld.raw_deps[key]=names
		self.are_implicit_nodes_ready()
		bld.task_sigs[(key,'imp')]=sig=self.compute_sig_implicit_deps()
		return sig
	def compute_sig_implicit_deps(self):
		upd=self.m.update
		bld=self.generator.bld
		env=self.env
		self.are_implicit_nodes_ready()
		try:
			for k in bld.node_deps.get(self.uid(),[]):
				upd(k.get_bld_sig())
		except AttributeError:
			nodes=[]
			for k in bld.node_deps.get(self.uid(),[]):
				try:
					k.get_bld_sig()
				except AttributeError:
					nodes.append(k)
			raise Errors.WafError('Missing node signature for %r (for implicit dependencies %r)'%(nodes,self))
		return self.m.digest()
	def are_implicit_nodes_ready(self):
		bld=self.generator.bld
		try:
			cache=bld.dct_implicit_nodes
		except:
			bld.dct_implicit_nodes=cache={}
		try:
			dct=cache[bld.cur]
		except KeyError:
			dct=cache[bld.cur]={}
			for tsk in bld.cur_tasks:
				for x in tsk.outputs:
					dct[x]=tsk
		modified=False
		for x in bld.node_deps.get(self.uid(),[]):
			if x in dct:
				self.run_after.add(dct[x])
				modified=True
		if modified:
			for tsk in self.run_after:
				if not tsk.hasrun:
					raise Errors.TaskNotReady('not ready')
	def can_retrieve_cache(self):
		if not getattr(self,'outputs',None):
			return None
		env=self.env
		sig=self.signature()
		ssig=Utils.to_hex(sig)
		dname=os.path.join(self.generator.bld.cache_global,ssig)
		try:
			t1=os.stat(dname).st_mtime
		except OSError:
			return None
		for node in self.outputs:
			orig=os.path.join(dname,node.name)
			try:
				shutil.copy2(orig,node.abspath())
				os.utime(orig,None)
			except(OSError,IOError):
				Logs.debug('task: failed retrieving file')
				return None
		try:
			t2=os.stat(dname).st_mtime
		except OSError:
			return None
		if t1!=t2:
			return None
		for node in self.outputs:
			node.sig=sig
			if self.generator.bld.progress_bar<1:
				self.generator.bld.to_log('restoring from cache %r\n'%node.abspath())
		self.cached=True
		return True
	def put_files_cache(self):
		if getattr(self,'cached',None):
			return None
		sig=self.signature()
		ssig=Utils.to_hex(sig)
		dname=os.path.join(self.generator.bld.cache_global,ssig)
		tmpdir=tempfile.mkdtemp(prefix=self.generator.bld.cache_global+os.sep+'waf')
		try:
			shutil.rmtree(dname)
		except:
			pass
		try:
			for node in self.outputs:
				dest=os.path.join(tmpdir,node.name)
				shutil.copy2(node.abspath(),dest)
		except(OSError,IOError):
			try:
				shutil.rmtree(tmpdir)
			except:
				pass
		else:
			try:
				os.rename(tmpdir,dname)
			except OSError:
				try:
					shutil.rmtree(tmpdir)
				except:
					pass
			else:
				try:
					os.chmod(dname,Utils.O755)
				except:
					pass
def is_before(t1,t2):
	to_list=Utils.to_list
	for k in to_list(t2.ext_in):
		if k in to_list(t1.ext_out):
			return 1
	if t1.__class__.__name__ in to_list(t2.after):
		return 1
	if t2.__class__.__name__ in to_list(t1.before):
		return 1
	return 0
def set_file_constraints(tasks):
	ins=Utils.defaultdict(set)
	outs=Utils.defaultdict(set)
	for x in tasks:
		for a in getattr(x,'inputs',[])+getattr(x,'dep_nodes',[]):
			ins[id(a)].add(x)
		for a in getattr(x,'outputs',[]):
			outs[id(a)].add(x)
	links=set(ins.keys()).intersection(outs.keys())
	for k in links:
		for a in ins[k]:
			a.run_after.update(outs[k])
def set_precedence_constraints(tasks):
	cstr_groups=Utils.defaultdict(list)
	for x in tasks:
		h=x.hash_constraints()
		cstr_groups[h].append(x)
	keys=list(cstr_groups.keys())
	maxi=len(keys)
	for i in range(maxi):
		t1=cstr_groups[keys[i]][0]
		for j in range(i+1,maxi):
			t2=cstr_groups[keys[j]][0]
			if is_before(t1,t2):
				a=i
				b=j
			elif is_before(t2,t1):
				a=j
				b=i
			else:
				continue
			for x in cstr_groups[keys[b]]:
				x.run_after.update(cstr_groups[keys[a]])
def funex(c):
	dc={}
	exec(c,dc)
	return dc['f']
reg_act=re.compile(r"(?P<backslash>\\)|(?P<dollar>\$\$)|(?P<subst>\$\{(?P<var>\w+)(?P<code>.*?)\})",re.M)
def compile_fun_shell(line):
	extr=[]
	def repl(match):
		g=match.group
		if g('dollar'):return"$"
		elif g('backslash'):return'\\\\'
		elif g('subst'):extr.append((g('var'),g('code')));return"%s"
		return None
	line=reg_act.sub(repl,line)or line
	parm=[]
	dvars=[]
	app=parm.append
	for(var,meth)in extr:
		if var=='SRC':
			if meth:app('tsk.inputs%s'%meth)
			else:app('" ".join([a.path_from(bld.bldnode) for a in tsk.inputs])')
		elif var=='TGT':
			if meth:app('tsk.outputs%s'%meth)
			else:app('" ".join([a.path_from(bld.bldnode) for a in tsk.outputs])')
		elif meth:
			if meth.startswith(':'):
				app('" ".join([env[%r] %% x for x in env[%r]])'%(var,meth[1:]))
				dvars.extend([var,meth[1:]])
			else:
				app('%s%s'%(var,meth))
		else:
			if not var in dvars:dvars.append(var)
			app("p('%s')"%var)
	if parm:parm="%% (%s) "%(',\n\t\t'.join(parm))
	else:parm=''
	c=COMPILE_TEMPLATE_SHELL%(line,parm)
	Logs.debug('action: %s'%c)
	return(funex(c),dvars)
def compile_fun_noshell(line):
	extr=[]
	def repl(match):
		g=match.group
		if g('dollar'):return"$"
		elif g('subst'):extr.append((g('var'),g('code')));return"<<|@|>>"
		return None
	line2=reg_act.sub(repl,line)
	params=line2.split('<<|@|>>')
	assert(extr)
	buf=[]
	dvars=[]
	app=buf.append
	for x in range(len(extr)):
		params[x]=params[x].strip()
		if params[x]:
			app("lst.extend(%r)"%params[x].split())
		(var,meth)=extr[x]
		if var=='SRC':
			if meth:app('lst.append(tsk.inputs%s)'%meth)
			else:app("lst.extend([a.path_from(bld.bldnode) for a in tsk.inputs])")
		elif var=='TGT':
			if meth:app('lst.append(tsk.outputs%s)'%meth)
			else:app("lst.extend([a.path_from(bld.bldnode) for a in tsk.outputs])")
		elif meth:
			if meth.startswith(':'):
				app('lst.extend([env[%r] %% x for x in env[%r]])'%(var,meth[1:]))
				dvars.extend([var,meth[1:]])
			else:
				app('lst.extend(gen.to_list(%s%s))'%(var,meth))
		else:
			app('lst.extend(to_list(env[%r]))'%var)
			if not var in dvars:dvars.append(var)
	if extr:
		if params[-1]:
			app("lst.extend(%r)"%params[-1].split())
	fun=COMPILE_TEMPLATE_NOSHELL%"\n\t".join(buf)
	Logs.debug('action: %s'%fun)
	return(funex(fun),dvars)
def compile_fun(line,shell=False):
	if line.find('<')>0 or line.find('>')>0 or line.find('&&')>0:
		shell=True
	if shell:
		return compile_fun_shell(line)
	else:
		return compile_fun_noshell(line)
def task_factory(name,func=None,vars=[],color='GREEN',ext_in=[],ext_out=[],before=[],after=[],shell=False,scan=None):
	params={'vars':vars,'color':color,'name':name,'ext_in':Utils.to_list(ext_in),'ext_out':Utils.to_list(ext_out),'before':Utils.to_list(before),'after':Utils.to_list(after),'shell':shell,'scan':scan,}
	if isinstance(func,str):
		params['run_str']=func
	else:
		params['run']=func
	cls=type(Task)(name,(Task,),params)
	global classes
	classes[name]=cls
	return cls
def always_run(cls):
	old=cls.runnable_status
	def always(self):
		ret=old(self)
		if ret==SKIP_ME:
			ret=RUN_ME
		return ret
	cls.runnable_status=always
	return cls
def update_outputs(cls):
	old_post_run=cls.post_run
	def post_run(self):
		old_post_run(self)
		for node in self.outputs:
			node.sig=Utils.h_file(node.abspath())
	cls.post_run=post_run
	old_runnable_status=cls.runnable_status
	def runnable_status(self):
		status=old_runnable_status(self)
		if status!=RUN_ME:
			return status
		try:
			bld=self.generator.bld
			new_sig=self.signature()
			prev_sig=bld.task_sigs[self.uid()]
			if prev_sig==new_sig:
				for x in self.outputs:
					if not x.sig:
						return RUN_ME
				return SKIP_ME
		except KeyError:
			pass
		except IndexError:
			pass
		return RUN_ME
	cls.runnable_status=runnable_status
	return cls
