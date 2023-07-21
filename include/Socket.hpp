#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include "Request.hpp"
#include <vector>
#include "Server.hpp"
#define BACKLOG 50


class Socket {
	struct sockaddr_in	serv;
	struct sockaddr_in	cli;
	int	sfd;
	int	cnx;
	char buff[5000];
	std::vector<Server*> servers;
	//this one i will make it like a vector so any request come read it
	Request	request;
	//struct pollfd	poll_fd;
	public :
		Socket(void);
		Socket(const Socket &obj);
		Socket &operator=(const Socket &obj);
		//func
		void	open_sockets(void);
		//setters
		void	set_servers(std::vector<Server*> &servers);
		~Socket(void);
};
