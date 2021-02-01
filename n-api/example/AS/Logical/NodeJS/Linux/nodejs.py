import sys

import libNodeJS

nodejs = libNodeJS.libNodeJS_init()

try:
    nodejs.connect()
    while True:
        nodejs.process()
        # if nodejs.is_connected:
            # nodejs.countUp.value = .. 
            # nodejs.countUp.publish();
            
            # nodejs.countDown.value = .. 
            # nodejs.countDown.publish();
            
except (KeyboardInterrupt, SystemExit):
    print 'Application terminated, shutting down'

nodejs.disconnect()
nodejs.dispose()

