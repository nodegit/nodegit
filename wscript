import Options
import os
from subprocess import Popen
from os import unlink, symlink, popen
from os.path import exists 
from logging import fatal

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')

  if not conf.check(lib='git2'):
    if not conf.check(lib='git2', libpath=['/usr/local/lib'], uselib_store='GIT2'):
      print 'libgit2 is required in the folder /usr/local/lib'
      print ''
      print 'Run the following commands to install:'
      print 'git submodule init vendor/libgit2'
      print 'git submodule update vendor/libgit2'
      print 'vendor/libgit2'
      print 'python waf configure build-shared install'

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'nodegit2'
  obj.rpath = '/usr/local/lib'
  obj.source = 'src/base.cc src/error.cc src/reference.cc src/repo.cc src/commit.cc src/oid.cc src/revwalk.cc'
  obj.uselib = 'GIT2'
