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

	//if ((this->req_input.find("\n\n") != this->req_input.npos) ||
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
	std::string::iterator	itr;

	//here maybe check
	for (itr = path.begin(); path.length() && *itr == sp; itr = path.begin())
		path.erase(itr);
	for (itr = path.end() - 1; path.length() && *itr == sp; itr = path.end() - 1)
		path.erase(itr);
	//return (path.substr(i, (j - i) + 1));
	return path;
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
		//maybe i shouldn't check this one
		for (std::string::iterator itr = var.first.begin(); itr != var.first.end(); itr++) {
			if (isspace(*itr))
				throw "400";
		}
		this->req_headers.insert(var);
	}
	//here check
	this->check_header_variables();
}

bool	Request::parse_body(void) {
	//here check if content length is int and give it to a long

	//std::map<std::string, std::string>::iterator itr = this->req_headers.find("Content-Length");
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

	//comment for now
	//if (this->req_body.length() == static_cast<size_t>(atol(itr->second.c_str())))
	//	return true;
	//return false;

	return true;
}

void	Request::set_res_body(void) {


	//here i should open the file_path file not this and i should check if the body is empty
	if (this->res_body.size())
		return ;
	//std::map<int, std::string> page = this->server->get_status_page();
	//
	//fix this one in header and request line this->location_data don't get set
	//std::map<int, std::string> page = this->location_data->get_status_page();
	//std::map<int, std::string>::iterator itr = page.find(atoi(this->res_status.first.c_str()));
	std::map<int, std::string> page = this->server->get_status_page();
	std::string	file = page.find(atoi(this->res_status.first.c_str()))->second;
	if (file_path_body.length())
		file = this->file_path_body;
	std::ifstream in;
	//char c;
	in.open(file);
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

void	Request::send_the_response(std::string status) {
	//i will make a path var holds what file will open to read the body
	this->set_res_status(status);
	this->set_res_body();
	this->set_res_header();
	this->response.append(this->response_status_line());
	this->response.append(this->response_header());
	this->response.append(this->response_msg_body());
	//std::cout << this->response << std::endl;
	write(this->fd, this->response.c_str(), this->response.length());
	//send it and clear the var
}

void	Request::fix_uri_slashes(std::string src, std::string &dest) {
	std::vector<std::string>	hld;
	int	i = 0;

	hld = this->split(this->substr_sp(src, '/'), '/');
	//std::cout << this->uri << std::endl;
	for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++, i++) {
		dest.append("/");
		if (!i && src.front() != '/')
			dest.erase(0);
		dest.append(*itr);
	}
	if (src.back() == '/')
		dest.append("/");
}

std::string	Request::fix_location_slashes(std::string location) {
	//size_t	i;
	//for (i = 0; location[i] && location[i] == '/'; i++)
	//	;
	//if (i == location.length())
	//	return "/";

	//int chk = 0;
	//if (location[location.length() - 1] == '/')
	//	chk = 1;
	for (int i = location.length() - 1; location.length() && location[i] == '/'; i = location.length() - 1)
		location.erase(i);
	if (!location.length())
		return "/";
	//if (chk)
	//	location.push_back('/');
	return location;
}

//void	Request::check_location_or_dir_if_exist(void) {
void	Request::check_location_and_dir_and_red(void) {
	//std::vector<std::string>	hld;

	//hld = this->split(this->substr_sp(this->uri, '/'), '/');
	//std::cout << this->uri << std::endl;
	//for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++) {
	//	this->hld_uri.append("/");
	//	this->hld_uri.append(*itr);
	//}
	//if (this->uri.back() == '/')
	//	this->hld_uri.append("/");

	size_t	i;
	std::string	hld;
	std::string hld_loc;
	struct stat stt;
	bool	chk = false;
	for (i = 0; this->uri[i] && this->uri[i] == '/'; i++)
		;
	if (i == this->uri.length())
		this->hld_uri = "/";
	else
	{
		this->fix_uri_slashes(this->uri, this->hld_uri);
		if (this->hld_uri[this->hld_uri.length() - 1] == '/')
			hld = this->hld_uri.substr(0, this->hld_uri.length() - 1);
		else
			hld = this->hld_uri;
	}
	//std::cout << hld << std::endl;

	//here search for the location path
	std::map<std::string, Location*>::iterator itr;
	for (itr = this->server->get_location().begin(); itr != this->server->get_location().end(); itr++) {
		hld_loc = fix_location_slashes(itr->first);
		//std::cout << "* "<< hld_loc << std::endl;
		if (!hld_loc.compare(hld)) {
			chk = true;
			this->location_data = new Default_serv(*this->server, *itr->second);
			//this->root_uri = this->location_data->get_root() + hld;
			//this->check_location_redirection();
			//return ;
		}
	}
	if (!chk)
		this->location_data = new Default_serv(*this->server);
	//this->root_uri = this->location_data->get_root() + hld;
	this->root_uri = this->location_data->get_root() + this->hld_uri;

	//std::cout << this->root_uri << std::endl;
	if (stat(this->root_uri.c_str(), &stt) == -1) {
		if (chk)
			this->check_location_redirection();
		else
			throw "404";
	}
}

void	Request::check_location_redirection(void) {
	//only 301 can be in return
	if (!this->location_data->get_retur().size())
		throw "404";
	this->res_header.insert(std::make_pair("Location", this->location_data->get_retur().begin()->second));
	throw "301";
}

void	Request::check_if_method_allowed(void) {
	//maybe i should not have GET method by default
	//if (!this->location_data->get_allow_method().size())
	//	throw "405";
	std::vector<std::string> hld = this->location_data->get_allow_methods();
	//for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++) {
	//	std::cout << *itr << std::endl;
	//}
	//std::cout << "********" << std::endl;
	if (std::find(hld.begin(), hld.end(), this->method) == hld.end())
		throw "405";
	//std::cout << "method allowed" << std::endl;

	//if (this->method.compare())
}

bool	Request::check_if_dir_has_index(void) {
	//DIR *dir;
	//struct dirent *opn;

	//std::cout << "** " << this->root_uri << std::endl;
	//std::cout << "! " << this->root_uri + *this->location_data->get_index().begin() << std::endl;
	std::vector<std::string>	index_sp;
	std::vector<std::string>	index_path_sp;
	std::string index_path;
	struct stat stt;
	std::vector<std::string>	all_index = this->location_data->get_index();
	for (std::vector<std::string>::iterator index = all_index.begin(); index != all_index.end(); index++) {
		//std::cout << *itr << std::endl;
		index_sp = this->split(this->substr_sp(*index, '/'), '/');
		if (!index_sp.size() || index->back() == '/')
				continue ;
		//	/index.html
		//	../index.html
		index_path = (index->front() == '/' ? this->location_data->get_root() : this->root_uri);
		// here check if index start with / and if the index have root inside it

		//std::cout << "path = " << (index_path + *index) << std::endl;
		if (stat((index_path + *index).c_str(), &stt) == -1)
			continue ;

		if (S_ISDIR(stt.st_mode))
			continue ;
		//else dayz accept socket and other

		size_t size = index_path.length();
		//std::cout << uri_index << std::endl;
		index_path_sp = this->split(this->substr_sp(index_path, '/'), '/');
		//check if root was only /
		//exit(1);
		for (std::vector<std::string>::iterator sp_itr = index_sp.begin(); \
				sp_itr != index_sp.end() ; sp_itr++) {
			//here check if i was only poppin ../../../../ also check / only in the root
			//(!sp_itr->compare("..") ? (index_path_sp.pop_back()) : index_path_sp.push_back(*sp_itr));
			if (!sp_itr->compare("..")) {
				if (index_path_sp.size())
					index_path_sp.pop_back();
			}
			else
				index_path_sp.push_back(*sp_itr);
		}
		index_path.clear();
		//here check if size is 0
		for (size_t i = 0; i != index_path_sp.size(); i++) {
			//here check if root start with /
			index_path.append("/");
			if (!i && this->root_uri.front() != '/')
				index_path.erase(index_path.begin());
			index_path.append(index_path_sp[i]);
		}
		//this one still have error have to be fix asap maybe it's right
		if (index->front() != '/' && index_path.compare(0, size, this->root_uri, 0, size))
			continue ;
		//keep the path of the index file in a variable so you can read it in the path body response
		this->file_path_body = index_path;
		//std::cout << this->file_path_body << std::endl;
		return true;

		//if (index->front() != '/' && index_path.compare(0, size, this->root_uri, 0, size))
		//	continue ;

		//std::fstream	in;

		//in.open(index_path, std::fstream::in);
		//if (in.is_open())
		//	return (true);

		//uri_index.erase(uri_index.end() - 1);

		//std::cout << uri_index << std::endl;
		//exit(1);

		//for (size_t i = 0; i != hld.size(); i++) {
		//	std::cout << "	" << hld[i] << std::endl;
		//}

	}
	//here i should throw an exception
	//while (1);
	
	//close dir
	//dir = opendir(this->root_uri.c_str());
	//while ((opn = readdir(dir))) {
	//	if (opn->d_type == DT_DIR)
	//		std::cout << "dir = " << opn->d_name << std::endl;
	//	else if (opn->d_type == DT_REG)
	//		std::cout << "file = " << opn->d_name << std::endl;
	//}
	//if (!dir)
	//	std::cout << "error "<< std::endl;

	return false;
}

bool	Request::check_if_loc_has_cgi(void) {
	std::string	file;
	std::string	extension;
	std::vector<std::pair<std::string, std::string> > cgi = this->location_data->get_cgi_info();
	std::vector<std::pair<std::string, std::string> >::iterator itr;
	size_t find;

	if (cgi.size()) {
		file = this->file_path_body.substr(\
				this->file_path_body.rfind('/') + 1, this->file_path_body.length() - 1);
		//std::cout << extension << std::endl;
		if ((find = file.find('.')) == file.npos)
			return false;
		extension = file.substr(find, file.length() - 1);
		//std::cout << extension << std::endl;
		for (itr = cgi.begin(); itr != cgi.end(); itr++) {
			if (!itr->first.compare(extension))
				return true;
		}
	}
	return false;
}

void	Request::get_method(void) {
	struct stat	stt;
	struct stat	stt1;
	DIR	*dir;
	struct dirent *file;


	std::cout << "* " << this->root_uri << std::endl;
	stat(this->root_uri.c_str(), &stt);
	//std::cout << "get method: " << this->root_uri << std::endl;
	if (S_ISDIR(stt.st_mode)) {
		std::cout << "dir" << std::endl;
		//std::cout << "dir ** " << this->hld_uri << std::endl;
		if (this->hld_uri.back() != '/') {
			this->res_header.insert(std::make_pair("Location", this->hld_uri + "/"));
			throw "301";
		}
		//this fucntion need more test
		if (this->check_if_dir_has_index()) {
			//this->res_body += "<?php\r\n";
			//this->res_body += "phpinfo()\r\n";
			//throw "200";
			//std::string l = "hey.pyy";
			//if (l.find(".py") != l.npos) {
			//	std::cout << "true" <<  std::endl;
			//}
			//else {
			//	std::cout << "false" << std::endl;
			//}
			//std::cout << this->file_path_body << std::endl;
			if (this->check_if_loc_has_cgi()) {
				//here send it to CGI
				//std::cout << "index exists" << std::endl;
			}
			else {
				//std::map<int, std::string> page = this->server->get_status_page();
				//std::map<int, std::string>::iterator itr = page.find(200);
				//itr->second = this->file_path_body;
				//std::cout << "ll" << std::endl;
				throw "200";
				std::cout << "index doesn't exists" << std::endl;
			}
		}
		else {
			if (!this->location_data->get_autoindex())
				throw "403";
			//check if empty means the body is full if not read the file
			//this->file_path_body.clear();
			//std::cout << this->root_uri << std::endl;
			dir = opendir(this->root_uri.c_str());
			this->res_body.append("<!DOCTYPE html>\r\n");
			this->res_body.append("<html>\r\n");
			//this->res_body.append("<head>Index of </head>\r\n");
			this->res_body += "<head><title>Index of " + this->hld_uri + "</title></head>\r\n";
			this->res_body.append("<body>\r\n");
			this->res_body += "<h1>Index of " + this->hld_uri + "</h1>\r\n";
			this->res_body += "<pre class=\"tab\">\r\n";
			while ((file = readdir(dir))) {
				std::string hld = file->d_name;
				if (file->d_type == DT_DIR)
					hld.push_back('/');
				stat((this->root_uri + file->d_name).c_str(), &stt1);
				this->res_body += "<a href=\"" + hld + "\">" + hld + "</a>";
				this->res_body += "				";
				hld = ctime(&stt1.st_mtimespec.tv_sec);
				hld.erase(hld.end() - 1);
				this->res_body += hld;
				this->res_body += "				";
				this->res_body += std::to_string(stt1.st_size);
				this->res_body.append("\r\n");
			}
			this->res_body += "</pre>\r\n";
			this->res_body += "<hr>";
			this->res_body += "<center>webserv/1.0.0</center>";
			this->res_body.append("</body>\r\n");
			this->res_body.append("</html>\r\n");
			//std::cout << this->res_body;
			//while (1);
			throw "200";
		}
	}
	//here it will take symbolic link and more
	else if (S_ISREG(stt.st_mode)) {
		this->file_path_body = this->root_uri;
		std::cout << this->file_path_body << std::endl;
		throw "200";
		std::cout << "file ** " << this->hld_uri << std::endl;
	}
	//in this case if it was a non file or dir throw an error
	//here sock and other
	else {
		//maybe here throw 500 ineternal server error
		std::cout << "something else ** " << this->hld_uri << std::endl;
	}
}

void	Request::post_method(void) {

}

void	Request::delete_method(void) {

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
		//std::cout << "ll" << std::endl;
		this->check_location_and_dir_and_red();
		//check the redirection in up
		//this->check_location_redirection();
		this->check_if_method_allowed();

		//typedef	

		if (!this->method.compare("GET"))
			this->get_method();
		else if (!this->method.compare("POST"))
			this->post_method();
		else if (!this->method.compare("DELETE"))
			this->delete_method();

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
		this->send_the_response(status);
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
	//here i have to delete location_data
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
