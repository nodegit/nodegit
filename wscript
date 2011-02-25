import Options, Utils
from os import system
from os.path import exists, abspath

VERSION = '0.0.1'
APPNAME = 'nodegit2'
srcdir = '.'
blddir = 'build'

def set_options(opt):
  opt.tool_options('gcc')
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('gcc')
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')

  conf.env.append_value('LIBPATH_GIT2', abspath('vendor/libgit2/build/static/'))
  conf.env.append_value('LIB_GIT2', 'git2')
  conf.env.append_value('CPPPATH_GIT2', abspath('vendor/libgit2/build/static/'))

    #Popen('git submodule init vendor/libgit2', shell=True).wait()
    #Popen('git submodule update vendor/libgit2', shell=True).wait()
    #os.chdir('vendor/libgit2')
    #Popen('python waf configure build-static', shell=True).wait()

def build(bld):
  #libgit2 = bld.new_task_gen('wafadmin')
  #libgit2.features = 'waf configure build-static'
  #libgit2.target = 'libgit2'

  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'nodegit2'
  obj.source = 'src/base.cc src/error.cc src/reference.cc src/repo.cc src/commit.cc src/oid.cc src/revwalk.cc'
  #obj.uselib_local = 'libgit2'
