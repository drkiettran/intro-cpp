#include <iostream>
using namespace std;

#include <stdio.h>
#include <unistd.h>

int main()
{

    int status, pid, pipefds[2];
    char instring[20];

	/* Create the pipe and return 2 file descriptors in the pipefds array */
	/* This operation is done before the fork so that both processes will */
	/* know about the same pipe, which will allow them to communicate.  */
    status = pipe(pipefds); 
	if (status == -1)
	{
		perror("Trouble");
		exit(1);
	}

	/* create child process; both processes continue from here */
    pid = fork();
    if (pid == -1)           
	{
		perror("Trouble");
		exit(2);
	}
    else if (pid == 0)           /* child : sends message to parent*/
	{
		/* close unused end of pipe */
		/* because this process only needs to write */
		close(pipefds[0]);

		/* send 7 characters in the string, including end-of-string */
		cout << "About to send a message: " << endl;
                write(pipefds[1], "Hi Mom!", 7);  

		close(pipefds[1]);
		exit(0);
	}
    else		/* parent : receives message from child */
	{
		/* close unused end of pipe */
		/* because this process only needs to read */
		close(pipefds[1]);

		/* read from the pipe */
                read(pipefds[0], instring, 7);
		cout << "Just received a message that says: " << instring << endl;

		close(pipefds[0]);
		exit(0);
	}
}