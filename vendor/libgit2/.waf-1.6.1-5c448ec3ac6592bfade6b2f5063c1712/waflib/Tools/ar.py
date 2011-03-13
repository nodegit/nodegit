#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os
from waflib.Configure import conf
def find_ar(conf):
	conf.load('ar')
def configure(conf):
	conf.find_program('ar',var='AR')
	conf.env.ARFLAGS='rcs'

conf(find_ar)