#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/wait.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

int
setSpeed(int fd, int speed, int parity)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if(tcgetattr (fd, &tty) == -1){
		fprintf(stderr, "error %d from tcgetattr", errno);
		exit(1);
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // ignore break signal
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 1;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
									// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if(tcsetattr (fd, TCSANOW, &tty) == -1){
		fprintf(stderr, "error %d from tcsetattr", errno);
		exit(1);
	}
	return 0;
}

void
epsToPdf(char *epsName, char *pdfName)
{
	int pid = fork();
	if(pid == -1){
		fprintf(stderr, "epsToPdf: fork failed\n");
	} else if(pid == 0){
		execl("/usr/bin/epstopdf", "epstopdf", epsName, pdfName, NULL);
		exit(1);
	} else {
		wait(NULL);
	}
}

void
pdfToSvg(char *pdfName, char *svgName)
{
	int pid = fork();
	if(pid == -1){
		fprintf(stderr, "pdfToSvg: fork failed\n");
	} else if(pid == 0){
		execl("/usr/bin/pdf2svg", "pdf2svg", pdfName, svgName, NULL);
		exit(1);
	} else {
		wait(NULL);
	}
}

enum {
	ErrorWait = 5,
};

int
main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s /dev/ttyUSB0\n", argv[0]);
		exit(1);
	}
	char *tekName = argv[1];
	for(;;){
		int tekFd = open(tekName, O_RDONLY);
		if(tekFd == -1){
			fprintf(stderr, "could not open tek at %s: %s\n", tekName, strerror(errno));
			sleep(ErrorWait);
			continue;
		}

		setSpeed(tekFd, B19200, 0);

		int seq = 1;
		for(;;){
			size_t cap = 1024*1024;
			char *buf = malloc(cap);

			int nrd;
			size_t len = 0;
			while((nrd = read(tekFd, buf + len, cap - len)) != -1){
				if(len == 0 && nrd > 0){
					fprintf(stderr, "started receiving hardcopy\n");
				}
				if(nrd > 0){
					len += nrd;
					buf[len] = '\0';
					if(strstr(buf, "%%EOF\n")){
						char *epsName = malloc(256);
						char *pdfName = malloc(256);
						char *svgName = malloc(256);
						for(;;seq++){
							snprintf(epsName, 256, "%s-%d.eps", argv[2], seq);
							snprintf(pdfName, 256, "%s-%d.pdf", argv[2], seq);
							snprintf(svgName, 256, "%s-%d.svg", argv[2], seq);
							int fd = open(epsName, O_RDONLY);
							if(fd != -1){
								fprintf(stderr, "%s exists\n", epsName);
								close(fd);
								continue;
							}
							close(fd);
							fd = open(pdfName, O_RDONLY);
							if(fd != -1){
								fprintf(stderr, "%s exists\n", pdfName);
								close(fd);
								continue;
							}
							close(fd);
							fd = open(svgName, O_RDONLY);
							if(fd != -1){
								fprintf(stderr, "%s exists\n", svgName);
								close(fd);
								continue;
							}
							close(fd);
							break;
						}
						int outFd = open(epsName, O_CREAT|O_WRONLY, 0666);
						if(write(outFd, buf, len) == (ssize_t)len){
							fprintf(stderr, "received %s (%zu bytes)\n", epsName, len);
						} else {
							fprintf(stderr, "error writing %s: %s\n", epsName, strerror(errno));
						}
						close(outFd);
						epsToPdf(epsName, pdfName);
						pdfToSvg(pdfName, svgName);
						free(epsName);
						break;
					}
				} else {
					fprintf(stderr, "EOF from tek %s\n", tekName);
					goto reopen;
				}
			}
			if(nrd == -1){
				fprintf(stderr, "error reading from tek %s: %s\n", tekName, strerror(errno));
reopen:
				sleep(ErrorWait);
				free(buf);
				break;
			}

			free(buf);
		}
		close(tekFd); // close port, loop back.
	}
	return 0;
}
