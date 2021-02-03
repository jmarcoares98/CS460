/********************* more.c file ****************/
#include "ucode.c"
// more[filename]: if no filename : show inputs from stdin to stdout same as cat
// more f : show ONE screen of f;
//		<enter> : show one more line
//		<space> : show on more screen(of 25 lines)

// cat f | more:
//  	<enter> : show one more line
//  	<space> : show one more screen(of 25 lines)

int main(int argc, char* argv[])
{
	int fd, fd2, n, i, j = 0, k = 0, stdin = 0, stdout = 0;
	char buf[1024], c, r = '\r';
	struct stat sp, mystat;
	printf("**********MARCO ARES [MORE]**********\n");

	// (1). stat(gettty(), &st_tty); fstat(0, &st0); fstat(1,&st1);
	// (2). 0 has been redirected if st_ttty.(dev,ino) != st0.(dev,ino)
	// (3). 1 has been redirected if st_tty.(dev,ino) != st1.(dev,ino)

	gettty(mytty);
	stat(mytty, &myst);
	fstat(0, &st0);
	fstat(1, &st1);

	if (myst.st_ino != st0.st_ino) {
		stdin = 1;
	}
	if (myst.st_ino != st1.st_ino) {
		stdout = 1;
	}

	if (stdin == 1 && stdout == 0) {	// check if stin is redirected or not
		fd = dup(0);
		close(0);
		fd2 = open(mytty, O_RDONLY);
	}
	else if(argc > 1) {
		fd = open(argv[1], O_RDONLY);
		if (fd < 0) {
			printf("MORE: CANNOT OPEN FILE\n");
			exit(1);
		}

		for (j = 0; j < 20; j++) {
			while (1) {
				n = read(fd, buf, 1);
				if (!n)
					exit(0);

				write(1, &buf[0], 1);

				if (buf[0] == '\n' || buf[0] == '\r') {
					write(2, &r, 1);
					break;
				}

			}
		}
	}

	while (1) {
		read(0, &c, 1);
		if (c == '\n' || c == '\r') { //  	<enter> : show one more line
			while (1) {
				n = read(fd, buf, 1);
				if (!n)
					exit(0);
				write(1, &buf[0], 1);

				if (buf[0] == '\n' || buf[0] == '\r') {
					write(2, &r, 1);
					break;
				}
			}
		}
		else if (c == ' ') {
			for (j = 0; j < 25; j++) { //  	<space> : show one more screen(of 25 lines)
				while (1) {
					n = read(fd, buf, 1);
					if (!n)
						exit(0);
					write(1, &buf[0], 1);

					if (buf[0] == '\n' || buf[0] == '\r') {
						write(2, &r, 1);
						break;
					}

				}
			}
		}
		else {//  	<any char> : one char at a time
			n = read(fd, buf, 1);
			if (!n)
				exit(0);
			write(1, &buf[0], 1);

			if (buf[0] == '\n' || buf[0] == '\r') {
				write(2, &r, 1);
			}
		}

	}
	close(fd);
	close(fd2);

}