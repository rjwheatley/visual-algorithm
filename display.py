#!/usr/bin/env python

import sys
import os
import pygame
import numpy as np
import socket
from comms import comms
        
class Display():
    def __init__(self):
        print "Display instantiated"
        # SDL default sound driver is ALSA but the Python program can select something else.
        # Change it to avoid:  ALSA lib pcm.c:8306:(snd_pcm_recover) underrun occurred
        os.environ['SDL_AUDIODRIVER'] = 'dsp'
        # thin rectangle dimensions
        self.thickness = 5
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
        self.comms = comms()
        self.comms.setNumItems(self.numItems)
        self.comms.setCallbacks({self.comms.UPDATE:self.updateScreen, \
                                 self.comms.SET_CAPTION:self.setCaption, \
                                 self.comms.RETURN_SCREEN_PARAMS:self.returnScreenParams, \
                                 self.comms.TERMINATE:self.terminate})
        self.running = True
        
    def setDisplayDimensions(self, maxW, maxH):
        self.wdth = (maxW / 100) * 100
        self.ht = ((maxH / 100) * 100) - 100
        # the bottom y-coordinate of all the thin rectangles
        self.ry = self.ht - self.floor
        # the number of items to display
        self.numItems = (self.wdth - (2 * self.margin)) / self.thickness
        # the maximum value of an item
        self.maxValue = self.ht - self.ceiling

    # update screen callback - when the update message is handled in comms, the incoming
    # data is handed to this callback and used to update the screen.
    def updateScreen(self,dispData):
        self.screen.fill((0,0,0))
        for ndx in range(0,dispData.shape[0]):
            rct = (self.margin + (ndx * self.thickness),self.ry,self.thickness,-dispData[ndx])
            pygame.draw.rect(self.screen, self.white, rct)
        pygame.display.update()

    # set caption callback - when the set caption message is handled in comms, the incoming
    # data is handed to this callback and used to update the screen caption
    def setCaption(self,str):
        print "caption: %s" % str
        pygame.display.set_caption(str)

    # terminate callback - after the terminate message is handled in comms, this
    # callback sets self.running to false
    def terminate(self,nullData):
        self.running = False

    # return screen parameters callback - when the return screen parameters message is handled
    # in comms, this callback queues an acknowledgement containing the parameters.
    def returnScreenParams(self,str):
        # Queue data, the acknowledgement, to be sent to the client.
        status = 0
        ack = np.array([3,status,self.numItems,self.maxValue], dtype=np.int32)
        self.comms.qDataForSending(ack.data)

    def runIt(self,sys):
        print "Running"
        # create a surface
        w, h = self.screen.get_size()
        print "Display size (widthxheight) = %d x %d" % (w,h)
        self.updateScreen(self.data)
        # define a variable to control the main loop
        clock = pygame.time.Clock()
        # main loop
        while self.running:
            # event handling, gets all event from the event queue
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    print "display:  got quit event"
                    # change the value to False, to exit the main loop
                    self.comms.setExitStatus()
            self.comms.processSockets()
            clock.tick(40)
            pygame.display.flip()
        
if __name__ == "__main__":
    dsp = Display()
    dsp.runIt(sys)
