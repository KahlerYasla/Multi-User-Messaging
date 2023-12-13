#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

// my header files
#include "ansiTerminalColors.h"

//=================================================================================================
#define MAX_MESSAGE_LENGTH 256
//=================================================================================================
typedef struct Date
{
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int second;
} Date;
//-------------------------------------------------------------------------------------------------
typedef struct Message
{
    int messageID;
    char senderID[15];
    char receiverID[15];
    char body[MAX_MESSAGE_LENGTH];
    Date date;
} Message;
//-------------------------------------------------------------------------------------------------
typedef struct User
{
    char phoneNumber[12];
    char name[50];
    char surname[50];
} User;
//=================================================================================================
void *handle_client_messages(void *arg);
void getCurrentDateAndTime(Date *date);

// CSV file functions
void listContacts(char *userID, int client_socket);
void addUser(char *userID, Message message, int client_socket);
void deleteUser(char *userID, Message message, int client_socket);
void createCSVIfNotExists(char *userID);
void sortTheCSVFileAccordingToDate(char *messagesCSVPath);
int compareDates(const Date *date1, const Date *date2);

// Message functions
void sendMessage(char *userID, Message message, int client_socket);
void checkMessages(char *userID, int client_socket);
//=================================================================================================
int main()
{
    srand(time(NULL));

    // Server socket variables
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Socket listen failed");
        exit(EXIT_FAILURE);
    }

    LogGreen("Server is now listening connections on port 8080\n");

    while (1)
    {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Create a new thread to handle client messages
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client_messages, (void *)&client_socket) != 0)
        {
            perror("Thread creation failed");
            close(client_socket);
        }

        // Detach the thread to clean up resources when it exits
        pthread_detach(tid);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
//=================================================================================================
void *handle_client_messages(void *arg) // function to handle client messages
{
    int client_socket = *((int *)arg);

    char userID[15];
    // Receive user ID from the client
    if (recv(client_socket, userID, sizeof(userID), 0) <= 0)
    {
        perror("Error receiving user ID from client");
        close(client_socket);
        pthread_exit(NULL);
    }

    LogGreen("=============================================================\n");
    printf("Client connected with UserID: %s\n", userID);
    LogGreen("=============================================================\n");

    createCSVIfNotExists(userID);

    while (1)
    {
        Message message;
        // Receive message from the client
        if (recv(client_socket, &message, sizeof(message), 0) <= 0)
        {
            perror("Error receiving message from client");
            break;
        }

        // Get current date and time
        getCurrentDateAndTime(&message.date);

        int userInput = message.body[0] - '0';
        switch (userInput)
        {
        case 1:
            // List contacts
            listContacts(userID, client_socket);
            break;
        case 2:
            // Add user to contacts
            addUser(userID, message, client_socket);
            break;
        case 3:
            // Delete user from contacts
            deleteUser(userID, message, client_socket);
            break;
        case 4:
            // Send message to the server
            sendMessage(userID, message, client_socket);
            break;
        case 5:
            // Check messages from the server
            checkMessages(userID, client_socket);
            break;
        default:
            send(client_socket, "Invalid input!", sizeof("Invalid input!"), 0);
            break;
        }

        // Log the message to the server console
        printf("%d/%d/%d %d:%d >> from %s to %s message is: %s\n",
               message.date.day, message.date.month, message.date.year,
               message.date.hour, message.date.minute,
               message.senderID, message.receiverID, message.body);
    }

    // Close the client socket and terminate the thread
    close(client_socket);
    pthread_exit(NULL);
}
//-------------------------------------------------------------------------------------------------
void getCurrentDateAndTime(Date *date)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    date->day = t->tm_mday;
    date->month = t->tm_mon + 1;
    date->year = t->tm_year + 1900;
    date->hour = t->tm_hour;
    date->minute = t->tm_min;
    date->second = t->tm_sec;
}
//=================================================================================================
// Message functions
void listContacts(char *userID, int client_socket) // function to list contacts
{
    // Open contacts CSV file
    char contactsCSVPath[50];
    strcpy(contactsCSVPath, "Contacts/");
    strcat(contactsCSVPath, userID);
    strcat(contactsCSVPath, ".csv");
    FILE *contactsCSV = fopen(contactsCSVPath, "r");

    // Create message
    Message message;
    strcpy(message.senderID, "Server");
    strcpy(message.receiverID, userID);

    // Send contacts to the client
    char contact[50];
    while (fgets(contact, sizeof(contact), contactsCSV))
    {
        strcat(message.body, contact);
    }

    send(client_socket, &message, sizeof(message), 0);

    fclose(contactsCSV);
}
//-------------------------------------------------------------------------------------------------
void addUser(char *userID, Message message, int client_socket)
{
    // Extract user details from the message body
    char phoneNumber[12];
    char name[50];
    char surname[50];

    sscanf(message.body, "2,%[^,],%[^,],%[^,]", phoneNumber, name, surname);

    // Open contacts CSV file
    char contactsCSVPath[50];
    strcpy(contactsCSVPath, "Contacts/");
    strcat(contactsCSVPath, userID);
    strcat(contactsCSVPath, ".csv");
    FILE *contactsCSV = fopen(contactsCSVPath, "a");

    // Add user to contacts CSV file
    fprintf(contactsCSV, "%s,%s,%s\n", phoneNumber, name, surname);

    fclose(contactsCSV);

    // Create response message
    Message responseMessage;
    strcpy(responseMessage.senderID, "Server");
    strcpy(responseMessage.receiverID, userID);
    strcpy(responseMessage.body, "User added!");

    // Send response message to the client
    send(client_socket, &responseMessage, sizeof(responseMessage), 0);
}
//-------------------------------------------------------------------------------------------------
void deleteUser(char *userID, Message message, int client_socket) // function to delete user from contacts
{
    // Get user ID from the message
    char userToDeleteID[15];
    strcpy(userToDeleteID, message.body);

    // Open contacts CSV file
    char contactsCSVPath[50];
    strcpy(contactsCSVPath, "Contacts/");
    strcat(contactsCSVPath, userID);
    strcat(contactsCSVPath, ".csv");
    FILE *contactsCSV = fopen(contactsCSVPath, "r");

    // Open temporary CSV file
    char tempCSVPath[50];
    strcpy(tempCSVPath, "Contacts/temp.csv");
    FILE *tempCSV = fopen(tempCSVPath, "w");

    // Copy all contacts except the one to delete to the temporary CSV file
    char contact[50];
    while (fgets(contact, sizeof(contact), contactsCSV))
    {
        if (strcmp(contact, userToDeleteID) != 0)
        {
            fprintf(tempCSV, "%s", contact);
        }
    }

    fclose(contactsCSV);
    fclose(tempCSV);

    // Delete contacts CSV file
    remove(contactsCSVPath);

    // Rename temporary CSV file to contacts CSV file
    rename(tempCSVPath, contactsCSVPath);

    // Create message
    Message newMessage;
    strcpy(newMessage.senderID, "Server");
    strcpy(newMessage.receiverID, userID);
    strcpy(newMessage.body, "User deleted!");

    // Send message to the client
    send(client_socket, &newMessage, sizeof(newMessage), 0);
}
//-------------------------------------------------------------------------------------------------
// Function to send messages to the client (actually it just saves the message to the receiver's messages CSV file so called messagebox)
void sendMessage(char *userID, Message message, int client_socket) // function to save the message to receivers messagebox
{
    // Get receiver ID from the message
    char receiverID[15];
    strcpy(receiverID, message.receiverID);

    // Get message body from the message
    char messageBody[256];
    strcpy(messageBody, message.body);

    // delete the first character from the message body
    for (int i = 0; i < strlen(messageBody); i++)
    {
        messageBody[i] = messageBody[i + 1];
    }

    // Create message
    Message newMessage;
    strcpy(newMessage.senderID, userID);
    strcpy(newMessage.receiverID, receiverID);
    strcpy(newMessage.body, messageBody);

    // Get current date and time
    getCurrentDateAndTime(&newMessage.date);

    // Create a ID for the message
    newMessage.messageID = rand() % 10000;

    // Save message to the receiver's messages CSV file
    char messagesCSVPath[50];
    strcpy(messagesCSVPath, "Messages/");
    strcat(messagesCSVPath, receiverID);
    strcat(messagesCSVPath, ".csv");
    FILE *messagesCSV = fopen(messagesCSVPath, "a");

    fprintf(messagesCSV, "%d,%s,%s,%s,%d,%d,%d,%d,%d,%d\n",
            newMessage.messageID,
            newMessage.senderID, newMessage.receiverID, newMessage.body,
            newMessage.date.day, newMessage.date.month, newMessage.date.year,
            newMessage.date.hour, newMessage.date.minute, newMessage.date.second);

    fclose(messagesCSV);

    // Create message
    Message responseMessage;
    strcpy(responseMessage.senderID, "Server");
    strcpy(responseMessage.receiverID, userID);
    strcpy(responseMessage.body, "Message sent!");

    // Send message to the client
    send(client_socket, &responseMessage, sizeof(responseMessage), 0);
}
//-------------------------------------------------------------------------------------------------
void checkMessages(char *userID, int client_socket) // function to check messages
{
    // Open messages CSV file
    char messagesCSVPath[50];
    strcpy(messagesCSVPath, "Messages/");
    strcat(messagesCSVPath, userID);
    strcat(messagesCSVPath, ".csv");

    sortTheCSVFileAccordingToDate(messagesCSVPath);

    FILE *messagesCSV = fopen(messagesCSVPath, "r");

    // Create message
    Message message;
    strcpy(message.senderID, "Server");
    strcpy(message.receiverID, userID);

    // Send messages to the client
    char messageBuffer[256];
    while (fgets(messageBuffer, sizeof(message), messagesCSV))
    {
        strcat(message.body, messageBuffer);
        strcat(message.body, "\n");
    }

    send(client_socket, &message, sizeof(message), 0);

    fclose(messagesCSV);
}
//-------------------------------------------------------------------------------------------------
void sortTheCSVFileAccordingToDate(char *messagesCSVPath)
{
    // Open messages CSV file
    FILE *messagesCSV = fopen(messagesCSVPath, "r");
    if (!messagesCSV)
    {
        perror("Error opening messages CSV file");
        return;
    }

    // Read messages into an array
    Message messages[100]; // Assuming a maximum of 100 messages
    int messageCount = 0;
    while (fread(&messages[messageCount], sizeof(Message), 1, messagesCSV) == 1)
    {
        messageCount++;
    }

    // Bubble sort messages based on date
    for (int i = 0; i < messageCount - 1; i++)
    {
        for (int j = 0; j < messageCount - i - 1; j++)
        {
            if (compareDates(&messages[j].date, &messages[j + 1].date) > 0)
            {
                // Swap messages
                Message temp = messages[j];
                messages[j] = messages[j + 1];
                messages[j + 1] = temp;
            }
        }
    }

    // Close the file
    fclose(messagesCSV);

    // Reopen the file for writing
    messagesCSV = fopen(messagesCSVPath, "w");
    if (!messagesCSV)
    {
        perror("Error opening messages CSV file for writing");
        return;
    }

    // Write sorted messages back to the file
    for (int i = 0; i < messageCount; i++)
    {
        fwrite(&messages[i], sizeof(Message), 1, messagesCSV);
    }

    // Close the file
    fclose(messagesCSV);
}
//-------------------------------------------------------------------------------------------------
// Function to compare two dates
int compareDates(const Date *date1, const Date *date2)
{
    if (date1->year != date2->year)
        return date1->year - date2->year;
    if (date1->month != date2->month)
        return date1->month - date2->month;
    if (date1->day != date2->day)
        return date1->day - date2->day;
    if (date1->hour != date2->hour)
        return date1->hour - date2->hour;
    if (date1->minute != date2->minute)
        return date1->minute - date2->minute;
    return date1->second - date2->second;
}
//=================================================================================================
// CSV file functions
void createCSVIfNotExists(char *userID) // function to create contacts and messages CSV files if they don't exist
{
    char messagesDirectory[10] = "Messages/";
    char contactsDirectory[10] = "Contacts/";

    // Create Messages directory if it doesn't exist
    struct stat st = {0};
    if (stat(messagesDirectory, &st) == -1)
    {
        mkdir(messagesDirectory, 0700);
    }

    // Create Contacts directory if it doesn't exist
    if (stat(contactsDirectory, &st) == -1)
    {
        mkdir(contactsDirectory, 0700);
    }

    // Create contacts CSV file if it doesn't exist
    char contactsCSVPath[50];
    strcpy(contactsCSVPath, contactsDirectory);
    strcat(contactsCSVPath, userID);
    strcat(contactsCSVPath, ".csv");
    FILE *contactsCSV = fopen(contactsCSVPath, "a");
    fclose(contactsCSV);

    // Create messages CSV file if it doesn't exist
    char messagesCSVPath[50];
    strcpy(messagesCSVPath, messagesDirectory);
    strcat(messagesCSVPath, userID);
    strcat(messagesCSVPath, ".csv");
    FILE *messagesCSV = fopen(messagesCSVPath, "a");
    fclose(messagesCSV);
}
//-------------------------------------------------------------------------------------------------
