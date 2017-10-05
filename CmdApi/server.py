#!/usr/bin/env python
#-*- encoding: utf-8 -*-
'''
Created on 2017-07-07 11:13:58

@author: vforbox <vforbox@gmail.com>
'''

import lib.web
import sys

from ctrl.index import IndexHandler
from ctrl.api import ApiHandler

# 解决 windows 编码问题
if 'utf-8' != sys.getdefaultencoding():
    reload(sys)
    sys.setdefaultencoding('utf-8')

urls = (
    '/'	  ,  'IndexHandler',
    '/api',  'ApiHandler',
)

lib.web.config.debug = False

if __name__ == "__main__":
    app = lib.web.application(urls, globals())
    app.run()
