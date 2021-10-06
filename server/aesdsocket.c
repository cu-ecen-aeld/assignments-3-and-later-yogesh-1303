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

#define BACKLOG (10)
#define PORT "9000"

int main()
{
	struct addrinfo hints;
	struct addrinfo *res, *p;
	//clear the structure instance
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;	//any protocol: IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	//TCP
	hints.ai_flags = AI_PASSIVE;    //assign address

	//starting the connection with the client using the series of functions
	if(getaddrinfo(NULL, PORT, &hints, &res) != 0)
	{
		perror("\ngetaddrinfo");
		return -1;
	}	

	//converting the IP address into string
	char ipstr[30];
	for(p = res; p != NULL; p = res->ai_next)
	{
		void *addr;
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
    	addr = &(ipv4->sin_addr);
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
	}

	//calling the socket function
	int socketfd;

	if((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
	{
		perror("\nsocket");
		return -1;
	}

	//bind to a connection
	if(bind(socketfd, res->ai_addr, res->ai_addrlen) != 0)
	{
		perror("\nbind");
		return -1;
	}

	//listen to a connection request from a client
	if(listen(socketfd, BACKLOG) == -1)
	{
		perror("\nlisten");
		return -1;	
	}

	//accept the connection with the client
	struct sockaddr_storage client_addr;	
	socklen_t addr_size = sizeof(client_addr);
	int new_fd;
	if((new_fd = accept(socketfd, (struct sockaddr *)&client_addr, &addr_size)) == -1 )
	{
		perror("\naccept");
		return -1;	
	}
	else
	{
		printf("Connected with the IP: ");
		puts(ipstr);
	}

	freeaddrinfo(res);
	freeaddrinfo(p);

	//receive from the client
	//char buf[10];
	char *buf = (char *) malloc(10);
	int rc;
	if((rc=recv(new_fd, buf, 10, 0)) == -1)
	{
		perror("\nreceive");	
		close(new_fd);
		close(socketfd);
		return -1;
	}
	else
	{
		printf("\nrc: %d\n", rc);
		puts(buf);
	}

	//open the file 
	int fd;
	fd = open("/var/tmp/aesdsocketdata.txt", O_RDWR | O_CREAT | O_APPEND, 0777);
	if(fd == -1)
	{
		perror("\nfile open");
		return -1;
	}

	//write to the file
	if(write(fd, buf, 10) == -1)
	{
		perror("\nwrite");
		close(new_fd);
		close(socketfd);
		return -1;
	}
	else
	{
		printf("\nwritten succesfully");
	}

	//read from the file to send back the content
	lseek(fd, 10, SEEK_SET);
	char *send_buf = (char *) malloc(10);
	if(read(fd, send_buf, 10) == -1)
	{
		perror("\nread");
		close(new_fd);
		close(socketfd);
		return -1;
	}
	else
	{
		printf("\nsending buf: ");
		puts(send_buf);
	}

	//send back to the client
	if(send(new_fd, send_buf, 10, 0) == -1)
	{
		perror("\nsend");
		close(new_fd);
		close(socketfd);
		return -1;
	}
	else
	{
		printf("\nbuffer send");
	}

	remove("/var/tmp/aesdsocketdata.txt");
	close(fd);
	close(new_fd);
	close(socketfd);
	return 0;
}
