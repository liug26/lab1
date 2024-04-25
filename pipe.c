#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	//creating appropriate # of pipes
	int fds[argc - 2][2]; //array of file descriptors
	for (int i = 0; i < argc-2; i++) {
		pipe(fds[i]);
	}

	//for loop to create children
	for (int i = 1; i < argc; i++) { 
		int res = fork();
		if (res == 0) { //CHILD
			//insert a dup2 to redirect stdout
			if (i < argc-1) {
				dup2(fds[i-1][1], STDOUT_FILENO);
			}
			else { //if its the last one, print to terminal now
				dup2(STDOUT_FILENO, STDOUT_FILENO);
			}
			//execute program
			execlp(argv[i], argv[i], NULL);
			perror("execlp");
			return EXIT_FAILURE;
		
		}
		else if (res > 0) { //PARENT
			//insert a waitpid
			int pid = res;
			int status = 0;
			waitpid(pid, &status, 0);
			if (WEXITSTATUS(status) == EXIT_FAILURE) {
				return EXIT_FAILURE;
			}
			//close writing for curr pipe
			close(fds[i-1][1]);
			//close reading for previous pipe
			if (i != 1) {
				close(fds[i-2][0]);
			}
			//insert a dup2 to set stdin
			dup2(fds[i-1][0], STDIN_FILENO); //next pipe
			
		}
		else { //error
			return EXIT_FAILURE;
		}
	}
	return 0;
}
