#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <utmp.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

// Defining a struct to store system stats
typedef struct SystemStats{
    char* header;
    int uptime[4];
    char sys_info[5][256];
    double cpu_usage;
    double sys_usage[4];
    char user_usage[3][256];
    int cpu_cores;
    long self_mem_utl;
}SystemStats;


/* My Functions to Retrieve Information */
// Function to get system uptime
void getUptime(SystemStats *stats){
    double uptime;
    FILE *uptimeFile = NULL;
    
    uptimeFile = fopen("/proc/uptime", "r");
    if (uptimeFile == NULL) {
        perror("Error getting uptime");
    }

    fscanf(uptimeFile, "%lf", &uptime);
    fclose(uptimeFile);

    int days = (int)(uptime /(60*60*24));
    int hours = (int)((uptime-days * (60*60*24)) / (60*60));
    int minutes = (int)((uptime-days * (60*60*24) - hours * (60*60)) / 60);
    int seconds = (int)(uptime-days * (60*60*24) - hours * (60*60) - minutes * 60);

    stats->uptime[0] = seconds;
    stats->uptime[1] = minutes;
    stats->uptime[2] = hours;
    stats->uptime[3] = days;

}

// Function to get system information
void getSysInfo(SystemStats *stats) {
    struct utsname buffer;

    errno = 0;
    if (uname(&buffer) < 0) {
        perror("Error getting system information");
        exit(EXIT_FAILURE);
    }

    strncpy(stats->sys_info[0], buffer.sysname, sizeof(stats->sys_info[0]));
    strncpy(stats->sys_info[1], buffer.nodename, sizeof(stats->sys_info[1]));
    strncpy(stats->sys_info[2], buffer.version, sizeof(stats->sys_info[2]));
    strncpy(stats->sys_info[3], buffer.release, sizeof(stats->sys_info[3]));
    strncpy(stats->sys_info[4], buffer.machine, sizeof(stats->sys_info[4]));

}

// Function to get CPU usage
void getCPUUsage(SystemStats *stats){
    char line[256];
    unsigned long user, nice, system, idle, total_cpu_time;
    double cpu_usage;
    FILE *statFile = NULL;
    
    statFile = fopen("/proc/stat", "r");
    if (statFile == NULL) {
        perror("Error opening /proc/stat");
        exit(EXIT_FAILURE);
    }
    
    if (fgets(line, sizeof(line), statFile) == NULL) {
        perror("Error reading /proc/stat");
        exit(EXIT_FAILURE);
    }

    sscanf(line, "cpu %lu %lu %lu %lu", &user, &nice, &system, &idle);

    fclose(statFile);

    total_cpu_time = user + nice + system + idle;
    cpu_usage = ((double)(total_cpu_time - idle)/total_cpu_time)*100.0; 

    stats->cpu_usage = cpu_usage;
}

// Function to get system memory usage
void getSystemUsage(SystemStats *stats) {
    double total_mem, used_mem, total_virt_mem, used_virt_mem;
    struct sysinfo info;

        if (sysinfo(&info) != 0) {
            perror("Error getting system information");
            exit(EXIT_FAILURE);
        }

    total_mem = (double)info.totalram * info.mem_unit / (1024.0 * 1024 * 1024);
    used_mem = (double)(info.totalram - info.freeram) * info.mem_unit / (1024.0 * 1024 * 1024);

    total_virt_mem = (double)(info.totalram + info.totalswap) * info.mem_unit / (1024 * 1024 * 1024);
    used_virt_mem = (double)((info.totalram + info.totalswap) - (info.freeram - info.freeswap)) * info.mem_unit / (1024 * 1024 * 1024);
    
    stats->sys_usage[0] = total_mem;
    stats->sys_usage[1] = used_mem;

    stats->sys_usage[2] = total_virt_mem;
    stats->sys_usage[3] = used_virt_mem;
}

// Function to get the number of CPU cores
void getCPUCores(SystemStats *stats){
    char line[256];
    int cpu_cores = 0;
    FILE *cpuinfoFile = NULL;
   
    cpuinfoFile = fopen("/proc/cpuinfo", "r");

    if (cpuinfoFile == NULL) {
        perror("Error opening /proc/cpuinfo");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, 256, cpuinfoFile) != NULL) {
        if (strstr(line, "cpu cores")) {
            sscanf(line, "cpu cores : %d", &cpu_cores);
            break;
        }
    }

    fclose(cpuinfoFile);

    if (cpu_cores > 0) {
        stats->cpu_cores = cpu_cores;
    } else {
        perror("Could not get number of cores.\n");
    }
}

// Function to get self memory utilization
void getSelfMemUtl(SystemStats *stats){
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    stats->self_mem_utl = usage.ru_maxrss;
}


/* My Functions for Printing Information */
// Function to print user sessions and usage
void printUserUsage(SystemStats *stats) {
    struct utmp *ut;

    printf("### Sessions/users ### \n");

    setutent();
    while ((ut = getutent()) != NULL) {
        if (ut->ut_type == USER_PROCESS) {
            printf("%-12s %-16s (%s)\n", ut->ut_user, ut->ut_line, ut->ut_host);
        }
    }
    endutent();

    printf("%s", stats->header);
}

// Function to print running parameters
void printRunningParam(SystemStats *stats, int num_samples, int tdelay){
    printf("Nbr of samples: %d -- every %d secs\n", num_samples, tdelay);
    printf("Memory Self-Utilization: %ld KB\n", stats->self_mem_utl);
    printf("%s", stats->header);
}

// Function to print system memory usage
void printSystemUsage(SystemStats *stats){
    printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", 
                     stats->sys_usage[1], stats->sys_usage[0], stats->sys_usage[3], stats->sys_usage[2]);
}

// Function to print CPU information
void printCPUInfo(SystemStats *stats){
    printf("CPU Usage: %.2f%%\n", stats->cpu_usage);
    printf("Number of CPU cores: %d\n", stats->cpu_cores);
    printf("%s", stats->header);
}

// Function to print system information
void printSysInfo(SystemStats *stats){
    printf("### System Information ###  \n");
    printf("System Name: %s\n", stats->sys_info[0]);
    printf("Machine Name: %s\n", stats->sys_info[1]);
    printf("Version: %s\n", stats->sys_info[2]);
    printf("Release: %s\n", stats->sys_info[3]);
    printf("Architecture: %s\n", stats->sys_info[4]);
    printf("System running since last reboot: ");

    //printint uptime
    printf("%d days, %02d:%02d:%02d (%02d:%02d:%02d)\n",
                                stats->uptime[3], stats->uptime[2], 
                                stats->uptime[1], stats->uptime[0], 
                                (stats->uptime[3])*24 + stats->uptime[2], 
                                stats->uptime[1], stats->uptime[0]);
    printf("%s", stats->header);
}

// Function to display a message for invalid commands
void Message() {
    printf("Valid commands: --system, --user, --sequential, --samples N, --tdelay\n");
}

/* Compiling All the System Stats */
// Function to retrieve and compile all system statistics
SystemStats getMySystemStats(){
    SystemStats stats;

    stats.header = "---------------------------------------\n";
    getUptime(&stats);
    getSysInfo(&stats);
    getCPUUsage(&stats);
    getSystemUsage(&stats);
    getCPUCores(&stats);
    getSelfMemUtl(&stats);

    return stats;
}

/*Main function*/

int main(int argc, char *argv[]) {
    int option;
    int system_flag = 0, user_flag = 0, sequential_flag = 0, num_samples = 10, tdelay = 1;
    SystemStats myStats = getMySystemStats();

    // Define long options for command-line arguments
    static struct option long_options[] = {
        {"system", no_argument, 0, 's'},
        {"user", no_argument, 0, 'u'},
        {"sequential", no_argument, 0, 'q'},
        {"samples", required_argument, NULL, 'n'},
        {"tdelay", required_argument, NULL, 't'},
        {">", required_argument, NULL, '>'},
        {NULL, 0, NULL, 0}
    };

    // Parse command-line arguments using getopt_long
    while ((option = getopt_long(argc, argv, "suqn:t:>:", long_options, NULL)) != -1) {
        switch (option) {
            case 's':
                system_flag = 1;
                break;
            case 'u':
                user_flag = 1;
                break;
            case 'q':
                sequential_flag = 1;
                break;
            case 'n':
                num_samples = atoi(optarg);
                break;
            case 't':
                tdelay = atoi(optarg);
                break;
            case '?':
            default:
                Message();
                exit(EXIT_FAILURE);
        }
    }

    // Check for additional arguments for number of samples and tdelay
    if (optind < argc) {
        num_samples = atoi(argv[optind]);
        optind++;
    }

    if (optind < argc) {
        tdelay = atoi(argv[optind]);
    }

    // Performing actions based on command-line arguments
    if(user_flag){
        printRunningParam(&myStats, num_samples, tdelay);
        printUserUsage(&myStats);
    }

    if (system_flag){
        printRunningParam(&myStats, num_samples, tdelay);
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        for (int i = 0; i < num_samples; ++i) {
            printSystemUsage(&myStats);
            sleep(tdelay);
        }
    }

    if(sequential_flag){
        for(int i = 0; i < num_samples; i++){
            printf("%s", myStats.header);
            printRunningParam(&myStats, num_samples, tdelay);
            printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
            printSystemUsage(&myStats);
            printf("%s", myStats.header);
            printUserUsage(&myStats);
            printCPUInfo(&myStats);
            printf(">>> iteration %d\n", i+1);
            printf("%s", myStats.header);
            printSysInfo(&myStats);

            sleep(tdelay);
        }
    }

    if(!user_flag && !system_flag && !sequential_flag){
        printRunningParam(&myStats, num_samples, tdelay);
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        for (int i = 0; i < num_samples; ++i) {
            printSystemUsage(&myStats);
            sleep(tdelay);
        }
        printf("%s", myStats.header);
        printUserUsage(&myStats);
        printCPUInfo(&myStats);
        printf("%s", myStats.header);
        printSysInfo(&myStats);
    }

    return 0;
}