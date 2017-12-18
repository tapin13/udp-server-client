/* 
 * File:   main.c
 * Author: tapin13
 *
 * Created on December 14, 2017, 1:05 AM
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

unsigned char is = 1;

void sig_int(int sig_no) {
    printf("sig_int: %d\n", sig_no);

    is = 0;
    
    return;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Please provide UDP Server IP\n");
        return EXIT_FAILURE;
    }

    signal(SIGINT, sig_int);

    const unsigned short destination_port = 30001;

    struct sockaddr_in destination_address;

    destination_address.sin_family = AF_INET;
    destination_address.sin_port = htons(destination_port);
    
    if(inet_pton(AF_INET, argv[1], &destination_address.sin_addr.s_addr) == 0) {
            printf("IP not valid\n");
            return EXIT_FAILURE;
    }

    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(handle <= 0) {
        printf("failed to create socket\n");
        return EXIT_FAILURE;
    }
    
    int nonBlocking = 1;
    if(fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
        printf("failed to set non blocking socket\n");
        return EXIT_FAILURE;
    }
    
    unsigned char packet_data[256] = { "ping" };
    unsigned int packet_size = sizeof(packet_data);
    
    unsigned char received_packet_data[256] = { "" };
    unsigned int received_packet_size = sizeof(packet_data);
    
    while(is) {
        struct sockaddr_in from;
        socklen_t fromLength = sizeof(from);
        
        int send_bytes = sendto(handle, (const char*)packet_data, packet_size, 0, (struct sockaddr*)&destination_address, sizeof(struct sockaddr_in));

        if(send_bytes != (unsigned int)sizeof(packet_data)) {
            printf("failed to send packet: return value = %d\n", send_bytes);
            //return EXIT_FAILURE;
        } else {
            printf("send: %s\n", packet_data);
        }
        
        int received_bytes = recvfrom(handle, (unsigned char*)received_packet_data, received_packet_size, 0, (struct sockaddr*)&from, &fromLength);

        if(received_bytes <= 0) {
            printf("received_bytes: %d\n", received_bytes);
            //continue;
        } else {
            //unsigned int from_address = ntohl(from.sin_addr.s_addr);
            //unsigned int from_port = ntohs(from.sin_port);

            printf("received: %s\n", received_packet_data);
        }

        sleep(1);
    }
    
    close(handle);
    
    return (EXIT_SUCCESS);
}
