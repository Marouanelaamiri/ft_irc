#include "./INC/Server.hpp"
#include <csignal>


bool isPortPassValid(std::string port, std::string password) {
    if (port.find_first_not_of("0123456789") != std::string::npos)
        std::cout << "Invalid Port number! Must be numeric.\n";
    else if (std::atoi(port.c_str()) < 1024 || std::atoi(port.c_str()) > 65535)
        std::cout << "Invalid Port number! Must be between 1024 and 65535.\n";
    else if (password.length() > 20)
        std::cout << "Invalid Password! Must be between 1 and 20 characters.\n";
    else
        return true;
    return false;
}

int main(int ac, char **av) {
    if (ac != 3) {
        std::cout << "Usage: " << av[0] << " <port number> <password>" << std::endl; 
        return 1;
    }

    Server serve;
    std::cout << "---- SERVER ----" << std::endl;
    try {
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        signal(SIGPIPE, SIG_IGN);

        if (!isPortPassValid(av[1], av[2]))
            return 1;
        serve.init(std::atoi(av[1]), av[2]);
    }
    catch (const std::exception& e) {
        serve.close_fds();
        std::cerr << "\n[CRITICAL ERROR] Server crashed: " << e.what() << std::endl;
    }

    std::cout << "The Server Closed!" << std::endl;
    // serve.close_fds();
    return 0;
}