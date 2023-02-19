#!/usr/bin/env python
from mfrc522 import SimpleMFRC522
import RPi.GPIO as GPIO
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import time
import cereal
import serial

DEVICE_ID="98bb0735e28656bac098d927d410c3138a4b5bca"
CLIENT_ID="d98ce18a07b645ff8d57e0de98ffc53f"
CLIENT_SECRET="80b66aa52bf64028964968a45036afab"

playlists = {584184296801:'spotify:album:3Gt7rOjcZQoHCfnKl5AkK7', 584189866001:'spotify:playlist:5cAz3DR6eFPhxH4ozTKQcx', 584198583691:'spotify:album:2nLOHgzXzwFEpl62zAgCEC', 584191474035:'spotify:playlist:1h0CEZCm6IbFTbxThn6Xcs'}
#		yellow green white blue
reader = SimpleMFRC522()
sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID,
                                               client_secret=CLIENT_SECRET,
                                               redirect_uri="http://localhost:8080",
                                               scope="user-read-playback-state,user-modify-playback-state"))

#         s = serial.Serial("/dev/ttyACM0", 9600, timeout=1) 
s = serial.Serial("/dev/ttyUSB0", 9600, timeout=1) 
s.flush()
volumes = []

while True:
    try:
        old_play_type = 0
        old_volume = -1
        old_power = 0
        prev_vol = -1
        check_count = 0
        
        while True:
            
            if check_count <= 100:
                check_count += 1
            elif check_count % 50 == 0:
                trackInfo = sp.current_user_playing_track()
                duration = trackInfo['item']['duration_ms']
                progress = trackInfo['progress_ms']
                time_left = duration-progress
                
                curr_track_name = trackInfo['item']['name']
                
                cereal.write_serial(s, 0, time_left, curr_track_name)
            else:
                check_count = 0
                id = reader.read_id_no_block()
                print("id:", id)
                queue = playlists.get(id)
                if queue:
                    sp.transfer_playback(device_id=DEVICE_ID, force_play=False)
                    sp.start_playback(device_id=DEVICE_ID, context_uri=queue)
                    print("Playing music!")
                    time.sleep(2)
            
            play_type, volume, power = cereal.read_serial(s)
            if prev_vol == volume:
                counter += 1
            else:
                counter = 0
            prev_vol = volume
            
            if volume != old_volume:
                if counter >= 10:
                    # Set volume between 0 and 100
                    #print("***************HERE*****************")
                    old_volume = volume
                    sp.volume(volume)
    
                    
                    
            print("volume:", volume)    
            
            if play_type == old_play_type:
                continue
                        
            old_play_type = play_type
            
            if play_type==2:
                print("Previous Track")
                sp.next_track()
            
            elif play_type==3:
                print("Next Track")
                sp.previous_track()
            
            elif play_type==0:
                sp.pause_playback()
                print("Paused, Waiting scan...")
                
                    
            elif play_type==1:
                sp.start_playback()
                print("Resuming Playback")
                
            elif power==1:
                print("SHUTDOWN")
                os.system('sudo systemctl isolate poweroff.target')
            
            
            time.sleep(0.01)
            

    # if there is an error, skip it and try the code again (i.e. timeout issues, no active device error, etc)
    except Exception as e:
        print(e)
        pass

    finally:
        print("Cleaning  up...")
        GPIO.cleanup()
