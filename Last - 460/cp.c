/********************* cp.c file ****************/
#include "ucode.c"


int main(int argc, char* argv[]) {
	int fd, gd, n;
	char buf[1024];
	// 1. fd = open src for READ;
	fd = open(argv[1], O_RDONLY);

	if (fd < 0) {
		printf("FAILED TO OPEN SRC\n");
		exit(1);
	}
	// 2. gd = open dst for WR | CREAT;
	gd = open(argv[2], O_WRONLY);
	if (gd < 0) {
		creat(argv[2]);
		gd = open(argv[2], O_WRONLY);
	}

	while (n = read(fd, buf, 1024))
	{
		write(gd, buf, n);
	}
	printf("\n\r");

	close(fd);
	close(gd);
}

