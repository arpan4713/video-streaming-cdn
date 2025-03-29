import subprocess
import sys

if len(sys.argv) != 2:
    print('Error: Usage is python3 start_server.py <host_number>')
    sys.exit(1)

conf = sys.argv[1]

if not conf.isdigit() or not (1 <= int(conf) <= 8):
    print('Error: <host_number> must be between 1 and 8')
    sys.exit(1)

subprocess.call(["/usr/local/apache2/bin/apachectl", "-f", f"/home/proj2/.confs/httpd{conf}.conf"])
