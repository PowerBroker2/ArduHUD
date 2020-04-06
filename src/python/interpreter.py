import os
import serial
import serial.tools.list_ports
import argparse
import pandas as pd
import matplotlib.pyplot as plt


SCRIPT_PATH   = os.path.realpath(__file__)
SCRIPT_DIR    = os.path.dirname(SCRIPT_PATH)
SER_DATA_DIR  = os.path.join(SCRIPT_DIR, 'ser_data')
SER_DATA_PATH = os.path.join(SER_DATA_DIR, 'ser_data.txt')
TEST_FILE_NUM = 1
TEST_FILE     = os.path.join(SCRIPT_DIR, 'test_data', 'drive_{}.txt'.format(TEST_FILE_NUM))


plot_list = []


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


def plot_data(data_file, plot_title='Drive Data'):
    df = pd.read_csv(data_file)
    df.plot(subplots=True, figsize=(8, 8), x='Epoch', title=plot_title)
    plt.legend(loc='best')
    plot_list.append(plt)

def display_plots():
    for plot in plot_list:
        plot.show()

def conn_to_ser():
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
        
        print('Connected on {}'.format(port_name))
    
    return connection

def drives_from_ser(connection):
    connection.write('ls\n'.encode())
                    
    while b'?' in connection.readline():
        connection.write('ls\n'.encode())
        
    drive_files = []
    
    while connection.in_waiting:
        try:
            line = connection.readline().decode('utf-8')
            print(' '.join(line.split()))
            
            if ('.txt' in line) or ('.csv' in line):
                drive_files.append(line.split()[0])
        except UnicodeDecodeError:
            pass
    
    return drive_files

def grab_ser_data(connection, drive):
    connection.write(drive.encode())
    
    while b'?' in connection.readline():
        connection.write(drive.encode())
    
    data_lines = []
    
    while True:
        line = connection.readline()
        
        if (b'-' * 50) in line:
            break
        
        elif (b'.txt' not in line) and (b'.csv' not in line):
            data_lines.append(line)
    
    return data_lines

def log_ser_data(connection):
    with open(SER_DATA_PATH, 'w') as ser_data:
        for data_line in data_lines:
            write_line = ''
            
            for char in data_line:
                if chr(char).isascii():
                    write_line += chr(char)
            
            if write_line.count(',') == 5:
                ser_data.write(write_line.strip())
                ser_data.write('\n')


if __name__ == '__main__':
    args = setup_args()

    if args.ser:
        if args.ser.lower() == 'true':
            if args.port and args.baud:
                try:
                    connection = conn_to_ser()
                    drive_files = drives_from_ser(connection)
                    
                    print(drive_files)
                    
                    if not os.path.exists(SER_DATA_DIR):
                        os.makedirs(SER_DATA_DIR)
                    
                    if args.dn:
                        drive_number = int(args.dn)
                        drive = 'drive_{}.txt'.format(drive_number)
                        
                        if drive in drive_files:
                            print(drive)
                            
                            data_lines = grab_ser_data(connection, drive)
                            log_ser_data(connection)
                            
                            if os.path.exists(SER_DATA_PATH):
                                try:
                                    plot_data(SER_DATA_PATH, drive)
                                except pd.errors.ParserError:
                                    pass
                                except TypeError:
                                    pass
                                except pd.errors.EmptyDataError:
                                    print('ERROR - Empty text file')
                                
                                display_plots()
                            else:
                                print('ERROR - Could not find file {}'.format(SER_DATA_PATH))
                        else:
                            print('ERROR - Drive file {} not found on SD'.format(drive))
                        
                    else:
                        for drive in drive_files:
                            print(drive)
                            
                            data_lines = grab_ser_data(connection, drive)
                            log_ser_data(connection)
                            
                            if os.path.exists(SER_DATA_PATH):
                                try:
                                    plot_data(SER_DATA_PATH, drive)
                                except pd.errors.ParserError:
                                    pass
                                except TypeError:
                                    pass
                                except pd.errors.EmptyDataError:
                                    print('ERROR - Empty text file')
                            else:
                                print('ERROR - Could not find file {}'.format(SER_DATA_PATH))
                        
                        display_plots()
                except:
                    import traceback
                    traceback.print_exc()
                    
                    data_file = TEST_FILE
            else:
                data_file = TEST_FILE
    elif args.file:
        if os.path.exists(args.file):
            try:
                plot_data(args.file)
                display_plots()
            except pd.errors.ParserError:
                pass
            except TypeError:
                pass
            except pd.errors.EmptyDataError:
                print('ERROR - Empty text file')
        else:
            print('ERROR - Could not find file {}'.format(args.file))
    elif args.td:
        data_file = TEST_FILE
    else:
        data_file = TEST_FILE
    
    try:
        if os.path.exists(data_file):
            try:
                plot_data(data_file)
                display_plots()
            except pd.errors.ParserError:
                pass
            except TypeError:
                pass
            except pd.errors.EmptyDataError:
                print('ERROR - Empty text file')
        else:
            print('ERROR - Could not find file {}'.format(data_file))
    except NameError:
        pass
    
    try:
        connection.close()
    except:
        pass



