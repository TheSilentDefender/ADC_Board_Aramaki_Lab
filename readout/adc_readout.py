import serial
from serial.tools import list_ports

def start_serial(baud=115200):
    ports = list_ports.comports()
    for p in ports:
        if p.manufacturer == "STMicroelectronics":
            serial_port = p.device
            print(p.device)
            break
    ser = serial.Serial(port=serial_port, baudrate=baud)# rtscts=True, dsrdtr=True, exclusive=True)
    if not ser.is_open:
        ser.open()
    return ser

def read_all(ser):
    ser.write(b"a")
    output = ser.read(384)
    print(output)
    out_list = output.split(b"_")
    out_dict = {}
    for i, chan in enumerate(out_list[::2]):
        if chan: # discard empty string
            val = int(out_list[2*i+1])
            out_dict[chan.decode('utf-8')] = val
    return out_dict


if __name__ == "__main__":
    ser = start_serial()
    output = read_all(ser)
    ser.close()