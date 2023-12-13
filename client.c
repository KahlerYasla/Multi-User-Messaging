#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// my header files
#include "ansiTerminalColors.h"

//-------------------------------------------------------------------------------------------------
#define MAX_MESSAGE_LENGTH 256
//=================================================================================================
typedef struct Message
{
    int messageID;
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
void *listenForServer(void *arg);
void display_menu();

// CSV file functions
void listContactsRequest(char *userID, int client_socket);
void addUserRequest(char *userID, int client_socket);
void deleteUserRequest(char *userID, int client_socket);

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

    // Create a thread for listening to messages
    pthread_t listener_thread;
    if (pthread_create(&listener_thread, NULL, listenForServer, (void *)&client_socket) != 0)
    {
        perror("Thread creation failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        display_menu();
        int userInput;
        scanf("%d", &userInput);

        LogCyan("----------------------------------------");

        switch (userInput)
        {
        case 1:
            // List contacts from UserID.csv
            listContactsRequest(userID, client_socket);
            break;
        case 2:
            // Add user to UserID.csv
            addUserRequest(userID, client_socket);
            break;
        case 3:
            // Delete user from UserID.csv
            deleteUserRequest(userID, client_socket);
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

        sleep(1);
    }
}
//=================================================================================================
void *listenForServer(void *arg)
{
    int client_socket = *((int *)arg);
    int messageCount = 0;

    while (1)
    {
        // Receive messages from the server
        struct Message message;

        if (recv(client_socket, &message, sizeof(message), 0) > 0)
        {
            if (messageCount % 2 == 1)
            {
                // Display message
                LogCyan("\n-Server:\n");
                printf("%s\n", message.body);
            }

            messageCount++;
        }
    }

    // This part is never reached, but pthread_exit is used to avoid compiler warnings
    pthread_exit(NULL);
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
    printf("\nEnter receiver ID: ");
    scanf("%s", receiverID);
    getchar(); // Consume the newline character left in the buffer

    // Get message body from the user
    char messageBody[MAX_MESSAGE_LENGTH];
    printf("Enter message body: ");
    fgets(messageBody, sizeof(messageBody), stdin);
    messageBody[strcspn(messageBody, "\n")] = 0; // Remove trailing newline character

    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, receiverID);
    strcpy(message.body, "4");
    strcat(message.body, messageBody);

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
//-------------------------------------------------------------------------------------------------
// Function to check messages from the server
void checkMessagesRequest(char *userID, int client_socket)
{
    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, "Server");
    strcpy(message.body, "5");

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
//=================================================================================================
// Function to list contacts from UserID.csv
void listContactsRequest(char *userID, int client_socket)
{
    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, "Server");
    strcpy(message.body, "1");

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
//-------------------------------------------------------------------------------------------------
// Function to add user to UserID.csv
void addUserRequest(char *userID, int client_socket)
{
    // Get user details from the user
    char phoneNumber[12];
    char name[50];
    char surname[50];
    printf("Enter user details:\n");
    printf("Phone number: ");
    scanf("%s", phoneNumber);
    printf("Name: ");
    scanf("%s", name);
    printf("Surname: ");
    scanf("%s", surname);

    // Create user
    User user;
    strcpy(user.phoneNumber, phoneNumber);
    strcpy(user.name, name);
    strcpy(user.surname, surname);

    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, "Server");
    strcpy(message.body, "2");
    strcat(message.body, ",");
    strcat(message.body, user.phoneNumber);
    strcat(message.body, ",");
    strcat(message.body, user.name);
    strcat(message.body, ",");
    strcat(message.body, user.surname);

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
//-------------------------------------------------------------------------------------------------
// Function to delete user from UserID.csv
void deleteUserRequest(char *userID, int client_socket)
{
    // Get user details from the user
    char phoneNumber[12];
    printf("Enter user details:\n");
    printf("Phone number: ");
    scanf("%s", phoneNumber);

    // Create user
    User user;
    strcpy(user.phoneNumber, phoneNumber);

    // Create message
    Message message;
    strcpy(message.senderID, userID);
    strcpy(message.receiverID, "Server");
    strcpy(message.body, "3");
    strcat(message.body, ",");
    strcat(message.body, user.phoneNumber);

    // Send message to the server
    send(client_socket, &message, sizeof(message), 0);
}
