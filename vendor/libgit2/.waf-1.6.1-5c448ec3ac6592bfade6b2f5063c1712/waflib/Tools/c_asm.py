#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys
import waflib.Task
from waflib.TaskGen import extension,feature
class asm(waflib.Task.Task):
	color='BLUE'
	run_str='${AS} ${ASFLAGS} ${CPPPATH_ST:INCPATHS} ${AS_SRC_F}${SRC} ${AS_TGT_F}${TGT}'
def asm_hook(self,node):
	return self.create_compiled_task('asm',node)

extension('.s','.S','.asm','.ASM','.spp','.SPP')(asm_hook)