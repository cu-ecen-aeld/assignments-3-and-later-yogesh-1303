/*man pages for socket related functions are referred to while making this code */ 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef void (*sighandler_t)(int);

#define BACKLOG 20
#define NUM_CHARACTERS 1000
#define PORT "9000"
#define BUFF_LEN 1

int sig = 1;
int fd; //for the file /var/tmp/aesdsocketdata.txt
sighandler_t sigint_handler()
{
	syslog(LOG_INFO,"Caught signal, exiting");
	remove("/var/tmp/aesdsocketdata.txt");
}

int main()
{
	int socketfd, s, new_fd;
	int wr,rd;
	struct addrinfo hints;
	struct addrinfo *res, *rp, *p;
	struct sockaddr_storage client_addr;
	//const char *port  = "9000";
	char ipstr[30];
	socklen_t addr_size;
	//buffer for reading the data from the file and sending
	char *send_buf = (char *)malloc(NUM_CHARACTERS * sizeof(char));
	//buffer for receiving the data
	char *buf = (char *)malloc(NUM_CHARACTERS * sizeof(char));
	if(buf == NULL)
	{
		perror("\nmalloc:");
		exit(1);
	}

	//file in which the received data is stored
	fd = open("/var/tmp/aesdsocketdata.txt", O_RDWR | O_CREAT | O_APPEND, 0777);


	//clear the structure instance
	memset(&hints, 0, sizeof(hints));
	//calling getaddrinfo for getting the address for bind
	hints.ai_family = AF_UNSPEC;	//any protocol: IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	//TCP
	hints.ai_flags = AI_PASSIVE;    //assign address	


	//starting the connection with the client using the series of functions
	if(getaddrinfo(NULL, PORT, &hints, &res) != 0)
	{
		perror("\ngetaddrinfo");
		exit(1);
	}
	
	//converting the IP address into string format
	for(p = res; p != NULL; p = res->ai_next)
	{
		void *addr;
		char *iptype;

		//if ipv4
		if (p->ai_family == AF_INET) 
		{ 
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            iptype = "IPv4";
        } 
		//if ipv6
		else 
		{
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            iptype = "IPv6";
        }

		//convert the IP address to string
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	}

	//calling the socket function
	if(socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) == -1)
	{
		perror("\nsocket");
		exit(1);
	}

	//bind to a connection
	if(bind(socketfd, res->ai_addr, res->ai_addrlen) != 0)
	{
		perror("\nbind");
		exit(1);
	}

	//listen to a connection request from a client
	if(listen(socketfd, BACKLOG) == -1)
	{
		perror("\nlisten");
		exit(1);	
	}
	
	//accept the connection with the client
	addr_size = sizeof(client_addr);
	if((new_fd = accept(socketfd, (struct sockaddr *)&client_addr, &addr_size)) == -1 )
	{
		perror("\naccept");
		exit(1);	
	}
	else
	{
		printf( "Accepted connection from %s", ipstr);
	}
	
	int count = 0;
	
	//receive 1 byte at a time until the end of line character is received
	while(*(buf+count) != '\0')
	{
		if(recv(socketfd, buf, BUFF_LEN, 0) == -1)
		{
			perror("\nrecv");
			exit(1);
		}
		count++;

		//check if the size of the buffer is exceeded
		if(count >= NUM_CHARACTERS)
		{
			printf("\nlimit for storage reached");
		}
	}

	//print the received buffer
	printf("\nreceived string: ");
	puts(buf);

	count = 0;

	//write the buffer received in the file and check for partial writes
	while(*(buf+count) != '\0')
	{
	if((wr = write(fd, buf, BUFF_LEN)) == -1)
	{
		perror("\nwrite");
		exit(1);
	}
	count++;
	if(wr != strlen(buf))
	{
		perror("\nwrite partial data");
	}
	}

	count = 0;
	//read and send untill the end of line character is reached
	while(*(send_buf + count) != '\0')
	{
		rd = read(fd, send_buf, 1);
		if(rd == -1)
		{
			perror("Read from the file");
			exit(1);
		}

		if(send(socketfd, send_buf, 1, 0))
		count++;
		//check if the size of the buffer is exceeded
		if(count >= NUM_CHARACTERS)
		{
			printf("\nlimit for storage reached");
		}
	}

	freeaddrinfo(res);
	close(new_fd);
	close(fd);
	syslog(LOG_INFO, "Closed connections from %s", ipstr);
	int new_fd_loop;
	
	//keep accepting the connections until SIGINT is sent
	signal(SIGINT, sigint_handler());
	while(sig != 0)
	{
		addr_size = sizeof(client_addr);
		new_fd_loop = accept(socketfd, (struct sockaddr *)&client_addr, &addr_size);
	}

	return 0;
}
