#pragma once

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <algorithm>
#include <vector>
#include <map>
#include "Request.hpp"
#include "Server.hpp"
#define BACKLOG 50
#define KB 1024

class Data
{
	protected :
		std::map<std::string, std::string>	mime_types_parse;
		std::vector<Server*> servers;
		std::vector<Server*>::iterator	servers_itr;
		//this variables for socket
		//Socket	*socket;
		struct sockaddr_in	serv;
		struct sockaddr_in	cli;
		int	sfd;
		int	cnx;
		char buff[KB];
		//this one should be a vector that holds all requests
		Request	request;
	public :
		Data();
		Data(const Data & obj);
		Data	& operator=(const Data & obj);
		//func
		void	server_execution(void);
		void	open_sockets(void);
		void	loop_tst(std::string &request, int rd);
		//getters
		std::vector<Server*> &get_servers(void);
		//setters
		~Data();
};
