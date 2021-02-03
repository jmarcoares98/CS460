/********************* init.c file ****************/
#include "ucode.c"
typedef enum __BOOL__ { false, true } bool;

int console, serial0, serial1;

int parent() {
	int status, pid;

	while (true) {
		pid = wait(&status);

		if (pid == console) {
			console = fork();

			if (console)
				continue;
			else
				exec("login /dev/tty0");
		}

		if (pid == serial0) {
			serial0 = fork();

			if (serial0)
				continue;
			else
				exec("login /dev/ttyS0");
		}

		if (pid == serial1) {
			serial1 = fork();

			if (serial1)
				continue;
			else
				exec("login /dev/ttyS1");
		}
	}
}

int main() {
	int stdin, stdout; // file descriptors for terminal I/O

	stdin = open("/dev/tty0", O_RDONLY); // file descriptor 0
	stdout = open("/dev/tty0", O_WRONLY); // for display to console
	printf("**********MARCO ARES [INIT]**********\n");
	printf("INIT : FORK A LOGIN ON CONSOLE\n");

	console = fork();

	if (console == 0)
		exec("login /dev/tty0");
	else {
		serial0 = fork();
		if (serial0 == 0) {
			exec("login /dev/ttyS0");
		}
		else {
			serial1 = fork();
			if (serial1 == 0) {
				exec("login /dev/ttyS1");
			}
			else
				parent();
		}
	}
}