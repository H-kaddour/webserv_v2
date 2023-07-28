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
std::string Request::get_req_input(void) const {
	return this->req_input;
}

void	Request::set_req_input(char *buff, int size) {
	//this->req_input.append(input);
	//std::cout << buff << std::endl;
	for (int i = 0; i != size; i++) {
		this->req_input.push_back(buff[i]);
		//request.append(1, this->buff[i]);
	}
	//std::cout << "*********8" << std::endl;
	//std::cout << this->req_input << std::endl;
	//while (1);

	//if ((this->req_input.find("\n\n") != this->req_input.npos) || \
	//		(this->req_input.find("\n\r\n") != this->req_input.npos))
	//	return 1;
	//return 0;
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

	pos = this->req_input.find('\n');
	hld = this->req_input.substr(0, pos);
	this->req_input.erase(0, pos + 1);
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

	//here also check ? and save the query of the uri
	for (std::string::iterator itr = req.at(1).begin(); itr != req.at(1).end(); itr++) {
		if (check_uri_character(*itr))
			throw "400";
	}
	//here take off the query ?ll=lll
	//this->uri = req.at(1);
	pos = (req.at(1).find('?') != req.at(1).npos) ? req.at(1).find('?') : req.at(1).length();
	this->uri = req.at(1).substr(0, pos);
	//i need here another request uri so i can edit it 
	//this->hld_uri = req.at(1).substr(0, pos);
	req.at(1).erase(0, ((req.at(1)[pos] == '?') ? pos + 1 : pos));
	this->query = req.at(1).substr(0, req.at(1).length());

	this->req_http = req.at(2);
	//check if method is correct
	if (this->method.compare("GET") && this->method.compare("POST") && this->method.compare("DELETE"))
		throw "400";
	//here check url uncoding if character is right
	if (this->uri.front() != '/')
		throw "400";
	if (this->uri.length() > 2048)
		throw "414";
	if (this->req_http.compare(this->res_http))
		throw "400";
	//return *this->status_code_des.find("200");
	//this->set_res_status(status_code);
}

void	Request::check_header_variables(void) {
	std::map<std::string, std::string>::iterator	itr = this->req_headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator	itr2 = this->req_headers.find("Content-Length");
	if (itr != this->req_headers.end() && itr->second.compare("chuncked"))
		throw "501";
	if (!this->method.compare("POST") && itr2 == this->req_headers.end())
		throw "400";
	//check the uri chracter if allowed and check length of uri
	//check if request body large than client max body
}

std::string	Request::substr_sp(std::string path, char sp) {
	size_t	i;
	size_t	j;

	for (i = 0; (i < (path.length() -1) && path.at(i) == sp);)
		i++;
	for (j = (path.length() - 1); ( (path.length() -1) && path.at(j) == sp);)
		j--;
	return (path.substr(i, (j - i) + 1));
}

void	Request::parse_header(void) {
	std::vector<std::string>	header;
	std::string	hld;
	int	check = 0;
	unsigned long find;

	for (find = this->req_input.find('\n'); !this->req_input.empty(); find = this->req_input.find('\n')) {
		hld = this->req_input.substr(0, find + 1);
		if ((hld.length() == 2 && hld.at(0) == '\r') || hld.length() == 1) {
			this->req_input.erase(0, find + 1);
			check = 1;
			break ;
		}
		header.push_back(hld);
		this->req_input.erase(0, find + 1);
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
		//std::pair<std::string, std::string>	var(itr->substr(0, find), itr->substr(find + 1, itr->length() - 1));
		std::pair<std::string, std::string>	var(itr->substr(0, find), \
				this->substr_sp(itr->substr(find + 1, itr->length() - 1), ' '));
		for (std::string::iterator i = var.first.begin(); i != var.first.end(); i++) {
			if (isspace(*i))
				throw "400";
		}
		this->req_headers.insert(var);
	}
	//here check
	this->check_header_variables();
}

bool	Request::parse_body(void) {
	//here check if content length is int and give it to a long
	std::map<std::string, std::string>::iterator itr = this->req_headers.find("Content-Length");
	this->req_body.append(this->req_input);
	//this clear should not be in here
	this->req_input.clear();

	//if (!this->method.compare("Post") && this->req_body.length() > atoi(server->get_client_max_body_size().c_str()))

	//this one check client_max_body_size of server and location
	//if (static_cast<int>(this->req_body.length()) > atoi(server->get_client_max_body_size().c_str()))
	//	throw "413";

	//if (itr == this->req_headers.end()) {
	//	if (!this->req_input.length())
	//		return true;
	//}
	if (this->req_body.length() == static_cast<size_t>(atol(itr->second.c_str())))
		return true;
	return false;
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

void	Request::fix_uri_slashes(std::string right_uri, std::string &uri) {
	std::vector<std::string>	hld;
	int	i = 0;

	hld = this->split(this->substr_sp(right_uri, '/'), '/');
	//std::cout << this->uri << std::endl;
	for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++, i++) {
		if (!i && *right_uri.begin() == '/')
			uri.append("/");
		uri.append(*itr);
	}
	if (right_uri.back() == '/')
		uri.append("/");
}

void	Request::check_location_if_exist(void) {
	std::vector<std::string>	hld;

	hld = this->split(this->substr_sp(this->uri, '/'), '/');
	std::cout << this->uri << std::endl;
	for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++) {
		this->hld_uri.append("/");
		this->hld_uri.append(*itr);
	}
	if (this->uri.back() == '/')
		this->hld_uri.append("/");

	this->fix_uri_slashes(this->hld_uri, this->uri);

	//here search for the location path
	for (std::map<std::string, Location*>::iterator itr = this->server->get_location().begin(); \
			itr != this->server->get_location().end(); itr++) {
	}
	//std::cout << this->hld_uri << std::endl;
	//while (1);
	
}

void	Request::handle_request_and_response(int &chk) {
	try {
		//DIR *dir;

		//if (!(dir = opendir("/Users/hkaddour/////zbi")))
		//	std::cout << "Error: root directory not exists." << std::endl;
  	////closedir(dir);
		//exit(1);
		//throw static_cast<std::pair<std::string, std::string> >(*this->status_code_des.find("200"));

		//if (this->request.set_req_input(buff)) {

		//}
		if (chk == 1) {
			this->parse_request_line();
			this->parse_header();
			chk++;
		}
		if (!this->parse_body())
			return ;
		this->check_location_if_exist();

		//
		//this->send_the_request("400");

		//here parse the header
		//std::vector<std::string>	req;
		//unsigned long	pos;
		//pos = this->req_input.find('\n');
		//std::string	hld = this->req_input.substr(0, pos);
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
	//also check the parsing part like if status pages not exist or something
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
