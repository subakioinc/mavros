#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include "spi.h"

#define MAX_BUF 1024

#define VCC33 24
#define VCC15 25

#define CS0   8
#define CS1   7

#define SPI0_SPEED 0
#define SPI1_SPEED 500*1000

static void export_port(int gpio) {
    char buf[MAX_BUF];
    int fd;

    sprintf(buf, "%d", gpio);

    fd = open("/sys/class/gpio/export", O_WRONLY);

    write(fd, buf, strlen(buf));

    close(fd);
}

static void unexport_port(int gpio) {
    char buf[MAX_BUF];
    int fd;

    sprintf(buf, "%d", gpio);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);

    write(fd, buf, strlen(buf));

    close(fd);
}

static void direction(int gpio, int dir) {
    char buf[MAX_BUF];
    int fd;

    sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);

    fd = open(buf, O_WRONLY);

    if (dir == 1) {
        write(fd, "out", 3);
    }
    else {
        write(fd, "in", 2);
    }

    close(fd);
}

static void write_val(int gpio, int val) {
    char buf[MAX_BUF];
    int fd;

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

    fd = open(buf, O_WRONLY);

    if (val == 1) {
        write(fd, "1", 1); // Set GPIO high status
    }
    else {
        write(fd, "0", 1); // Set GPIO low status
    }

    close(fd);
}

static void read_val(int gpio, int *val) {
    char buf[MAX_BUF];
    int fd;
    char value;

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY);

    read(fd, &value, 1);

    close(fd);

    (*val) = value;
}

static void dalpu_power_on_vcc33(void) {
    export_port(VCC33);
    direction(VCC33,1);
    write_val(VCC33,1);

    printf("dalpu_power_on_vcc33\n");
}

static void dalpu_power_off_vcc33(void) {
    export_port(VCC33);
    direction(VCC33,1);
    write_val(VCC33,0);

    printf("dalpu_power_off_vcc33\n");
}

static void dalpu_power_on_vcc15(void) {
    export_port(VCC15);
    direction(VCC15,1);
    write_val(VCC15,1);

    printf("dalpu_power_on_vcc15\n");
}

static void dalpu_power_off_vcc15(void) {
    export_port(VCC15);
    direction(VCC15,1);
    write_val(VCC15,0);

    printf("dalpu_power_off_vcc15\n");
}

void dalpu_power_on(void) {
    dalpu_power_on_vcc33();
    dalpu_power_on_vcc15();

#if 0
    export_port(CS0);
    direction(CS0, 1);
    write_val(CS0, 1);

    export_port(CS1);
    direction(CS1, 1);
    write_val(CS1, 1);
#endif

    dalpu_spi_open(0, 12000000, 0);

    dalpu_spi_open(1, 250000, 1);

    printf("dalpu_power_on\n\n");
}

void dalpu_power_off(void) {
    dalpu_spi_close(0);

    dalpu_spi_close(1);

#if 0
    export_port(CS0);
    direction(CS0, 1);
    write_val(CS0, 0);

    export_port(CS1);
    direction(CS1, 1);
    write_val(CS1, 0);
#endif

    dalpu_power_off_vcc33();
    dalpu_power_off_vcc15();

    printf("dalpu_power_off\n\n");
}
