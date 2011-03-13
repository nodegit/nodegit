#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os.path,shutil,re
from waflib import Context,Task,Runner,Utils,Logs,Build,Node,Options,Errors
from waflib.TaskGen import extension,after,before
from waflib.Configure import conf
class valac_task(Task.Task):
	vars=["VALAC","VALAC_VERSION","VALAFLAGS"]
	ext_out=['.h']
	def run(self):
		env=self.env
		cmd=[env['VALAC'],'-C','--quiet']
		cmd.extend(Utils.to_list(env['VALAFLAGS']))
		if self.threading:
			cmd.append('--thread')
		if self.profile:
			cmd.append('--profile=%s'%self.profile)
		if self.target_glib:
			cmd.append('--target-glib=%s'%self.target_glib)
		if self.is_lib:
			output_dir=self.outputs[0].bld_dir()
			cmd.append('--library='+self.target)
			for x in self.outputs:
				if x.name.endswith('.h'):
					cmd.append('--header='+x.name)
			if self.gir:
				cmd.append('--gir=%s.gir'%self.gir)
		for vapi_dir in self.vapi_dirs:
			cmd.append('--vapidir=%s'%vapi_dir)
		for package in self.packages:
			cmd.append('--pkg=%s'%package)
		for package in self.packages_private:
			cmd.append('--pkg=%s'%package)
		for define in self.vala_defines:
			cmd.append('--define=%s'%define)
		cmd.extend([a.abspath()for a in self.inputs])
		ret=self.exec_command(cmd,cwd=self.outputs[0].parent.abspath())
		if ret:
			return ret
		for x in self.outputs:
			if id(x.parent)!=id(self.outputs[0].parent):
				shutil.move(self.outputs[0].parent.abspath()+os.sep+x.name,x.abspath())
		if self.packages and getattr(self,'deps_node',None):
			self.deps_node.write('\n'.join(self.packages))
		return ret
def vala_file(self,node):
	valatask=getattr(self,"valatask",None)
	if not valatask:
		def _get_api_version():
			api_version=getattr(Context.g_module,'API_VERSION',None)
			if api_version==None:
				version=Context.g_module.VERSION.split(".")
				if version[0]=="0":
					api_version="0."+version[1]
				else:
					api_version=version[0]+".0"
			return api_version
		valatask=self.create_task('valac')
		self.valatask=valatask
		self.includes=Utils.to_list(getattr(self,'includes',[]))
		self.uselib=self.to_list(getattr(self,'uselib',[]))
		valatask.packages=[]
		valatask.packages_private=Utils.to_list(getattr(self,'packages_private',[]))
		valatask.vapi_dirs=[]
		valatask.target=self.target
		valatask.threading=False
		valatask.install_path=getattr(self,'install_path','')
		valatask.profile=getattr(self,'profile','gobject')
		valatask.vala_defines=getattr(self,'vala_defines',[])
		valatask.target_glib=None
		valatask.gir=getattr(self,'gir',None)
		valatask.gir_path=getattr(self,'gir_path','${DATAROOTDIR}/gir-1.0')
		valatask.vapi_path=getattr(self,'vapi_path','${DATAROOTDIR}/vala/vapi')
		valatask.pkg_name=getattr(self,'pkg_name',self.env['PACKAGE'])
		valatask.header_path=getattr(self,'header_path','${INCLUDEDIR}/%s-%s'%(valatask.pkg_name,_get_api_version()))
		valatask.is_lib=False
		if not'cprogram'in self.features:
			valatask.is_lib=True
		packages=Utils.to_list(getattr(self,'packages',[]))
		vapi_dirs=Utils.to_list(getattr(self,'vapi_dirs',[]))
		includes=[]
		if hasattr(self,'use'):
			local_packages=Utils.to_list(self.use)[:]
			seen=[]
			while len(local_packages)>0:
				package=local_packages.pop()
				if package in seen:
					continue
				seen.append(package)
				try:
					package_obj=self.bld.get_tgen_by_name(package)
				except Errors.WafError:
					continue
				package_name=package_obj.target
				package_node=package_obj.path
				package_dir=package_node.path_from(self.path)
				for task in package_obj.tasks:
					for output in task.outputs:
						if output.name==package_name+".vapi":
							valatask.set_run_after(task)
							if package_name not in packages:
								packages.append(package_name)
							if package_dir not in vapi_dirs:
								vapi_dirs.append(package_dir)
							if package_dir not in includes:
								includes.append(package_dir)
				if hasattr(package_obj,'use'):
					lst=self.to_list(package_obj.use)
					lst.reverse()
					local_packages=[pkg for pkg in lst if pkg not in seen]+local_packages
		valatask.packages=packages
		for vapi_dir in vapi_dirs:
			try:
				valatask.vapi_dirs.append(self.path.find_dir(vapi_dir).abspath())
				valatask.vapi_dirs.append(self.path.find_dir(vapi_dir).get_bld().abspath())
			except AttributeError:
				Logs.warn("Unable to locate Vala API directory: '%s'"%vapi_dir)
		self.includes.append(self.bld.srcnode.abspath())
		self.includes.append(self.bld.bldnode.abspath())
		for include in includes:
			try:
				self.includes.append(self.path.find_dir(include).abspath())
				self.includes.append(self.path.find_dir(include).get_bld().abspath())
			except AttributeError:
				Logs.warn("Unable to locate include directory: '%s'"%include)
		if valatask.profile=='gobject':
			if hasattr(self,'target_glib'):
				Logs.warn('target_glib on vala tasks is not supported --vala-target-glib=MAJOR.MINOR from the vala tool options')
			if getattr(Options.options,'vala_target_glib',None):
				valatask.target_glib=Options.options.vala_target_glib
			if not'GOBJECT'in self.uselib:
				self.uselib.append('GOBJECT')
		if hasattr(self,'threading'):
			if valatask.profile=='gobject':
				valatask.threading=self.threading
				if not'GTHREAD'in self.uselib:
					self.uselib.append('GTHREAD')
			else:
				Logs.warn("Profile %s does not have threading support"%valatask.profile)
		if valatask.is_lib:
			valatask.outputs.append(self.path.find_or_declare('%s.h'%self.target))
			valatask.outputs.append(self.path.find_or_declare('%s.vapi'%self.target))
			if valatask.gir:
				valatask.outputs.append(self.path.find_or_declare('%s.gir'%self.gir))
			if valatask.packages:
				d=self.path.find_or_declare('%s.deps'%self.target)
				valatask.outputs.append(d)
				valatask.deps_node=d
	valatask.inputs.append(node)
	c_node=node.change_ext('.c')
	valatask.outputs.append(c_node)
	self.source.append(c_node)
	if valatask.is_lib:
		headers_list=[o for o in valatask.outputs if o.suffix()==".h"]
		self.install_vheader=self.bld.install_files(valatask.header_path,headers_list,self.env)
		vapi_list=[o for o in valatask.outputs if(o.suffix()in(".vapi",".deps"))]
		self.install_vapi=self.bld.install_files(valatask.vapi_path,vapi_list,self.env)
		gir_list=[o for o in valatask.outputs if o.suffix()==".gir"]
		self.install_gir=self.bld.install_files(valatask.gir_path,gir_list,self.env)
valac_task=Task.update_outputs(valac_task)
def find_valac(self,valac_name,min_version):
	valac=self.find_program(valac_name,var='VALAC')
	try:
		output=self.cmd_and_log(valac+' --version')
	except Exception:
		valac_version=None
	else:
		ver=re.search(r'\d+.\d+.\d+',output).group(0).split('.')
		valac_version=tuple([int(x)for x in ver])
	self.msg('Checking for %s version >= %r'%(valac_name,min_version),valac_version,valac_version and valac_version>=min_version)
	if valac and valac_version<min_version:
		self.fatal("%s version %r is too old, need >= %r"%(valac_name,valac_version,min_version))
	self.env['VALAC_VERSION']=valac_version
	return valac
def check_vala(self,min_version=(0,8,0),branch=None):
	if not branch:
		branch=min_version[:2]
	try:
		find_valac(self,'valac-%d.%d'%(branch[0],branch[1]),min_version)
	except self.errors.ConfigurationError:
		find_valac(self,'valac',min_version)
def check_vala_deps(self):
	if not self.env['HAVE_GOBJECT']:
		pkg_args={'package':'gobject-2.0','uselib_store':'GOBJECT','args':'--cflags --libs'}
		if getattr(Options.options,'vala_target_glib',None):
			pkg_args['atleast_version']=Options.options.vala_target_glib
		self.check_cfg(**pkg_args)
	if not self.env['HAVE_GTHREAD']:
		pkg_args={'package':'gthread-2.0','uselib_store':'GTHREAD','args':'--cflags --libs'}
		if getattr(Options.options,'vala_target_glib',None):
			pkg_args['atleast_version']=Options.options.vala_target_glib
		self.check_cfg(**pkg_args)
def configure(self):
	self.load('gnu_dirs')
	self.check_vala_deps()
	self.check_vala()
def options(opt):
	opt.load('gnu_dirs')
	valaopts=opt.add_option_group('Vala Compiler Options')
	valaopts.add_option('--vala-target-glib',default=None,dest='vala_target_glib',metavar='MAJOR.MINOR',help='Target version of glib for Vala GObject code generation')

extension('.vala','.gs')(vala_file)
conf(find_valac)
conf(check_vala)
conf(check_vala_deps)