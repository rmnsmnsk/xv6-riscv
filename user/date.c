#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int main(int argc, char *argv[]) {
    int fd = open("rtc", 0);
    if (fd < 0) {
        printf("date: cannot open rtc\n");
        exit(1);
    }

    uint64 time_ns;
    if (read(fd, &time_ns, 8) != 8) {
        printf("date: read error\n");
        close(fd);
        exit(1);
    }
    close(fd);

    uint64 total_sec = time_ns / 1000000000;
    uint64 ns = time_ns % 1000000000;

    uint64 days = total_sec / 86400;
    uint64 rem_sec = total_sec % 86400;

    int hour = rem_sec / 3600;
    int min = (rem_sec % 3600) / 60;
    int sec = rem_sec % 60;

    int year = 1970;
    while (1) {
        int days_in_year = is_leap(year) ? 366 : 365;
        if (days >= days_in_year) {
            days -= days_in_year;
            year++;
        } else {
            break;
        }
    }

    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (is_leap(year)) 
        days_in_month[1] = 29;

    int month = 0;
    while (days >= days_in_month[month]) {
        days -= days_in_month[month];
        month++;
    }
    int day = days + 1;
    month += 1;

    printf("%02d.%02d.%04d %02d:%02d:%02d.%09d\n", day, month, year, hour, min, sec, (int)ns);

    exit(0);
}