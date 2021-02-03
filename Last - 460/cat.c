/********************* cat.c file ****************/
#include "ucode.c"

int main(int argc, char * argv[]) {
	char buf[1024], buf2[1];  // a null char at end of mybuf[ ]
	char* s, r = '\r';
	int n, i, fd = 0, stdin = 0, stdout = 0;

	gettty(mytty);
	stat(mytty, &myst);
	fstat(0, &st0);
	fstat(1, &st1);

	if (myst.st_ino != st0.st_ino)
		stdin = 1;
	if (myst.st_ino != st1.st_ino)
		stdout = 1;

	if (argc > 1) {
		fd = open(argv[1], O_RDONLY);

		if (fd < 0)
			exit(1);
	}

	// 1. int fd = open filename for READ;
	if (fd) {

		// 2. while (n = read(fd, buf[1024], bufsize)) {
		while (n = read(fd, buf, 1024)) {
			i = 0;
			buf[n] = '\0';             // as a null terminated string

			while (i < n) {
				write(1, &buf[i], 1);
				if (buf[i] == '\n' || buf[i] == '\r')
					write(2, &r, 1);

				i++;
			}
		}

	}
	else {	// there was no path given
		while (gets(buf2) > 0) {
			printf("%s\n", buf2);
		}
	}
	// 3. close(fd);
	close(fd);
}
