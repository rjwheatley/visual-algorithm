#!/usr/bin/env python

import sys
import os
import pygame
import numpy as np
import socket
from comms import comms

class UdpToPygame():
    def __init__(self):
        self.comms = comms()
        
    def update(self,numItems):
        msgType, array = self.comms.lookForIncoming(numItems)
        # first element of array should be the type; save and
        # delete it before giving data to event handler
#        msgType = array[0]
        print "display.py update: msgType= %d" % msgType
#        np.delete(array,[0])
#        if len(array) > 0:
        ev = pygame.event.Event(pygame.USEREVENT, {'data': array, 'addr': self.comms.addr, 'msgType': msgType})
        pygame.event.post(ev)
        
class Display():
    def __init__(self):
        print "Display instantiated"
        # thin rectangle dimensions
        self.thickness = 1
        # the color of all the thin rectangles
        self.white = (255,255,255)
        # how much margin to leave on left and right
        self.margin = 10
        # height of the display floor
        self.floor = 5
        # space between max value and top of display
        self.ceiling = 10
        pygame.init()
        infoObject = pygame.display.Info()
        print "infoObject.current_w= %d" % infoObject.current_w
        print "infoObject.current_h= %d" % infoObject.current_h
        # set dimensions of display
        self.setDisplayDimensions(infoObject.current_w, infoObject.current_h)
        self.screen = pygame.display.set_mode((self.wdth,self.ht))
        self.data = np.random.randint(self.maxValue, size=self.numItems)
        
    def setDisplayDimensions(self, maxW, maxH):
        self.wdth = (maxW / 100) * 100
        self.ht = ((maxH / 100) * 100) - 100
        # the bottom y-coordinate of all the thin rectangles
        self.ry = self.ht - self.floor
        # the number of items to display
        self.numItems = self.wdth - (2 * self.margin)
        # the maximum value of an item
        self.maxValue = self.ht - self.ceiling
        
    def updateScreen(self,dispData):
        self.screen.fill((0,0,0))
        for ndx in range(0,dispData.shape[0]):
            rct = (self.margin + (ndx * self.thickness),self.ry,self.thickness,-dispData[ndx])
            pygame.draw.rect(self.screen, self.white, rct, self.thickness)
        pygame.display.update()

    def setCaption(self,str):
        print "caption: %s" % str
        pygame.display.set_caption(str)

    def runIt(self,sys):
        print "Running"
        dispatcher = UdpToPygame()
        print self.data
        print self.data.shape[0]
        """
        baseName = os.path.basename(sys.argv[0])
        # initialize the pygame module
        try:
            data = dispatcher.comms.conn.recv(100)
            str = data.decode('utf-8')
        except socket.error:
            pass
        print "caption: %s" % str
        pygame.display.set_caption(str)
        """
        # create a surface
        w, h = self.screen.get_size()
        print "Display size (widthxheight) = %d x %d" % (w,h)
        self.updateScreen(self.data)
        """
        # send display parameters
        params = np.array([self.numItems,self.maxValue], dtype=np.int32)
        try:
            dispatcher.comms.conn.send(params.data)
        except socket.error:
            pass
        """
        # define a variable to control the main loop
        running = True
        # main loop
        while running:
            # event handling, gets all event from the event queue
            for event in pygame.event.get():
                # only do something if the event is of type QUIT
                if event.type == pygame.QUIT:
                    # change the value to False, to exit the main loop
                    running = False
                if event.type == pygame.USEREVENT:
#                    print "updating screen, msgType= %d" % event.msgType
                    if event.msgType == 1:
                        self.updateScreen(event.data)
                    elif event.msgType == 2:
                        print "caption: %s" % event.data
                        pygame.display.set_caption(event.data)
                    elif event.msgType == 3:
                        try:
                            print "sending ack"
                            # send acknowledgement
                            status = 0
                            ack = np.array([3,status,self.numItems,self.maxValue], dtype=np.int32)
                            print ack
                            dispatcher.comms.conn.send(ack.data)
#                            dispatcher.comms.conn.send(params.data)
                        except socket.error:
                            pass
            dispatcher.update(self.numItems)
        
if __name__ == "__main__":
    dsp = Display()
    dsp.runIt(sys)


    """
        pygame.display.flip()
                t = pygame.font.SysFont('', 40).render(ev.data, True, (255,255,255))
                screen.blit(t, (0, l*20))
                l += 1
    """
