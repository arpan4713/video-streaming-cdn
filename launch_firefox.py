import subprocess
import sys
import os

if len(sys.argv) != 2:
    print('Error: Usage is python3 launch_firefox.py <profile_num>')
    sys.exit(1)

profileNum = sys.argv[1]

with open(os.devnull, 'w') as devNull:
    subprocess.call(["firefox", "-P", f"eecs489profile{profileNum}"], stdout=devNull, stderr=devNull)
