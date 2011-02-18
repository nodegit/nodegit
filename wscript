import Options
import os
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
      os.popen('git submodule init vendor/libgit2')
      os.popen('git submodule update vendor/libgit2')
      os.chdir('./vendor/libgit2')
      os.popen('sudo python waf configure build-shared install')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'git2'
  obj.rpath = '/usr/local/lib:'+os.getcwd()+'/vendor/libgit2/build/shared'
  obj.source = './src/index.cc ./src/repo.cc ./src/oid.cc'
  obj.uselib = 'GIT2'
