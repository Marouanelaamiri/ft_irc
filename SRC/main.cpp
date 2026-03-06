#include "Fakeserver.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <exception>

bool isPortValid(std::string port) {
    return (port.find_first_not_of("0123456789") == std::string::npos && 
            std::atoi(port.c_str()) >= 1024 && 
            std::atoi(port.c_str()) <= 65535);
}

int main(int ac, char **av) {
    if (ac != 3) {
        std::cout << "Usage: " << av[0] << " <port number> <password>" << std::endl; 
        return 1;
    }

    Server ser;
    std::cout << "---- SERVER ----" << std::endl;

    try {
        // Handle signals to prevent abrupt background crashing
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        signal(SIGPIPE, SIG_IGN); 

        if (!isPortValid(av[1]) || !*av[2] || std::strlen(av[2]) > 20) {
            std::cout << "Invalid Port number / Password!" << std::endl; 
            return 1;
        }

        // Pass control to the engine loop
        ser.init(std::atoi(av[1]), av[2]);
    }
    catch (const std::exception& e) {
        // If the Parser or Engine throws a fatal error, it lands here.
        ser.close_fds();
        std::cerr << "\n[CRITICAL ERROR] Server crashed: " << e.what() << std::endl;
    }

    std::cout << "The Server Closed!" << std::endl;
    return 0;
}