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
        self.handlers = { 1:self.update, 2:self.setCaption, 3:self.sendParameters }

    def update(self):
        data = self.conn.recv(self.numItems * 4)
        if (len(data) / 4) == self.numItems:
            array = np.fromstring(data,dtype='>i4')
            print "sending ack"
            # send acknowledgement
            status = 0
            ack = np.array([1,status], dtype=np.int32)
            self.conn.send(ack.data)
            return 1, array
        else:
            print "self.numItems= %d" % self.numItems
            print "data length= %d" % len(data)
            return 1, np.empty(0)

    def setCaption(self):
        str = ""
        data = self.conn.recv(100)
        str = data.decode('utf-8')
        print str
        print "sending ack"
        # send acknowledgement
        status = 0
        ack = np.array([2,status], dtype=np.int32)
        self.conn.send(ack.data)
        return 2, str

    def sendParameters(self):
        return 3, []
        
    def lookForIncoming(self,numItems):
        self.numItems = numItems
        try:
            print "looking for incoming data"
            # data will be preceded by type
            msgTypeData = self.conn.recv(1 * 4)
            msgType = np.fromstring(msgTypeData,dtype='>i4')
            print "received message of type %d" % msgType
            print msgType
            print msgType.shape
            print msgType[0]
            mType, data = (self.handlers[msgType[0]])()
            return mType, data
        except socket.error:
            return np.empty(0)
        
if __name__ == "__main__":
    comms = comms()
