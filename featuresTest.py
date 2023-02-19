

# shows acoustic features for tracks for the given artist

from __future__ import print_function    # (at top of module)
from spotipy.oauth2 import SpotifyOAuth
import json
import spotipy
import time
import sys

DEVICE_ID="98bb0735e28656bac098d927d410c3138a4b5bca"
CLIENT_ID="d98ce18a07b645ff8d57e0de98ffc53f"
CLIENT_SECRET="80b66aa52bf64028964968a45036afab"

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID,
                                                       client_secret=CLIENT_SECRET,
                                                       redirect_uri="http://localhost:8080",
                                                       scope="user-read-playback-state,user-modify-playback-state"))
        
sp.trace = True

#track id
tid = "1XS9ZXy3Z17ptOVRO3i02e"

# len(sys.argv) > 1:
#    tids = sys.argv[1:]
#    print(tids)

start = time.time()
features = sp.audio_features(tid)
delta = time.time() - start
print(json.dumps(features, indent=4))
print("features retrieved in %.2f seconds" % (delta,))
