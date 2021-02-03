/********************* login.c file ****************/
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
#include "ucode.c"

int in, out, err, fd; 
char tokens[7][256];

int main(int argc, char* argv[])
{
	signal(2, 1);
	int i, j = 0, n, fd;
	char buf[256], * line, *line2;
	char username[128], password[128];
	char* temp;
	// (1).close file descriptors 0, 1 inherited from INIT.
	close(0);
	close(1);

	// (2).open argv[1] 3 times as in(0), out(1), err(2).
	in = open(argv[1], O_RDONLY);
	out = open(argv[1], O_WRONLY);
	err = open(argv[1], 2);

	// (3).settty(argv[1]); // set tty name string in PROC.tty
	settty(argv[1]);

	printf("**********MARCO ARES [LOGIN]**********\n");
	// (4).open / etc / passwd file for READ;
	fd = open("/etc/passwd", O_RDONLY);
	if (fd < 0) {
		printf("LOGIN: CANNOT OPEN FILE\n");
		exit(1);
	}

	n = read(fd, buf, 1024);

	while (1) {
		// (5).printf("login:"); gets(name);
		printf("login: "); gets(username);
		// printf("password:"); gets(password);
		printf("password: "); gets(password);
		temp = &buf[0];

		// for each line in / etc / passwd file do {
		// tokenize user account line;	
		tokenize(temp);

		// (6). if (user has a valid account) {
		if (strcmp(tokens[0], username) == 0 && strcmp(tokens[1], password) == 0) {
			printf("LOGGED IN\n");
			// (7).change uid, gid to user's uid, gid; // chuid()
			chuid(atoi(tokens[2]), atoi(tokens[3]));

			// change cwd to user's home DIR // chdir()
			chdir(tokens[5]);

			// close opened / etc / passwd file // close()
			close(fd);

			// (8).exec to program in user account // exec()
			exec("sh");
		}


		printf("login failed, try again\n");
	}
}

int tokenize(char* line)
{
	int i = 0, j = 0, k = 0;
	while (*line) {
		if (*line != ':') {
			tokens[i][j] = *line;
			j++;
		}
		else {
			tokens[i][j++] = 0;
			i++;
			j = 0;
		}

		line++;
	}

	tokens[i][j] = 0;
}