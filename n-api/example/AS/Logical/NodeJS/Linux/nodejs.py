import sys

import libNodeJS

nodejs = libNodeJS.libNodeJS_init()

try:
    nodejs.connect()
    while True:
        nodejs.process()
        # if nodejs.is_connected:
            # nodejs.counters.value = .. 
            # nodejs.counters.publish();
            
except (KeyboardInterrupt, SystemExit):
    print 'Application terminated, shutting down'

nodejs.disconnect()
nodejs.dispose()

