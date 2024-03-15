# The problem of readers and writers
### SFML Library
To run this project, you need to download the SFML library and include it in your development environment. The library can be downloaded from the official SFML website:

[SFML Download Page](https://www.sfml-dev.org/download.php)

Please follow the instructions on the download page to configure SFML for your operating system and development environment.

The writing process presented as a list of steps:
 The operation of Writer processes can be described as follows:
 Write_Function() { 
1 If there are Readers or another Writer in the Reading Room: 
The Writer_Thread changes its state to waiting. 
2 The Writer_Thread enters the Reading Room and writes.
3 Upon leaving the Reading Room, the Writer_Thread checks if there are any Readers in the Reading Room. 
4 If some Readers are waiting for access: 
All waiting Reader threads gain access to the Reading Room. 
Otherwise: 
If a Writer is waiting for access: 
The Writer process gains access to the Reading Room. 
} 
The reading process presented as a list of steps: 
The operation of Reader processes can be described as follows: 
Read_Function() { 
1 If a Writer is in the Reading Room or a Writer is waiting to enter: The Reader_Thread changes its state to waiting. 
2 The Reader_Thread enters the Reading Room and reads. 
3 Upon leaving the Reading Room, the Reader_Thread checks if the Reading Room is empty after their departure. 
4 If the Reading Room is empty and a Writer is waiting for access: 
Access to the Reading Room is granted to one waiting Writer. 
}
Program Interface: 

The program was created in C++. After starting the program, the user enters in the console the number of Writers and Readers demanding access to the Reading Room.
Then, information about the number of:
•	Writers inside the Reading Room,
•	Readers inside the Reading Room,
•	Writers waiting for access,
•	Readers waiting for access. 

