#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    int serverSocketfd, clientSocketfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    if ((serverSocketfd = socket(AF_INET, SOCK_STREAM, 0)) <0) {
        cerr << "Error creating socket." << endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error binding socket." << endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocketfd, 5) == -1) {
        cerr << "Error listening for connections." << endl;
        return 1;
    }

    cout << "Server listening on port " << atoi(argv[1]) << "..." << endl;

    while (true) {
        // Accept incoming connection
        clientSocketfd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocketfd == -1) {
            cerr << "Error accepting connection." << endl;
            continue;
        }
        //file read in this buffer
        char buffer[2048];
        memset(buffer, 0, sizeof(buffer));

        // Receive source code from client
        ssize_t bytesRead = recv(clientSocketfd, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            cerr << "Error receiving data." << endl;
            close(clientSocketfd);
            continue;
        }

        // Save the received code to a temporary file studentcode.cpp
        ofstream codeFile("student_code.cpp");
        codeFile << buffer;
        codeFile.close();


        // Compile and run the code
        int compileResult = system("g++ -o student_program student_code.cpp 2>compile_error.txt");

        if (compileResult != 0) {
            // Compilation failed, send the error message back
            ifstream errorFile("compile_error.txt");
            string compileError((istreambuf_iterator<char>(errorFile)), istreambuf_iterator<char>());
            string response = "COMPILER ERROR\n" + compileError;
            send(clientSocketfd, response.c_str(), response.size(), 0);
            close(clientSocketfd);
            continue;
        }

        //Execution or Run
        
        int executionResult = system("./student_program 2>run-error.txt 1>output.txt");

        if (executionResult != 0) {
            // Runtime error, send the error message back
            ifstream errorFile("run-error.txt");
            string runError((istreambuf_iterator<char>(errorFile)), istreambuf_iterator<char>());
            string result = "RUNTIME ERROR \n"+runError;
            send(clientSocketfd, result.c_str(), result.size(), 0);
            close(clientSocketfd);
            continue;
        }
        
        cout << "Program executed";
        
        // Execute the diff command to actual output and expected output
            
        int result_status = system("diff expected-output.txt output.txt >difference.txt");
	    ifstream resultFile("difference.txt"); 

	    cout << "Comparing the results: ";

	    if(result_status < 0 || !resultFile.is_open()) {
	      perror("Error while evaluating submission");
	      close(clientSocketfd);
	      continue;
	    }
	    string result;
	    
	    // If there is no difference between actual output and expected output return PASS else return output of diff
	    if(resultFile.peek() == ifstream::traits_type::eof()) {
		    result = "PASS";
	    } 
        else {
	        string outputError((istreambuf_iterator<char>(resultFile)), istreambuf_iterator<char>());
		    result = "OUTPUT ERROR \n" + outputError;    
	    } 

	    send(clientSocketfd, result.c_str(), result.size(), 0);
	    close(clientSocketfd);
        }
	return 0;


}
