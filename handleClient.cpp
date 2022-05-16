/*
 * Will Walker
 * ----------
 * CISC450
 * Programming Assignment I
 * wwalker@udel.edu
 */

// imports
#include <string> // for the message in the buffer
#include <cstdio> // for printing functions
#include <unistd.h> // needed to close the socket
#include <sys/socket.h> // for socket functions

// global constant declarations
#define MESSAGE_BUFFER_SIZE 256 // size of message buffer

/* handleClient Function
 * ---------------------
 * consuming the socket ID
 * from the server process,
 * this function operates on the
 * communication socket link with
 * the client. When a message
 * is placed in the buffer by the client,
 * handleClient will check to see if
 * the file name requested matches the
 * one that is made available in the
 * fileObject declaration.
 */
void handleClient(int commSocket) {
    char checksum = 1; // checksum value for the file
    auto fileObject = fopen("server-files/image.png", "rb+"); // a file that the server can provide
    char messageBuffer[MESSAGE_BUFFER_SIZE]; // buffer for filename string
    ssize_t messageSize; // the size of the requestedFilename
    std::string requestedFilename; // the filename request

    /* Receive Function
     * ----------------
     * Once the connection has been handed over by
     * the server process, this operation confirms
     * that the incoming connection is correctly
     * receiving data.
     */
    if (0 > (messageSize = recv(commSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0))) {
        fprintf(stderr, "recv() function failed\n");
        exit(1);
    } else
        printf("requestedFilename received\n");

    messageBuffer[messageSize] = '\0'; // mark the end of the messageBuffer
    printf("client requested: %s\n", messageBuffer);
    requestedFilename = messageBuffer; // initialize the requestedFilename variable with the data from the messageBuffer

    /* Conditional Filename Check
     * --------------------------
     * If the incoming requestedFilename
     * is equal to the name of the file of the
     * fileObject, a checksum is evaluated and
     * the file is read into the buffer to send
     * across the wire. If the requestedFilename
     * is not equal to the fileObject, then the server
     * notifies the client by replying "file not found".
     */
    if (requestedFilename == "image.png") {
        printf("the file requested is available!\n");

        auto checksumFilePointer = fopen("server-files/image.png", "rb+"); // assign the file a pointer for checksum

        /* Checksum Algorithm
         * ------------------
         * Use fgetc to return the read-in
         * character.  Sum up the returned values
         * to generate a checksum for the file.
         */
        while (!feof(checksumFilePointer) && !ferror(checksumFilePointer)) {
            checksum += fgetc(checksumFilePointer);
        }
        fclose(checksumFilePointer); // close the pointer for checksum purposes
        printf("file checksum is: %d\n", checksum);

        /* Send
         * -----------
         * Using fread, read the fileObject into the
         * socket buffer and then send across the wire
         * to the client.
         */
        while ((messageSize = fread(messageBuffer, 1, MESSAGE_BUFFER_SIZE, fileObject)) > 0) {
            send(commSocket, messageBuffer, static_cast<size_t>(messageSize), 0);
        }
        printf("file sent\n");
    } else {
        printf("the file requested is not available\n");
        send(commSocket, "file not found", sizeof("file not found"), 0);
    }

    fclose(fileObject); // close the file now that it has been sent
    close(commSocket); // close the communication socket
}