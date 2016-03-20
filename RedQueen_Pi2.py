import smbus
import time
bus = smbus.SMBus(1)
address = 0x04
previous_reading = 0

print "System Active"

while True:
    try:
        reading = bus.read_byte(address)
        if reading == previous_reading:
            pass
        else:
            if reading == 26:
                print "All Closed"
            elif reading == 24:
                print "Door # 2"
            elif reading == 18:
                print "Door # 4"
            elif reading == 25:
                print "Door #1"
            elif reading == 22:
                print "Door #3"
            elif reading == 23:
                print "Door #s 1 and 2"
            elif reading == 21:
                print "Door #s 1 and 3"
            elif reading == 17:
                print "Door #s 1 and 4"
            elif reading == 20:
                print "Door #s 2 and 3"
            elif reading == 16:
                print "Door #s 2 and 4"            
            elif reading == 14:
                print "Door #s 3 and 4"    
            else:
                print "Unknown: ",reading
        previous_reading = reading
    except:
        pass
    time.sleep(0.05)
