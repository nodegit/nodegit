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
      Popen('git submodule init vendor/libgit2', shell=True).wait()
      Popen('git submodule update vendor/libgit2', shell=True).wait()
      os.chdir('vendor/libgit2')
      Popen('python waf configure build-shared install', shell=True).wait()

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'git2'
  obj.rpath = '/usr/local/lib'
  obj.source = 'src/ref.cc src/index.cc src/repo.cc src/commit.cc src/oid.cc src/revwalk.cc'
  obj.uselib = 'GIT2'
