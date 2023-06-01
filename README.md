# Port_Scanner
Port_Scanner is a C++ command-line application for scanning open ports on a target host🛢🔍.It utilizes multithreading for efficient scanning and supports a list of famous ports commonly used by various network services⚙!

🔍🔒

**Features**
 - Scan a target host for open ports
 - Multithreaded scanning for faster results
 - Support for famous ports used by network services
 - Colorful emojis indicate the status of each port
 - Option to save results to a file
 
**Usage**
 - Enter the target host's IP address or domain name.
 - Choose whether to save the output to a file.
 - If saving to a file, provide the desired filename.
 - Sit back and relax while the Port Scanner scans the target host's ports.
 - View the scan results in the console or the output file.

🏃🔎💡

**Example**

``` cpp
$ ./port-scanner

Enter the target host: 192.168.1.1 // IP VALUE
Do you want to save the output to a file? (yes/no): yes 
Enter the filename to save the output: scan_results.txt

Port    Status
-------------------
22    ✅ [OPEN]
80    ✅ [OPEN]
443   ✅ [OPEN]
...

Progress: 50/100 ports scanned.
Progress: 75/100 ports scanned.
All ports scanned.

Scan results saved to scan_results.txt.
```
