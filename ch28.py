# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import matplotlib.pyplot as plt 
from statistics import mean 
def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()


def read_o_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    tnew = [0]*n_int
    for i in t:
        tnew[i] = t[i]/200
    plt.plot(tnew,ref,'r*-',tnew,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('Angle (deg)')
    plt.xlabel('seconds')
    plt.show()

from genref import genRef

import serial
ser = serial.Serial('COM3',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('b: Current (mA) \tc: Read encoder (counts) \td: Read encoder (deg) \te: Reset encoder \tf: Set PWM (-100 to 100) \tg: Set current gains \th: Get current gains \ti: Set position gains \tj: Get position gains \tk: Test current gains \tl: Go to angle (deg) \tm: Load step trajectory \tn: Load cubic trajectory \to: Execute trajectory \tp: Unpower the motor \tq: Quit \tr: Get mode \tx: test') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'b'):
        current_str = ser.read_until(b'\n');  # get the current (mA)
        current_float = float(current_str) # turn it into a float
        print('Current (mA): ' + str(current_float) + '\n') # print it to the screen
    elif (selection == 'c'):
        encoder_count_str = ser.read_until(b'\n');  # get the encoder count
        encoder_count_int = int(encoder_count_str) # turn it into an int
        print('Encoder count: ' + str(encoder_count_int) + '\n') # print it to the screen
    elif (selection == 'd'):
        encoder_degrees_str = ser.read_until(b'\n');  # get the encoder degrees
        encoder_degrees_float = float(encoder_degrees_str) # turn it into a float
        print('Encoder degrees: ' + str(encoder_degrees_float) + '\n') # print it to the screen
    elif (selection == 'e'):
        print('Encoder Reset')
    elif (selection == 'f'):
        PWM_str = input('Enter PWM: ') # get the number to send
        PWM_int = int(PWM_str) # turn it into an int
        ser.write((str(PWM_int) + '\n').encode()); # send the numbers
        print('PWM set to ' + str(PWM_int) + '\n') # print it to the screen
    elif (selection == 'g'):
        kp_str = input('Enter kp: ') # get the kp to send
        ki_str = input('Enter ki: ') # get the ki to send
        kp_float = float(kp_str) #turn it into an int
        ki_float = float(ki_str) #turn it into an int
        ser.write((str(kp_float) + '\n').encode()); # send the numbers
        ser.write((str(ki_float) + '\n').encode()); # send the numbers
        print('current gains set')
    elif (selection == 'h'):
        kp_str = ser.read_until(b'\n');  # get the kp
        ki_str = ser.read_until(b'\n');  # get the ki
        kp_float = float(kp_str) # turn it into an int
        ki_float = float(ki_str) # turn it into an int
        print('Kp: ' + str(kp_float) + ' Ki: ' + str(ki_float) + '\n') # print it to the screen
    elif (selection == 'i'):
        kp_str = input('Enter kp: ') # get the kp to send
        ki_str = input('Enter ki: ') # get the ki to send
        kd_str = input('Enter kd: ') # get the kd to send
        kp_float = float(kp_str) #turn it into an int
        ki_float = float(ki_str) #turn it into an int
        kd_float = float(kd_str) #turn it into an int
        ser.write((str(kp_float) + '\n').encode()); # send the numbers
        ser.write((str(ki_float) + '\n').encode()); # send the numbers
        ser.write((str(kd_float) + '\n').encode()); # send the numbers
        print('position gains set')
    elif (selection == 'j'):
        kp_str = ser.read_until(b'\n');  # get the kp
        ki_str = ser.read_until(b'\n');  # get the ki
        kd_str = ser.read_until(b'\n');  # get the ki
        kp_float = float(kp_str) # turn it into an int
        ki_float = float(ki_str) # turn it into an int
        kd_float = float(kd_str) # turn it into an int
        print('Kp: ' + str(kp_float) + ' Ki: ' + str(ki_float) + ' Kd: ' + str(kd_float) + '\n') # print it to the screen
    elif (selection == 'k'):
        read_plot_matrix()
    elif (selection == 'l'):
        angle_str = input('Enter angle: ') # get the kp to send
        angle_float = float(angle_str) #turn it into an int
        ser.write((str(angle_float) + '\n').encode()); # send the numbers
        print('angle set')
    elif (selection == 'm'):
        ref_array = genRef('step')
        N = len(ref_array)
        ser.write((str(N) + '\n').encode()); # send the length
        for x in ref_array:
            ser.write((str(x) + '\n').encode()); # send the length
    elif (selection == 'n'):
        ref_array = genRef('cubic')
        N = len(ref_array)
        ser.write((str(N) + '\n').encode()); # send the length
        for x in ref_array:
            ser.write((str(x) + '\n').encode()); # send the length
    elif (selection == 'o'):
        read_o_matrix()
    elif (selection == 'p'):
        print('Motor turned off \n')
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    elif (selection == 'r'):
        mode_str = ser.read_until(b'\n');  # get the mode
        mode_int = int(mode_str) # turn it into an int
        print('Mode: ' + str(mode_int) + '\n') # print it to the screen
    elif (selection == 'x'):
        n_str = ser.read_until(b'\n');  # get the added number back
        n_int = float(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen
    else:
        print('Invalid Selection ' + selection_endline)



