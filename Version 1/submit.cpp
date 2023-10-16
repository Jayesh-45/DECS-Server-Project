//client

#include<iostream>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fstream>
#include<chrono> // Include for getting time

using namespace std;

//uint64_t et_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <serverIP:port> <sourceCodeFileTobeGraded> <loopNum> <sleepTimeSeconds>" << endl;
        return 1;
    }
    
    char* serverAddress = strtok(argv[1], ":");
    char* serverPort = strtok(NULL, ":");
    char* CodeFile = argv[2];
    int loopNum = atoi(argv[3]);
    int sleepTimeSeconds = atoi(argv[4]);

    int clientSocket;
    struct sockaddr_in serverAddr;
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(serverPort));
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress);

    
    uint64_t totalTime = 0; // To keep track of the total time taken
    int successfulResponses = 0; // To count the number of successful responses

    uint64_t loopst_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    for (int i = 0; i < loopNum; i++) {
        // Create socket
        if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            cerr << "Error creating socket." << endl;
            return 1;
        }

        // Connect to the server
        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
          cerr << "Error connecting to server." << endl;
            return 1;
        }

        // Read the source code file
        ifstream file(CodeFile);
        string sourceCode((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        // Record the start time
        uint64_t st_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        

        // Send the source code to the server
        ssize_t bytesSent = send(clientSocket, sourceCode.c_str(), sourceCode.size(), 0);
        if (bytesSent == -1) {
            cerr << "Error sending data." << endl;
            close(clientSocket);
            return 1;
        }

        char buffer[2048];
        memset(buffer, 0, sizeof(buffer));

        // Receive result from the server
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            cerr << "Error receiving data." << endl;
            close(clientSocket);
            return 1;
        }
        
        // Record the end time
        uint64_t et_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

        // Calculate the response time in microseconds
        totalTime += (et_ms-st_ms);
        
        // Check for successful response 
        // If it's successful, increment the counter
        if (bytesRead!= -1) {
            successfulResponses++;
        }

        close(clientSocket);
        // Sleep for the specified duration
        sleep(sleepTimeSeconds);
    }
    uint64_t loopet_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

    // Calculate the average response time
    // Output the results
    float avg_rt = (float)totalTime/loopNum;
    cout << "Average response time: "<< avg_rt << " ms."<< endl;
    cout << "Successful responses: "<< successfulResponses << endl;
    cout << "Time to complete loop: "<< loopet_ms-loopst_ms << " ms."<< endl;
    
    

    return 0;
}
