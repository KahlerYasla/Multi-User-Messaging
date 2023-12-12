// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_USERS 10
#define MAX_MESSAGE_SIZE 256

// Define structures for user information
struct User
{
    int userId;
    char phoneNumber[12];
    char name[50];
    char surname[50];
};

// Define structure for a message
struct Message
{
    int senderId;
    int receiverId;
    char messageBody[MAX_MESSAGE_SIZE];
};

// Function to handle client connections
void handleClient(int clientSocket, struct User userList[MAX_USERS]);

int main()
{
    // Initialize server socket and other necessary variables

    // Initialize user list
    struct User userList[MAX_USERS];

    // Initialize server socket and bind

    // Listen for incoming connections

    // Accept incoming connections and handle them
    while (1)
    {
        // Accept a connection

        // Create a new process to handle the client
        if (fork() == 0)
        {
            // Inside the child process
            close(serverSocket); // Close the server socket in the child process
            handleClient(clientSocket, userList);
            exit(0); // Terminate the child process
        }
        else
        {
            close(clientSocket); // Close the client socket in the parent process
        }
    }

    return 0;
}

// Implement the handleClient function
void handleClient(int clientSocket, struct User userList[MAX_USERS])
{
    // Implement the logic for handling client communication
    // This includes handling login requests, message sending/receiving, etc.
}
