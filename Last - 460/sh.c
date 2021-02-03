/********************* sh.c file ****************/
#include "ucode.c"

int main(int argc, char* argv[]) {
	int pid, status, nk;
	char cmdLine[1024], cmd[1024]; 
	printf("**********MARCO ARES [SH]**********\n");
	while (1) {
		// get a command line; e.g. cmdLine = cmd | cam2 | ... | cmd &
		// get cmd token command line;
		printf("MA sh %d: ", getpid());
		gets(cmdLine);

		if (cmdLine[0] == 0)	// prompts again if nothing was entered
			continue;

		if (strcmp(cmdLine, "exit") == 0 || strcmp(cmdLine, "logout") == 0) {
			printf("LOGGING OUT...\n");
			printf("ADIOS!\n");
			exit(0);
		}
		else {
			// for binary executable command
			pid = fork();		// fork a child sh process
			if (pid) {			// parent sh
				printf("PARENT SH: %d WAIT CHILD %d TO DIE\n", getpid(), pid);
				pid = wait(&status);
				printf("SH %d: CHILD %d EXIT STATUS: %x\n", getpid(), pid, status);
			}
			else {
				printf("CHILD SH %d RUNNING\n", getpid());
				do_pipe(cmdLine, 0);
			}
		}
	}
}

int do_pipe(char* cmdLine, int* pd) {
	int hasPipe, pid, lpd[2], check;
	char* head = "", * tail, temp[1024];

	if (pd) {
		close(1);
		dup2(pd[1], 1); 
		close(pd[1]);
	}
	
	temp[0] = 0;
	strcpy(temp, cmdLine);
	hasPipe = scan(temp, head, tail);

	if (hasPipe) {
		//create a pipe lpd;
		check = pipe(lpd);

		if (check < 0) {
			printf("PIPE(LPD) FAILED\n");
			exit(1);
		}

		printf("HEAD: %s TAIL: %s\n", head, tail);
		pid = fork();	// forks a child
		if (pid) {
			// as reader on lpd;
			close(0);
			dup2(lpd[0], 0); 
			close(lpd[1]);
			do_command(tail);
		}
		else {
			do_pipe(head, lpd);
		}
	}
	else
		do_command(cmdLine);
}

int do_command(char* cmdLine) {
	int nk, i, j, len;
	char* name[1024], temp[1024], * head;

	temp[0] = 0;
	strcpy(temp, cmdLine);
	nk = eatpath(temp, name);
	// scan cmdLine for I/O redirection symbols;
	for (i = 0; i < nk; i++) {
		// do I/O redirections;
		if (strcmp(name[i], "<") == 0) {
			close(0);
			if (open(name[i + 1], O_RDONLY) < 0) {
				printf("CANNOT OPEN FILE\n");
				exit(1);
			}
			break;
		}

		if (strcmp(name[i], ">") == 0) {
			close(1);
			if (open(name[i + 1], O_WRONLY | O_CREAT) < 0) {
				printf("CANNOT OPEN FILE\n");
				exit(1);
			}
			break;
		}

		if (strcmp(name[i], ">>") == 0) {
			close(1);
			if (open(name[i + 1], O_WRONLY | O_CREAT | O_APPEND) < 0) {
				printf("CANNOT OPEN FILE\n");
				exit(1);
			}
			break;
		}
	}

	strcpy(head, name[0]);

	for (j = 1; j < nk; j++) {
		strcat(head, " ");
		strcat(head, name[j]);
	}

	exec(head);
}

int scan(char* cmdLine, char* head, char* tail) {
	char temp[1024];
	int check = 0;

	// divide cmdLine int head and tail by rightmost | symbol
	for (int i = strlen(cmdLine) - 1; i >= 0; i--) { // going from the rightmost
		if (cmdLine[i] == '|') {
			printf("SH: PIPE FOUND\n");
			strcpy(tail, cmdLine + i + 2);	// add it as the tail

			cmdLine[i - 1] = 0;
			strcpy(head, cmdLine);

			return 1;
		}
	}

	// no pipe found
	head = cmdLine;
	tail = 0;
	return 0;
}
