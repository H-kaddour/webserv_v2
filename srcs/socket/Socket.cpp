#include "../../include/Socket.hpp"

Socket::Socket(void) {
	//this->input = this->buff;
}

Socket::Socket(const Socket &obj) {
	*this = obj;
}

Socket & Socket::operator=(const Socket &obj) {
	(void)obj;
	return (*this);
}

void	Socket::set_servers(std::vector<Server*> &servers) {
	this->servers = servers;
}

void	Socket::open_sockets(void) {
	//here i just use a small server to work in response but later on i will use the real server
	int	opt = 1;
	try {
		this->sfd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->sfd < 0)
			throw("Error: socket cant't open.");
		memset(&this->serv, 0, sizeof(this->serv));
		this->serv.sin_family = AF_INET;
		this->serv.sin_port = htons(2222);
		inet_pton(AF_INET, "localhost", &this->serv.sin_addr);
		if (setsockopt(this->sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw("Error: setsockopt can't set the socket.");
		if (bind(this->sfd, (struct sockaddr *)&this->serv, sizeof(this->serv)))
			throw("Error: can't bind this socket to this address and port.");
		if (listen(this->sfd, BACKLOG))
			throw("Error: this socket can't no set to listing mode.");
		socklen_t size = sizeof(cli);
		//while (1)
		//{
			std::cout << "Server waiting...." << std::endl;
			this->cnx = accept(this->sfd, (struct sockaddr *)&this->cli, &size);
			if (this->cnx < 0)
				throw ("Error: the connection can not accept.");
			std::cout << "Connection accept...." << std::endl;
			//bzero(&this->poll_fd, sizeof(this->poll_fd));
			//this->poll_fd.fd = this->cnx;
			//this->poll_fd.events = POLLIN;
			//if (poll(&this->poll_fd, 1, -1) < 0)
			//	throw ("Error: poll error.");

			bzero(this->buff, 5000);
			read(this->cnx, this->buff, 5000);
			//here i will parse the request
			//those setter i will put them in the constructer
			this->request.set_input(this->buff);
			this->request.set_fd(this->cnx);
			this->request.set_server(*this->servers.begin());
			this->request.parse_request();

			//std::cout << this->input;
			//this->input.clear();
			//bzero(this->buff, 5000);

			//int	d;
			//int	i = 0;
			//while (i < 10)
			//{
			//	d = read(this->cnx, &this->buff, 1);
			//	printf("i = %d || read = %d || char = %c\n", i, d, this->buff);
			//	if (this->buff == 0)
			//		exit(1);
			//	i++;
			//}
			//std::cout << this->buff << std::endl;
			//while ((d = read(this->cnx, &this->buff, 1)) > 0)
			//{
			//	std::cout << d << std::endl;
			//	std::string c = &this->buff;
			//	this->input.append(c);
			//	std::cout << this->input << std::endl;;
			//}
			//std::cout << this->input;
			close(this->cnx);
		//}
		close(this->sfd);
	}
	catch (const char *error) {
		std::cout << error << std::endl;
		exit(1);
	}
}

Socket::~Socket() {

}
