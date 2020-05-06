
#include <stdio.h>
#include <windows.h>

void
epsToPdf(char *epsName, char *pdfName)
{
	char *command = malloc(256);
	snprintf(command, 256,
		"c:\\Program Files\\gs\\gs9.52\\bin\\gswin64c.exe -sDEVICE=pdfwrite -dEPSCrop -o %s %s",
		pdfName,
		epsName
	);
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&pi, 0, sizeof pi);
	memset(&si, 0, sizeof si);
	if(CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
		fprintf(stderr, "createprocess did not work\n");
		free(command);
		return;
	}
	free(command);
	WaitForSingleObject(pi.hProcess, INFINITE);
}

void
epsToPng(char *epsName, char *pngName)
{
	char *command = malloc(256);
	snprintf(command, 256,
		"c:\\Program Files\\gs\\gs9.52\\bin\\gswin64c.exe -sDEVICE=png16m -dEPSCrop -r600 -dDownScaleFactor=3 -o %s %s",
		pngName,
		epsName
	);
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&pi, 0, sizeof pi);
	memset(&si, 0, sizeof si);
	if(CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
		fprintf(stderr, "createprocess did not work\n");
		free(command);
		return;
	}
	free(command);
	WaitForSingleObject(pi.hProcess, INFINITE);
}

int
main(int argc, char *argv[])
{

	char *tekName = argc > 2 ? argv[2] : "\\\\.\\COM3";
	char *baseName = argc > 1 ? argv[1] : "tds820d-";


	for(;;){

		HANDLE tekFd;

		tekFd = CreateFile(tekName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		DCB serialParams;
		memset(&serialParams, 0, sizeof serialParams);
		serialParams.DCBlength = sizeof serialParams;

		GetCommState(tekFd, &serialParams);
		serialParams.BaudRate = CBR_19200;
		serialParams.ByteSize = 8;
		serialParams.Parity = NOPARITY;
		serialParams.StopBits = ONESTOPBIT;
		SetCommState(tekFd, &serialParams);

		COMMTIMEOUTS timeouts;
		memset(&timeouts, 0, sizeof timeouts);
		timeouts.ReadIntervalTimeout = MAXDWORD;
		timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
		timeouts.ReadTotalTimeoutConstant = 10000; // return with 0 bytes every 10 seconds.
		SetCommTimeouts(tekFd, &timeouts);


		size_t cap = 1024*1024;
		char *buf = malloc(cap);

		unsigned long nrd;
		size_t len = 0;
		int seq = 1;
		fprintf(stderr, "ready\n");
		while(ReadFile(tekFd, buf + len, cap - len, &nrd, NULL)){
			if(len == 0 && nrd > 0){
				fprintf(stderr, "started receiving hardcopy\n");
			}
			if(nrd > 0){
				len += nrd;
				buf[len] = '\0';
				char *p;
				if((p = strstr(buf, "%%EOF\n")) != NULL){
					char *epsName = malloc(256);
					char *pdfName = malloc(256);
					char *pngName = malloc(256);
					for(;;seq++){
						snprintf(epsName, 256, "%s-%d.eps", baseName, seq);
						snprintf(pdfName, 256, "%s-%d.pdf", baseName, seq);
						snprintf(pngName, 256, "%s-%d.png", baseName, seq);
						FILE *epsFp = fopen(epsName, "rb");
						if(epsFp != NULL){
							fprintf(stderr, "%s exists, skipping\n", epsName);
							fclose(epsFp);
							continue;
						}
						FILE *pdfFp = fopen(pdfName, "rb");
						if(pdfFp != NULL){
							fprintf(stderr, "%s exists, skipping\n", pdfName);
							fclose(pdfFp);
							continue;
						}
						FILE *pngFp = fopen(pngName, "rb");
						if(pngFp != NULL){
							fprintf(stderr, "%s exists, skipping\n", pngName);
							fclose(pngFp);
							continue;
						}

						break;
					}
					FILE *fp = fopen(epsName, "wb");
					if(fwrite(buf, 1, len, fp) == len){
						fprintf(stderr, "received %s (%zu bytes)\n", epsName, len);
					} else {
						strerror_s(buf, cap, errno);
						fprintf(stderr, "error writing %s: %s\n", epsName, buf);
					}
					epsToPdf(epsName, pdfName);
					epsToPng(epsName, pngName);
					free(epsName);
					free(pdfName);
					free(pngName);
					fclose(fp);
					if(p-buf != len-6)
						fprintf(stderr, "p-buf %zu len-6 %zu\n", p-buf, len-6);
					len = 0; // next file.
				}
			} else {
				fprintf(stderr, "read timeout from tek %s\n", tekName);
				continue;
			}
		}
		if(nrd == -1){
			strerror_s(buf, cap, errno);
			fprintf(stderr, "error reading from tek %s: %s\n", tekName, buf);
			free(buf);
			break;
		}

reopen:
		fprintf(stderr, "readfile error %lu\n", GetLastError());

		free(buf);
		CloseHandle(tekFd);
		break;
	}
}
