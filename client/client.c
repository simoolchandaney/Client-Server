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
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 4) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // argv[1] - IP
    // argv[2] - port number
    char *IPbuffer = argv[1];

    if (argv[1][0] != '1') {
        char hostbuffer[BUFSIZ];
        struct hostent *host_entry;

        // to retrieve host name
        host_entry = gethostbyname(hostbuffer);

        if(host_entry == NULL) {
            fprintf(stderr, "client: invalid host name\n");
            return 2;
        }
        IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    }

    printf("IP: %s\n", IPbuffer);
    if ((rv = getaddrinfo(IPbuffer, argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
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

    //add prefix to file name so file placed in client directory
	char new_filename[BUFSIZ] = "client/";
	strcat(new_filename, argv[3]);
    FILE *fp = fopen(new_filename, "w");

    char buffer[BUFSIZ] = {0};

    //receive file data and write to file
    while(1) {
        if(recv(sockfd, buffer, BUFSIZ, 0) <= 0)
            break;
        printf("LINE: %s\n", buffer);
        fprintf(fp, "%s", buffer);
        bzero(buffer, BUFSIZ);
    }

    //compute transmission time
	double t_final_f = timestamp();
	double time_elapsed = t_final_f - t_init_f;
	double speed = (numbytes * (0.000001)) / (time_elapsed * (0.000001));
	printf("%s bytes transferred over %lf microseconds for a speed of %lf MB/s\n", buf, time_elapsed, speed);

    fclose(fp);
    close(sockfd);

    return 0;
}
