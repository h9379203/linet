#!/usr/bin/env python
# -*- coding: utf-8 -*-
# UNIX domain socket with SOCK_STREAM
import socket
import sys
import string
import time

# ---------------------------[ End of Variable definition ]-------------------------------
# DESC : 사용법을 출력한다.
# ARGV : none
# RET  : none
def usages():
    print "Usage :", sys.argv[0], "<path>"
    sys.exit()
# ---------------------------[ End of Function Definition ]-------------------------------
if len(sys.argv) != 2:
    usages()

PATH = sys.argv[1]
print "UNIX Domain Socket : ", PATH
s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
s.connect((PATH))
while 1:
    sbuf = 'Time:' + str(time.time())
    s.send(sbuf, 0)
    print "[Send:" + str(len(sbuf)) + "] ", sbuf
    rbuf = s.recv(2048)
    print "[Recv:" + str(len(rbuf)) + "] ", rbuf, "[Echo Msg]"
    time.sleep(1)
s.close()
