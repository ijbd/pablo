import serial
import time
import os
import sys
from datetime import datetime, timedelta

def manual_mode():
    print('---------- MANUAL INPUT MODE ------------')
    print('')
    print('Refer to the README for manual command options. Type \'quit\' to exit manual mode.')
    print('')
    with serial.Serial(arduino_port, 9600, timeout=1) as ser:
        time.sleep(2)

        user_input = input('<human> $ ')

        while(user_input != 'quit'):
            ser.write(user_input.encode())
            ser.write(b'\n')
            response = ser.readline().decode().replace('\n','')
            print('<pablone> : {}'.format('All good' if response != 'b' else 'No good. Try again :('))
            user_input = input('<human> $ ')
        
        # Tell Pablone to end program and return home :(
        ser.write(b'\e')
        ser.write(b'\n')
    print('---------- END MANUAL INPUT MODE ------------')
    print('')
    return

def file_mode():
    print('---------- FILE INPUT MODE ------------')
    print('')
    print('Please identify the command file, which contain line-by-line instructions for Pablone to follow.')
    command_filename = input('Enter Filename: ')

    # Error Handling
    while(not os.path.exists(command_filename) and command_filename != 'quit'):
        print("Invalid filename: {}".format(command_filename))
        command_filename = input("Enter Filename: ")
    print('')

    # Quit
    if command_filename == 'quit': 
        sys.exit(0)

    # count number of commands
    num_commands = 0
    with open(command_filename, 'r') as commands:
        for command in commands:
            num_commands += 1

    # share with the class
        print('Beginning Print!')
        print('Total Commands: {}'.format(num_commands))
        print('')

    with open(command_filename, 'r') as commands:

        with serial.Serial(arduino_port, 9600, timeout=1) as ser:
            time.sleep(2)

            i = 0
            for command in commands:       
                # For consistency strip newlines
                command = command.replace('\n','')    

                # Send command
                ser.write(command.encode())
                ser.write(b'\n')

                # wait for response
                echo = ser.readline().decode().replace('\n','')

                #timeout
                wait_until = datetime.now() + timedelta(seconds=10)

                while(echo == '' and datetime.now() < wait_until):
                    echo = ser.readline().decode().replace('\n','')
                if(echo == ''):
                    print('TIMEOUT')

                # check for error
                if(command != echo):
                    print('ERROR RETURNED FOR COMMAND: {}'.format(command))
                    print('EXITING :(')
                    sys.exit(0)

                print('Command {} of {} complete...'.format(i,num_commands))
                i += 1

            # Bring our girl home
            ser.write(b'e')
            ser.write(b'\n')
    
    print('All Finished :)')
    print('---------- END FILE INPUT MODE ------------')
    print('')
    return

# General meet and greet
print('')
print('Welcome to the Pablone computer interface!')
print('')

# Port
arduino_port = input('Enter Arduino Port (e.g. "COM4"): ')
print('')

if arduino_port == 'quit': sys.exit(0)

def main():
    # Input mode 
    print('For manual input, type \'m\'. For file input, type \'f\'. To quit, type "quit".')
    input_mode = input('Enter input mode (m/f/quit): ')
    while(input_mode != 'm' and input_mode != 'f' and input_mode != 'quit'):
        input_mode = input('Enter input mode (m/f/quit): ')
    print('')

    # quit
    if input_mode == 'quit':
        sys.exit(0)

    # Manual input
    elif input_mode == 'm':
        manual_mode()

    # File input
    elif input_mode == 'f':
        file_mode()

    main()

main()

