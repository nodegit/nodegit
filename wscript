import Options, Utils
from subprocess import Popen
import os
from os import system
from os.path import exists, abspath

VERSION = '0.0.1'
APPNAME = 'nodegit'
srcdir = '.'
blddir = 'build'

def set_options(opt):
  opt.tool_options('gcc')
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('gcc')
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')

  os.chdir('vendor/libgit2')
  Popen('python waf configure', shell=True).wait()

  conf.env.append_value('LIBPATH_GIT2', abspath('build/shared'))
  conf.env.append_value('LIB_GIT2', 'git2')

def build(bld):
  try: os.chdir('vendor/libgit2')
  except: pass
  Popen('python waf build-shared', shell=True).wait()

  os.chdir('../../')
    
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'git'
  obj.source = 'src/base.cc src/sig.cc src/blob.cc src/error.cc src/object.cc src/reference.cc src/repo.cc src/commit.cc src/oid.cc src/revwalk.cc src/tree.cc'
  obj.rpath = abspath('build/shared')
  obj.uselib = 'GIT2'
