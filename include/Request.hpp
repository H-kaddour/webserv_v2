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
#include <sys/stat.h>
#include <sstream>
#include <time.h>
#include "Server.hpp"
#define URI "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="

class Request {
	protected :
		//attributes of Request
		std::string	req_input;
		int	fd;
		//
		std::string	method;
		std::string uri;
		std::string	req_http;
		std::map<std::string, std::string>	req_headers;
		std::string	req_body;
		std::string	query;
		//i need here another request uri so i can edit it 
		//this one to put the location and server data in
		Default_serv	*location_data;
		//this normalize the uri if uri == ///okk// turn to /okk/
		std::string	normalize_uri;
		//std::string	hld_uri;

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


		//this one holds root + the uri
		//std::string root_uri;
		std::string path_root_loc;

		//this is the path of the file i will read in response body
		std::string	file_path_body;

		//this variable check if the uri have a location or not
		bool	if_has_loc;
	public :
		Request(void);
		Request(const Request &obj);
		Request &operator=(const Request &obj);
		//func
		std::vector<std::string>	split(std::string input, char sp);
		//void	parse_request(void);
		void	handle_request_and_response(int &chk);
		std::string	response_status_line(void);
		std::string	response_header(void);
		std::string	response_msg_body(void);
		void	parse_request_line(void);
		void	parse_header(void);
		bool	parse_body(void);
		bool	check_uri_character(char c);
		void	send_the_response(std::string status);
		void	check_location_and_dir_and_red(void);
		std::string	substr_sp(std::string path, char sp);
		void	fix_uri_slashes(std::string right_uri, std::string &uri);
		std::string	fix_location_slashes(std::string location);
		void	check_location_redirection(void);
		void	check_if_method_allowed(void);
		void	get_method(void);
		void	post_method(void);
		void	delete_method(void);
		bool	check_if_dir_has_index(void);
		bool	check_if_loc_has_cgi(void);
		void	auto_index_page_maker(void);
		//setters
		void	set_req_input(char *buff, int size);
		void	set_fd(int fd);
		void	set_res_status(std::string status);
		void	set_server(Server *serv);
		void	check_header_variables(void);
		void	set_res_body(void);
		void	set_res_header(void);
		//getter
		std::string get_req_input(void) const;
		~Request(void);
};
//    GET   /   HTTP/1.1  \r\n
