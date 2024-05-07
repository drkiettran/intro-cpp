/**
 * https://raw.githubusercontent.com/toprakkeskin/Cpp-Socket-Simple-TCP-Echo-Server-Client/master/server/tcp-echo-server-main.cpp
*/

#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>
#include <tclap/CmdLine.h>

bool getArgs(int argc, char* argv[], std::string* hostname, int* portno, bool* verbose) {
     try {
        std::cout << "Processing command line arguments" << std::endl;
        TCLAP::CmdLine cmd(argv[0], ' ', "1.0");
        TCLAP::ValueArg<std::string> hostnameArg("s", "hostname", "name of hosting the server", false, "localhost", "string");
        cmd.add(hostnameArg);
        TCLAP::SwitchArg verboseArg ("v", "verbose", "detail debugging output", false);
        cmd.add(verboseArg);
        TCLAP::ValueArg<int> portnoArg("p", "port", "well known port number", false, 7777, "int");
        cmd.add(portnoArg);

        cmd.parse(argc, argv);
        *hostname = hostnameArg.getValue();
        *verbose = verboseArg.getValue();
        *portno = portnoArg.getValue();

        std::cout << "hostname: " << *hostname << std::endl;
        std::cout << "portno: " << *portno << std::endl;
        std::cout << "verbose: " << (*verbose ? "True" : "False") << std::endl;
        return true;
    }
    catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << "for arg " << e.argId() << std::endl;
        return false;
    }
    return true;
}

class EchoServer {
public:
    EchoServer(std::string hostname, int portno);
    ~EchoServer();
    int serve();
protected:
    std::string hostname;
    int portno;
    int sock_listener;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr); 
    int sock_client;

    int getSocket();
    int bindSocket();
    int listenToCall();
    int acceptCall();
    int processRequest();

};

EchoServer::EchoServer(std::string hostname, int portno) {
    this->hostname = hostname;
    this->portno = portno;
}

int EchoServer::getSocket() {
     // Create a socket & get the file descriptor
    sock_listener = socket(AF_INET, SOCK_STREAM, 0);
    // Check If the socket is created
    if (sock_listener < 0) {
        std::cerr << "[ERROR] Socket cannot be created!\n";
        return -2;
    }
    return 0;
}

int EchoServer::bindSocket() {
    // Address info to bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    //server_addr.sin_addr.s_addr = INADDR_ANY;
    // OR
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
  
    char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(sock_listener, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
             std::cerr << "[ERROR] Created socket cannot be binded to ( "
              << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
              << ":" << ntohs(server_addr.sin_port) << ")\n";
        return -3;
    }

    std::cout << "[INFO] Sock is binded to ("  
              << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
              << ":" << ntohs(server_addr.sin_port) << ")\n";
    return 0;

}

int EchoServer::listenToCall() {
    // Start listening
    if (listen(sock_listener, SOMAXCONN) < 0) {
        std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
        return -4;
    }
    std::cout << "[INFO] Socket is listening now.\n";
    return 0;
}

int EchoServer::acceptCall() {
    // Accept a call
    if ((sock_client = accept(sock_listener, (sockaddr*)&client_addr, &client_addr_size)) < 0) {
        std::cerr << "[ERROR] Connections cannot be accepted for a reason.\n";
        return -5;
    }
    std::cout << "[INFO] A connection is accepted now.\n";
 
    // Close main listener socket
    close(sock_listener);
    std::cout << "[INFO] Main listener socket is closed.\n";
    return 0;
}

int EchoServer::processRequest() {
    char msg_buf[4096];
    int bytes;
    // While receiving - display & echo msg
    while (true) {
        bytes = recv(sock_client, &msg_buf, 4096, 0);
        // Check how many bytes recieved
        // If there is no data, it means server is disconnected
        if (bytes == 0) {
            std::cout << "[INFO] Client is disconnected.\n";
            break;
        }
        // If something gone wrong
        else if (bytes < 0) {
            std::cerr << "[ERROR] Something went wrong while receiving data!.\n";
            break;
        }
        // If there is some bytes
        else {
            // Print message
            std::cout << "client> " << std::string(msg_buf, 0, bytes) << "\n";
            // Resend the same message
            if (send(sock_client, &msg_buf, bytes, 0) < 0) {
                std::cerr << "[ERROR] Message cannot be send, exiting...\n";
                break;
            }
        }
    }
    
    // Close client socket
    close(sock_client);
    std::cout << "[INFO] Client socket is closed.\n";
    return 0;
};

int EchoServer::serve() {
    std::cout << "Start servering ..." << std::endl;
    if (getSocket()) {
        return -2;
    }
    if (bindSocket()) {
        return -3;
    }
    if (listenToCall()) {
        return -4;
    }
    if (acceptCall()) {
        return -5;
    }
    if (processRequest()) {
        return -6;
    }
    std::cout << "Done servering ..." << std::endl;
    return 0;
};

EchoServer::~EchoServer() {
    std::cout << "Echo Server is terminated.\n";
}

int main(int argc, char **argv) {
    bool verbose = false;
    std::string hostname;
    int portno;

    if (!getArgs(argc, argv, &hostname, &portno, &verbose)) {
        std::cout << "Errors in providing parameters/arguments. Program exits." << std::endl;
        exit(-1);
    }
    EchoServer echoServer(hostname, portno);
    return echoServer.serve();
}