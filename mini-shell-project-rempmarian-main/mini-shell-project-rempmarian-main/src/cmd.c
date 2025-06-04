// SPDX-License-Identifier: BSD-3-Clause
/*Name: Marian Rempola
  Student ID: mhr5220
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include "cmd.h"
#include "utils.h"

#include "stdio.h" // TA approved lib

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	/* TODO: Execute cd. */
	// Sanity checks
	if (dir == NULL || dir->string == NULL) {
		perror("cd: missing argument(s)\n");
	} 
	// Call cd	
	else if (chdir(get_word(dir)) == -1) {
		return 1;
	}
	return 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	/* TODO: Execute exit/quit. */
	exit(0);
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* TODO: Sanity checks. */
	if (s == NULL || s->verb == NULL || level < 0) return shell_exit();

	/* TODO: If builtin command, execute the command. */
	// ptr to command
	char *cmd = get_word(s->verb);

	// 1. exit command(s): call shell_exit 
	if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
		shell_exit();
	}
	// 2. cd command: handle redirection, then call shell_cd, exit with status
	else if(strcmp(cmd, "cd") == 0) {
		// save a copy of output fd and get destination fd as a char
		int fdOriginal = dup(STDOUT_FILENO);
		char *destination = get_word(s->out);
		// system call to open the destination fd (syntax: open (const char *__path, int __oflag, ...) )
		int fd;
		if (s->io_flags & IO_OUT_APPEND) {
			fd = open(destination, O_WRONLY | O_APPEND | O_CREAT, 0644);
		} else {
			fd = open(destination, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		}
		// redirect output ( syntax from pdf: dup2(int oldfd, newfd) 
		dup2(fd, STDOUT_FILENO);
		close(fd);
		
		// get the status of the call and closing the copy
		int status = shell_cd(s->params);

		// redirect stdout back to original
		dup2(fdOriginal, STDOUT_FILENO);
		close(fdOriginal);
		return status;
	}
	/* TODO: If variable assignment, execute the assignment and return
	 * the exit status. (e.g. HOME=/home/user)
	 */
	char *var = get_word(s->verb);
	// set my environment variables
	if(strchr(var, '=') != NULL) {
		// as per discussion FAQ, use putenv instead of env
		putenv(var);		
		return 0;
	}

	/* TODO: If external command:
	 *   1. Fork new process
	 *     2c. Perform redirections in child
	 *     3c. Load executable in child
	 *   2. Wait for child
	 *   3. Return exit status
	 */
	// 1. fork new process
	int pid = fork(); 
	if(pid < 0) {
		perror("fork() failed");
		return -1;
	} else if (pid == 0) {
		// 2c. perform redireciton: get the input, check if it's null, open/read it, redirect it
		if (s->in != NULL) {
			char *in = get_word(s->in);
			int fdIn = open(in, O_RDONLY);
			dup2(fdIn, STDIN_FILENO);
			close(fdIn);
		}

		// if output is an error, redirect it! (refer to notes below)
		if (s->err != NULL && s->out != NULL && strcmp(get_word(s->out), get_word(s->err)) == 0) {
			char *outFd = get_word(s->out);
			int fd;
			// check what io flags were passed (referred to discussion post on FAQ and pdf guide)
			if (s->io_flags & IO_OUT_APPEND) {
				fd = open(outFd, O_WRONLY | O_APPEND | O_CREAT, 0644);
			} else {
				fd = open(outFd, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			}
			// redirection
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			close(fd);
		} 
		else {
			// if there is just an error
			if (s->err != NULL) {
				char *errFd = get_word(s->err);
				int fd;
				// check what io flags were passed
				if (s->io_flags & IO_ERR_APPEND) {
					fd = open(errFd, O_WRONLY | O_APPEND | O_CREAT, 0644);
				} else {
					fd = open(errFd, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				}
				// redirection
				dup2(fd, STDERR_FILENO);
				close(fd);
			} 
			// if there is just an output 
			if (s->out != NULL) {
				char *outFd = get_word(s->out);
				int fd;
				// check what io flags were passed
				if (s->io_flags & IO_OUT_APPEND) {
					fd = open(outFd, O_WRONLY | O_APPEND | O_CREAT, 0644);
				} else {
					fd = open(outFd, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				}
				// redirection
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
		}
		
		// 3c. load executable in child
		// use get_argv() --> note from pdf
		int size =0;
		char **argv = get_argv(s, &size);
		// call execvp(const char *file, char *const argv[])
		execvp(argv[0], argv);
		// if it failed, using exit(127) --> cite: looked up what the 'command not found' exit code is
		// citing discussion post 'Mini Shell Imports' and office hours on Friday, that verify that I can use stdio.h lib
		fprintf(stderr, "execution failed for '%s'\n", argv[0]);
		exit(127);
		
	} else {
		// wait for child
		int waitStatus;
		waitpid(pid, &waitStatus, 0);

		return WEXITSTATUS(waitStatus);
	}

	return 0; /* TODO: Replace with actual exit status. */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Execute cmd1 and cmd2 simultaneously. */
	// child 1: put on execution tree and exit 
	pid_t child1 = fork();
	if(child1 == 0) {
		int status1 = parse_command(cmd1, level+1, father);
		exit(status1);
	}

	// child 2: put on execution tree and exit 
	pid_t child2 = fork();
	if(child2 == 0) {
		int status2 = parse_command(cmd2, level+1, father);
		exit(status2);
	}

	// exit and return proper exit type
	int status1;
	int status2;
	waitpid(child1, &status1, 0);
	waitpid(child2, &status2, 0);
	return WEXITSTATUS(status1) || WEXITSTATUS(status2);
	/* TODO: Replace with actual exit status. */
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 * 
 * Cite: referring to sync/pipe.c from class
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Redirect the output of cmd1 to the input of cmd2. */
	// file desc for the pipe (pipefd[0] for reading, pipefd[1] for writing)
	int pipefd[2];

	// creating pipe 
	if(pipe(pipefd) == -1) {
		perror("failed to create pipe");
		return SHELL_EXIT;
	}

	// creating child process for 1
	pid_t pid1 = fork();
	if(pid1 < 0) {
		perror("failed to fork process 1");
		return SHELL_EXIT;
	}
	// 1. redirect output of process1 to input of process2
	if (pid1 == 0) {
		//close unused read
		close(pipefd[READ]);
		// write to pipe and copying it using dup2
		dup2(pipefd[WRITE], STDOUT_FILENO);
		// closing write end
		close(pipefd[WRITE]);
		// exit with status 
		int status = parse_command(cmd1, level+1, father);
		exit(status);
	} 

	// creating child process for 2
	pid_t pid2 = fork();
	if(pid2 < 0) {
		perror("failed to fork process 2");
		return SHELL_EXIT;
	}
	// 2. reading what I just wrote to one of the pipe from process 1
	if (pid2 == 0) {
		//close unused write
		close(pipefd[WRITE]);
		// read from pipe and duplicate it
		dup2(pipefd[READ], STDIN_FILENO);
		// closing read end
		close(pipefd[READ]);
		// exit with status 
		int status = parse_command(cmd2, level+1, father);
		exit(status);
	}
	
	// close both ends of the pipe
	close(pipefd[READ]);
	close(pipefd[WRITE]);
	// exit and return proper exit type
	int status1;
	int status2;
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	return  WEXITSTATUS(status2);

	// return true; /* TODO: Replace with actual exit status. */
}

/**
 * Parse and execute a command: main entry point that handles all cmds 
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* TODO: sanity checks */
	if (c == NULL || level < 0 ) return shell_exit(); 

	if (c->op == OP_NONE) {
		/* TODO: Execute a simple command. */
		int status = parse_simple(c->scmd, level, father);
		return status; /* TODO: Replace with actual exit code of command. */
	}

	switch (c->op) {
	case OP_SEQUENTIAL: {
		/* TODO: Execute the commands one after the other. */
		int status1 = parse_command(c->cmd1, level + 1, c);
		int status2 = parse_command(c->cmd2, level + 1, c);
		return status1 || status2; // assuming just one of the commands work
	}
	case OP_PARALLEL:
		/* TODO: Execute the commands simultaneously. */
		return run_in_parallel(c->cmd1, c->cmd2, level + 1, c);
	case OP_CONDITIONAL_NZERO: {
		/* TODO: Execute the second command only if the first one
		 * returns non zero.
		 */
		int firstStatus = parse_command(c->cmd1, level +1, c);
		if(firstStatus != 0) {
			// execute second command since first one return non zero
			return parse_command(c->cmd2, level+1, c);
		}
		break;
	}
	case OP_CONDITIONAL_ZERO: {
		/* TODO: Execute the second command only if the first one
		 * returns zero.
		 */
		int firstStatus = parse_command(c->cmd1, level+1, c);
		if(firstStatus == 0) {
			// execute second command since first one return zero
			return parse_command(c->cmd2, level+1, c);
		}
		break;
	}
	case OP_PIPE:
		/* TODO: Redirect the output of the first command to the
		 * input of the second.
		 */
		return run_on_pipe(c->cmd1, c->cmd2, level+1, c); 
	default:
		return SHELL_EXIT;
	}

	/* TODO: Replace with actual exit code of command. */
	return 1;
}

/* notes on redirection: 
	1. output: dup2(fd, STDOUTFILENO)
	where fd=open(out,...); char *out=get_word(s->in);
	2. if error: redirect it to error 
	   if output: redirect it to output
*/