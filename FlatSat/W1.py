#
#  Select a relay and turn it on
import time
import board
import adafruit_pcf8575

print("PCF8575 one output test")

i2c = board.I2C()  # uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()  # For using the built-in STEMMA QT connector on a microcontroller
pcf = adafruit_pcf8575.PCF8575(i2c)

# break out board connects oddly to the relay board.  This undoes the transform there so
# that i corresponds to the relay number K? on the PCB.  Also note the relay board
# inverts the sense of the signal (1 means off).

if (1):
    for i in range (1, 17):
        print ("all off except ", i)
        if (i % 2):  #Odd
            pcf.write_gpio(0xFFFF ^ 1 << (16 - (i + 1)//2))
        else:        #even
            pcf.write_gpio(0xFFFF ^ 1 << (i//2 - 1))  # walking one
        time.sleep(0.2)

    # Turn everything off
    time.sleep(1)
    pcf.write_gpio(0xFFFF)
    time.sleep(1)

if (1):
    for i in range (1, 17):
        print ("all on except ", i)
        if (i % 2):  #Odd
            pcf.write_gpio(0x0000 | 1 << (16 - (i + 1)//2))  # walking one
        else:        #Even
            pcf.write_gpio(0x0000 | 1 << (i//2 - 1))
        time.sleep(0.2)

    time.sleep(1)
    pcf.write_gpio(0xFFFF)
    time.sleep(1)
