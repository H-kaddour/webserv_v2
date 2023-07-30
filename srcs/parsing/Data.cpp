#include "../../include/Data.hpp"

Data::Data() {
	//std::cout << "Data Default constructer" << std::endl;
}

Data::Data(const Data & obj)
{
	//std::cout << "Data obj constructer" << std::endl;
	*this = obj;
}

Data	& Data::operator=(const Data & obj)
{
	this->mime_types_parse = obj.mime_types_parse;
	for (int i = 0; i < static_cast<int>(obj.servers.size()); i++)
	{
		Server *server = new Server(*obj.servers[i]);
		this->servers.push_back(server);
	}
	return (*this);
}

//func
void	Data::loop_tst(std::string &request, int rd) {
	for (int i = 0; i != rd; i++) {
		request.push_back(this->buff[i]);
		//request.append(1, this->buff[i]);
	}
}

void	Data::open_sockets(void) {
	//here i just use a small server to work in response but later on i will use the real server
	int	opt = 1;
	int chk;
	std::string	request;
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

			//those one will be init 
			this->request.set_fd(this->cnx);
			this->request.set_server(*this->servers.begin());
			chk = false;

			while (1) {
				bzero(this->buff, KB);
				//request.clear();
				int	rd = read(this->cnx, this->buff, KB);
				//std::cout << rd << std::endl;
				//if (!rd) {
				//	//here get of delete don't gave content length
				//	//if (this->request.get_req_method().compare("POST"))
				//		break ;
				//	//else
				//	//	this->request.handle_request_and_response(chk);
				//}
				if (rd < 0)
					throw ("end of file");
				if (!rd)
					break ;
				//this->loop_tst(request, rd);
				//for (int i = 0; i != rd; i++) {
				//	request.push_back(this->buff[i]);
				//	//request.append(1, this->buff[i]);
				//}
				//std::cout << request;
				//while (1);
				//if (this->request.set_req_input(this->buff))
				this->request.set_req_input(buff, rd);
				request = this->request.get_req_input();
				//std::cout << request << std::endl;
				//while (1);
				if (!chk) {
					if ((request.find("\n\n") != request.npos) || \
							(request.find("\n\r\n") != request.npos))
						chk = 1;
					else
						continue ;
				}
				this->request.handle_request_and_response(chk);
				//this->request.parse_request();

			}

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

//maybe i will use the open_socket func only
void	Data::server_execution(void)
{
	//this one delete it in the destructer
	//first i need to make a vector or a deque of requests that have fd and everything
	//then init the vector and bind the sockets and init their poll struct
	//then open sockets and read from it
	this->open_sockets();
	//this->socket = new Socket;
	//this->socket->set_servers(this->servers);
	//this->socket->open_sockets();
}

//getter
std::vector<Server*> &Data::get_servers(void)
{
	return (this->servers);
}

Data::~Data()
{
	//std::cout << "Data Default destructer" << std::endl;
	std::vector<Server*>::iterator itr;
	for (itr = this->servers.begin(); itr != this->servers.end(); itr++)
		delete *itr;
}
