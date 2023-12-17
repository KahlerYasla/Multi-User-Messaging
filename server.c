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
void listMessagesFromUser(char *userID, Message message, int client_socket);
void deleteMessage(char *userID, Message message, int client_socket);
//=================================================================================================
int main()
{
    LogGreen("\n***** Multi-User-Messaging Server *****");
    printf("\n=============================================================\n");

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
        LogRed("Socket bind failed on port 8080 (Maybe another server is running on the same port)\n");
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
        case 6:
            // List messages from the specified user
            listMessagesFromUser(userID, message, client_socket);
            break;
        case 7:
            // Delete message from the specified user
            deleteMessage(userID, message, client_socket);
            break;
        default:
            send(client_socket, "Invalid input!", sizeof("Invalid input!"), 0);
            break;
        }

        // Log the message to the server console
        printf("%d/%d/%d %d:%d >> from %s to %s message is >> %s\n",
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

    // Clear the message body
    strcpy(message.body, "Listing the Contacts:\n(Phone Number, Name, Surname)\n\n");

    strcpy(message.senderID, "Server");
    strcpy(message.receiverID, userID);

    // Send contacts to the client
    char contact[50];
    while (fgets(contact, sizeof(contact), contactsCSV))
    {
        strcat(message.body, contact);
    }

    // Remove the last "\n" character
    message.body[strlen(message.body) - 1] = '\0';

    send(client_socket, &message, sizeof(message), 0);

    fclose(contactsCSV);
}
//-------------------------------------------------------------------------------------------------
void listMessagesFromUser(char *userID, Message message, int client_socket)
{
    // Extract the specified userID from message
    char specifiedUserID[15];
    strcpy(specifiedUserID, message.body);

    // Delete the first 2 character from the user ID
    for (int i = 0; i < strlen(specifiedUserID); i++)
    {
        specifiedUserID[i] = specifiedUserID[i + 2];
    }

    // Open messages CSV file
    char messagesCSVPath[50];
    strcpy(messagesCSVPath, "Messages/");
    strcat(messagesCSVPath, userID);
    strcat(messagesCSVPath, ".csv");
    FILE *messagesCSV = fopen(messagesCSVPath, "r");

    // Create message
    Message newMessage;

    // Clear the message body
    strcpy(newMessage.body, "");

    strcpy(newMessage.senderID, "Server");
    strcpy(newMessage.receiverID, userID);

    // Send messages to the client
    char messageBuffer[256];
    char **token = malloc(10 * sizeof(char *));
    int isFound = 0;

    strcat(newMessage.body, "\nListing the Messages From User:\n");
    strcat(newMessage.body, specifiedUserID);
    strcat(newMessage.body, "\nFrom Who ~~> The Message\n(ID DD,MM,YYYY Hour:Minute)  \n\n");

    while (fgets(messageBuffer, sizeof(messageBuffer), messagesCSV))
    {
        // Divide messageBuffer from "," characters
        token[0] = strtok(messageBuffer, ",");

        int i = 1;
        while (token[i - 1] != NULL)
        {
            // Get the next token
            token[i] = strtok(NULL, ",");

            i++;
        }

        // If the message is from the specified user, send it to the client
        if (strcmp(token[1], specifiedUserID) == 0)
        {
            strcat(newMessage.body, token[1]);
            strcat(newMessage.body, " ~~> ");

            strcat(newMessage.body, token[3]);

            strcat(newMessage.body, "\n");

            strcat(newMessage.body, token[0]);
            strcat(newMessage.body, " ");

            strcat(newMessage.body, token[4]);
            strcat(newMessage.body, "/");
            strcat(newMessage.body, token[5]);
            strcat(newMessage.body, "/");
            strcat(newMessage.body, token[6]);

            strcat(newMessage.body, " ");
            strcat(newMessage.body, token[7]);
            strcat(newMessage.body, ":");
            strcat(newMessage.body, token[8]);

            strcat(newMessage.body, "\n\n");

            isFound = 1;
        }
    }

    // Remove the last 2 "\n" character
    if (strlen(newMessage.body) > 3)
    {
        newMessage.body[strlen(newMessage.body) - 1] = '\0';
        newMessage.body[strlen(newMessage.body) - 2] = '\0';
    }

    if (!isFound)
    {
        strcpy(newMessage.body, "No message found from the specified user!");
    }

    send(client_socket, &newMessage, sizeof(newMessage), 0);

    fclose(messagesCSV);
}
//-------------------------------------------------------------------------------------------------
void deleteMessage(char *userID, Message message, int client_socket)
{
    // Extract the specified messageID from message
    char specifiedMessageID[10];
    strcpy(specifiedMessageID, message.body);

    // Delete the first 2 character from the message ID
    for (int i = 0; i < strlen(specifiedMessageID); i++)
    {
        specifiedMessageID[i] = specifiedMessageID[i + 2];
    }

    // Open messages CSV file
    char messagesCSVPath[50];
    strcpy(messagesCSVPath, "Messages/");
    strcat(messagesCSVPath, userID);
    strcat(messagesCSVPath, ".csv");
    FILE *messagesCSV = fopen(messagesCSVPath, "r");

    // Open a temporary CSV file
    char tempCSVPath[50];
    strcpy(tempCSVPath, "Messages/temp.csv");
    FILE *tempCSV = fopen(tempCSVPath, "w");

    int isDeleted = 0;

    LogGreen("=============================================================\n");
    LogGreen("Controlling messages one by one:\n");
    // Copy all messages except the one to be deleted to the temporary CSV file
    char messageBuffer[256];
    while (fgets(messageBuffer, sizeof(messageBuffer), messagesCSV))
    {
        // Set the message first string before first "," character
        char messageID[10];

        // Clear the messageID
        memset(messageID, 0, sizeof(messageID));

        int i = 0;
        while (messageBuffer[i] != ',')
        {
            messageID[i] = messageBuffer[i];
            i++;
        }

        printf("Controlling message's ID: %s\n", messageID);

        // If the message is not the one to be deleted, copy it to the temporary CSV file
        if (strcmp(messageID, specifiedMessageID) != 0)
        {
            fprintf(tempCSV, "%s", messageBuffer);
        }
        else
        {
            LogMagenta("Message found and deleted\n");
            isDeleted = 1;
        }

        // Clear the messageBuffer
        memset(messageBuffer, 0, sizeof(messageBuffer));
    }

    LogGreen("=============================================================\n");

    // Close the files
    fclose(messagesCSV);
    fclose(tempCSV);

    // Delete the messages CSV file
    remove(messagesCSVPath);

    // Rename the temporary CSV file to messages CSV file
    rename(tempCSVPath, messagesCSVPath);

    // Create message
    Message newMessage;

    // Clear the message body
    strcpy(newMessage.body, "");

    strcpy(newMessage.senderID, "Server");

    if (isDeleted)
    {
        strcpy(newMessage.receiverID, userID);
        strcpy(newMessage.body, "Message deleted!");
    }
    else
    {
        strcpy(newMessage.receiverID, userID);
        strcpy(newMessage.body, "Message not found!");
    }

    // Send message to the client
    send(client_socket, &newMessage, sizeof(newMessage), 0);

    // Clear the newMessage
    memset(&newMessage, 0, sizeof(newMessage));

    // Clear the message body
    strcpy(newMessage.body, "");
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

    // Clear the message body
    strcpy(responseMessage.body, "");

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
    char userPhoneNumberToDelete[20];
    strcpy(userPhoneNumberToDelete, message.body);

    // Delete the first 2 character from the user ID
    for (int i = 0; i < strlen(userPhoneNumberToDelete); i++)
    {
        userPhoneNumberToDelete[i] = userPhoneNumberToDelete[i + 2];
    }

    // Open contacts CSV file
    char contactsCSVPath[50];
    strcpy(contactsCSVPath, "Contacts/");
    strcat(contactsCSVPath, userID);
    strcat(contactsCSVPath, ".csv");
    FILE *contactsCSV = fopen(contactsCSVPath, "r");

    // Open a temporary CSV file
    char tempCSVPath[50];
    strcpy(tempCSVPath, "Contacts/temp.csv");
    FILE *tempCSV = fopen(tempCSVPath, "w");

    int isDeleted = 0;

    LogGreen("=============================================================\n");
    LogGreen("Controlling phones one by one:\n");
    // Copy all contacts except the one to be deleted to the temporary CSV file
    char contact[50];
    while (fgets(contact, sizeof(contact), contactsCSV))
    {
        // Set the contact first string before first "," character
        char contactPhoneNumber[20];

        // Clear the contactPhoneNumber
        memset(contactPhoneNumber, 0, sizeof(contactPhoneNumber));

        int i = 0;
        while (contact[i] != ',')
        {
            contactPhoneNumber[i] = contact[i];
            i++;
        }

        printf("Controlling contact's phone number: %s\n", contactPhoneNumber);

        // If the contact is not the one to be deleted, copy it to the temporary CSV file
        if (strcmp(contactPhoneNumber, userPhoneNumberToDelete) != 0)
        {
            fprintf(tempCSV, "%s", contact);
        }
        else
        {
            LogMagenta("Contact found and deleted\n");
            isDeleted = 1;
        }

        // Clear the contact
        memset(contact, 0, sizeof(contact));
    }

    LogGreen("=============================================================\n");

    // Close the files
    fclose(contactsCSV);
    fclose(tempCSV);

    // Delete the contacts CSV file
    remove(contactsCSVPath);

    // Rename the temporary CSV file to contacts CSV file
    rename(tempCSVPath, contactsCSVPath);

    // Create message
    Message newMessage;

    // Clear the message body
    strcpy(newMessage.body, "");

    strcpy(newMessage.senderID, "Server");
    strcpy(newMessage.receiverID, userID);

    if (isDeleted)
    {
        strcpy(newMessage.body, "User deleted!");
    }
    else
    {
        strcpy(newMessage.body, "User not found in your contacts!");
    }

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

    // Clear the message body
    strcpy(newMessage.body, "");

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

    // Clear the message body
    strcpy(responseMessage.body, "");

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

    // sortTheCSVFileAccordingToDate(messagesCSVPath);

    FILE *messagesCSV = fopen(messagesCSVPath, "r");

    // Create message
    Message message;

    // Clear the message body
    strcpy(message.body, "");

    strcpy(message.senderID, "Server");
    strcpy(message.receiverID, userID);

    // Send messages to the client
    char messageBuffer[256];
    char **token = malloc(10 * sizeof(char *));

    strcat(message.body, "\nFrom Who ~~> The Message\n(ID DD,MM,YYYY Hour:Minute)  \n\n");

    while (fgets(messageBuffer, sizeof(messageBuffer), messagesCSV))
    {
        // Divide messageBuffer from "," characters
        token[0] = strtok(messageBuffer, ",");

        int i = 1;
        while (token[i - 1] != NULL)
        {
            // Get the next token
            token[i] = strtok(NULL, ",");

            i++;
        }

        strcat(message.body, token[1]);
        strcat(message.body, " ~~> ");

        strcat(message.body, token[3]);

        strcat(message.body, "\n");

        strcat(message.body, token[0]);
        strcat(message.body, " ");

        strcat(message.body, token[4]);
        strcat(message.body, "/");
        strcat(message.body, token[5]);
        strcat(message.body, "/");
        strcat(message.body, token[6]);

        strcat(message.body, " ");
        strcat(message.body, token[7]);
        strcat(message.body, ":");
        strcat(message.body, token[8]);

        strcat(message.body, "\n\n");
    }

    // Remove the last 2 "\n" character
    if (strlen(message.body) > 3)
    {
        message.body[strlen(message.body) - 1] = '\0';
        message.body[strlen(message.body) - 2] = '\0';
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
