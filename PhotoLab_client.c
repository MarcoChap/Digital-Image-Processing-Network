/*********************************************************************/
/* PhotoLab_client.c: Assignment 8 for CENG231, Spring 2023          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 256

/* Displays an error message to the command line */
void FatalError(const char *Program, const char *ErrorMsg);

/* Sends a request to the specified socket */
int SendRequest(const int SocketFD, const char *ReqMsg);

/* Prints out the available command flags */
void PrintUsage();

int main(int argc, char *argv[])
{
	int x = 1, b_autotest = 0, b_print = 0;
	char *server_ip;
	int port_no;

	while (x < argc)
	{
		/* the server ip address option */
		if (strcmp(argv[x], "-ip") == 0)
		{
			if (x < argc - 1)
			{
				server_ip = argv[x + 1];
			} /*fi*/
			else
			{
				FatalError(argv[0], "Missing argument for server IP address!");
			} /*esle*/
			x += 2;
			continue;
		}
		else if (strcmp(argv[x], "-p") == 0)
		{
			if (x < argc - 1)
			{
				port_no = atoi(argv[x + 1]);
				if (port_no < 2000 || port_no > 65535)
				{
					FatalError(argv[0], "Port must be greater than 2000");
				}
			}
			else
			{
				FatalError(argv[0], "Missing argument for server port!");
			}
			x += 2;
			continue;
		}
		else if (strcmp(argv[x], "-autotest") == 0)
		{
			b_autotest = 1;
			x++;
			continue;
		}
		else if (strcmp(argv[x], "-print") == 0)
		{
			b_print = 1;
			x++;
			continue;
		}
		else if (strcmp(argv[x], "-h") == 0)
		{
			PrintUsage();
			return 0;
		}
		else
		{
			FatalError(argv[0], "Unknown command detected. Do -h for help");
		}
		x++;
	}

	if (b_autotest == 0 && b_print == 0)
	{
		FatalError(argv[0], "Missing command: -autotest or -print");
	}

	int n;
	int socket_fd; /* socket file descriptor */
	struct sockaddr_in
		server_addr; /* server address we connect with */
	struct hostent
		*server;		   /* server host */
	char buffer[BUF_SIZE]; /* message buffer */
	/* command line handling */
	if (argc < 3)
	{
		FatalError(argv[0], "Usage: <Program> hostname port");
	}

	printf("%s: Creating a socket...\n", argv[0]);
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		FatalError(argv[0], "Socket creation failed");
	}

	server = gethostbyname(server_ip);
	if (server == NULL)
	{
		char errorMsg[256];
		sprintf(errorMsg, "No such host named '%s'", argv[1]);
		FatalError(argv[0], errorMsg);
	}

	printf("%s: Using port %d...\n", argv[0], port_no);
	printf("%s: Preparing the server address...\n", argv[0]);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

	printf("%s: Connecting to the server...\n", argv[0]);
	if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		FatalError(argv[0], "Connection failed");
	}
	if (b_print)
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "PRINT");
		printf("%s: Sending a request %s\n", argv[0], buffer);
		n = write(socket_fd, buffer, strlen(buffer));
		if (n < 0)
		{
			FatalError(argv[0], "Writing to socket failed");
		}

		printf("%s: Waiting for response...\n", argv[0]);
		memset(buffer, 0, sizeof(buffer));
		n = read(socket_fd, buffer, BUF_SIZE - 1);
		if (n < 0)
		{
			FatalError(argv[0], "Reading from socket failed");
		}

		printf("%s: Received response: %s\n", argv[0], buffer);
	}

	if (b_autotest)
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "AUTOTEST");
		printf("%s: Sending a request %s\n", argv[0], buffer);
		n = write(socket_fd, buffer, strlen(buffer));
		if (n < 0)
		{
			FatalError(argv[0], "Writing to socket failed");
		}

		printf("%s: Waiting for response...\n", argv[0]);
		memset(buffer, 0, sizeof(buffer));
		n = read(socket_fd, buffer, BUF_SIZE - 1);
		if (n < 0)
		{
			FatalError(argv[0], "Reading from socket failed");
		}

		printf("%s: Received response: %s\n", argv[0], buffer);
	}

	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "CLOSE");
	printf("%s: Sending a request %s\n", argv[0], buffer);
	n = write(socket_fd, buffer, strlen(buffer));
	if (n < 0)
	{
		FatalError(argv[0], "Writing to socket failed");
	}

	printf("%s: Waiting for response...\n", argv[0]);
	memset(buffer, 0, sizeof(buffer));
	n = read(socket_fd, buffer, BUF_SIZE - 1);
	if (n < 0)
	{
		FatalError(argv[0], "Reading from socket failed");
	}

	printf("%s: Received response: %s\n", argv[0], buffer);

	printf("%s: Exiting...\n", argv[0]);
	close(socket_fd);
	return 0;
}

void PrintUsage()
{
	printf("Usage: PhotoLab_client -ip <server_ip> -p <port_no> -print -autotest\n");
	printf("Options:\n");
	printf("-ip      \t Specify the server IP address\n");
	printf("-p       \tSpecify the server port number\n");
	printf("-print   \t Send request to server to print supported DIP operations\n");
	printf("-autotest\t Send request to server to run AutoTest functionality\n");
	printf("-h       \t Display this usage information\n");
}

void FatalError(const char *Program, const char *ErrorMsg)
{
	fprintf(stderr, "%s: %s\n", Program, ErrorMsg);
	exit(EXIT_FAILURE);
}

int SendRequest(const int SocketFD, const char *ReqMsg)
{
	int n;

	/* write the request message to the socket */
	n = write(SocketFD, ReqMsg, strlen(ReqMsg));

	/* check if the write operation was successful */
	if (n < 0)
	{
		FatalError("SendRequest", "Writing to socket failed");
	}

	return n;
}