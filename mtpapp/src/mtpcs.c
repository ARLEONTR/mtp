#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define TESTPORT 4689

int main(int argc, char **argv)
{
	int rc = 0;
	// Let's create an MTP socket using UDP packets
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_MTP);
	if (sock < 0) {
		printf("%s: Error creating Socket: %d\n", __func__, sock);
		goto out;
	}
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = TESTPORT;

	// Let's bind the socket to an MTP port number
	rc = bind(sock, (struct sockaddr *)&sin, sizeof(sin));
	if (rc < 0) {
		printf("%s: Error binding to address: %d\n", __func__, rc);
		goto out;
	}

	// Let's try to get a message!
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	rc = recvmsg(sock, &msg, 0);
	if (rc < 0) {
		printf("%s: Error receiving message: %d\n", __func__, rc);
		goto out;
	}

	rc = sendmsg(sock, &msg, 0);
	if (rc < 0) {
		printf("%s: Error sending message: %d\n", __func__, rc);
		goto out;
	}

out:
	return -1;
}