# Multi_User_Messaging 
 Multithreaded Realtime Console Chat App 💬
 
![Featured Photo](./Public/Project%20ScreenShots/12.png)

# List of Contents:
- [Project Description](#project-description)
   - [Built With](#built-with)
   - [Libraries and Dependencies](#libraries-and-dependencies)
   - [Compatible With](#compatible-with)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Future Improvements](#future-improvements)
- [Screenshots](#screenshots)
- [Code Documentation](#code-documentation)
  - [server.c](#serverc)
    - [Function Prototypes and Explanations](#function-prototypes-and-explanations)
  - [client.c](#clientc)
    - [Function Prototypes and Explanations](#function-prototypes-and-explanations-1)
   - [ansiTerminalColors.h](#ansiterminalcolorsh)
      - [Function Prototypes and Explanations](#function-prototypes-and-explanations-2)

---
# Project Description:
Multi-threaded **`console`** chat application implemented in C, consisting of both server and client components. The server manages user contacts and messages, while the client provides a user-friendly interface to interact with the server. The communication between the server and client is facilitated through socket programming.

#### Built With:

Programming Languages:
- C

Frameworks and Technologies:
- No frameworks used (only standard libraries)
- Only C standard libraries for Unix-like systems
- Csv file system for storing messages and contacts


#### Libraries and Dependencies:

- Standart libraries Listed Only Important Ones:
   - arpa/inet.h (for socket programming)
   - unistd.h (for read and write functions)
   - pthread.h (for multithreading)
   - time.h (for date and time functions)
   - sys/types.h (for socket programming)
   - sys/stat.h (for file permissions)
- External libraries:
   - [ansiTerminalColors.h](./ansiTerminalColors.h) (written by me)
- Dependencies:
   - [gcc](https://gcc.gnu.org/) (GNU Compiler Collection)

#### Compatible with:
```diff
+ Linux

+ MacOS

- Windows (needs to include winsock.h instead of arpa/inet.h)
```

---

# Usage:
It is well described with screenshots in [Screenshots](#screenshots) section.

---

# Project Structure:

```console
.
├── Contacts
│   ├── userID.csv // example
│   └── ...
├── Messages
│   ├── userID.csv // example
│   └── ...
├── Public // this folder is not included in the project
│   └── Project ScreenShots
│       └── ...
├── Readme.md
├── ansiTerminalColors.h // for colored output
├── client // executable of client component
├── client.c // source code of client component
├── server  // executable of server component
└── server.c // source code of server component

5 directories, 24 files
```

---

# Future Improvements:
```diff
+ Add a proper database instead of CSV files (maybe SQLite or PostgreSQL)

+ Add a decent GUI (maybe with Qt)
```

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
---

# Code Documentation

## server.c

#### Function Prototypes and Explanations:

<details><summary>See more</summary>
---
1. Handler function for each client thread

   ```c 
   void *handleClient(void *arg)
   ```
   > This function runs in a separate thread for each connected client. It handles incoming messages from the client, interprets user input, and performs corresponding actions.

   > |  |  |
   > | - | - |
   > | Input | void *arg (client socket) |
   > | Output | None |

---

2. Getter function for the current date and time

   ```c 
   Date getCurrentDateAndTime()
   ```
   > This function retrieves the current date and time and returns it as a `Date` structure.

   > |  |  |
   > | - | - |
   > | Input | None |
   > | Output | Date (structure) |

---

3. Response function for listing contacts

   ```c 
   void listContacts(char *userID, int client_socket)
   ```
   > This function reads the user's contacts from a CSV file and sends them to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, int client_socket |
   > | Output | Sends a list of contacts to the client |

---

4. Response function for listing messages from a user

   ```c 
   void listMessagesFromUser(char *userID, Message message, int client_socket)
   ```
   > This function reads messages from a CSV file, filters them based on the specified user, and sends the result to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, Message message, int client_socket |
   > | Output | Sends messages from a specified user to the client |

---

5. Response function for deleting a message

   ```c 
   void deleteMessage(char *userID, Message message, int client_socket)
   ```
   > This function deletes a specified message from the user's message history and sends a confirmation or error message to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, Message message, int client_socket |
   > | Output | Sends a confirmation or error message to the client |

---

6. Response function for adding a user

   ```c 
   void addUser(char *userID, Message message, int client_socket)
   ```
   > This function adds a user to the contact list and sends a confirmation message to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, Message message, int client_socket |
   > | Output | Sends a confirmation message to the client |

---

7. Response function for deleting a user

   ```c 
   void deleteUser(char *userID, Message message, int client_socket)
   ```
   > This function deletes a specified user from the contact list and sends a confirmation or error message to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, Message message, int client_socket |
   > | Output | Sends a confirmation or error message to the client |

---

8. Response function for sending a message

   ```c 
   void sendMessage(char *userID, Message message, int client_socket)
   ```
   > This function sends a message from the user to another user, updating the recipient's message history.

   > |  |  |
   > | - | - |
   > | Input | char *userID, Message message, int client_socket |
   > | Output | Sends a confirmation message to the client |

---

9. Response function for checking messages

   ```c 
   void checkMessages(char *userID, int client_socket)
   ```
   > This function reads the user's messages from a CSV file and sends them to the client.

   > |  |  |
   > | - | - |
   > | Input | char *userID, int client_socket |
   > | Output | Sends a list of messages to the client |

---

10. Function for sorting messages in a CSV file

    ```c 
    void sortTheCSVFileAccordingToDate(char *messagesCSVPath)
    ```
    > This function sorts the messages in a CSV file based on their dates.

    > |  |  |
    > | - | - |
    > | Input | char *messagesCSVPath |
    > | Output | None |

---

11. Function for comparing two dates

    ```c 
    int compareDates(const Date *date1, const Date *date2)
    ```
    > This function compares two date structures and returns the result.

    > |  |  |
    > | - | - |
    > | Input | const Date *date1, const Date *date2 |
    > | Output | Returns an integer (comparison result) |

---

12. Function for creating CSV files if they don't exist

    ```c 
    void createCSVIfNotExists(char *userID)
    ```
    > This function creates contacts and messages CSV files if they don't exist for the given user.

    > |  |  |
    > | - | - |
    > | Input | char *userID |
    > | Output | None |

---

</details>

## client.c

#### Function Prototypes and Explanations:

<details><summary>See more</summary>
1. Function for displaying the menu

   ```c 
   void displayMenu()
   ```
   > This function displays the menu options for the user.

   > |  |  |
   > | - | - |
   > | Input | None |
   > | Output | None |

---

2. Request function for sending a message to the server

   ```c 
   void sendMessageToServer(int client_socket, char *userID)
   ```
   > This function takes user input for sending a message, constructs a `Message` structure, and sends it to the server.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Sends a message to the server |

---

3. Request function for receiving messages from the server

   ```c 
   void receiveMessagesFromServer(int client_socket, char *userID)
   ```
   > This function receives and displays messages from the server.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Displays messages received from the server |

---

4. Request function for adding a user to the contacts list

   ```c 
   void addUserToContacts(int client_socket, char *userID)
   ```
   > This function takes user input for adding a contact, constructs a `Message` structure, and sends it to the server.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Adds a user to the contacts list |

---

5. Request function for deleting a user from the contacts list

   ```c 
   void deleteUserFromContacts(int client_socket, char *userID)
   ```
   > This function takes user input for deleting a contact, constructs a `Message` structure, and sends it to the server.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Deletes a user from the contacts list |

---

6. Request function for displaying the list of contacts

   ```c 
   void displayContacts(int client_socket, char *userID)
   ```
   > This function requests and displays the list of contacts from the server.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Displays the list of contacts |

---

7. Request function for displaying messages from a specified user

   ```c 
   void displayMessagesFromUser(int client_socket, char *userID)
   ```
   > This function takes user input for a specified user, constructs a `Message` structure, sends it to the server, and displays the response.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Displays messages from a specified user |

---

8. Request function for deleting a message from a specified user

   ```c 
   void deleteMessageFromUser(int client_socket, char *userID)
   ```
   > This function takes user input for a specified user and message, constructs a `Message` structure, sends it to the server, and displays the response.

   > |  |  |
   > | - | - |
   > | Input | int client_socket, char *userID |
   > | Output | Deletes a message from a specified user |

---

9. Function for clearing the console screen

   ```c 
   void clearScreen()
   ```
   > This function clears the console screen.

   > |  |  |
   > | - | - |
   > | Input | None |
   > | Output | None |

---
</details>

## ansiTerminalColors.h

#### Function Prototypes and Explanations:

<details><summary>See more</summary>
0. Macro function for printing colored text to the console

   ```c 
   // XXX is the color name
   #define LogXXX(x) printf(ANSI_COLOR_XXX x ANSI_COLOR_RESET)
   ```
   > This function prints colored text to the console.

   > |  |  |
   > | - | - |
   > | Input | char *text, char *color |
   > | Output | None |

---

</details>