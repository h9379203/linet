#!/usr/bin/env python
# -*- coding: utf-8 -*-
# UDP sender
import socket
import sys
import string
import time

# ---------------------------[ End of Variable definition ]-------------------------------
# DESC : 사용법을 출력한다.
# ARGV : none
# RET  : none
def usages():
    print sys.argv[0], "<host> <port>"
    sys.exit()
# ---------------------------[ End of Function Definition ]-------------------------------

if len(sys.argv) != 3:
    usages()

HOST = sys.argv[1]
PORT = int(sys.argv[2])
print "Destination Address :", HOST, PORT
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while 1:
    sbuf = 'Time:' + str(time.time())
    s.sendto(sbuf, 0, (HOST, PORT))
    print "[Send] (" + str(len(sbuf)) + ") ", sbuf
    rbuf = s.recvfrom(2048)
    print "[Recv] Addr" + str(rbuf[1]) + " Len(" + str(len(rbuf[0])) + ")"
    print "       Buff(" + rbuf[0] + ")" # rbuf[0] is data, [1] is address
    time.sleep(2)
s.close()
