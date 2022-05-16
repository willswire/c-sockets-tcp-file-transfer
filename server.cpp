/*
 * Will Walker
 * ----------
 * CISC450
 * Programming Assignment I
 * wwalker@udel.edu
 */

// imports
#include "handleClient.cpp" // user-written code to handle clients once connected
#include <arpa/inet.h> // for socket addressing

// global constant declarations
#define MAXPENDING 5 // maximum outstanding connection requests

/*
 * Main Function
 * -------------
 * Consumes a port number to start
 * the server program.  Listens for
 * incoming traffic from clients,
 * and then hands off the connection
 * to 'handleClient' on the communication
 * socket.  Will not exit until killed
 * by the operator.
 */
int main(int argc, char *argv[]) {

    // main variable declarations
    int port; // the designated port which the server will listen for incoming traffic on
    int commSocket; // the designated socket which the server will communicate on
    int listenSocket; // the designated socket which the server will listen on
    struct sockaddr_in serverAddress; // the server's socket address
    struct sockaddr_in clientAddress; // the client's socket address
    unsigned int clientAddressLength; // the length of the client's socket address

    /* Argument Check
     * ---------------
     * Verifies that the appropriate
     * number of arguments are used
     * to start the program.
     */
    if (argc < 1) {
        printf("usage: ./server [port number]\n");
        exit(0);
    }

    port = atoi(argv[1]); // assigns the port number to the passed-in argument

    /* Socket Creation
     * ---------------
     * Creates a socket for the
     * server to listen on.
     */
    if ((listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("socket() function failed\n");
        exit(1);
    } else
        printf("socket created\n");

    /*
     * Socket Information
     * --------------------
     * Initializes variables with
     * information from the OS. Includes
     * assignments for info like the host's
     * IP address and port.
     */
    memset(&serverAddress, 0, sizeof(serverAddress));  // zero-our the serverAddress variable
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    /*
     * Socket Binder
     * -------------
     * Connects the running process
     * with the newly created socket.
     */
    if (bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        printf("bind() function failed\n");
        exit(1);
    } else
        printf("socket bound\n");

    /*
     * Listener
     * --------
     * Begins listening for incoming connections
     * by the client.
     */
    if (listen(listenSocket, MAXPENDING) < 0) {
        printf("listen() function failed\n");
        exit(1);
    } else
        printf("listening...\n");

    while (true) {
        clientAddressLength = sizeof(clientAddress); // assigns the clientAddressLength variable

        /*
         * Accept
         * ------
         * Accepts incoming connections
         * from the client.
         */
        if ((commSocket = accept(listenSocket, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0) {
            fprintf(stderr, "accept() function failed\n");
            exit(1);
        } else
            printf("client accepted\n");

        printf("client connected from %s\n", inet_ntoa(clientAddress.sin_addr));
        handleClient(commSocket); // designates the new connection to be handled by 'handleClient'
    }
}