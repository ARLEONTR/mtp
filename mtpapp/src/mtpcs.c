#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_MTP);
    if (sock < 0)
    {
        printf("%s: Error creating Socket: %d\n", __func__, sock);
        goto out;
    }

out:
    return -1;
}