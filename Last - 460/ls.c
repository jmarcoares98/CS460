/********************* ls.c file ****************/
#include "ucode.c"

const char* t1 = "xwrxwrxwr-------";
const char* t2 = "----------------";

int ls_file(char* name)
{
	int r, i;
	struct stat sp;
	r = stat(name, &sp);
	char lnk[256];

	if ((sp.st_mode & 0xF000) == 0x4000)			// prints out that its a directory
		printf("%s", " d");
	else if ((sp.st_mode & 0xF000) == 0x8000)	// prints out that its a regular file
		printf("%s", " -");
	else if ((sp.st_mode & 0xF000) == 0xA000)	// prints out that its a link
		printf("%s", " l");
	else						// prints default
		printf("???");

	for (i = 8; i >= 0; i--) {
		if (sp.st_mode & (1 << i))
			printf("%c", (char*)t1[i]);
		else
			printf("%c", (char*)t2[i]);
	}
	printf("%d	%d	%d	%d	%s", sp.st_nlink, sp.st_uid, sp.st_gid, sp.st_size, name);

	// shows whats it point to when file a symlink
	if ((sp.st_mode & 0120000) == 0120000) {
		readlink(name, lnk);
		printf(" => %s\n", lnk);
	}
	else
		printf("\r\n");
}

int ls_dir(char *name)
{
	char* temp[256], buf[1024], * cp;
	DIR* dp;
	int fd;

	fd = open(name, O_RDONLY);
	read(fd, buf, 1024);
	cp = buf;
	dp = (DIR*)cp;

	while (cp < buf + 1024) {
		// get the name of the blocks
		strncpy(temp, dp->name, dp->name_len);
		temp[dp->name_len] = 0; //set the last one to null

		ls_file(temp);
		memset(temp, 0, 256);
		cp += dp->rec_len;
		dp = (DIR*)cp;
	}
	printf("\n");
}


int main(int argc, char *argv[])
{
	struct stat sp;
	char* s, filename[1024],cwd[1024];
	int check;

	printf("**********MARCO ARES [LS]**********\n");

	if (argc == 1) { // ls dir if ls was just entered
		getcwd(cwd);	// gets current working directory path
		strcpy(filename, cwd);
		ls_dir(filename);
	}
	else {
		getcwd(cwd);
		strcpy(filename, cwd);
		strcat(filename, "/");
		strcat(filename, argv[1]);


		if ((sp.st_mode & 0xF000) == 0x4000) {
			ls_dir(filename);		// list DIR
		}
		else {
			ls_file(filename);		// list single file
		}
	}

	return;
}