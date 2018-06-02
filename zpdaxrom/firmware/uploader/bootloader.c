#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <poll.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

void nonblock(int state)
{
    static struct termios stdin_old;
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state)
    {
	tcgetattr(STDIN_FILENO, &stdin_old);

        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
	ttystate.c_cc[VTIME] = 0; /* Inter-character timer unused */

	ttystate.c_iflag &= ~(ICRNL); // Do not translate CR -> NL on input
	ttystate.c_oflag &= ~(OPOST);
	ttystate.c_lflag &= ~(ECHO|ICANON|ISIG|ECHOE|ECHOK|ECHONL);
	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    }
    else
    {
	tcsetattr(STDIN_FILENO, TCSANOW, &stdin_old);
    }

}

void terminal(int fd_serial)
{
    char prev_char = 0;
    char buf[256];
    struct pollfd fd_poll[2];

    fd_poll[0].fd = STDIN_FILENO;
    fd_poll[0].events |= POLLIN;

    fd_poll[1].fd = fd_serial;
    fd_poll[1].events |= POLLIN;


//    fd_set fdlist;

//    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    nonblock(1);

//    FD_ZERO(&fdlist);
//    FD_SET(STDIN_FILENO, &fdlist);
//    FD_SET(fd_serial, &fdlist);




    while(1) {
//	int ret = select(fd_serial, &fdlist, NULL, NULL, NULL);
//	if (ret == 0) {
//	    continue;
//	} else if (ret == -1) {
//	    perror("select");
//	    break;
//	}
	int ret = poll(fd_poll, 2, 1000);

	if (ret == -1) {
	    perror("poll");
	    break;
	}

//	if (FD_ISSET(STDIN_FILENO, &fdlist)) {
	if (fd_poll[0].revents & POLLIN) {
//	    fputs(">>>>>>>>\n", stdout);
	    int len = read(STDIN_FILENO, buf, sizeof(buf));
	    if (len == -1) {
		    perror("stdin");
		    break;
	    }
	    if (prev_char == 0x01 && buf[0] == 'x') {
		    fprintf(stderr, "Bye bye!\n");
		    break;
	    }
	    prev_char = buf[0];
//	    fprintf(stderr, "[0x%02X]", prev_char);
//	    fflush(stderr);
	    if (write(fd_serial, buf, len) != len) {
		fprintf(stderr, "error write to serial port!\n");
		break;
	    }
	} else if (fd_poll[1].revents & POLLIN) {
//	} else if (FD_ISSET(fd_serial, &fdlist)) {
	    int len = read(fd_serial, buf, sizeof(buf));
	    if (len == -1) {
		perror("serial read");
		break;
	    }
	    buf[len] = 0;
//	    fprintf(stderr, "{%02X}", buf[0]);
//	    fflush(stderr);
	    fputs(buf, stdout);
	    fflush(stdout);
	}
    }

    nonblock(0);
}

int check_soc_lm32(int fd)
{
    char buf[64];
    int i = 0;

    buf[0] = '*';

    if (write(fd, buf, 1) < 0) {
	return 0;
    }

    do {
	if (read(fd, &buf[i], 1) < 0) {
	    return 0;
	}
	if (buf[i] == '\n') {
	    break;
	}
    } while (++i < sizeof(buf));

    buf[++i] = 0;

    fprintf(stderr, ">>> %s\n", buf);

    if (!strcmp(buf, "**soc-lm32/bootloader** > \r\n")) {
	return 1;
    }

    return 0;
}

int write_uint32(int fd, uint32_t val)
{
    char tmp[4];
    tmp[0] =  val >> 24;
    tmp[1] = (val >> 16) & 0xff;
    tmp[2] = (val >>  8) & 0xff;
    tmp[3] =  val        & 0xff;

    return write(fd, tmp, 4);
}

int main(int argc, char *argv[])
{
    int fd, res;
    char buf[255];

    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);


    fd = open(argv[1], O_RDWR | O_NOCTTY); 

    if (fd <0) {
	perror(MODEMDEVICE);
	exit(-1);
    }

    if ( tcgetattr(fd, &tty) != 0 ) {
	fprintf(stderr, "error %d from tcgetattr\n", errno);
    }

    /* Save old tty parameters */
    tty_old = tty;

    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)BAUDRATE);
    cfsetispeed (&tty, (speed_t)BAUDRATE);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    cfmakeraw(&tty);

    /* Flush Port, then applies attributes */
    tcflush(fd, TCIFLUSH);
    if ( tcsetattr(fd, TCSANOW, &tty ) != 0) {
	fprintf(stderr, "error %d from tcsetattr\n", errno);
    }

    if (check_soc_lm32(fd)) {
	fprintf(stderr, "SOC LM32 found!\n");
	if (!strncmp(argv[2], "up", 2)) {
	    char tmp[1024];
	    uint32_t start, size;

	    FILE *inf = fopen(argv[3], "rb");
	    if (inf) {
		if (argc < 5) {
		    start = 0x40000000;
		} else {
		    sscanf(argv[4], "%x", &start);
		}

		if (argc < 6) {
		    fseek(inf, 0, SEEK_END);
		    size = ftell(inf);
		    fseek(inf, 0, SEEK_SET);
		} else {
		    sscanf(argv[5], "%x", &size);
		}

		fprintf(stderr, "Upload file %s to 0x%08X size 0x%08X\n", argv[3], start, size);
		tmp[0] = 'u';
		write(fd, tmp, 1);
		write_uint32(fd, start);
		write_uint32(fd, size);
		while (size) {
		    int rd = fread(tmp, 1, sizeof(tmp), inf);
		    if (rd <= 0) {
			fprintf(stderr, "Error read file\n");
			break;
		    }
		    int wr = write(fd, tmp, rd);
		    if (wr != rd) {
			fprintf(stderr, "hmm, send less bytes than read!\n");
			break;
		    }
		    size -= wr;
		    fprintf(stderr, "%d wrote, %d left\n", wr, size);
		}
	    } else {
		fprintf(stderr, "can't open file %s\n", argv[3]);
	    }
	} else if (!strncmp(argv[2], "do", 2)) {
	    char tmp[1024];
	    uint32_t start, size;

	    FILE *inf = fopen(argv[3], "wb");
	    if (inf) {
		sscanf(argv[4], "%x", &start);
		sscanf(argv[5], "%x", &size);

		fprintf(stderr, "Download file %s to 0x%08X size 0x%08X\n", argv[3], start, size);
		tmp[0] = 'd';
		write(fd, tmp, 1);
		write_uint32(fd, start);
		write_uint32(fd, size);
		while (size) {
		    int rd = read(fd, tmp, sizeof(tmp));
		    if (rd <= 0) {
			fprintf(stderr, "Error read file\n");
			break;
		    }
		    int wr = fwrite(tmp, 1, rd, inf);
		    if (wr != rd) {
			fprintf(stderr, "write error!\n");
			break;
		    }
		    size -= wr;
		    fprintf(stderr, "%d wrote, %d left\n", wr, size);
		}
	    } else {
		fprintf(stderr, "can't open file %s\n", argv[3]);
	    }
	} else if (!strncmp(argv[2], "go", 2)) {
	    char tmp[1];
	    uint32_t start;

	    if (argc < 4) {
		start = 0x40000000;
	    } else {
		sscanf(argv[3], "%x", &start);
	    }

	    fprintf(stderr, "Start app 0x%08X\n", start);
	    tmp[0] = 'g';
	    write(fd, tmp, 1);
	    write_uint32(fd, start);

	    terminal(fd);
	}
    }

    tcsetattr(fd, TCSANOW, &tty_old);

    return 0;
}

