import serial
import time
from platformio import util

def main():
    devices = util.get_serial_ports()
    if not devices:
        print("No serial ports found")
        return

    port = devices[0]["port"]
    print("Using port:", port)

    # Wait until the port is free
    while True:
        try:
            ser = serial.Serial(port, 1200)
            print("Port opened at 1200bps")
            ser.close()
            time.sleep(2)
            break
        except serial.SerialException:
            print("Port busy, please close Serial Monitor...")
            time.sleep(1)

main()