#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ansiTerminalColors.h"
//-------------------------------------------------------------------------------------------------
#define MAX_MESSAGE_LENGTH 256
//=================================================================================================
typedef struct Message
{
    char senderID[15];
    char receiverID[15];
    char body[MAX_MESSAGE_LENGTH];
} Message;
//-------------------------------------------------------------------------------------------------
typedef struct User
{
    char phoneNumber[12];
    char name[50];
    char surname[50];
} User;
//=================================================================================================
// Function templates

void display_menu();

// CSV file functions
void listContactsRequest(char *userID);
void addUserRequest(char *userID);
void deleteUserRequest(char *userID);

// Socket functions
void sendMessageRequest(char *userID, int client_socket);
void checkMessagesRequest(char *userID, int client_socket);
//=================================================================================================
int main()
{
    // Socket variables
    int client_socket;

    // Get user ID from the user
    char userID[15];
    printf("Enter your user ID to start: ");
    scanf("%s", userID);

    // Initialize client socket and other necessary variables
    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    // Send login request to the server
    send(client_socket, userID, strlen(userID), 0);

    while (1)
    {
        // Display menu and get user input
        display_menu();

        int userInput;
        scanf("%d", &userInput);

        switch (userInput)
        {
        case 1:
            // List contacts from UserID.csv
            listContactsRequest(userID);
            break;
        case 2:
            // Add user to UserID.csv
            addUserRequest(userID);
            break;
        case 3:
            // Delete user from UserID.csv
            deleteUserRequest(userID);
            break;
        case 4:
            // Send message to the server
            sendMessageRequest(userID, client_socket);
            break;
        case 5:
            // Check messages from the server
            checkMessagesRequest(userID, client_socket);
            break;
        case 6:
            // Close the client socket
            close(client_socket);
            return 0;
        default:
            printf("Invalid input!\n");
            break;
        }
    }
}
//=================================================================================================
// Function to display menu and handle user input
void display_menu()
{
    LogCyan("========================================\n");
    printf("1. List Contacts\n");
    printf("2. Add User to Contacts\n");
    printf("3. Delete User from Contacts\n");
    printf("4. Send Message\n");
    printf("5. Check Messages\n");
    printf("6. Exit\n");
    printf("Your choice: ");
}
//=================================================================================================
// Function to send message to the server
void sendMessageRequest(char *userID, int client_socket)
{
    // Get receiver ID from the user
    char receiverID[15];
    printf("Enter receiver ID: ");
    scanf("%s", receiverID);

    // Get message body from the user
    char messageBody[256];
    printf("Enter message body: ");
    scanf("%s", messageBody);

    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, receiverID);
    strcpy(message.body, messageBody);

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
//-------------------------------------------------------------------------------------------------
// Function to check messages from the server
void checkMessagesRequest(char *userID, int client_socket)
{
    // Send check messages request to the server
    send(client_socket, "check", strlen("check"), 0);

    // Receive messages from the server
    struct Message message;
    while (recv(client_socket, &message, sizeof(message), 0) > 0)
    {
        // Display message
        printf("Message from %s: %s\n", message.senderID, message.body);
    }
}
//=================================================================================================
// Function to list contacts from UserID.csv
void listContactsRequest(char *userID)
{
}
//-------------------------------------------------------------------------------------------------
// Function to add user to UserID.csv
void addUserRequest(char *userID)
{
}
//-------------------------------------------------------------------------------------------------
// Function to delete user from UserID.csv
void deleteUserRequest(char *userID)
{
}
