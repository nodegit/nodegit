#! /usr/bin/env python
# encoding: utf-8
# WARNING! All changes made to this file will be lost!

import os,sys,random
try:
	from queue import Queue
except:
	from Queue import Queue
from waflib import Utils,Logs,Task,Errors
GAP=10
MAXJOBS=999
class TaskConsumer(Utils.threading.Thread):
	def __init__(self):
		Utils.threading.Thread.__init__(self)
		self.ready=Queue()
		self.setDaemon(1)
		self.start()
	def run(self):
		try:
			self.loop()
		except:
			pass
	def loop(self):
		while 1:
			tsk=self.ready.get()
			tsk.process()
pool=Queue()
def get_pool():
	try:
		return pool.get(False)
	except:
		return TaskConsumer()
def put_pool(x):
	pool.put(x)
class Parallel(object):
	def __init__(self,bld,j=2):
		self.numjobs=j
		self.bld=bld
		self.outstanding=[]
		self.maxjobs=MAXJOBS
		self.frozen=[]
		self.out=Queue(0)
		self.count=0
		self.processed=1
		self.stop=False
		self.error=[]
		self.biter=None
		self.dirty=False
	def get_next_task(self):
		if not self.outstanding:
			return None
		return self.outstanding.pop(0)
	def postpone(self,tsk):
		if random.randint(0,1):
			self.frozen.insert(0,tsk)
		else:
			self.frozen.append(tsk)
	def refill_task_list(self):
		while self.count>self.numjobs*GAP or self.count>=self.maxjobs:
			self.get_out()
		while not self.outstanding:
			if self.count:
				self.get_out()
			elif self.frozen:
				try:
					cond=self.deadlock==self.processed
				except:
					pass
				else:
					if cond:
						msg='check the build order for the tasks'
						for tsk in self.frozen:
							if not tsk.run_after:
								msg='check the methods runnable_status'
								break
						lst=[]
						for tsk in self.frozen:
							lst.append('%s\t-> %r'%(repr(tsk),[id(x)for x in tsk.run_after]))
						raise Errors.WafError("Deadlock detected: %s%s"%(msg,''.join(lst)))
				self.deadlock=self.processed
			if self.frozen:
				self.outstanding+=self.frozen
				self.frozen=[]
			elif not self.count:
				self.outstanding.extend(self.biter.next())
				self.total=self.bld.total()
				break
	def add_more_tasks(self,tsk):
		if getattr(tsk,'more_tasks',None):
			self.outstanding+=tsk.more_tasks
			self.total+=len(tsk.more_tasks)
	def get_out(self):
		tsk=self.out.get()
		if not self.stop:
			self.add_more_tasks(tsk)
		self.count-=1
		self.dirty=True
	def error_handler(self,tsk):
		if not self.bld.keep:
			self.stop=True
		self.error.append(tsk)
	def add_task(self,tsk):
		try:
			pool=self.pool
		except AttributeError:
			pool=self.pool=[get_pool()for i in range(self.numjobs)]
		a=pool[random.randint(0,len(pool)-1)]
		siz=a.ready.qsize()
		if not siz:
			a.ready.put(tsk)
			return
		b=pool[random.randint(0,len(pool)-1)]
		if siz>b.ready.qsize():
			b.ready.put(tsk)
		else:
			a.ready.put(tsk)
	def start(self):
		self.total=self.bld.total()
		while not self.stop:
			self.refill_task_list()
			tsk=self.get_next_task()
			if not tsk:
				if self.count:
					continue
				else:
					break
			if tsk.hasrun:
				self.processed+=1
				continue
			try:
				st=tsk.runnable_status()
			except Exception ,e:
				self.processed+=1
				if self.stop and not self.bld.keep:
					tsk.hasrun=Task.SKIPPED
					continue
				tsk.err_msg=Utils.ex_stack()
				tsk.hasrun=Task.EXCEPTION
				self.error_handler(tsk)
				continue
			if st==Task.ASK_LATER:
				self.postpone(tsk)
				if self.outstanding:
					for x in tsk.run_after:
						if x in self.outstanding:
							self.outstanding.remove(x)
							self.outstanding.insert(0,x)
			elif st==Task.SKIP_ME:
				self.processed+=1
				tsk.hasrun=Task.SKIPPED
				self.add_more_tasks(tsk)
			else:
				tsk.position=(self.processed,self.total)
				self.count+=1
				tsk.master=self
				self.processed+=1
				if self.numjobs==1:
					tsk.process()
				else:
					self.add_task(tsk)
		while self.error and self.count:
			self.get_out()
		try:
			while self.pool:
				x=self.pool.pop()
				put_pool(x)
		except AttributeError:
			pass
		assert(self.count==0 or self.stop)
