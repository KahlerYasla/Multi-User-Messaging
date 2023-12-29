# Multi_User_Messaging

![University Logo](./Public/LogoUniversity.png)

**STUDENT ID:** 21011603  
**STUDENT NAME:** SELİN ÇIRAK  
**STUDENT EMAIL:** selin.cirak@std.yildiz.edu.tr

---

# Table of Contents:
- [Project Description](#project-description)
   - [Built With](#built-with)
   - [Libraries and Libraries and Dependencies](#libraries-and-dependencies)
   - [Compatible With](#compatible-with)
- [Project Structure](#project-structure)
- [Screenshots](#screenshots)
- [Code Documentation](#code-documentation)
  - [Server Component](#server-component)
    - [Function Prototypes and Explanations](#function-prototypes-and-explanations)
  - [Client Component](#client-component)
    - [Function Prototypes and Explanations](#function-prototypes-and-explanations-1)

---
# Project Description:
Multi-threaded chat application implemented in C, consisting of both server and client components. The server manages user contacts and messages, while the client provides a user-friendly interface to interact with the server. The communication between the server and client is facilitated through socket programming.

#### Built with:
- C
- Socket Programming
- Multi-threading
- CSV Files

#### Libraries and Dependencies:
- <stdio.h>
- <stdlib.h>
- <string.h>
- <unistd.h>
- <arpa/inet.h>
- <pthread.h>
- No external dependencies

#### Compatible with:
- Linux
- MacOS

---

# Project Structure:
Server Side 
   - `server.c` file contains the source code for the server component.
   - `Messages` folder contains the messagebox for each user.
      - `userID.csv` files contain the messages for each user.
   - `Contacts` folder contains the contacts list for each user.
      - `userID.csv` files contain the contacts list for each user.

Client Side

   - `client.c` file contains the source code for the client component.

Root Folder
- `ansiTerminalColors.h` is a header file contains ANSI escape sequences for colored terminal output.
- `client and server` are the executables for the client and server components, respectively.
- `Public` folder contains the project logo and screenshots.
   - `Project ScreenShots` folder contains screenshots of the project.

---

# Screenshots:

![Screenshot 1](./Public/Project%20ScreenShots/0.png)
---
![Screenshot 2](./Public/Project%20ScreenShots/1.png)
---
![Screenshot 3](./Public/Project%20ScreenShots/2.png)
---
![Screenshot 4](./Public/Project%20ScreenShots/3.png)
---
![Screenshot 5](./Public/Project%20ScreenShots/4.png)
---
![Screenshot 6](./Public/Project%20ScreenShots/5.png)
---
![Screenshot 7](./Public/Project%20ScreenShots/6.png)
---
![Screenshot 8](./Public/Project%20ScreenShots/7.png)
---
![Screenshot 9](./Public/Project%20ScreenShots/8.png)
---
![Screenshot 10](./Public/Project%20ScreenShots/9.png)
---
![Screenshot 11](./Public/Project%20ScreenShots/10.png)
---
![Screenshot 12](./Public/Project%20ScreenShots/11.png)
---
![Screenshot 13](./Public/Project%20ScreenShots/12.png)
---
![Screenshot 14](./Public/Project%20ScreenShots/13.png)

# Code Documentation

## Server Component:

### Function Prototypes and Explanations:

1. **`void *handle_client_messages(void *arg)`**

   - **Inputs:** `void *arg` (client socket)
   - **Outputs:** None
   - **Explanation:** This function runs in a separate thread for each connected client. It handles incoming messages from the client, interprets user input, and performs corresponding actions.

2. **`void getCurrentDateAndTime(Date *date)`**

   - **Inputs:** `Date *date` (a structure for holding date and time information)
   - **Outputs:** None
   - **Explanation:** Retrieves the current date and time and stores it in the provided `Date` structure.

3. **`void listContacts(char *userID, int client_socket)`**

   - **Inputs:** `char *userID`, `int client_socket`
   - **Outputs:** Sends a list of contacts to the client
   - **Explanation:** Reads the user's contacts from a CSV file and sends them to the client.

4. **`void listMessagesFromUser(char *userID, Message message, int client_socket)`**

   - **Inputs:** `char *userID`, `Message message`, `int client_socket`
   - **Outputs:** Sends messages from a specified user to the client
   - **Explanation:** Reads messages from a CSV file, filters them based on the specified user, and sends the result to the client.

5. **`void deleteMessage(char *userID, Message message, int client_socket)`**

   - **Inputs:** `char *userID`, `Message message`, `int client_socket`
   - **Outputs:** Sends a confirmation or error message to the client
   - **Explanation:** Deletes a specified message from the user's message history and sends a confirmation or error message to the client.

6. **`void addUser(char *userID, Message message, int client_socket)`**

   - **Inputs:** `char *userID`, `Message message`, `int client_socket`
   - **Outputs:** Sends a confirmation message to the client
   - **Explanation:** Adds a user to the contact list and sends a confirmation message to the client.

7. **`void deleteUser(char *userID, Message message, int client_socket)`**

   - **Inputs:** `char *userID`, `Message message`, `int client_socket`
   - **Outputs:** Sends a confirmation or error message to the client
   - **Explanation:** Deletes a specified user from the contact list and sends a confirmation or error message to the client.

8. **`void sendMessage(char *userID, Message message, int client_socket)`**

   - **Inputs:** `char *userID`, `Message message`, `int client_socket`
   - **Outputs:** Sends a confirmation message to the client
   - **Explanation:** Sends a message from the user to another user, updating the recipient's message history.

9. **`void checkMessages(char *userID, int client_socket)`**

   - **Inputs:** `char *userID`, `int client_socket`
   - **Outputs:** Sends a list of messages to the client
   - **Explanation:** Reads the user's messages from a CSV file and sends them to the client.

10. **`void sortTheCSVFileAccordingToDate(char *messagesCSVPath)`**

    - **Inputs:** `char *messagesCSVPath`
    - **Outputs:** None
    - **Explanation:** Sorts the messages in a CSV file based on their dates.

11. **`int compareDates(const Date *date1, const Date *date2)`**

    - **Inputs:** `const Date *date1`, `const Date *date2`
    - **Outputs:** Returns an integer (comparison result)
    - **Explanation:** Compares two date structures and returns the result.

12. **`void createCSVIfNotExists(char *userID)`**

    - **Inputs:** `char *userID`
    - **Outputs:** None
    - **Explanation:** Creates contacts and messages CSV files if they don't exist for the given user.

## Client Component:

### Function Prototypes and Explanations:

1. **`void displayMenu()`**

   - **Inputs:** None
   - **Outputs:** None
   - **Explanation:** Displays the menu options for the user.

2. **`void sendMessageToServer(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Sends a message to the server
   - **Explanation:** Takes user input for sending a message, constructs a `Message` structure, and sends it to the server.

3. **`void receiveMessagesFromServer(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Displays messages received from the server
   - **Explanation:** Receives and displays messages from the server.

4. **`void addUserToContacts(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Adds a user to the contacts list
   - **Explanation:** Takes user input for adding a contact, constructs a `Message` structure, and sends it to the server.

5. **`void deleteUserFromContacts(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Deletes a user from the contacts list
   - **Explanation:** Takes user input for deleting a contact, constructs a `Message` structure, and sends it to the server.

6. **`void displayContacts(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Displays the list of contacts
   - **Explanation:** Requests and displays the list of contacts from the server.

7. **`void displayMessagesFromUser(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Displays messages from a specified user
   - **Explanation:** Takes user input for a specified user, constructs a `Message` structure, sends it to the server, and displays the response.

8. **`void deleteMessageFromUser(int client_socket, char *userID)`**

   - **Inputs:** `int client_socket`, `char *userID`
   - **Outputs:** Deletes a message from a specified user
   - **Explanation:** Takes user input for a specified user and message, constructs a `Message` structure, sends it to the server, and displays the response.

9. **`void clearScreen()`**

   - **Inputs:** None
   - **Outputs:** None
   - **Explanation:** Clears the console screen.