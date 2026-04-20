import serial
import struct
import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np
import threading
import sys
import time

# === CONFIGURATION ===
PORT = 'COM6'         
BAUDRATE = 115200     
BUFFER_SIZE_RX = 199  
NUM_TRANSMISSIONS = 1 
BUFFER_SIZE_PLOT = BUFFER_SIZE_RX * NUM_TRANSMISSIONS 
PACKET_LEN = 2 + (BUFFER_SIZE_RX * 2) # 2 Header bytes + 398 Data bytes
# =====================

plot_data = np.zeros(BUFFER_SIZE_PLOT)
current_mode = "Waiting for data..."
data_lock = threading.Lock() 
is_running = True            

def serial_worker():
    global plot_data, current_mode, is_running
    
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=0.1)
        buffer = bytearray() 
        
        while is_running:
            if ser.in_waiting > 0:
                buffer.extend(ser.read(ser.in_waiting))
            else:
                time.sleep(0.005) 
                continue
                
            while len(buffer) >= PACKET_LEN:
                # Sync check for Convolution (0xAABB) or Correlation (0xCCDD)
                idx_conv = buffer.find(b'\xaa\xbb') 
                idx_corr = buffer.find(b'\xcc\xdd') 
                
                valid_idx = -1
                mode_str = ""
                
                if idx_conv != -1 and (idx_corr == -1 or idx_conv < idx_corr):
                    valid_idx = idx_conv
                    mode_str = "Current Signal: Convolution"
                elif idx_corr != -1:
                    valid_idx = idx_corr
                    mode_str = "Current Signal: Correlation"
                
                if valid_idx != -1:
                    if valid_idx > 0:
                        del buffer[:valid_idx] 
                    
                    if len(buffer) >= PACKET_LEN:
                        raw_data = buffer[2:PACKET_LEN]
                        
                        try:
                            # Decode Big-Endian 16-bit Unsigned Integers
                            unpacked = struct.unpack(f'>{BUFFER_SIZE_RX}H', raw_data)
                            raw_array = np.array(unpacked, dtype=np.float64)
                            
                            # Scale ADC (0-4095) to Voltage (0-3.3V)
                            volts_array = raw_array * (3.3 / 4095.0)
                            
                            with data_lock:
                                plot_data = np.roll(plot_data, -BUFFER_SIZE_RX)
                                plot_data[-BUFFER_SIZE_RX:] = volts_array
                                current_mode = mode_str
                                
                        except Exception as e:
                            print(f"Error: {e}")
                            
                        del buffer[:PACKET_LEN]
                    else:
                        break 
                else:
                    del buffer[:-1]
                    break
                    
    except Exception as e:
        print(f"Serial Error: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

# === GUI INITIALIZATION ===
app = QtWidgets.QApplication(sys.argv)
win = pg.GraphicsLayoutWidget(show=True, title="MSP430 Convolution & Correlation")
win.resize(1200, 600) 
win.setBackground('w')

plot = win.addPlot(title=current_mode)
plot.setXRange(0, BUFFER_SIZE_PLOT - 1)
plot.setYRange(-0.2, 3.5) 
plot.showGrid(x=True, y=True)
plot.setLabel('left', 'Amplitude', units='V')

curve = plot.plot(pen=pg.mkPen('b', width=2))

def update_gui():
    with data_lock:
        curve.setData(plot_data)
        plot.setTitle(current_mode, color='k', size='14pt')

timer = QtCore.QTimer()
timer.timeout.connect(update_gui)
timer.start(30)

def on_close():
    global is_running
    is_running = False

app.aboutToQuit.connect(on_close)

serial_thread = threading.Thread(target=serial_worker, daemon=True)
serial_thread.start()

sys.exit(app.exec_())