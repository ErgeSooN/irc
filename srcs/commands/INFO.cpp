#include "../../includes/Server.hpp"

std::string infoMessage()
{
    std::string message;

    message += " +--------------------------------------------------------+\n";
    message += "|                                                          |\n";
    message += "| * Developers:                                            |\n";
    message += "|   - Onur Öztürk                 (oozturk)                |\n";
    message += "|   - Ali Eren Yaman              (ayaman)                 |\n";
    message += "|                                                          |\n";
    message += "| * Current commands:                                      |\n";
    message += "|   - INFO                                                 |\n";
    message += "|   - JOIN                                                 |\n";
    message += "|   - KICK                                                 |\n";
    message += "|   - LIST                                                 |\n";
    message += "|   - MODE                                                 |\n";
    message += "|   - NICK                                                 |\n";
    message += "|   - NOTICE                                               |\n";
    message += "|   - PASS                                                 |\n";
    message += "|   - CAP                                                  |\n";
    message += "|   - PRIVMSG                                              |\n";
    message += "|   - WHO                                                  |\n";
    message += "|   - QUIT                                                 |\n"; 
    message += "|   - PART                                                 |\n";
    message += "|   - USER                                                 |\n";
    message += " +--------------------------------------------------------+\n";
    return (message);
}

void Server::Info(std::vector<std::string>& params, Client &cli)
{
    passChecker(cli);
    (void)params;
    Utils::writeMessage(cli._cliFd, RPL_INFO(cli._nick, infoMessage()));
}
