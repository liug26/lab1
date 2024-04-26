#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (argc == 1) {
		exit(22); // EINVAL
	}

	if (argc == 2) {
		execlp(argv[1], argv[1], NULL);
		exit(errno);
	}

	int fd[2];
	int fd2[2];
	int status, pid;
	status = pipe(fd);
	if (status < 0) {
		return status;
	}
	pid = fork();

	switch (pid) {
		case -1:
			return -1;
		case 0:
			// Child
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			execlp(argv[1], argv[1], NULL);
			exit(errno);exit(errno);
		default:
			waitpid(pid, &status, 0);
			if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
				return WEXITSTATUS(status);
			}
			close(fd[1]);
	}

	int i = 2;
	for (; i < argc-1; i++) {
		status = pipe(fd2);
		if (status < 0) {
			return status;
		}
		pid = fork();
		switch (pid) {
			case -1:
				return -1;
			case 0:
				// Child
				dup2(fd[0], STDIN_FILENO);
				dup2(fd2[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd2[0]);
				close(fd2[1]);
				execlp(argv[i], argv[i], NULL);
				exit(errno);
			default:
				// Parent
				waitpid(pid, &status, 0);
				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
					return WEXITSTATUS(status);
				}
				close(fd[0]);	// Close fd for good
				close(fd2[1]);

				fd[0] = fd2[0];
				fd[1] = fd2[1];
		}
	}

	if (i == 2) {
		fd2[0] = fd[0];
	}
	
	
	pid = fork();
	switch (pid) {
		case -1:
			return -1;
		case 0:
			// Child
			dup2(fd2[0], 0);
			close(fd2[0]);
			execlp(argv[i], argv[i], NULL);
			exit(errno);
		default:
			// Parent
			waitpid(pid, &status, 0);
			if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
				return WEXITSTATUS(status);
			}
			close(fd2[0]);
	}

	return 0;
}