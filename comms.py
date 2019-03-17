#!/usr/bin/env python

import sys
import os
import pygame
import numpy as np
import socket

class comms():
    def __init__(self):
        UDP_IP="127.0.0.1"
        UDP_PORT=15006
        self.sock = socket.socket()
        self.sock.bind((UDP_IP,UDP_PORT))
        print "waiting for socket connection"
        # wait for client
        # this server has only one client
        self.sock.listen(1)
        # accept the client's connection
        self.conn, self.addr = self.sock.accept()
        print("Accepted connection from: " + str(self.addr))
        
    def lookForIncoming(self,numItems):
        try:
            # data will be preceded by type
            msgTypeData = self.conn.recv(1 * 4)
            msgType = np.fromstring(msgTypeData,dtype='>i4')
            data = self.conn.recv(numItems * 4)
            if (len(data) / 4) == numItems:
                array = np.fromstring(data,dtype='>i4')
                return array
            else:
                print "numItems= %d" % numItems
                print "data length= %d" % len(data)
                return np.empty(0)
        except socket.error:
            return np.empty(0)
        
if __name__ == "__main__":
    comms = comms()
