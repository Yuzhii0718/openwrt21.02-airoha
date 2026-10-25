#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int sockfd = 0;
	int err = -1;
	struct sockaddr_in info;
	char message[] = {"hello ated, i'm tcli.\n"};
	char receiveMessage[100] = {'\0'};

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1)
		printf("Fail to create a socket.");

	bzero(&info, sizeof(info));
	bzero(&receiveMessage, sizeof(receiveMessage));
	info.sin_family = PF_INET;
	info.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* info.sin_addr.s_addr = INADDR_ANY; */
	info.sin_port = htons(7990);

	err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
	if (err == -1) {
		perror("why?");
		printf("Connection error");
	}

	/* Send a message to server */
	send(sockfd, message, sizeof(message), 0);
	recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);

	printf("tcli: %s", receiveMessage);
	printf("tcli: close Socket\n");
	close(sockfd);

	return 0;
}
