/* 
 * File:   main.c
 * Author: tapin13
 *
 * Created on December 14, 2017, 1:05 AM
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

unsigned char is = 1;

void sig_int(int sig_no) {
    printf("sig_int: %d\n", sig_no);

    is = 0;
    
    return;
}

int main(int argc, char** argv) {
    signal(SIGINT, sig_int);
    
    struct sockaddr_in address;

    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(handle <= 0) {
        printf("failed to create socket\n");
        return EXIT_FAILURE;
    }
    
    unsigned short port = 30001;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) port);
    
    if(bind(handle, (const struct sockaddr*)&address, sizeof(struct sockaddr_in)) < 0) {
        printf("failed to bind socket\n");
        return EXIT_FAILURE;
    }
    
    int nonBlocking = 1;
    if(fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
        printf("failed to set non blocking socket\n");
        return EXIT_FAILURE;
    }
    
    printf("Server listen on port %hu...\n", port);
    
    unsigned char send_packet_data[256] = { "pong" };
    unsigned int send_packet_size = sizeof(send_packet_data);
    
    while(is) {
        unsigned char packet_data[256] = { "" };
        unsigned int packet_size = sizeof(packet_data);
        
        struct sockaddr_in from;
        socklen_t fromLength = sizeof(from);
        
        int received_bytes = recvfrom(handle, (unsigned char*)packet_data, packet_size, 0, (struct sockaddr*)&from, &fromLength);

        //printf("received length: %d\n", received_bytes);
        
        if(received_bytes <= 0) {
            //printf("received_bytes: %d\n", received_bytes);
            continue;
        }
        
        //unsigned int from_address = ntohl(from.sin_addr.s_addr);
        //unsigned int from_port = ntohs(from.sin_port);
        
        printf("received: %s\n", packet_data);

        int send_bytes = sendto(handle, (const char*)send_packet_data, send_packet_size, 0, (struct sockaddr*)&from, sizeof(struct sockaddr_in));
        
        if(send_bytes != (unsigned int)sizeof(send_packet_data)) {
            printf("failed to send packet: return value = %d\n", send_bytes);
            return EXIT_FAILURE;
        }        
    }
    
    close(handle);
    
    return (EXIT_SUCCESS);
}
