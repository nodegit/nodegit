#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,shutil,sys,platform
from waflib import TaskGen,Task,Build,Options,Utils
from waflib.TaskGen import taskgen_method,feature,after,before
app_info='''
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="0.9">
<dict>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleGetInfoString</key>
	<string>Created by Waf</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>NOTE</key>
	<string>THIS IS A GENERATED FILE, DO NOT MODIFY</string>
	<key>CFBundleExecutable</key>
	<string>%s</string>
</dict>
</plist>
'''
def set_macosx_deployment_target(self):
	if self.env['MACOSX_DEPLOYMENT_TARGET']:
		os.environ['MACOSX_DEPLOYMENT_TARGET']=self.env['MACOSX_DEPLOYMENT_TARGET']
	elif'MACOSX_DEPLOYMENT_TARGET'not in os.environ:
		if sys.platform=='darwin':
			os.environ['MACOSX_DEPLOYMENT_TARGET']='.'.join(platform.mac_ver()[0].split('.')[:2])
def create_bundle_dirs(self,name,out):
	bld=self.bld
	dir=out.parent.get_dir(name)
	if not dir:
		dir=out.__class__(name,out.parent,1)
		bld.rescan(dir)
		contents=out.__class__('Contents',dir,1)
		bld.rescan(contents)
		macos=out.__class__('MacOS',contents,1)
		bld.rescan(macos)
	return dir
def bundle_name_for_output(out):
	name=out.name
	k=name.rfind('.')
	if k>=0:
		name=name[:k]+'.app'
	else:
		name=name+'.app'
	return name
def create_task_macapp(self):
	if self.env['MACAPP']or getattr(self,'mac_app',False):
		apptask=self.create_task('macapp')
		apptask.set_inputs(self.link_task.outputs)
		out=self.link_task.outputs[0]
		name=bundle_name_for_output(out)
		dir=self.create_bundle_dirs(name,out)
		n1=dir.find_or_declare(['Contents','MacOS',out.name])
		apptask.set_outputs([n1])
		apptask.chmod=Utils.O755
		apptask.install_path=os.path.join(self.install_path,name,'Contents','MacOS')
		self.apptask=apptask
def create_task_macplist(self):
	if self.env['MACAPP']or getattr(self,'mac_app',False):
		if not getattr(self,'mac_plist',False):
			self.mac_plist=app_info
		plisttask=self.create_task('macplist')
		plisttask.set_inputs(self.link_task.outputs)
		out=self.link_task.outputs[0]
		self.mac_plist=self.mac_plist%(out.name)
		name=bundle_name_for_output(out)
		dir=self.create_bundle_dirs(name,out)
		n1=dir.find_or_declare(['Contents','Info.plist'])
		plisttask.set_outputs([n1])
		plisttask.mac_plist=self.mac_plist
		plisttask.install_path=os.path.join(self.install_path,name,'Contents')
		self.plisttask=plisttask
def apply_bundle(self):
	if not('cshlib'in self.features or'cxxshlib'in self.features):
		return
	if self.env['MACBUNDLE']or getattr(self,'mac_bundle',False):
		self.env['cshlib_PATTERN']=self.env['cxxshlib_PATTERN']=self.env['macbundle_PATTERN']
		uselib=self.uselib=self.to_list(getattr(self,'use',[]))
		if not'MACBUNDLE'in uselib:
			uselib.append('MACBUNDLE')
def apply_bundle_remove_dynamiclib(self):
	if self.env['MACBUNDLE']or getattr(self,'mac_bundle',False):
		if not getattr(self,'vnum',None):
			try:
				self.env['LINKFLAGS'].remove('-dynamiclib')
			except ValueError:
				pass
app_dirs=['Contents','Contents/MacOS','Contents/Resources']
class macapp(Task.Task):
	color='PINK'
	def run(self):
		shutil.copy2(self.inputs[0].srcpath(),self.outputs[0].abspath(self.env))
class macplist(Task.Task):
	color='PINK'
	ext_in=['.bin']
	def run(self):
		self.outputs[0].write(self.mac_plist)

feature('c','cxx')(set_macosx_deployment_target)
before('apply_lib_vars')(set_macosx_deployment_target)
taskgen_method(create_bundle_dirs)
feature('cprogram','cxxprogram')(create_task_macapp)
after('apply_link')(create_task_macapp)
feature('cprogram','cxxprogram')(create_task_macplist)
after('apply_link')(create_task_macplist)
feature('c','cxx')(apply_bundle)
before('apply_link','propagate_uselib_vars')(apply_bundle)
feature('cshlib','cxxshlib')(apply_bundle_remove_dynamiclib)
after('apply_link')(apply_bundle_remove_dynamiclib)