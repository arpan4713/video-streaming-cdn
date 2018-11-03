# Video Streaming via CDN
Implemented adaptive bitrate selection, DNS load balancing (round-robin and geographic using Dijkstra's), and an HTTP proxy server to stream video at high bit rates from the closest server to a given client.

## Included:
* miProxy: an HTTP proxy using TCP client and server connection that parses and modifies outgoing HTTP requests to the server (adaptive bitrate)
* nameserver: TCP server that listens on requests from miProxy to perform load balancing and server selection
  * DNS header and message structs to parse and handle rerouting 

## To compile:
Run `make` from the root directory. Make sure to cd to `nameserver` and to `miProxy` and `make clean` inside each
of the subdirectories.

## To run:
Requests from the video player will pass through miProxy if browser and miProxy are on the same host. (e.g. if miProxy is
running on h1, browser must also be launched through h1. Enter `10.0.0.1:<listen_port>` to play video through miProxy.
miProxy will communicate with the DNS server using the given DNS IP and host, receive the resource IP, and retrieve video
from the resource IP.

## Sample Run Using Topology from Assignment 3 in Mininet:
h1 = miProxy client and browser
h2 = nameserver
h3 = video server
h4 = video server

1. Run video server on h3 and h4 (or run python tester.py)
2. xterm h1
3. xterm h2
4. Run nameserver on h2 as `./nameserver/nameserver/ nsLog 4000 <0,1> <servers>`
5. Run miProxy and launch firefox on h1 as `./miProxy/miProxy miLog 1.0 1337 10.0.0.2 4000 & python launch_firefox.py 1`
6. Enter in firefox URL `10.0.0.1:1337/index.html` 
