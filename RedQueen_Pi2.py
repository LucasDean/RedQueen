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
            elif reading == :
                print "Door #"
            elif reading == :
                print "Door #"
            elif reading == :
                print "Door #"
            elif reading == :
                print "Door #"
            elif reading == :
                print "Door #"            
            else:
                print "Unknown: ",reading
        previous_reading = reading
    except:
        pass
    time.sleep(0.05)
