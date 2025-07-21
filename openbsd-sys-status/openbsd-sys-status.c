#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <machine/apmvar.h>


/* 
 * C script to fetch OpeBSD system data to use in WMs status bar 
 * e.g dwm
 * */

void getTime(char timeData[], size_t dataSize) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(timeData, dataSize, "%H:%M:%S", timeinfo);
}

void getDate(char dateData[], size_t dataSize) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(dateData, dataSize, "%a %Y-%d-%m", timeinfo);
}

void fetchBat(char batData[], size_t dataSize) {
	struct apm_power_info info;
	int fd;
	u_char state;	

	fd = open("/dev/apm", O_RDONLY);
		
	if(ioctl(fd, APM_IOC_GETPOWER, &info) == -1 ) {
		printf("Error getting battery info in ioctl.");
		return;
	}

	if (close(fd) == -1) {
		printf("Error when closing file descriptor of /dev/apm.");
		return;
	}

	state = info.battery_state;

	if(state == APM_BATT_CHARGING) {
		snprintf(batData, dataSize, "CHARG %u%%", info.battery_life);
	} else if(state == APM_BATT_HIGH || state == APM_BATT_LOW || state == APM_BATT_CRITICAL ) {
		snprintf(batData, dataSize, "BAT %u%%", info.battery_life);
	} else if(state == APM_BATTERY_ABSENT) {
		snprintf(batData, dataSize, "NO BAT");
	} else {
		snprintf(batData, dataSize, "ERR BAT");
	}
}

/* Needs to refactor to use uvmexp struct for a "real usage" */
void memUsage(char memData[], size_t dataSize) {
	int lvls[] = { CTL_HW, HW_USERMEM64 };
	int64_t usermem;
	int64_t availmem;
	size_t usermemSize = sizeof(usermem);

	if(sysctl(lvls, 2, &usermem, &usermemSize, NULL, 0) == -1) {
		printf("Error getting user memory in sysctl.");
		return;	
	}
	
	availmem = sysconf(_SC_PAGESIZE)*sysconf(_SC_AVPHYS_PAGES);

	snprintf(memData, dataSize, "%lluM/%lluM", (usermem - availmem)/1024/1024, usermem/1024/1024);
}

int main() {
	char bat[128];
	char memU[128];
	char date[128];
	char time[128];

	fetchBat(bat, sizeof(bat));
	memUsage(memU, sizeof(memU));
	getDate(date, sizeof(date));
	getTime(time, sizeof(time));

	printf("%s | %s | %s | %s\n", bat, memU, date, time);
	
	return 0;	
}
