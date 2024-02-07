#include "../../includes/Server.hpp"

void Server::Notice(std::vector<std::string>& params, Client& cli)
{
    passChecker(cli); // Kullanıcı giriş yapmış mı yapmamış mı onunu kontrolü sağlanır

    if (params[0][0] == '#') {   
        Utils::writeMessage(cli._cliFd, "Cannot notice a channel\r\n");
        return ;
    }
    if (params.size() < 2) { // yeterli parametre gönderildi mi ?
        Utils::writeMessage(cli._cliFd, ERR_NEEDMOREPARAMS(cli._nick, "NOTICE"));
        return ;
    }
    if (cli._nick == params[0]) { // kendine mesaj göndermeye calışırsa, hata mesajı döndürür.
        Utils::writeMessage(cli._cliFd, "Cannot notice yourself\r\n");
        return ;
    }
    size_t flag = 0;
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->_nick == params[0])
        {
            if (params[1][0] == ':')
                getAfterColon(params);
            flag = 1;
            it->_messageBox.push_back(RPL_NOTICE(cli._nick, params[0], params[1]));
            FD_SET(it->_cliFd, &_writeFds);
            break;
        }
    }
    if (flag == 0) {
        Utils::writeMessage(cli._cliFd, ERR_NOSUCHNICK);
    }
}
