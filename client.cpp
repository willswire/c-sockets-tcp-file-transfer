/*
 * Will Walker
 * ----------
 * CISC450
 * Programming Assignment I
 * wwalker@udel.edu
 */

#include <string> // for the message in the buffer
#include <iostream> // for cout and cin functions
#include <unistd.h> // needed to close the socket
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */

#define MESSAGE_BUFFER_SIZE 256 // size of receive buffer

/*
 * Main Function
 * -------------
 * Consumes a server IP address and port
 * number to start the client program. Once
 * a connection has been established with the
 * server, the user will be prompted to enter
 * a filename for the desired file.
 */
int main(int argc, char *argv[]) {
    auto fp = fopen("client-files/image.png", "wb+"); // a file buffer to store the incoming download
    int commSocket; // the designated socket which the client will communicate on
    struct sockaddr_in serverAddress; // the server's socket address
    int serverPort; // the designated port which the server will listen for incoming traffic on
    char *serverIP; // the IP address of the server
    char messageBuffer[MESSAGE_BUFFER_SIZE]; // the message buffer on which the request/download will be transferred
    ssize_t bytesReceived; // the bytes received
    char fileRequest[50]; // the requested filename
    char checksum = 1; // checksum value for the file
    std::string serverResponse; // the server's response message

    /* Argument Check
     * ---------------
     * Verifies that the appropriate
     * number of arguments are used
     * to start the program.
     */
    if (argc < 3) {
        printf("usage: ./client [server ip] [server port]\n");
        exit(0);
    }

    serverIP = argv[1]; // the server IP address
    serverPort = atoi(argv[2]); // the server port

    /* Socket Creation
     * ---------------
     * Creates a socket for the
     * client to send its request on.
     */
    if ((commSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
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
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIP);
    serverAddress.sin_port = htons(serverPort);

    /*
     * Socket Connect
     * --------------
     * The client establishes a connection
     * with the server on the newly created
     * socket.
     */
    if (connect(commSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        printf("connect() function failed\n");
        exit(1);
    } else
        printf("connect succeed\n");

    /* User Interaction
     * ----------------
     * Prompt user to enter in the filename
     * for the file that they wish to download.
     */
    std::cout << "enter filename for download request: ";
    std::cin >> fileRequest;

    /* Send
     * -----------
     * Insert the requested filename into the
     * socket buffer and then send across the wire
     * to the server.
     */
    if (send(commSocket, fileRequest, sizeof(fileRequest), 0) != sizeof(fileRequest)) {
        printf("send() function failed\n");
        exit(1);
    } else
        printf("request sent successfully\n");

    /* Reception Loop
     * --------------
     * While the bytes received are greater
     * than zero, the client will continue to
     * receive data on the socket and store it
     * in the message buffer. If the requested
     * filename is not matched by the server,
     * an error will return telling the user
     * "file not found".  If no error message is
     * returned, then write the data into a new file.
     */
    while ((bytesReceived = recv(commSocket, messageBuffer, MESSAGE_BUFFER_SIZE - 1, 0)) > 0) {
        serverResponse = messageBuffer; // initialize the server's response message with data from the messageBuffer
        if (serverResponse == "file not found") {
            printf("file not found\n");
            remove("client-files/image.png");
            exit(0);
        } else {
            fwrite(messageBuffer, 1, bytesReceived, fp);
        }
    }
    printf("file downloaded\n");
    fclose(fp); // close the downloaded file

    auto checksumFilePointer = fopen("client-files/image.png", "rb+"); // assign the file a pointer for checksum

    /* Checksum Algorithm
     * ------------------
     * Use fgetc to return the read-in
     * character.  Sum up the returned values
     * to generate a checksum for the file.
     */
    while (!feof(checksumFilePointer) && !ferror(checksumFilePointer)) {
        checksum += fgetc(checksumFilePointer);
    }
    printf("checksum is: %d\n", checksum);
    fclose(checksumFilePointer); // close the pointer for checksum purposes

    close(commSocket); // close the communication socket
    exit(0); // exit the program
}