import Options
from os import unlink, symlink, popen
from os.path import exists 
from logging import fatal

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

  if not conf.check(lib='git2'):
    if not conf.check(lib='git2', libpath=['/usr/local/lib'], uselib_store='GIT2'):
      fatal('libgit2 not found')


def build(bld):
  #bld.env.append_value('LD_LIBRARY_PATH', '/usr/local/lib')
  bld.env['LD_LIBRARY_PATH'] = '/usr/local/lib'
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "git2"
  obj.source = "./src/git2.cc"
  obj.uselib = 'GIT2'
