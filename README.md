
# CSCB09: A1 - System Monitoring Tool

System Monitoring Tool is a C program that displays system information and reports metrics of user utilization and system utilization. 

This program works in a Linux type OS. 

## Table of Contents
- Project Name
  - Table of Contents
  - Problem Solving Approach
  - Functions Overview
  - Usage

## Problem Solving Approach

My approach to solving this assignment is the following: 
- Consulted a TA to gain insights on how to start the assignment
- Review the assignment video resource to better understand the assignment 
- Learned how to read the Linux manual pages for the library resources listed in the assignment
- Used the CTRL-F function to navigate and find the system stats needed
- Built functions for each stat one-by-one to help with debugging and comparing stats
- Used `free`, `top`, and `htop` commands to compare program results with a real-time system monitor
- Reviewed class materials for command line arguments and further consulted a TA, deciding to use `getopt.h` library to handle this task 
- Consulted piazza on how to optimize information handling and implement a 'struct' to help in organizing print functions and get (information) functions

## Functions Overview
### SystemsStats Struct

The `SystemStats` struct stores the reported system stats. It is used to help organize and manage information related to the system's performance.

**Members**
- **header**: type char and stores a string of lines for design formatting
- **uptime**: array of type int to store uptime information
- **sys_info**: array of type char to store strings of system info
- **cpu_usage**: a double to store CPU usage
- **sys_usage**: array of type double to store system usage
- **user_usage**: array of type char to store user-related information
- **cpu_cores**: a double to store the number of CPU cores
- **self_mem_utl**: a long to store self memory utilization

**My Problem Solving Approach:**
- I decided to use a struct to store information to help with reducing repeated code and better organize system stats. It especially helps for the sequential command.

## My Getter Functions
### getUptime Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where the uptime info will be stored. 

Reads from `/proc/uptime` file and displays and error if it goes wrong. Then converts the seconds into days, hours, minutes, seconds. (This is helpful for formatting to a readable time.) It is stored into `stats` into `uptime` array.

**My Problem Solving Approach:**
- I found uptime in one of the libraries, but I found reading from the file to be easier. 

### getSysInfo Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where the system info will be stored. 

From the `<sys/utsname.h>` library, uses the struct `utsname` to get the system info and stores this info into 'stats' into `sys_info` array. 

**My Problem Solving Approach:**
- This one is straight forward because the library is easy to understand.

### getCPUUsage Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where cpu usage will be stored. 

Reads from `/proc/stat` file and displays and error is opening the file goes wrong. It reads from the file to get the first line of in this file to get cpu info and if something goes wrong it displays an error. The cpu usage is stored into `stats` into `cpu_usage`.

**My Problem Solving Approach:**
- I decided to use `/proc/stat` because getting the cpu time for this formula `(% CPU usage = (CPU time) / (# of cores) / (wall time))` was much too tedious. To calculate the cpu usage, I followed the formula `(Total Time Spent on Non-Idle Tasks / Total Time) x 100.` 

### getSystemUsage Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where system usage will be stored. 

From the `<sys/sysinfo.h>` library, uses the struct 'sysinfo' to get the system usage and stores this info into 'stats' into 'sys_usage' array. 

If getting the error goes wrong it displays and error. Gets the physical total memory, physical used memory, total virtual memory and used virtual memory and converts the bytes to GB. 

**My Problem Solving Approach:**
- The data doesn't report the exact same result you'd find from the `free` command and that's because the cpu usage is calulated using cached memory as well. I considered using `/proc/meminfo`but decided against it so that my program is portable. 

### getCPUCores Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where cpu core number will be stored. 

Reads from `/proc/cpuinfo` file and displays and error is opening the file goes wrong. 

If getting the cores goes wrong, it displays an error, otherwise it stores the information into `stats` into `cpu_cores`.

**My Problem Solving Approach:**
- From reading the file, I saw that each processor has the same amount of cpu cores, so this function looks for the first finding of cpu cores, gets it's value then stores it into a variable. 

### getSelfMemUtl Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, where memory self-utilization will be stored. 

Uses struct `rusage` to get the self-utilization and stores it into struct `stats` into `self_mem_utl`

### getMySystemStats Function

Function to gather and organize system stats into a `SystemStats` structure.
(Runs all the getter functions.)

Returns a `SystemStats` structure containing information about the system's performance.

## My Print Functions
### printUserUsage Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, to access header. 

Utilizes struct `utmp` from `<utmp.h>` library. 

It prints a header for the session users, then uses `setutent()`, `getutent()`, and `endutent();` to find to users connected and how many sessions each user is connected to. 

**My Problem Solving Approach:**
- From the library, I tried using one of the listed files, but to no avail I asked a TA for help. 

### printRunningParam Function

Takes as a param `stats`, a pointer to a `SystemStats` struct, `num_samples` number of samples, and `tdelay` the time delay for printing stats.

Formats and prints the number of samples, memory self-utilization and header from the `SystemStats` struct. 

### printSystemUsage Function

Takes as a param `stats`, a pointer to a `SystemStats` struct to access system usage stats. 

Formats and prints physical and virtual used memory from the total memory. 

### printCPUInfo Function

Takes as a param `stats`, a pointer to a `SystemStats` struct to access cpu info.

Formats and prints percentage of cpu usage, number of cpu cores, and a header to separate information. 

### printSysInfo Function

Takes as a param `stats`, a pointer to a `SystemStats` struct to access system info and uptime.

Formats and prints system name, machine name, versino, release, architecture and uptime. It formats the uptime to human readable time.  

### Message Function

Prints valid commands for the user running the program to use.

### main Function

Main part of the program that handles command-line options and what to display.
It initializes a SystemStats structure, myStats, using the getMySystemStats function to gather initial system statistics. 

With the use of getopt, it parses command-line arguments and can accept `-s` for system statistics,`-u` for user-related statistics, `--q` for sequential display, `--n` for the number of samples, and `--t` for the delay between samples.


Takes as parameters 
- `argc` - an integer representing the number of command-line arguments
- `argv` - an array of string holding the command-line arguments

## How to use
Download the zip file and extract all the files. Locate mySystemStats.c program in your Linux terminal. 

Run this command: 
`gcc -Wall mySystemStats.c -o mySystemStats`

Then run `./mySystemStats [--system | --user | --sequential] [--samples N] [--tdelay N]`

Can also run `./mySystemStats --sequential > textfile.txt` to print to a file. 

**! Will print 10 samples every 1 second unless otherwise specified.!**

### `--system`

Prints the running parameters and system memory statistics by the specified number of samples with the specified delay. 

The memory will print one by one following after another. It does not use ESCape Codes for position and format how it prints because my terminal does not support it. 

Sample output: 
```Nbr of samples: 10 -- every 1 secs
Memory Self-Utilization: 2352 KB
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
```

### `--user`

Prints the running parameters and user-related statistics.

Sample output: 

```
Nbr of samples: 10 -- every 1 secs
Memory Self-Utilization: 3904 KB
---------------------------------------
### Sessions/users ###
magatcai     pts/0            (138.51.13.191)
alyoshin     pts/7            (tmux(505368).%1)
sekitama     pts/9            (tmux(1042367).%0)
iskand97     pts/11           (tmux(551028).%10)
iskand97     pts/22           (tmux(551028).%3)
iskand97     pts/32           (tmux(551028).%7)
mantil11     pts/49           (tmux(956975).%1)
```

### `--sequential``

Prints the running parameters and displays statistics iteratively for a specified number of samples with a specified delay.

For each iteration, it tells you the current iteration and prints current memory usage for that iteration. Does not maintain blank space because my terminal does not support ESCape Codes. 

Sample output:
```
Nbr of samples: 3 -- every 1 secs
Memory Self-Utilization: 2356 KB
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
---------------------------------------
### Sessions/users ### 
caile        pts/1            ()
---------------------------------------
CPU Usage: 1.43%
Number of CPU cores: 4
---------------------------------------
>>> iteration 1
---------------------------------------
### System Information ###  
System Name: Linux
Machine Name: DESKTOP-B22FFCA
Version: #1 SMP Thu Oct 5 21:02:42 UTC 2023
Release: 5.15.133.1-microsoft-standard-WSL2
Architecture: x86_64
System running since last reboot: 0 days, 08:58:53 (08:58:53)
---------------------------------------
---------------------------------------
Nbr of samples: 3 -- every 1 secs
Memory Self-Utilization: 2356 KB
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
---------------------------------------
### Sessions/users ### 
caile        pts/1            ()
---------------------------------------
CPU Usage: 1.43%
Number of CPU cores: 4
---------------------------------------
>>> iteration 2
---------------------------------------
### System Information ###  
System Name: Linux
Machine Name: DESKTOP-B22FFCA
Version: #1 SMP Thu Oct 5 21:02:42 UTC 2023
Release: 5.15.133.1-microsoft-standard-WSL2
Architecture: x86_64
System running since last reboot: 0 days, 08:58:53 (08:58:53)
---------------------------------------
---------------------------------------
Nbr of samples: 3 -- every 1 secs
Memory Self-Utilization: 2356 KB
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)
1.67 GB / 7.70 GB -- 5.67 GB / 9.70 GB
---------------------------------------
### Sessions/users ### 
caile        pts/1            ()
---------------------------------------
CPU Usage: 1.43%
Number of CPU cores: 4
---------------------------------------
>>> iteration 3
---------------------------------------
### System Information ###  
System Name: Linux
Machine Name: DESKTOP-B22FFCA
Version: #1 SMP Thu Oct 5 21:02:42 UTC 2023
Release: 5.15.133.1-microsoft-standard-WSL2
Architecture: x86_64
System running since last reboot: 0 days, 08:58:53 (08:58:53)
---------------------------------------
```

### No commands

If none of the flags are set, prints system memory statistics for a specified number of samples with a specified delay.
