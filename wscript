import Options
import os
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
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.rpath = "/usr/local/lib"
  obj.target = "git2"
  obj.source = "./src/repo.cc"
  obj.uselib = 'GIT2'
