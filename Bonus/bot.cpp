#include "../INC/Server.hpp"


int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./bot <ip> <port> <password> <channel>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];
    std::string channel = argv[4];

    int botSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (botSocket < 0)
    {
        std::cerr << "Bot failed to create socket" << std::endl;
        return 1;
    }

    // 2. Set the Server Address we want to talk to
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr); // Converts IP string to binary

    if (connect(botSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Bot failed to connect to server" << std::endl;
        close(botSocket);
        return 1;
    }

    std::cout << "Connected! Sending registration data..." << std::endl;

    std::string auth = "PASS " + password + "\r\n";
    auth += "NICK CoolBot\r\n";
    auth += "USER bot 0 * :I am a simple bot\r\n";
    send(botSocket, auth.c_str(), auth.length(), 0);
    std::cout << "Bot sent authentication and registration commands." << std::endl;

    std::string join = "JOIN #" + channel + "\r\n";
    send(botSocket, join.c_str(), join.length(), 0);
    std::cout << "Bot joined #" << channel << " channel." << std::endl;
    char buffer[BUFFER_SIZE];
    std::string inBuffer = "";

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(botSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0)
        {
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        inBuffer += buffer;
        size_t pos;

        while ((pos = inBuffer.find("\r\n")) != std::string::npos)
        {
            std::string line = inBuffer.substr(0, pos);
            inBuffer.erase(0, pos + 2);

            if (line.find("PRIVMSG") != std::string::npos && line.find("!ping") != std::string::npos)
            {
                std::string reply = "PRIVMSG #" + channel + " :Pong! My sockets are working perfectly!\r\n";
                send(botSocket, reply.c_str(), reply.length(), 0);
                std::cout << "Bot reacted and sent a Pong!" << std::endl;
            }
            if (line.find("!uptime") != std::string::npos)
            {
                std::ifstream uptimeFile("/proc/uptime");
                std::string uptimeStr;
                
                if (uptimeFile >> uptimeStr) // Reads the first number (seconds)
                {
                    int totalSeconds = std::atoi(uptimeStr.c_str());
                    int hours = totalSeconds / 3600;
                    int minutes = (totalSeconds % 3600) / 60;

                    std::stringstream reply;
                    reply << "PRIVMSG #" + channel + " :System Uptime: " << hours << "h " << minutes << "m\r\n";
                    
                    send(botSocket, reply.str().c_str(), reply.str().length(), 0);
                }
                std::cout << "Bot reacted and sent uptime information!" << std::endl;
            }
        }
    }

    close(botSocket);
    return 0;
}