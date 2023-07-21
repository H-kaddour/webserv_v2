#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sstream>
#include "Server.hpp"
#define URI "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="

class Request {
	protected :
		//attributes of Request
		std::string	input;
		int	fd;
		//
		std::string	method;
		std::string uri;
		std::string	req_http;
		std::map<std::string, std::string>	req_headers;
		std::string	req_body;

		//attributes of Response
		std::string	res_http;
		std::pair<std::string, std::string>	res_status;
		std::map<std::string, std::string>	res_header;
		std::string	res_body;

		std::string	response;

		//this one holds all status code with description
		std::map<std::string, std::string>	status_code_des;

		//server where data is
		Server	*server;
	public :
		Request(void);
		Request(const Request &obj);
		Request &operator=(const Request &obj);
		//func
		std::vector<std::string>	split(std::string input, char sp);
		void	parse_request(void);
		std::string	response_status_line(void);
		std::string	response_header(void);
		std::string	response_msg_body(void);
		void	parse_request_line(void);
		void	parse_header(void);
		void	parse_body(void);
		bool	check_uri_character(char c);
		void	send_the_request(std::string status);
		void	check_location_if_exist(void);
		std::string	substr(std::string path, char sp);
		//setters
		void	set_input(std::string input);
		void	set_fd(int fd);
		void	set_res_status(std::string status);
		void	set_server(Server *serv);
		void	check_header_variables(void);
		void	set_res_body(void);
		void	set_res_header(void);
		//getter
		~Request(void);
};
//    GET   /   HTTP/1.1  \r\n
