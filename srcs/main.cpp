#include "../includes/Server.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 3) //burada argüman sayısını kontrol ediyoruz
            throw std::runtime_error("./ircserv <port> <password>");
        if (!Utils::portIsValid(av[1])) //burada portun 0-65535 arasında olup olmadığını kontrol ediyoruz
            throw std::runtime_error("invalid port");
        Server::getInstance()->manageServer(atoi(av[1]), av[2]); //burada serverı başlatıyoruzx
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}