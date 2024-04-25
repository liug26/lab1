#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	//checks to see if there are 1 argument
	if (argc < 2){
		fprintf(stderr, "Error: Invalid argument\n");
        exit(EINVAL);
	}

	//first arg is program name
	int num_programs = argc - 1;

	//if there is 1 command (no pipe needed)
	if (num_programs == 1){
		if (execlp(argv[1], argv[1], NULL) == -1){
			fprintf(stderr, "Error: execlp\n");
            exit(1);
		}
	}
	else{
		//an array that store fds
		int pipe_fd[num_programs-1][2];
		int i;
		for (i = 0; i < num_programs; i++){
	
			//create one less pipe than program
			if (i < num_programs - 1) {
            	if (pipe(pipe_fd[i]) == -1) {
					fprintf(stderr, "Error: pipe\n");
            		exit(1);
            	}
        	}

			pid_t child_pid = fork();

			if (child_pid == 0) {
				//child process 

				if(i > 0){
					dup2(pipe_fd[i-1][0],STDIN_FILENO); //redirect fd0 to read from last output 
				}
				
				if( i < num_programs -1){
					dup2(pipe_fd[i][1],STDOUT_FILENO); //redirect fd1 to write
					close(pipe_fd[i][0]); //close curr read and right pipe
					close(pipe_fd[i][1]);
				}

				//replace curr with the command
				if (execlp(argv[i+1], argv[i+1], NULL) == -1){
					fprintf(stderr, "Error: execlp\n");
            		exit(1);
				}
			}		 
			else if (child_pid > 0){
				//parent process

				//wait
				int status;
				waitpid(child_pid, &status, 0);

				//if child exit 1 the main program also exit 1
				if (WEXITSTATUS(status) == 1) {
					fprintf(stderr, "child's error\n");
					return 1;
				}

				close(pipe_fd[i][1]); //close write pipe

				if( i > 0){
					close(pipe_fd[i-1][0]); //close the previous read pipe
				}
			} 
			else {
				fprintf(stderr, "Error: fork\n");
            	exit(1);
			}
		}
	}
	return 0;
}
