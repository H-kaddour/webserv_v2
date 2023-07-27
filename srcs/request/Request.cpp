#include "../../include/Request.hpp"

Request::Request(void) {
	this->res_http = "HTTP/1.1";
	this->res_header.insert(std::make_pair("Server", "webserv"));
	//maybe here i will add the date
	//this->res_header.insert(std::make_pair("Date:", ""));
	this->res_header.insert(std::make_pair("Content-Type", ""));
	this->res_header.insert(std::make_pair("Content-Length", ""));
	this->res_header.insert(std::make_pair("Connection", ""));

	//map of all status_code
	std::vector<std::string> sp_status;
	std::string	status = "200OK 201Created 204No_Content 301Moved_Permanently 400Bad_Request 403Forbidden 404Not_Found 405Method_Not_Allowed 409Conflict 413Content_Too_Large 414URI_Too_Long 500Internal_Server_Error 501Not_Implemented";

	sp_status = this->split(status, ' ');
	unsigned long	find;
	for (std::vector<std::string>::iterator	itr = sp_status.begin(); itr != sp_status.end(); itr++) {
		std::pair<std::string, std::string> hld((*itr).substr(0, 3), (*itr).substr(3, (*itr).length()));
		while ((find = hld.second.find('_')) != std::string::npos)
			hld.second.at(find) = ' ';
		this->status_code_des.insert(hld);
	}
}

Request::Request(const Request &obj) {
	*this = obj;
}

Request &Request::operator=(const Request &obj) {
	(void)obj;
	return *this;
}

void	Request::set_input(std::string input) {
	this->input = input;
}

void	Request::set_fd(int fd) {
	this->fd = fd;
}

std::vector<std::string>	Request::split(std::string input, char sp) {
	std::vector<std::string>	header;

	for (unsigned long i = input.find(sp); input.length(); i = input.find(sp)) {
		if (i == std::string::npos) {
			header.push_back(input.substr(0, input.length()));
			break ;
		}
		header.push_back(input.substr(0, i));
		while (input.at(i) == sp)
			i++;
		input.erase(0, i);
	}
	return header;
}

std::string	Request::response_status_line(void) {
	return res_http + " " + res_status.first + " " + res_status.second + "\r\n";
}

std::string	Request::response_header(void) {
	std::map<std::string, std::string>::iterator	itr;
	std::string	header;

	for (itr = this->res_header.begin(); itr != this->res_header.end(); itr++) {
		header.append(itr->first);
		header.append(": ");
		header.append(itr->second);
		header.append("\r\n");
	}
	return header;
}

std::string	Request::response_msg_body(void) {
	std::string	body = "\r\n";
	body.append(this->res_body);
	return body;
}

void	Request::set_server(Server *serv) {
	this->server = serv;
}

void	Request::set_res_status(std::string status) {
	this->res_status = *this->status_code_des.find(status);
}

bool	Request::check_uri_character(char c) {
	std::string	allow = URI;
	if (allow.find(c) == std::string::npos)
		return 1;
	return 0;
}

void	Request::parse_request_line(void) {
	std::vector<std::string>	req;
	unsigned long	pos;
	std::string	hld;

	//try {
		pos = this->input.find('\n');
		hld = this->input.substr(0, pos);
		this->input.erase(0, pos + 1);
		if (hld.back() == '\r')
			hld.erase(hld.length() - 1);
		while (hld.back() == ' ')
			hld.erase(hld.length() - 1);
		if (hld.at(0) == ' ')
			throw "400";
		req = this->split(hld, ' ');
		if (req.size() > 3)
			throw "400";
		this->method = req.at(0);
		//here take off the query ?ll=lll
		this->uri = req.at(1);
		this->req_http = req.at(2);
		//check if method is correct
		if (this->method.compare("GET") && this->method.compare("POST") && this->method.compare("DELETE"))
			throw "400";
		//here check url uncoding if character is right
		if (this->uri.front() != '/')
			throw "400";
		if (this->uri.length() > 2048)
			throw "414";
		//here also check ? and save the query of the uri
		for (std::string::iterator itr = this->uri.begin(); itr != this->uri.end(); itr++) {
			if (check_uri_character(*itr))
				throw "400";
		}
		if (this->req_http.compare(this->res_http))
			throw "400";
		//return *this->status_code_des.find("200");
		//this->set_res_status(status_code);
	//}
	//catch (const char *status) {
	//	this->res_status = *this->status_code_des.find(status);
	//	std::cout << this->res_status.first << this->res_status.second << std::endl;
	//}
}

void	Request::check_header_variables(void) {
	std::map<std::string, std::string>::iterator	itr = this->req_headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator	itr2 = this->req_headers.find("Content-Length");
	if (itr != this->req_headers.end() && itr->second.compare("chuncked"))
		throw "501";
	if (!this->method.compare("POST") && \
			(itr == this->req_headers.end() || itr2 == this->req_headers.end()))
		throw "400";
	//check the uri chracter if allowed and check length of uri
	//check if request body large than client max body
}

void	Request::parse_header(void) {
	std::vector<std::string>	header;
	std::string	hld;
	int	check = 0;
	unsigned long find;

	//try {
		for (find = this->input.find('\n'); !this->input.empty(); find = this->input.find('\n')) {
			hld = this->input.substr(0, find + 1);
			if ((hld.length() == 2 && hld.at(0) == '\r') || hld.length() == 1) {
				this->input.erase(0, find + 1);
				check = 1;
				break ;
			}
			header.push_back(hld);
			this->input.erase(0, find + 1);
		}
		if (!check)
			throw "400";
		for (std::vector<std::string>::iterator	itr = header.begin(); itr != header.end(); itr++) {
			find = itr->find(':');
			if (find == std::string::npos)
				throw "400";
			itr->erase(itr->length() - 1);
			if (itr->back() == '\r')
				itr->erase(itr->length() - 1);
			std::pair<std::string, std::string>	var(itr->substr(0, find), itr->substr(find + 1, itr->length() - 1));
			for (std::string::iterator i = var.first.begin(); i != var.first.end(); i++) {
				if (isspace(*i))
					throw "400";
			}
			this->req_headers.insert(var);
		}
		//here check
		this->check_header_variables();
		//throw "200";
	//}
	//catch (const char *status) {
	//	this->res_status = *this->status_code_des.find(status);
	//	std::cout << this->res_status.first << this->res_status.second << std::endl;
	//}
}

void	Request::parse_body(void) {
	this->req_body = this->input;
	//if (!this->method.compare("Post") && this->req_body.length() > atoi(server->get_client_max_body_size().c_str()))
	if (static_cast<int>(this->req_body.length()) > atoi(server->get_client_max_body_size().c_str()))
		throw "414";
}

void	Request::set_res_body(void) {
	std::map<int, std::string> page = this->server->get_status_page();
	std::map<int, std::string>::iterator itr = page.find(atoi(this->res_status.first.c_str()));
	std::ifstream in;
	//char c;
	in.open(itr->second);
	//here error those one check them in the parsing
	if (!in.is_open())
		exit(1);
	std::getline(in, this->res_body, '\0');
	//for (int rd = read(in, &c, 1); rd; rd = read(this->fd, &c, 1)) {
	//	this->res_body.push_back(c);
		//this->res_body.append("djjk");
	//}
}

void	Request::set_res_header(void) {
	std::map<std::string, std::string>::iterator itr = this->res_header.find("Content-Length");
	std::stringstream strm;
	strm << this->res_body.length();
	itr->second = strm.str();
	itr = this->res_header.find("Content-Type");
	//here i have to search it in mime_types
	itr->second = "text/html";
	itr = this->res_header.find("Connection");
	itr->second = "close";
}

void	Request::send_the_request(std::string status) {
	//i will make a path var holds what file will open to read the body
	this->set_res_status(status);
	this->set_res_body();
	this->set_res_header();
	this->response.append(this->response_status_line());
	this->response.append(this->response_header());
	this->response.append(this->response_msg_body());
	write(this->fd, this->response.c_str(), this->response.length());
	//send it and clear the var
}

std::string	Request::substr(std::string path, char sp) {
	for (unsigned long i = 0; path.at(i) == sp; i++)
		path.erase(i);
	for (unsigned long i = path.length() - 1; path.at(i) == sp; i--)
		path.erase(i);
	return (path);
}

void	Request::check_location_if_exist(void) {
	std::vector<std::string>	hld;

	hld = this->split(this->substr(this->uri, '/'), '/');

}

void	Request::parse_request(void) {
	try {
		//DIR *dir;

		//if (!(dir = opendir("/Users/hkaddour/////zbi")))
		//	std::cout << "Error: root directory not exists." << std::endl;
  	////closedir(dir);
		//exit(1);
		//throw static_cast<std::pair<std::string, std::string> >(*this->status_code_des.find("200"));
		this->parse_request_line();
		//this->response.append(this->response_status_line());
		this->parse_header();
		this->parse_body();
		this->check_location_if_exist();
		//
		//this->send_the_request("400");

		//here parse the header
		//std::vector<std::string>	req;
		//unsigned long	pos;
		//pos = this->input.find('\n');
		//std::string	hld = this->input.substr(0, pos);
		////here return the response of bad request
		//pos = hld.rfind('\r');
		//if (pos != std::string::npos)
		//	hld.erase(pos);
		//if (hld.at(0) == ' ' || hld.at(hld.length() - 1) == ' ')
		//	throw (this->status_code_des.find("404"));
		//req = this->split(hld, ' ');
		//if (req.size() > 3)
		//	throw (this->status_code_des.find("404"));
		//this->method = req.at(0);
		//this->uri = req.at(1);
		//this->req_http = req.at(2);
		//for (std::vector<std::string>::iterator	itr = req.begin(); itr != req.end(); itr++) {
		//	std::cout << "|" << *itr << "|" << std::endl;
		//}
	}
	//here i will make a map for each code status to send the response
	//catch (const char *error) {
	//catch (std::pair<std::string, std::string> status_code) {
	catch (const char *status) {
		//this->set_res_status(status_code);

		//this->set_res_status(status_code);
		//this->res_status = *this->status_code_des.find(status);
		this->send_the_request(status);
		//this->response.append(this->response_status_line());
		//std::cout << this->response << std::endl;
		//request.append(this->response_status_line());
		//request.append(this->response_header());
		//request.append(this->response_msg_body());
		//write(0, request.c_str(), request.length());
		//write(fd, request.c_str(), request.length());
		exit(1);
	}
}

Request::~Request(void) {

}


//Response
//HTTP/1.1 400 Bad Request
//Server: nginx/1.25.1
//Date: Fri, 14 Jul 2023 17:13:03 GMT
//Content-Type: text/html
//Content-Length: 157
//Connection: close


//Request
//GET / HTTP/1.1
//Host: localhost:8080
//User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:109.0) Gecko/20100101 Firefox/113.0
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
//Accept-Language: en-US,en;q=0.5
//Accept-Encoding: gzip, deflate
//Connection: keep-alive
//Upgrade-Insecure-Requests: 1
//
//
