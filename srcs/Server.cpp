#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"

Server* Server::singleton = NULL;
Server::Server(): _botFd(0), _fdCount(0) {}

Server::~Server()
{
    if (singleton != NULL)
        delete singleton;
    singleton = NULL;
    close(_serverFd);
}

Server* Server::getInstance() //burası singleton yapısı
{
    try 
    {
        if (singleton == NULL)
            singleton = new Server;
        return singleton;
    } catch (std::exception & e) 
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

void Server::initCommands() //burada komutları initialize ediyoruz
{
    _commands["PASS"] = &Server::Pass;
    _commands["NICK"] = &Server::Nick;
    _commands["JOIN"] = &Server::Join;
    _commands["CAP"] = &Server::Cap;
    _commands["USER"] = &Server::User;
    _commands["WHO"] = &Server::Who;
    _commands["QUIT"] = &Server::Quit;
    _commands["PART"] = &Server::Part;
    _commands["INFO"] = &Server::Info;
    _commands["PRIVMSG"] = &Server::Privmsg;
    _commands["WHOIS"] = &Server::Whois;
    _commands["whois"] = &Server::Whois;
    _commands["NOTICE"] = &Server::Notice;
    _commands["KICK"] = &Server::Kick;
    _commands["MODE"] = &Server::Mode;
    _commands["mode"] = &Server::Mode;
    _commands["TOPIC"] = &Server::Topic;
    _commands["topic"] = &Server::Topic;
    _commands["LIST"] = &Server::List;
    _commands["INVITE"] = &Server::Invite;
    _commands["OPER"] = &Server::Oper;
    _commands["bot"] = &Server::Bot;
    _commands["HELPME"] = &Server::Help;
}

void Server::createSocket()
{
    if ((_serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //burada socketi oluşturuyoruz
        throw std::runtime_error("Socket");
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);//burada socketi nonblocking yapıyoruz
    const int enable = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) // burada socketi tekrar kullanılabilir yapıyoruz
        throw std::runtime_error("Setsockopt");
}

void Server::bindSocket(size_t const & port)
{
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr)); //burada server_addr structını sıfırlıyoruz
    server_addr.sin_addr.s_addr = INADDR_ANY;     //burada server_addr structının ip adresini
    server_addr.sin_family = AF_INET;             //burada server_addr structının family'sini
    server_addr.sin_port = htons(port);           //burada server_addr structının portunu set ediyoruz

    if (bind(_serverFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) //burada server_addr structını serverFd'ye bağlıyoruz
        throw std::runtime_error("Bind");

    if (listen(_serverFd, SOMAXCONN) < 0) //burada serverFd'yi dinliyoruz
        throw std::runtime_error("Listen");
}

void Server::acceptRequest()
{
    Client tmp;
    sockaddr_in cliAddr;
    socklen_t cliSize = sizeof(cliAddr);

    tmp._cliFd = accept(_serverFd, (sockaddr *)&cliAddr, &cliSize); //burada serverFd'den gelen bağlantıyı kabul ediyoruz
    fcntl(tmp._cliFd, F_SETFL, O_NONBLOCK); //burada bağlantıyı nonblocking yapıyoruz
    if (tmp._cliFd <= 0)
        throw std::runtime_error("Accept failed");
    tmp._port = ntohs(cliAddr.sin_port); //burada portu set ediyoruz bunu yapma sebebimiz clientin portunu almak
    inet_ntop(AF_INET, &(cliAddr.sin_addr), tmp._ipAddr, INET_ADDRSTRLEN); //bağlantı kabul edilen istemcinin IP adresini alır ve bu adresi metin formatına dönüştürür.
    FD_SET(tmp._cliFd, &_readFds);
    std::cout << GREEN << "New client connected!" << RESET << std::endl;
    _fdCount++;
    _clients.push_back(tmp);
}

std::map<std::string, std::vector<std::string> > Server::getParams(std::string const& str)
{
    std::stringstream ss(str);
    std::string tmp;
    std::map<std::string, std::vector<std::string> > ret;
    std::vector<std::string> params;
    ss >> tmp;
    std::string cmd;
    while (1)
    {
        cmd = tmp;
        if (ret.find(cmd) != ret.end())
            return ret;
        params.clear();
        ss >> tmp;
        while (_commands.find(tmp) == _commands.end())
        {
            params.push_back(tmp);
            ss >> tmp;
            if (ss.eof()) {
                ret[cmd] = params;
                return ret;
            }
        }
        if (ss.eof()) {
            params.push_back("");
            ret[cmd] = params;
            return ret;
        }
        if (params.empty())
            params.push_back("");
        ret[cmd] = params;
    }
    return ret;
}

void Server::commandHandler(std::string& str, Client& cli)
{
    std::map<std::string, std::vector<std::string> > params = getParams(str);
    for (std::map<std::string, std::vector<std::string> >::iterator it = params.begin(); it != params.end(); ++it) {
        if (_commands.find(it->first) == _commands.end())
        {
            Utils::writeMessage(cli._cliFd, "421 : " + it->first + " :Unknown command!\r\n");
            std::cout << RED << it->first << " command not found!" << RESET << std::endl;
        }
        else
            (this->*_commands[it->first])(it->second, cli);
    }
}

void Server::readEvent(int* state)
{
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (FD_ISSET(it->_cliFd, &_readFdsSup))
        {
            *state = 0;
            int readed = read(it->_cliFd, _buffer, 1024);
            if (readed <= 0) {
                std::vector<std::string> tmp;
                tmp.push_back("");
                (this->*_commands["QUIT"])(tmp, *it);
            }
            else
            {
                _buffer[readed] = 0;
                std::string tmp = _buffer;
                if (tmp == "\n") {
                    *state = 0; break;
                }
                if (tmp[tmp.length() - 1] != '\n')
                {
                    it->_buffer += tmp;
                    *state = 0; break;
                }
                else 
                    it->_buffer = it->_buffer + tmp;
                std::cout << YELLOW << it->_buffer << RESET;
                commandHandler(it->_buffer, *it);
                it->_buffer.clear();
            }
            break;
        }
    }
}

void Server::initFds()
{
    FD_ZERO(&_readFds); //burada readFds'i sıfırlıyoruz
    FD_ZERO(&_writeFds); //burada writeFds'i sıfırlıyoruz
    FD_ZERO(&_readFdsSup);  //burada readFdsSup'ı sıfırlıyoruz
    FD_ZERO(&_writeFdsSup);  //burada writeFdsSup'ı sıfırlıyoruz
    FD_SET(_serverFd, &_readFds); //burada serverFd'yi readFds'e ekliyoruz
}

void Server::writeEvent()
{
    for (cliIt it = _clients.begin(); it != _clients.end(); ++it) {
        if (FD_ISSET(it->_cliFd, &_writeFdsSup))
        {
            int writed = write(it->_cliFd, it->_messageBox[0].c_str(), it->_messageBox[0].size());
            it->_messageBox.erase(it->_messageBox.begin());
            if (it->_messageBox.empty())
                FD_CLR(it->_cliFd, &_writeFds);
            if (writed <= 0)
            {
                std::vector<std::string> tmp;
                tmp.push_back("");
                (this->*_commands["QUIT"])(tmp, *it);
            }
            break ;
        }
    }
}

void Server::run()
{
    int state = 0;

    initFds(); //burada fdsleri initialize ediyoruz
    while (1)
    {
        _readFdsSup = _readFds; //burada readFdsSup'ı readFds'e eşitliyoruz
        _writeFdsSup = _writeFds; //burada writeFdsSup'ı writeFds'e eşitliyoruz bunları eşitlelememizin sebebi select fonksiyonunun fdsleri değiştirmesidir
        state = select(_fdCount + 4, &_readFdsSup, &_writeFdsSup, NULL, 0); //burada _SUP'larda değişiklik olmadığı sürece select fonksiyonunu çalıştırıyoruz. değişiklik olursa select fonksiyonundan çıkıp state'i 1 yapıyoruz
        if (FD_ISSET(_serverFd, &_readFdsSup)) //burada serverFd'nin readFdsSup'da olup olmadığını kontrol ediyoruz
        {
            acceptRequest();
            state = 0; 
            continue;
        }
        if (state) 
        {
            readEvent(&state);
            if (state == 0)
                continue;
        }
        if (state) 
        {
            writeEvent();
            state = 0;
            continue;
        }
        
    }
}

void Server::manageServer(size_t const& port, std::string const& password)
{
    setPort(port); //burada portu set ediyoruz
    setPassword(password); //burada passwordu set ediyoruz
    initCommands(); //burada komutları initialize ediyoruz
    createSocket(); //burada socketi oluşturuyoruz
    bindSocket(port);   //burada socketi portla bağlıyoruz
    printStatus();  //burada serverın statusunu yazdırıyoruz
    run();  //burada serverı çalıştırıyoruz
}
