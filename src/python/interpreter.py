import os
import serial
import serial.tools.list_ports
import argparse
import pandas as pd
import matplotlib.pyplot as plt
from time import sleep


SCRIPT_PATH   = os.path.realpath(__file__)
SCRIPT_DIR    = os.path.dirname(SCRIPT_PATH)
SER_DATA_DIR  = os.path.join(SCRIPT_DIR, 'ser_data')
SER_DATA_PATH = os.path.join(SER_DATA_DIR, 'ser_data.txt')
TEST_FILE     = os.path.join(SCRIPT_DIR, 'test_data', 'drive_1.txt')


class InvalidSerialPort(Exception):
    pass


def setup_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('--td',   help='Use test data file')
    parser.add_argument('--ser',  help='Use serial interface to download data from the HUD')
    parser.add_argument('--port', help='Serial port to communicate with HUD')
    parser.add_argument('--baud', help='Serial baud to communicate with HUD')
    parser.add_argument('--dn',   help='Drive number - used with serial download only')
    parser.add_argument('--file', help='URL to file with data to be processed')
    
    return parser.parse_args()


def serial_ports():
    return [p.device for p in serial.tools.list_ports.comports(include_links=True)]


def plot_data(data_file):
    df = pd.read_csv(data_file)
    
    columns = df.keys()
    new_columns = {}
    
    for column in columns:
        new_columns[column] = column.strip()
    
    df = df.rename(columns=new_columns)
    df.plot(subplots=True, figsize=(8, 8), x='Epoch')
    plt.legend(loc='best')


if __name__ == '__main__':
    args = setup_args()

    if args.ser:
        if args.port and args.baud:
            try:
                port_name = None
                
                for p in serial_ports():
                    if p == args.port or os.path.split(p)[-1] == args.port:
                        port_name = p
                        break
    
                if port_name is None:
                    raise InvalidSerialPort('Invalid serial port specified.\
                        Valid options are {ports},  but {port} was provided'.format(
                        **{'ports': serial_ports(), 'port': args.port}))
                else:
                    connection = serial.Serial()
                    connection.port = port_name
                    connection.baudrate = int(args.baud)
                    connection.open()
                    
                    connection.write('ls\n'.encode())
                    sleep(0.01)
                    
                    drive_files = []
                    while connection.in_waiting:
                        line = connection.readline().decode('utf-8')
                        
                        if ('.txt' in line) or ('.csv' in line):
                            drive_files.append(line.split()[0])
                    
                    if not os.path.exists(SER_DATA_DIR):
                        os.makedirs(SER_DATA_DIR)
                    
                    if args.dn:
                        drive_number = int(args.dn)
                    else:
                        for drive in drive_files:
                            connection.write(drive.encode())
                            sleep(0.01)
                            
                            with open(SER_DATA_PATH, 'w') as ser_data:
                                while connection.in_waiting:
                                    try:
                                        line = connection.readline().decode('utf-8').replace('\n', '')
                                        
                                        if (('-' * 50) not in line) and ('.txt' not in line) and ('.csv' not in line):
                                            ser_data.write(line)
                                    except UnicodeDecodeError:
                                        pass
                            
                            if os.path.exists(SER_DATA_PATH):
                                try:
                                    plot_data(SER_DATA_PATH)
                                except pd.errors.ParserError:
                                    pass
                                except TypeError:
                                    pass
                            else:
                                print('ERROR - Could not find file {}'.format(SER_DATA_PATH))
            except:
                import traceback
                traceback.print_exc()
                
                data_file = TEST_FILE
        else:
            data_file = TEST_FILE
    elif args.td:
        data_file = TEST_FILE
    else:
        data_file = TEST_FILE
    
    try:
        if os.path.exists(data_file):
            plot_data(data_file)
        else:
            print('ERROR - Could not find file {}'.format(data_file))
    except:
        pass
    
    try:
        connection.close()
    except:
        pass



