#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

from waflib import Utils
from waflib.Configure import conf
def d_platform_flags(self):
	v=self.env
	if not v.DEST_OS:
		v.DEST_OS=Utils.unversioned_sys_platform()
	if Utils.destos_to_binfmt(self.env.DEST_OS)=='pe':
		v['dprogram_PATTERN']='%s.exe'
		v['dshlib_PATTERN']='lib%s.dll'
		v['dstlib_PATTERN']='lib%s.a'
	else:
		v['dprogram_PATTERN']='%s'
		v['dshlib_PATTERN']='lib%s.so'
		v['dstlib_PATTERN']='lib%s.a'
DLIB="""
version(D_Version2) {
	import std.stdio;
	int main() {
		writefln("phobos2");
		return 0;
	}
} else {
	version(Tango) {
		import tango.stdc.stdio;
		int main() {
			printf("tango");
			return 0;
		}
	} else {
		import std.stdio;
		int main() {
			writefln("phobos1");
			return 0;
		}
	}
}
"""
def check_dlibrary(self):
	ret=self.check_cc(features='d dprogram',fragment=DLIB,compile_filename='test.d',execute=True,define_ret=True)
	self.env.DLIBRARY=ret.strip()

conf(d_platform_flags)
conf(check_dlibrary)