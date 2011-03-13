#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys,re
from waflib import Utils,Build
from waflib.Configure import conf
def get_extensions(lst):
	ret=[]
	for x in Utils.to_list(lst):
		try:
			if not isinstance(x,str):
				x=x.name
			ret.append(x[x.rfind('.')+1:])
		except:
			pass
	return ret
def sniff_features(**kw):
	exts=get_extensions(kw['source'])
	type=kw['_type']
	if'cxx'in exts or'cpp'in exts or'c++'in exts or'cc'in exts:
		if type=='program':
			return'cxx cxxprogram'
		if type=='shlib':
			return'cxx cxxshlib'
		if type=='stlib':
			return'cxx cxxstlib'
		return'cxx'
	if'd'in exts:
		if type=='program':
			return'd dprogram'
		if type=='shlib':
			return'd dshlib'
		if type=='stlib':
			return'd dstlib'
		return'd'
	if'vala'in exts or'c'in exts:
		if type=='program':
			return'c cprogram'
		if type=='shlib':
			return'c cshlib'
		if type=='stlib':
			return'c cstlib'
		return'c'
	if'java'in exts:
		return'java'
	return''
def set_features(kw,_type):
	kw['_type']=_type
	kw['features']=Utils.to_list(kw.get('features',[]))+Utils.to_list(sniff_features(**kw))
def program(bld,*k,**kw):
	set_features(kw,'program')
	return bld(*k,**kw)
def shlib(bld,*k,**kw):
	set_features(kw,'shlib')
	return bld(*k,**kw)
def stlib(bld,*k,**kw):
	set_features(kw,'stlib')
	return bld(*k,**kw)
def objects(bld,*k,**kw):
	set_features(kw,'objects')
	return bld(*k,**kw)

conf(program)
conf(shlib)
conf(stlib)
conf(objects)