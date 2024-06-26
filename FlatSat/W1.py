#
#  Select a relay and turn it on
import time
import board
import adafruit_pcf8575

print("PCF8575 one output test")

i2c = board.I2C()  # uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()  # For using the built-in STEMMA QT connector on a microcontroller
pcf = adafruit_pcf8575.PCF8575(i2c)

if (0):
    for i in range (16):
        print ("all off except ", i)
        pcf.write_gpio(0xFFFF & 1 << i)  # walking one
        time.sleep(0.2)

    # Turn everything off
    time.sleep(1)
    pcf.write_gpio(0xFFFF)
    time.sleep(1)

if (1):
    for i in range (16):
        print ("all on except ", i)
        print ((i//2 + 8*(i%2)))
        pcf.write_gpio(0x0000 | 1 << (i//2 + 8*(i%2)))  # walking one
        time.sleep(0.2)
    time.sleep(1)
    pcf.write_gpio(0xFFFF)
    time.sleep(1)

if (1):
    for i in range (16):
        print ("all on except ", i)
        pcf.write_gpio(~(0x0000 | 1 << (i//2 + 8*(i%2))))  # walking zero
        time.sleep(0.2)
    time.sleep(1)
    pcf.write_gpio(0xFFFF)
    time.sleep(1)
