.NH "Table of Contents"
.C0 "Introduction" 1
.C1 "Using This Manual" 1
.C1 "Bibliography" 2
.C0 "TCP/IP History and Design" 3
.C1 "What Is A Network?" 3
.C2 "An Example of a Network" 3
.C2 "Protocols, the Internet, and TCP/IP" 4
.C1 "Basic TCP/IP Protocols" 5
.C2 "FTP: File Transfer Protocol" 5
.C2 "TELNET: Remote Login" 5
.C2 "SMTP: Simple Mail Transfer Protocol" 6
.C2 "Client/Server Architecture" 6
.C1 "Structure of the TCP/IP Protocols" 7
.C2 "TCP and IP Tasks" 9
.C2 "The TCP Level" 10
.C2 "The IP level" 12
.C2 "The Ethernet Level" 13
.C2 "The SLIP Level" 14
.C1 "Well-Known Ports and the Applications Layer" 15
.C2 "An Example Application: SMTP" 17
.C1 "Protocols other than TCP: UDP and ICMP" 18
.C2 "UDP" 19
.C2 "ICMP" 19
.C1 "TCP/IP Architecture:  A Summary" 19
.C1 "Tracking Names and Information: The Domain System" 21
.C1 "Routing" 22
.C2 "Gateways" 22
.C2 "Routing Through Gateways" 23
.C2 "Optimal Gatways" 23
.C1 "Classes of Internet Addresses" 23
.C2 "Special Addresses" 24
.C1 "Datagram Fragmentation and Reassembly" 25
.C1 "Ethernet Encapsulation: ARP" 25
.C1 "Getting More Information" 26
.C2 "How To Obtain Documents" 27
.C1 "Where To Go From Here" 27
.C0 "Installing cohulip" 29
.C1 "Phase One Ð Preliminary Information" 29
.C2 "Install Hardware and Driver" 29
.C2 "Choose Type of Network" 29
.C2 "Obtain an IP Address" 29
.C2 "Obtain the Domain-Name Server IP Address" 30
.C2 "Obtain Another Computer" 30
.C2 "Get the Dialing Sequence" 30
.C1 "Phase Two Ð Installing cohulip" 30
.C2 "De-archiving the Files" 30
.C2 "Contents of the cohulip Package" 30
.C2 "Invoking the Installation Script" 31
.C2 "Get Home Directory for cohulip" 31
.C2 "Get Local System Name" 32
.C2 "Get Internet Address" 32
.C2 "SLIP Information" 32
.C2 "Ethernet Information" 33
.C2 "Select the Default Route" 33
.C2 "Domain-Name Server" 33
.C2 "Select Servers" 34
.C2 "Final Actions" 34
.C2 "After the Dust Settles" 35
.C1 "Phase Three Ð Post-Installation Configuration" 36
.C2 "Configuring cohulip for Ethernet" 36
.C2 "Connecting to the Internet via SLIP" 37
.C2 "Startup Script" 37
.C2 "Modem Configuration" 37
.C2 "Setting Your IP Address:  Permanent Address" 38
.C2 "Setting Your IP Address:  Floating Address" 38
.C2 "Domain-Name Server Address" 39
.C2 "An Example Login Session" 39
.C2 "Writing a Script to Invoke net" 40
.C1 "Where To Go From Here" 42
.C0 "cohulip Configuration Files" 43
.C1 "startup.net" 43
.C2 "Order of Commands" 43
.C1 "ftpusers" 44
.C1 "hosts.net" 45
.C1 "Help Files" 45
.C1 "Finger Files" 45
.C0 "Running net" 47
.C1 "Starting Up" 47
.C1 "Modes of Operation" 47
.C1 "Host Names" 48
.C1 "Syntax of Commands" 48
.C2 "<return> Ð Send a Command" 48
.C2 "? Ð Print a Help Message" 48
.C2 "! Ð Send a Command to the Shell" 48
.C2 "# Ð Ignore a Command" 48
.C2 "attach Ð Attach net To a Network" 48
.C2 "cd Ð Change Directory‘R" 50
.C2 "close Ð Close a Session On Your End" 50
.C2 "dir Ð List Contents of Directory on Remote Host" 50
.C2 "domain Ð Manipulate Information About Domains" 50
.C2 "echo Ð Change Response to telnet Echo Request" 51
.C2 "eol Ð Display or Modify End-of-Line Behavior" 51
.C2 "escape Ð Display or Change the Escape Character" 51
.C2 "etherstat Ð Display Ethernet Statistics" 51
.C2 "exit Ð Exit From net‘R" 51
.C2 "finger Ð Display Information About Users" 52
.C2 "ftp Ð File Transfer Protocol" 52
.C2 "ip Ð Manipulate the IP Handler" 54
.C2 "help Ð Display a help file" 55
.C2 "hostname Ð Set the Local Host\(aas Name" 55
.C2 "kick Ð Kick Out Pending Text" 55
.C2 "log Ð Log Server Activity" 55
.C2 "ping Ð Reach Out and Touch A Remote Host" 56
.C2 "pwd Ð Print Working Directory" 56
.C2 "record Ð Record All Activity" 56
.C2 "reset Ð Blow Away a Session" 56
.C2 "route Ð Manipulate the Routing Table" 56
.C2 "session Ð Display Information About Sessions" 57
.C2 "shell Ð Invoke the COHERENT Shell" 57
.C2 "smtp Ð Simple Mail Transport Facility" 58
.C2 "start Ð Invoke a Server" 58
.C2 "stop Ð Shut Down a Server‘R" 59
.C2 "tcp Ð Manipulate TCP" 59
.C2 "telnet Ð Converse With Another System" 60
.C2 "trace Ð Trace the Handling of Datagrams" 60
.C2 "udp status Ð Display Status of UDP Receive Queues" 61
.C2 "upload Ð Write File Into Current TELNET Session" 61
.C1 "Where To Go From Here" 61
.C0 "Advanced cohulip" 63
.C1 "Sample FTP Session" 63
.C1 "Anonymous FTP" 67
.C2 "Accessing a Remote Host via Anonymous FTP" 67
.C2 "Configuring cohulip for Anonymous FTP" 68
.C1 "Tracing and Debugging a cohulip Session" 69
.C2 "Initiating a Trace" 69
.C2 "Interpreting a Packet" 69
.C2 "Reading a Tracefile" 70
.C2 "Dump of a Malformed Datagram" 71
.C1 "cohulip and DOS" 72
.C2 "cohulip and DOS via SLIP" 72
.C2 "cohulip and DOS via Netware" 72
.C1 "Routing Between Networks" 74
.C2 "Connecting Between Networks" 74
.C2 "Firewall Between Networks" 75
.C1 "telunix Ð Remote Login" 75
.C1 "Traffic Jams on the Information Superhighway" 75
.C2 "Basics of TCP" 76
.C2 "How To Tune Your TCP Timer" 76
.C2 "How To Detect Congestion" 77
.C2 "What To Do If TCP Is Jammed" 78
.C0 "Index" 79
.fi
