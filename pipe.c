#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int num_programs = argc - 1;

	if (num_programs < 1)
    {
		fprintf(stderr, "Error: No arguments\n");
        exit(EINVAL);
	}

	if (num_programs == 1)
    {
		if (execlp(argv[1], argv[1], NULL) == -1){
			fprintf(stderr, "Error: execlp returns -1\n");
            exit(1);
		}
	}
	else
    {
		int fds[num_programs - 1][2];
		for (int i = 0; i < num_programs; i++)
        {
			// if not last program
			if (i < num_programs - 1)
            {
            	if (pipe(fds[i]) == -1)
                {
					fprintf(stderr, "Error when creating pipe\n");
            		exit(1);
            	}
        	}

			pid_t pid = fork();

            // CHILD
			if (pid == 0)
            {
				if(i > 0)
					dup2(fds[i - 1][0], STDIN_FILENO);
				
				if(i < num_programs - 1)
                {
					dup2(fds[i][1], STDOUT_FILENO);
					close(fds[i][0]);
					close(fds[i][1]);
				}

                // run program
				if (execlp(argv[i + 1], argv[i + 1], NULL) == -1)
                {
                    perror("execlp");
            		exit(1);
				}
			}
            // PARENT
			else if (pid > 0)
            {
				int status;
				waitpid(pid, &status, 0);

				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
                {
					fprintf(stderr, "Child exits with error\n");
					return WEXITSTATUS(status);
				}

                if (i > 0)
					close(fds[i - 1][0]);

				if (i < num_programs - 1)
                    close(fds[i][1]);
			} 
			else
            {
				fprintf(stderr, "Fork error\n");
            	exit(1);
			}
		}
	}
	return 0;
}
