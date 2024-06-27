# Write your code here :-)
import board, busio, time
print("STARTUP")
uart = busio.UART(board.TX, board.RX, baudrate=2400)
count = 0
while True:
    byte = uart.read(1)
    if byte is None:
        continue
    if byte != b'U':
        continue

    data = uart.read(1)
    if data is None:
        continue
    # 0 !LP S2 S1 0 F2 F1 F0
    bdata = '{:08b}'.format(int.from_bytes(data, 'big'))
    nLP = bdata[1]
    S = bdata[2:5]
    F = bdata[5:]
    print("-----Count " + str(count) +"  -------")
    print("nLP = " + nLP)
    print("S = " + S)
    print("F = " + F)

    print(bdata)
    print(data)
    time.sleep(0.1)
    count +=1
