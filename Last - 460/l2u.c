/********************* l2u.c file ****************/
#include "ucode.c"

int main(int argc, char* argv[])
{
	int fd, gd, n, i;
	char lbuf[1024], ubuf[1024], stdin[1], nfile[1024];
	printf("**********MARCO ARES [L2U]**********");

	if (argc == 1) {
		while (n = read(0, stdin, 1)) {
			if (stdin[0] >= 97 && stdin[0] <= 122) // check if they are lowercase
				stdin[0] = stdin[0] - 32; // converting to uppercase;
			else if (stdin[0] == '\r' || stdin[0] == '\r') {
				putchar("\n");
			}
			write(1, stdin, 1);
		}
	}
	else if (argc == 3) {
		strcpy(nfile, argv[2]);

		fd = open(argv[1], O_RDONLY);
		if (fd < 0) {
			printf("L2U: CANNOT OPEN FILE\n");
			exit(1);
		}
		creat(nfile);
		gd = open(nfile, O_WRONLY);
		if (gd < 0) {
			printf("L2U: CANNOT OPEN FILE\n");
			exit(1);
		}
		while (n = read(fd, lbuf, 1024)) {
			i = 0;
			while (i < n) {
				if (lbuf[i] >= 97 && lbuf[i] <= 122)
					ubuf[i] = lbuf[i] - 32;
				else
					ubuf[i] = lbuf[i];
			}

			write(gd, ubuf, n);
		}
	}
}