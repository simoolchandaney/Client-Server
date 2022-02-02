/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <time.h>
#include <sys/time.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

double timestamp() {
    struct timeval current_time;
    if (gettimeofday(&current_time, NULL) < 0) {
        return time(NULL);
    }
    return (double) current_time.tv_sec + ((double) current_time.tv_usec / 1000000.0);
}


int main(int argc, char *argv[])
{
    int sockfd;
    int32_t numbytes;  
    char buf[BUFSIZ];
    struct addrinfo hints, *servinfo, *p;
	 struct sockaddr_in *h;
    int rv;
    char s[INET6_ADDRSTRLEN];
	 char ip[100];

    if (argc != 4) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // argv[1] - IP
    // argv[2] - port number
	

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
			if(argv[1][0] != '1') {
				printf("IN");
				h = (struct sockaddr_in *) p->ai_addr;
				strcpy(ip, inet_ntoa(h->sin_addr));
			}
			else {
				strcpy(ip, argv[1]);
			}
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

	

	printf("%s resolved to %s\n", argv[1], ip);

	char temp1[100];
	char temp2[100];
	char temp3[100];

	strncpy(temp1, &ip[0], 7);
	strncpy(temp2, &ip[0], 4);
	strncpy(temp3, &ip[0], 8);

	if(!(strcmp(temp1, "129.74.") == 0 || strcmp(temp2, "127.") == 0 || strcmp(temp3, "192.168.") == 0)) {
		fprintf(stderr, "Connection from invalid IP\n");
		exit(1);
	}


	double t_init_f = timestamp(); //start timer

    //get size of file name
    int16_t file_length = strlen(argv[3]);
    char file_sz[BUFSIZ];
    sprintf(file_sz, "%d", file_length);

    //send file name length to server
    if ((send(sockfd, file_sz, sizeof(file_sz), 0)) == -1) {
        perror("recv");
        exit(1);  
    }
    
    //send file name to server
    if ((send(sockfd, argv[3], strlen(argv[3]), 0)) == -1) {
        perror("recv");
        exit(1);  
    }

    // receive # of bytes from server
    if ((numbytes = recv(sockfd, buf, BUFSIZ, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    numbytes = atoi(buf);
	//printf("numbytes: %d\n", numbytes);

    //add prefix to file name so file placed in client directory
    FILE *fp = fopen(argv[3], "w");

    char buffer[BUFSIZ] = {0};

    //receive file data and write to file
    while(1) {
        if(recv(sockfd, buffer, BUFSIZ, 0) <= 0)
            break;
        //printf("LINE: %s\n", buffer);
        fprintf(fp, "%s", buffer);
        bzero(buffer, BUFSIZ);
    }

    //compute transmission time
	double t_final_f = timestamp();
	double time_elapsed = t_final_f - t_init_f;
	double speed = (numbytes * (0.000001)) / (time_elapsed);
	printf("%s bytes transferred over %lf microseconds for a speed of %lf MB/s\n", buf, time_elapsed, speed);

    fclose(fp);
    close(sockfd);

    return 0;
}
