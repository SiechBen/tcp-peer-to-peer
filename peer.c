/*
 * htons() host to network short
 * htonl() host to network long
 * ntohs() network to host short
 * ntohl() network to host long
 */

/* 
 * File:   main.c
 * Author: siech
 *
 * Created on July 4, 2016, 10:09 AM
 */

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h>
#include <stdlib.h> //inet_addr
#include <unistd.h> //
#include <assert.h> //
#include <time.h> //
#include<sys/types.h>
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define USAGE_REG 1
#define USAGE_REQ 0
#define USAGE_UNR -1

void *terminator(void *);
void *requester(void *);
void *chat_initiator(void *);
void *reply_handler(void *);
void *server_consultant(void *);

struct clients {
    int p;
    char ip[20];
};

struct port_data {
    int port;
    int usage;
};

int this_port_no = 0, last_peer = 0;
struct clients registered[10];
struct port_data port_inf[1];

int main(int argc, char *argv[]) {
    int this_socket_no;
    struct sockaddr_in client;

    //Create socket
    this_socket_no = socket(AF_INET, SOCK_STREAM, 0);
    if (this_socket_no == -1) {
        printf("\nCould not create socket");
    }

    //Initialize random number generator
    srand((unsigned) time(NULL));

    //Generate a random port number
    this_port_no = rand() % 15000 + 10000;

    // Local
    memset(&client, 0, sizeof (struct sockaddr_in));
    client.sin_family = AF_INET;
    client.sin_port = htons(this_port_no);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("\nSocket created at port %d\n", this_port_no);

    pthread_t sniffer_thread;
    int *n;
    n = malloc(1);
    *n = this_socket_no;
    int op;

    if (pthread_create(&sniffer_thread, NULL, server_consultant, (void *) n) < 0) {
        perror("Could not create consultant");
        return EXIT_FAILURE;
    }

    //Now join the thread , so that we dont terminate before the thread
    pthread_join(sniffer_thread, NULL);

    close(this_socket_no);

    //Create socket
    this_socket_no = socket(AF_INET, SOCK_STREAM, 0);
    if (this_socket_no == -1) {
        printf("\nCould not create socket");
    }

    // Local
    memset(&client, 0, sizeof (struct sockaddr_in));
    client.sin_family = AF_INET;
    client.sin_port = htons(this_port_no);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Bind local to port
    if (bind(this_socket_no, (struct sockaddr *) &client, sizeof (struct sockaddr)) < 0) {
        perror("An error occurred: bind failure.");
        return EXIT_FAILURE;
    }

    while (1) {
        puts("List of peers received from the  server");
        int i;
        for (i = 0; i < 10; i++) {
            if (strcmp(registered[i].ip, "") == 0 || strcmp(registered[i].ip, "0.0.0.0") == 0) {
                continue;
            }
            printf("\n%d.Peer: %s:", i + 1, registered[i].ip);
            printf("%d", registered[i].p);
            last_peer = i;
        }

        printf("\n\nSelect an option:\n  1. Initiate chat\n"
                "  2. Wait for peers to\n"
                "  3. Request for list of peers\n"
                "  4. Exit\n \t>> ");
        scanf("%d", &op);

        pthread_t sniffer_thread;
        int *n;
        n = malloc(1);
        *n = this_socket_no;

        switch (op) {
            case 1:

                if (pthread_create(&sniffer_thread, NULL, chat_initiator, (void*) n) < 0) {
                    perror("Could not create message handler");
                    return EXIT_FAILURE;
                }

                //Now join the thread , so that we dont terminate before the thread
                pthread_join(sniffer_thread, NULL);
                break;

            case 2:

                if (pthread_create(&sniffer_thread, NULL, reply_handler, (void*) n) < 0) {
                    perror("Could not create reply handler");
                    return EXIT_FAILURE;
                }

                pthread_join(sniffer_thread, NULL);
                break;

            case 3:


                if (pthread_create(&sniffer_thread, NULL, requester, (void *) n) < 0) {
                    perror("Could not create requester");
                    return EXIT_FAILURE;
                }

                pthread_join(sniffer_thread, NULL);
                break;

            case 4:

                if (pthread_create(&sniffer_thread, NULL, terminator, (void *) n) < 0) {
                    perror("Could not create terminator");
                    return EXIT_FAILURE;
                }

                //Now join the thread , so that we dont terminate before the thread
                pthread_join(sniffer_thread, NULL);

                close(this_socket_no);

                puts("\nGood bye!\n");
                return EXIT_SUCCESS;

            default:
                perror("\nInvalid choice");
        }
    }

    //Now join the thread , so that we dont terminate before the thread
    pthread_join(sniffer_thread, NULL);

    puts("\nGood bye!\n");

    close(this_socket_no);
    return EXIT_SUCCESS;
}

void *server_consultant(void *n) {
    int this_socket_no = *(int *) n;
    struct sockaddr_in central_server;

    // Central server
    memset(&central_server, 0, sizeof (struct sockaddr_in));
    central_server.sin_family = AF_INET;
    central_server.sin_port = htons(8888);
    central_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Connect to the central server
    connect(this_socket_no, (struct sockaddr *) &central_server, sizeof (central_server));

    puts("\nConnected to central server");

    memset(&port_inf, 0, sizeof (struct port_data));
    port_inf[0].port = this_port_no;
    port_inf[0].usage = USAGE_REG;
    send(this_socket_no, port_inf, sizeof (struct port_data), 0);

    puts("Registration details sent to the server\n");

    memset(&registered, 0, sizeof (registered));

    //Receive a list of peers from the central server
    recv(this_socket_no, registered, sizeof (registered), 0);
}

void *terminator(void *n) {
    int this_socket_no = *(int *) n;
    struct sockaddr_in central_server;

    // Central server
    memset(&central_server, 0, sizeof (struct sockaddr_in));
    central_server.sin_family = AF_INET;
    central_server.sin_port = htons(8888);
    central_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Connect to the central server
    connect(this_socket_no, (struct sockaddr *) &central_server, sizeof (central_server));

    memset(&port_inf, 0, sizeof (struct port_data));
    port_inf[0].port = this_port_no;
    port_inf[0].usage = USAGE_UNR;
    send(this_socket_no, port_inf, sizeof (struct port_data), 0);

}

void *requester(void *n) {
    int this_socket_no = *(int *) n;
    struct sockaddr_in central_server;

    // Central server
    memset(&central_server, 0, sizeof (struct sockaddr_in));
    central_server.sin_family = AF_INET;
    central_server.sin_port = htons(8888);
    central_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Connect to the central server
    connect(this_socket_no, (struct sockaddr *) &central_server, sizeof (central_server));

    memset(&port_inf, 0, sizeof (struct port_data));
    port_inf[0].port = this_port_no;
    port_inf[0].usage = USAGE_REQ;
    send(this_socket_no, port_inf, sizeof (struct port_data), 0);

    memset(&registered, 0, sizeof (registered));

    //Receive a list of peers from the central server
    recv(this_socket_no, registered, sizeof (registered), 0);
}

void *chat_initiator(void *n) {
    //Get the socket descriptor
    int i, peer_sock_no, peer_p;
    struct sockaddr_in peer;
    struct sockaddr_in this;
    char msg_to_peer[2000];
    char peer_reply[2000];

    //Create socket
    peer_sock_no = socket(AF_INET, SOCK_STREAM, 0);
    if (peer_sock_no == -1) {
        printf("\nCould not create socket");
    }

    //Initialize random number generator
    srand((unsigned) time(NULL));

    //Generate a random port number
    int port = rand() % 7000 + 70000;

    // Local
    memset(&this, 0, sizeof (struct sockaddr_in));
    this.sin_family = AF_INET;
    this.sin_port = htons(port);
    this.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Bind local to port
    if (bind(peer_sock_no, (struct sockaddr *) &this, sizeof (struct sockaddr)) < 0) {
        perror("An error occurred: bind failure.");
        exit(0);
    }

    int p;
    printf("Pick peer no. >> ");
    scanf("%i", &p);

    if (p < 1 || p > last_peer) {
        puts("Client does not exist");
        exit(0);
    }

    peer_p = registered[p - 1].p;

    //Peer
    memset(&peer, 0, sizeof (struct sockaddr_in));
    peer.sin_family = AF_INET;
    peer.sin_port = htons(peer_p);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Connect to the peer
    if (connect(peer_sock_no, (struct sockaddr *) &peer, sizeof (peer)) < 0) {
        perror("connect failed. Error");
        exit(0);
    } else {
        puts("\nConnected to peer");
    }

    getchar(); //Clear the EOF character from stdin
    while (1) {
        memset(msg_to_peer, 0, sizeof (msg_to_peer));
        memset(peer_reply, 0, sizeof (peer_reply));
        printf("Enter message: ");
        fgets(msg_to_peer, 2000, stdin);

        /* remove newline, if present */
        i = strlen(msg_to_peer) - 1;
        if (msg_to_peer[ i ] == '\n')
            msg_to_peer[i] = '\0';

        if (strcmp(msg_to_peer, "\\w") == 0 || strcmp(msg_to_peer, "\\W") == 0) {
            shutdown(peer_sock_no, 2);
            close(peer_sock_no);
            break;
        }

        char ack[23];
        memset(ack, 0, sizeof (ack));

        //Send some message to the peer
        if (write(peer_sock_no, msg_to_peer, strlen(msg_to_peer)) > 0) {
            recv(peer_sock_no, ack, 2000, 0);
            printf("%s", ack);
        }

        char recvd_ack[23] = "\t\t\tR&D acknowledged\n";
        //Receive a reply from the peer
        if (recv(peer_sock_no, peer_reply, 2000, 0) > 0) {
            write(peer_sock_no, recvd_ack, strlen(recvd_ack));
            printf("\nPeer reply : ");
            puts(peer_reply);
        } else {

            break;
        }
    }

    puts("\nInter-peer communication terminated\n");
    puts("Press enter to continue...");
    getchar();

}

void *reply_handler(void *this_socket_no) {
    int peer_socket_no, c, *new_sock;
    struct sockaddr_in peer;
    int socket_no = *(int *) this_socket_no;
    char peer_reply[2000];
    char message[2000];

    puts("\tWaiting...");

    //Listen
    listen(socket_no, 3);

    if ((peer_socket_no = accept(socket_no, (struct sockaddr *) &peer, (socklen_t*) & c)) > 0) {
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = peer_socket_no;

        getchar(); //Clear EOF from the stdin
        while (1) {
            memset(message, 0, sizeof (message));
            memset(peer_reply, 0, sizeof (peer_reply));
            char ack[23] = "\t\t\tR&D acknowledged\n";
            //Receive a message from the peer
            if (recv(peer_socket_no, message, 2000, 0) > 0) {
                write(peer_socket_no, ack, strlen(ack));
                printf("\nPeer message: ");
                puts(message);
            } else {
                break;
            }

            printf("Enter reply: ");
            fgets(peer_reply, 2000, stdin);

            /* remove newline, if present */
            int i = strlen(peer_reply) - 1;
            if (peer_reply[ i ] == '\n')
                peer_reply[i] = '\0';

            if (strcmp(peer_reply, "\\w") == 0 || strcmp(peer_reply, "\\W") == 0) {
                shutdown(peer_socket_no, 2);
                close(peer_socket_no);
                break;
            }

            char recvd_ack[23];
            memset(recvd_ack, 0, sizeof (recvd_ack));

            if (write(peer_socket_no, peer_reply, strlen(peer_reply)) > 0) {
                recv(peer_socket_no, recvd_ack, 2000, 0);
                printf("%s", recvd_ack);
            }
        }

        puts("\nInter-peer communication terminated\n");
        puts("Press enter to continue...");
        getchar();

    } else {
        perror("Could not accept message");
    }
}
