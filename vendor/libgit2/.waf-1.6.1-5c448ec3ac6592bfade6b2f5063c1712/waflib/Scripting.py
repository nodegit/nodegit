#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,shutil,traceback,datetime,inspect,errno,sys
from waflib import Utils,Configure,Logs,Options,ConfigSet,Context,Errors,Build,Node
build_dir_override=None
no_climb_commands=['configure']
def waf_entry_point(current_directory,version,wafdir):
	Logs.init_log()
	if Context.WAFVERSION!=version:
		Logs.error('Waf script %r and library %r do not match (directory %r)'%(version,WAFVERSION,wafdir))
		sys.exit(1)
	Context.waf_dir=wafdir
	Context.launch_dir=current_directory
	cur=current_directory
	while cur:
		lst=os.listdir(cur)
		if Options.lockfile in lst:
			env=ConfigSet.ConfigSet()
			try:
				env.load(os.path.join(cur,Options.lockfile))
			except Exception:
				pass
			else:
				Context.run_dir=env.run_dir
				Context.top_dir=env.top_dir
				Context.out_dir=env.out_dir
				try:
					os.stat(Context.run_dir)
				except:
					Context.run_dir=cur
				break
		if not Context.run_dir:
			if Context.WSCRIPT_FILE in lst:
				Context.run_dir=cur
		next=os.path.dirname(cur)
		if next==cur:
			break
		cur=next
		for k in no_climb_commands:
			if k in sys.argv:
				break
		else:
			continue
		break
	if not Context.run_dir:
		if'-h'in sys.argv or'--help'in sys.argv:
			Logs.warn('No wscript file found: the help message may be incomplete')
			opt_obj=Options.OptionsContext()
			opt_obj.curdir=current_directory
			opt_obj.parse_args()
			sys.exit(0)
		elif'--version'in sys.argv:
			opt_obj=Options.OptionsContext()
			opt_obj.curdir=current_directory
			opt_obj.parse_args()
			sys.exit(0)
		Logs.error('Waf: Run from a directory containing a file named %r'%Context.WSCRIPT_FILE)
		sys.exit(1)
	try:
		os.chdir(Context.run_dir)
	except OSError:
		Logs.error('Waf: The folder %r is unreadable'%Context.run_dir)
		sys.exit(1)
	try:
		set_main_module(Context.run_dir+os.sep+Context.WSCRIPT_FILE)
	except Errors.WafError ,e:
		Logs.pprint('RED',e.verbose_msg)
		Logs.error(str(e))
		sys.exit(1)
	except Exception ,e:
		Logs.error('Waf: The wscript in %r is unreadable'%Context.run_dir,e)
		traceback.print_exc(file=sys.stdout)
		sys.exit(2)
	parse_options()
	try:
		run_commands()
	except Errors.WafError ,e:
		if Logs.verbose>1:
			Logs.pprint('RED',e.verbose_msg)
		Logs.error(e.msg)
		sys.exit(1)
	except Exception ,e:
		traceback.print_exc(file=sys.stdout)
		sys.exit(2)
	except KeyboardInterrupt:
		Logs.pprint('RED','Interrupted')
		sys.exit(68)
def set_main_module(file_path):
	Context.g_module=Context.load_module(file_path)
	Context.g_module.root_path=file_path
	def set_def(obj):
		name=obj.__name__
		if not name in Context.g_module.__dict__:
			setattr(Context.g_module,name,obj)
	for k in[update,dist,distclean,distcheck]:
		set_def(k)
	if not'init'in Context.g_module.__dict__:
		Context.g_module.init=Utils.nada
	if not'shutdown'in Context.g_module.__dict__:
		Context.g_module.shutdown=Utils.nada
	if not'options'in Context.g_module.__dict__:
		Context.g_module.options=Utils.nada
def parse_options():
	opt=Options.OptionsContext().execute()
	if not Options.commands:
		Options.commands=['build']
	Logs.verbose=Options.options.verbose
	Logs.init_log()
	if Options.options.zones:
		Logs.zones=Options.options.zones.split(',')
		if not Logs.verbose:
			Logs.verbose=1
	elif Logs.verbose>0:
		Logs.zones=['runner']
	if Logs.verbose>2:
		Logs.zones=['*']
def run_command(cmd_name):
	ctx=Context.create_context(cmd_name)
	ctx.options=Options.options
	ctx.cmd=cmd_name
	ctx.execute()
	return ctx
def run_commands():
	run_command('init')
	while Options.commands:
		cmd_name=Options.commands.pop(0)
		timer=Utils.Timer()
		run_command(cmd_name)
		if not Options.options.progress_bar:
			elapsed=' (%s)'%str(timer)
			Logs.info('%r finished successfully%s'%(cmd_name,elapsed))
	run_command('shutdown')
def _can_distclean(name):
	for k in'.o .moc .exe'.split():
		if name.endswith(k):
			return True
	return False
def distclean_dir(dirname):
	for(root,dirs,files)in os.walk(dirname):
		for f in files:
			if _can_distclean(f):
				fname=root+os.sep+f
				try:
					os.unlink(fname)
				except:
					Logs.warn('could not remove %r'%fname)
	for x in[DBFILE,'config.log']:
		try:
			os.unlink(x)
		except:
			pass
	try:
		shutil.rmtree('c4che')
	except:
		pass
def distclean(ctx):
	'''removes the build directory'''
	lst=os.listdir('.')
	for f in lst:
		if f==Options.lockfile:
			try:
				proj=ConfigSet.ConfigSet(f)
			except:
				Logs.warn('could not read %r'%f)
				continue
			if proj['out_dir']!=proj['top_dir']:
				try:
					shutil.rmtree(proj['out_dir'])
				except IOError:
					pass
				except OSError ,e:
					if e.errno!=errno.ENOENT:
						Logs.warn('project %r cannot be removed'%proj[Context.OUT])
			else:
				distclean_dir(proj['out_dir'])
			for k in(proj['out_dir'],proj['top_dir'],proj['run_dir']):
				try:
					os.remove(os.path.join(k,Options.lockfile))
				except OSError ,e:
					if e.errno!=errno.ENOENT:
						Logs.warn('file %r cannot be removed'%f)
		if f.startswith('.waf-')and not Options.commands:
			shutil.rmtree(f,ignore_errors=True)
class Dist(Context.Context):
	cmd='dist'
	fun='dist'
	algo='tar.bz2'
	ext_algo={}
	def execute(self):
		self.recurse([os.path.dirname(Context.g_module.root_path)])
		self.archive()
	def archive(self):
		import tarfile
		arch_name=self.get_arch_name()
		try:
			self.base_path
		except:
			self.base_path=self.path
		node=self.base_path.make_node(arch_name)
		try:
			node.delete()
		except:
			pass
		files=self.get_files()
		if self.algo.startswith('tar.'):
			tar=tarfile.open(arch_name,'w:'+self.algo.replace('tar.',''))
			for x in files:
				tinfo=tar.gettarinfo(name=x.abspath(),arcname=self.get_tar_prefix()+'/'+x.path_from(self.base_path))
				tinfo.uid=0
				tinfo.gid=0
				tinfo.uname='root'
				tinfo.gname='root'
				fu=None
				try:
					fu=open(x.abspath())
					tar.addfile(tinfo,fileobj=fu)
				finally:
					fu.close()
			tar.close()
		elif self.algo=='zip':
			import zipfile
			zip=zipfile.ZipFile(arch_name,'w',compression=zipfile.ZIP_DEFLATED)
			for x in files:
				archive_name=self.get_base_name()+'/'+x.path_from(self.base_path)
				zip.write(x.abspath(),archive_name,zipfile.ZIP_DEFLATED)
			zip.close()
		else:
			self.fatal('Valid algo types are tar.bz2, tar.gz or zip')
		try:
			from hashlib import sha1 as sha
		except ImportError:
			from sha import sha
		try:
			digest=" (sha=%r)"%sha(node.read()).hexdigest()
		except:
			digest=''
		Logs.info('New archive created: %s%s'%(self.arch_name,digest))
	def get_tar_prefix(self):
		try:
			return self.tar_prefix
		except:
			return self.get_base_name()
	def get_arch_name(self):
		try:
			self.arch_name
		except:
			self.arch_name=self.get_base_name()+'.'+self.ext_algo.get(self.algo,self.algo)
		return self.arch_name
	def get_base_name(self):
		try:
			self.base_name
		except:
			appname=getattr(Context.g_module,Context.APPNAME,'noname')
			version=getattr(Context.g_module,Context.VERSION,'1.0')
			self.base_name=appname+'-'+version
		return self.base_name
	def get_excl(self):
		try:
			return self.excl
		except:
			self.excl=Node.exclude_regs+' **/waf-1.6.* **/.waf-1.6* **/*~ **/*.rej **/*.orig **/*.pyc **/*.pyo **/*.bak **/*.swp **/.lock-w*'
			nd=self.root.find_node(Context.out_dir)
			if nd:
				self.excl+=' '+nd.path_from(self.base_path)
			return self.excl
	def get_files(self):
		try:
			files=self.files
		except:
			files=self.base_path.ant_glob('**/*',excl=self.get_excl())
		return files
def dist(ctx):
	'''makes a tarball for redistributing the sources'''
	pass
class DistCheck(Dist):
	fun='distcheck'
	cmd='distcheck'
	def execute(self):
		self.recurse([os.path.dirname(Context.g_module.root_path)])
		self.archive()
		self.check()
	def check(self):
		import tempfile,tarfile
		t=None
		try:
			t=tarfile.open(self.get_arch_name())
			for x in t:
				t.extract(x)
		finally:
			if t:
				t.close()
		instdir=tempfile.mkdtemp('.inst',self.get_base_name())
		ret=Utils.subprocess.Popen([sys.argv[0],'configure','install','uninstall','--destdir='+instdir],cwd=self.get_base_name()).wait()
		if ret:
			raise Errors.WafError('distcheck failed with code %i'%ret)
		if os.path.exists(instdir):
			raise Errors.WafError('distcheck succeeded, but files were left in %s'%instdir)
		shutil.rmtree(self.get_base_name())
def distcheck(ctx):
	'''checks if the project compiles (tarball from 'dist')'''
	pass
def update(ctx):
	lst=os.listdir(Context.waf_dir+'/waflib/extras')
	for x in lst:
		if not x.endswith('.py'):
			continue
		tool=x.replace('.py','')
		Configure.download_tool(tool,force=True)
def autoconfigure(execute_method):
	def execute(self):
		if not Configure.autoconfig:
			return execute_method(self)
		env=ConfigSet.ConfigSet()
		do_config=False
		try:
			env.load(os.path.join(Context.top_dir,Options.lockfile))
		except Exception ,e:
			Logs.warn('Configuring the project')
			do_config=True
		else:
			h=0
			for f in env['files']:
				h=hash((h,Utils.readf(f,'rb')))
			do_config=h!=env.hash
		if do_config:
			Options.commands.insert(0,self.cmd)
			Options.commands.insert(0,'configure')
			return
		return execute_method(self)
	return execute
Build.BuildContext.execute=autoconfigure(Build.BuildContext.execute)
