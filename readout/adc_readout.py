import yaml
from pathlib import Path
import pandas as pd
import datetime
import serial
from serial.tools import list_ports
import numpy as np
from numpy.polynomial.polynomial import Polynomial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates


N_BITS = 12
ADC_MAX = 2**N_BITS - 1
RTD_A = 3.9083e-3
RTD_B = -5.775e-7
RTD_C = -4.183e-12


def rtd_temp_exact(adc_val):
    # precise resistance to temperature conversion formulas for PT1000 RTDs(Callendar-Van Dusen
    # equation)
    R_ratio = (adc_val / ADC_MAX)/(1 - adc_val / ADC_MAX)
    if R_ratio < 0.5: # resistance is below 1k / temp below 0C
        coeff = [1-R_ratio, RTD_A, RTD_B, -100*RTD_C, RTD_C]
        poly = Polynomial(coeff)
        roots = poly.roots()
        sel = (np.imag(roots) == 0) & (roots < 0)
    else:
        coeff = [1-R_ratio, RTD_A, RTD_B]
        poly = Polynomial(coeff)
        roots = poly.roots()
        sel = (np.imag(roots) == 0) & (roots > 0)
    if len(roots[sel]) < 1:
        return np.nan
    return float(np.real(roots[sel][0]))


def rtd_temp_linear(adc_val):
    # linear approximation for resistance to temp conversion - large errors for temps outside
    # 0-100C range
    R_ratio = (adc_val / ADC_MAX)/(1 - adc_val / ADC_MAX)
    return (R_ratio - 1)/RTD_A


class ADC_serial:
    # Class for collecting and parsing data from ADC board via serial communication

    def __init__(self, baudrate=115200):
        self.baudrate = baudrate
        self.serial_number = -1
        self.name = ""
        self.rtds = {}
        self.instruments = {}

    def start_serial(self):
        # opens a serial port corresponding to the ADC board (with manufacturer STMicroelectronics),
        # if it exists
        ports = list_ports.comports()
        serial_port = ""
        for p in ports:
            if p.manufacturer == "STMicroelectronics":
                serial_port = p.device
                serial_number = int(p.serial_number)
                break
        if not serial_port:
            raise OSError("STMicroelectronics serial device not found")
        self.ser = serial.Serial(port=serial_port, baudrate = self.baudrate)
        self.serial_number = serial_number

    def load_config(self, config_path=""):
        # reads in the configuration file and sets things up
        # if config_path is not provided (empty string), looks for a file called "config.yml"
        # in the same directory where this file is located
        if self.serial_number == -1:
            raise AttributeError("Device serial number not set, cannot set up configuration")
        if not config_path:
            current_path = Path(__file__).resolve()
            config_path = current_path.parent / "adc_config.yml"
        with open(config_path, 'r') as file:
            config = yaml.safe_load(file)
        for board_name in config.keys():
            if config[board_name]["serial_number"] == self.serial_number:
                self.name = board_name
                board_config = config[board_name]
                break
        if not self.name:
            raise ValueError(f"Config entry for serial number {self.serial_number} not found")
        if "RTDs" in board_config and board_config["RTDs"] is not None:
            for rtd in board_config["RTDs"]:
                self.rtds[rtd["channel"]] = rtd["name"]
        if "instruments" in board_config and board_config["instruments"] is not None:
            for instr in board_config["instruments"]:
                self.instruments[instr["channel"]] = ADC_instrument(**instr)

    def initialize(self):
        self.start_serial()
        self.load_config()

    def read_channels(self):
        # sends request for data to ADC board and parses the response. Returns only data from
        # channels specified in the config file, converted from ADCC to real units
        self.ser.write(b"a")
        output = self.ser.readline()
        adc_list = output.split(b"_")
        adc_dict = {ch.decode("utf-8") : int(adc_list[2*i+1]) for i, ch in enumerate(adc_list[::2])}
        out_dict = {}
        for chan, name in self.rtds.items():
            adc_val = adc_dict[chan]
            out_dict[name] = (rtd_temp_exact(adc_val), "C")
        for chan, instr in self.instruments.items():
            adc_val = adc_dict[chan]
            out_dict[instr.name] = (instr.adcc_to_val(adc_val), instr.unit)
        return out_dict


class ADC_instrument:
    # Basic class for converting data from individual ADC channels (except RTDs)

    def __init__(self, name, channel, unit, min, max):
        self.name = name
        self.channel = channel
        self.unit = unit
        self.offset = min
        self.slope = (max - min)/ADC_MAX

    def __str__(self):
        return f"ADC instrument: {self.name}"

    def adcc_to_val(self, adcc):
        return adcc*self.slope + self.offset


class RTD_plotter:
    # Basic real-time plotter for RTD data. Visualizes temperature data but does not save

    def __init__(self, board):
        self.board = board

    def _setup_plot(self):
        # initialize and format plot
        plt.rcParams.update({"font.size": 20})

        self.fig, self.ax = plt.subplots(figsize=(18,12))
        self.rtd_lines = {}
        for name in self.board.rtds.values():
            x = np.array([], dtype=pd.Timestamp)
            y = np.array([], dtype=np.float64)
            line, = self.ax.plot(x, y, label=f"{name}")
            self.ax.legend(loc="upper left")
            self.rtd_lines[name] = line
        self.ax.set_ylabel("Temperature (C)")
        self.ax.set_xlabel("Timestamp")
        self.ax.set_title(f"RTD temperatures, ADC board S/N {self.board.serial_number}")

        # set up axis formatting
        formats = ['%Y', '%b', '%d', '%H:%M', '%H:%M', '%H:%M:%S']
        offset_formats = ['', '%Y', '%Y-%b', '%Y-%b-%d', '%Y-%b-%d', '%Y-%b-%d']
        locator = mdates.AutoDateLocator()
        formatter = mdates.ConciseDateFormatter(locator, formats=formats, offset_formats=offset_formats)
        self.ax.xaxis.set_major_formatter(formatter)
        self.ax.yaxis.get_major_formatter().set_useOffset(False)

    def _animate(self, _, max_deltaT):
        # collects RTD data from board and updates plot
        now = pd.Timestamp(datetime.datetime.now())
        x_min = now - pd.Timedelta(max_deltaT, "seconds")
        new_data = self.board.read_channels()
        for name in self.board.rtds.values():
            line = self.rtd_lines[name]
            time_data = np.append(line.get_xdata(), now)
            temp_data = np.append(line.get_ydata(), new_data[name][0])
            time_data_trunc = time_data[time_data >= x_min]
            temp_data_trunc = temp_data[time_data >= x_min]
            line.set_data(time_data_trunc, temp_data_trunc)
        self.ax.relim()
        self.ax.autoscale_view()

    def run_plot(self, max_deltaT=1800, update_interval=1000):
        # starts the realtime data plotting
        # max_deltaT: maximum amount of time to plot, in seconds (default 1800s = 30 min)
        # update_interval: time between data points, in ns
        self._setup_plot()
        self.ani = animation.FuncAnimation(self.fig, self._animate, fargs=(max_deltaT,),
                                           interval=update_interval, save_count=10)
        plt.show()


if __name__ == "__main__":

    adc_board = ADC_serial()
    adc_board.initialize()
    plotter = RTD_plotter(adc_board)
    plotter.run_plot()
    