import sys

import libMyApp

class MyAppEventHandler(libMyApp.MyAppEventHandler):

    def __init__(self):
        libMyApp.MyAppEventHandler.__init__(self)

    # def on_connected(self):
    #     self.myapp. ..

    # def on_disconnected(self):
    #     self.myapp. ..

    # def on_operational(self):
    #     self.myapp. ..

    # def on_change_execute(self):
    #     .. = self.myapp.execute.value
    
    # def on_change_done(self):
    #     .. = self.myapp.done.value
    
    # def on_change_parameters(self):
    #     .. = self.myapp.parameters.value
    

myapp = libMyApp.libMyApp_init()

handler = MyAppEventHandler()
libMyApp.add_event_handler(myapp, handler)

try:
    myapp.connect()
    while True:
        myapp.process()
        # if myapp.is_connected:
            # myapp.done.value = .. 
            # myapp.done.publish()
            
            # myapp.results.value = .. 
            # myapp.results.publish()
            
except (KeyboardInterrupt, SystemExit):
    print 'Application terminated, shutting down'

myapp.disconnect()
myapp.dispose()

