import serial
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device, port.description)

ser = serial.Serial(port='/dev/cu.usbmodem122533301', baudrate=8795, timeout=1)

while True:
    print("USB CDC:")
