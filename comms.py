#!/usr/bin/env python

import sys
import os
import pygame
import numpy as np
import socket
import select
import sys
import Queue

class comms():
    def __init__(self):
        COMMS_IP="127.0.0.1"
        COMMS_PORT=15006
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setblocking(0)
        self.sock.bind((COMMS_IP,COMMS_PORT))
        print "waiting for socket connection"
        # wait for client
        # this server has only one client
        self.sock.listen(1)
        self.in_socks = [self.sock]
        self.out_socks = []
        # socket:Queue
        self.msg_out_qs = {}
        self.msg_in_qs = {}
        self.EXIT_STATUS = 0x55
        # message types
        self.UPDATE = 1
        self.SET_CAPTION = 2
        self.RETURN_SCREEN_PARAMS = 3
        self.TE3RMINATE = 4
        self.handlers = { self.UPDATE:self.update, \
                          self.SET_CAPTION:self.setCaption, \
                          self.RETURN_SCREEN_PARAMS:False }
        self.status = 0

    # update handler - receives data from the client, queues an acknowledgement,
    #                  packages data in array, and returns it; if the amount
    #                  of data is not the expected amount, return an empty
    #                  array.
    def update(self):
        data = self.conn.recv(self.numItems * 4)
        if (len(data) / 4) == self.numItems:
            array = np.fromstring(data,dtype='>i4')
            # send acknowledgement
            ack = np.array([self.UPDATE,self.status], dtype=np.int32)
            self.qDataForSending(ack.data)
            return array
        else:
            return np.empty(0)

    # set caption handler - receives data from the client, sends an acknowledgement,
    #                       packages data in a string, and returns it
    def setCaption(self):
        str = ""
        data = self.conn.recv(100)
        str = data.decode('utf-8')
        # send acknowledgement
        ack = np.array([self.SET_CAPTION,self.status], dtype=np.int32)
        self.qDataForSending(ack.data)
        return str

    # client connection handler - handles the special case of the server socket
    # being ready for reading indicating that a new client is trying to connect.
    def connectToClient(self,skt):
        self.conn, self.addr = skt.accept()
        print("Python display server accepted connection from: " + str(self.addr))
        self.conn.setblocking(0)
        del self.in_socks[:]
        self.in_socks.append(self.conn)
        # Give the connection a queue for data we want to send
        self.msg_out_qs[self.conn] = Queue.Queue()
        self.msg_in_qs[self.conn] = Queue.Queue()
        
    def processClientData(self):
        try:
            # data will be preceded by type
            msgTypeData = self.conn.recv(1 * 4)
            array = np.empty(0)
            if msgTypeData:
                msgType = np.fromstring(msgTypeData,dtype='>i4')
                # A handler for each messagetype, if present, reads in data
                # and returns it.
                if self.handlers[msgType[0]]:
                    array = (self.handlers[msgType[0]])()
                # The data returned by the handler is passed on
                # to a callback function if there is one.
                if self.callbacks[msgType[0]]:
                    (self.callbacks[msgType[0]])(array)
                return
            else:
                # The client has disconnected.  Quit.
                print("Python display server shutting down")
                # Stop listening for input on the connection
                if skt in self.out_socks:
                    self.out_socks.remove(skt)
                    self.in_socks.remove(skt)
                    skt.close()
                    return
        except socket.error:
            return

    def setNumItems(self,numItems):
        self.numItems = numItems

    def qDataForSending(self,data):
        self.msg_out_qs[self.conn].put(data)
        # Add output channel for response
        if self.conn not in self.out_socks:
            self.out_socks.append(self.conn)

    def setCallbacks(self, callbacks):
        self.callbacks = callbacks

    def setExitStatus(self):
        self.status = self.EXIT_STATUS

    # called from the display processing loop
    def processSockets(self):
        readable, writable, exceptional = select.select(self.in_socks, self.out_socks, self.in_socks)
        # Handle inputs
        for skt in readable:
            if skt is self.sock:
                # This python display server's socket is ready to accept a connection
                self.connectToClient(skt)
            else:
                # An already connected socket has sent data or disconnected
                self.processClientData()
        # Handle outputs
        for skt in writable:
            try:
                msgToSend = self.msg_out_qs[skt].get_nowait()
            except Queue.Empty:
                # No messages waiting so stop checking for writability.
                self.out_socks.remove(skt)
            else:
                skt.send(msgToSend)
        # Handle exceptions
        for skt in exceptional:
            print("Python display server:  socket %s got exception" % skt.getpeername())
            # Stop listening for input on the connection and quit
            in_socks.remove(skt)
            if skt in out_socks:
                out_socks.remove(skt)
                skt.close()
                return np.empty(0), np.empty(0)
        
if __name__ == "__main__":
    comms = comms()
