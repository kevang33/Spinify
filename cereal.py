import serial
import time

def read_serial(s):
    """
    Expects play_type and volume to be written to serial.
    Returns the play_type and volume.

    play_type = 0 -> pause
    play_type = 1 -> play
    play_type = 2 -> skip
    play_type = 3 -> prev

    volume -> between 0 to 100

    An example of string read from serial is:
    0,70
    """
    while s.in_waiting <= 0:
        #print("wait")
        pass

    try:
        line = s.readline().decode("utf=8").rstrip()
        print(line)
        play_type, volume, power = line.split(sep=',')
        play_type, volume, power = int(play_type), int(volume), int(power)
        s.flush()
        return play_type, volume, power
    except:
        print("Caught an error")
        s.flush()
        
    return None, None, None

def write_serial(s, on_off, time_left, curr_track_name):
    """
    Writes:
    <on_off>,<time_left>,<curr_track_name>
    
    to serial

    on_off -> 0 or 1
    time_left -> float in ms
    curr_track_name -> string
    """
    wrt = f"{on_off},{time_left},{curr_track_name}:;"
    s.write(wrt.encode())
    s.flush()
    

if __name__ == "__main__":
    s = serial.Serial("/dev/ttyACM0", 9600, timeout=1)
    s.flush()
    
    state = 0
    while True:
        state = (state + 1) % 2
        play_type, volume = read_serial(s)
        
        write_serial(s, state, state, "SONG NAME")   
        print(f"play type: {play_type}, volume: {volume}")

        #if play_type == 0:
            # run code to play
        #    pass
        
        #if play_type == 1:
            # run code to pause
        #    pass
        
        #if play_type == 2:
            # run code to skip
        #    pass
        
        time.sleep(.01)
            
