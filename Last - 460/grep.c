/********************* grep.c file ****************/
#include "ucode.c"

int main(int argc, char* argv[])
{
	int fd, fd2, n, i, j = 0, length, k = 0, found = 1, check, stdin = 0, stdout = 0;
	char buf[1024], buf2[1024], word[128], c[1], r = '\r';
	printf("**********MARCO ARES [GREP]**********\n");

	// (1). stat(gettty(), &st_tty); fstat(0, &st0); fstat(1,&st1);
	// (2). 0 has been redirected if st_ttty.(dev,ino) != st0.(dev,ino)
	// (3). 1 has been redirected if st_tty.(dev,ino) != st1.(dev,ino)

	gettty(mytty);
	stat(mytty, &myst);
	fstat(0, &st0);
	fstat(1, &st1);

	if (myst.st_ino != st0.st_ino)
		stdin = 1;
	if (myst.st_ino != st1.st_ino)
		stdout = 1;

	if (stdin == 1 && stdout == 0) {
		fd = dup(0);
		close(0);
		fd2 = open(mytty, O_RDONLY);
	}
	else if (argc > 2) {
		if (argc == 4 && stdin == 1) {
			fd = dup(0);
			close(0);
			fd2 = open(mytty, O_RDONLY);
		}
		else {
			close(0);
			fd = open(argv[2], O_RDONLY);
			dup(fd);
		}

		if (fd < 0) {
			printf("GREP: CANNOT OPEN FILE\n");
			exit(1);
		}
	}
	else if(argc < 2) {
		printf("GREP: NO PATTERN\n");
		exit(0);
	}
	else{
		fd = 0;
	}

	if (argc > 2 || stdin == 1) { // file was given
		length = strlen(argv[1]);
		strcpy(word, argv[1]);
		word[length] = 0;

		while (1) {
			n = read(fd, buf, 1024);

			//if (n == 0)		// exits loop when we hit the end of file
			//	break;

			i = 0;
			while (i < n) {
				// this will try to read each word in the line until it hits the end of line
				if (buf[i] == '\r' || buf[i] == '\n') {
					buf2[k] = 0;
					for (j = 0; j < k; j++) {
						if (word[found] == buf2[j]) {
							found++;

							if (found == length) {	// found the word
								printf("%s\n", buf2);
							}
						}
						else
							found = 0;
					}


					buf2[0] = '\0';
					k = 0;
				}
				else {
					buf2[k] = buf[i];	// grabs the sentence as a whole.
					k++;
				}


				i++;
			}
		}
		close(fd);
		close(fd2);
	}
	else{
		// no filename given
		while (gets(c)) {
			if (strcmp(c, argv[1]) == 0)
				printf("%s\n", c);
		}
	}

}