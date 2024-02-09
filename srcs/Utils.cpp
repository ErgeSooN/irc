#include "../includes/Utils.hpp"

void Utils::writeMessage(int socket, std::string const& message)
{
    if ((write(socket, message.c_str(), message.length())) < 0)
        std::cout << "Message cannot send!" << std::endl;
}

void Utils::writeAllMessage(std::vector<int> const& fds, std::string const& message)
{
    for (std::vector<int>::const_iterator it = fds.begin(); it != fds.end(); ++it) {
        writeMessage(*it, message);
    }
}

std::string Utils::intToString(int n)
{
    std::stringstream ss;
    ss << n;
    std::string str = ss.str();
    return str;
}

int Utils::portIsValid(std::string const& port)
{
    for (size_t i = 0; i < port.size(); ++i) {
        if (!isdigit(port[i]))
            return 0;
    }
    if (atoi(port.c_str()) > 65535)
        return 0;
    return 1;
}

std::string Utils::welcome()
{
    std::string data;

    data += "\n Serverimize Hoşgeldiniz\n";

    data += " Developers\n\n";
    data +=" Onur Öztürk    -> oozturk\n";
    data +=" Ali Eren Yaman -> ayaman \n";

    
    
    return data;
}

int ft_stoi(const std::string& str) 
{
    std::istringstream iss(str); // Stringi bir string stream'e yerleştir
    int result;
    if (!(iss >> result)) // int değere dönüşüm yap
        throw std::invalid_argument("Invalid argument");     // Dönüşüm hatası durumunda buraya düşülebilir
    return result;
}