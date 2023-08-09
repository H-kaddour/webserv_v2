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
	this->if_has_loc = false;
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
		index_path = (index->front() == '/' ? this->location_data->get_root() : this->path_root_loc);
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
			if (!i && this->path_root_loc.front() != '/')
				index_path.erase(index_path.begin());
			index_path.append(index_path_sp[i]);
		}
		//this one still have error have to be fix asap maybe it's right
		if (index->front() != '/' && index_path.compare(0, size, this->path_root_loc, 0, size))
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

void	Request::post_method(void) {

}

void	Request::delete_method(void) {

}

//utils
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

std::string	Request::substr_sp(std::string path, char sp) {
	std::string::iterator	itr;

	for (itr = path.begin(); path.length() && *itr == sp; itr = path.begin())
		path.erase(itr);
	for (itr = path.end() - 1; path.length() && *itr == sp; itr = path.end() - 1)
		path.erase(itr);
	return path;
}

//Parsing request
bool	Request::check_uri_character(char c) {
	std::string	allow = URI;
	if (allow.find(c) == std::string::npos)
		return true;
	return false;
}

void	Request::parse_request_line(void) {
	std::vector<std::string>	elem;
	unsigned long	pos;
	std::string	request_line;

	pos = this->req_input.find('\n');
	request_line = this->req_input.substr(0, pos);
	this->req_input.erase(0, pos + 1);
	if (request_line.back() == '\r')
		request_line.erase(request_line.length() - 1);
	while (request_line.back() == ' ')
		request_line.erase(request_line.length() - 1);
	if (request_line.at(0) == ' ')
		throw "400";
	elem = this->split(request_line, ' ');
	if (elem.size() > 3)
		throw "400";
	this->method = elem.at(0);
	for (std::string::iterator itr = elem.at(1).begin(); itr != elem.at(1).end(); itr++) {
		if (check_uri_character(*itr))
			throw "400";
	}
	pos = (elem.at(1).find('?') != elem.at(1).npos) ? elem.at(1).find('?') : elem.at(1).length();
	this->uri = elem.at(1).substr(0, pos);
	elem.at(1).erase(0, ((elem.at(1)[pos] == '?') ? pos + 1 : pos));
	this->query = elem.at(1).substr(0, elem.at(1).length());
	this->req_http = elem.at(2);
	if (this->method.compare("GET") && this->method.compare("POST") && this->method.compare("DELETE"))
		throw "400";
	if (this->uri.front() != '/')
		throw "400";
	if (this->uri.length() > 2048)
		throw "414";
	if (this->req_http.compare(this->res_http))
		throw "400";
}

void	Request::check_header_variables(void) {
	std::map<std::string, std::string>::iterator	itr = this->req_headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator	itr2 = this->req_headers.find("Content-Length");
	if (itr != this->req_headers.end() && itr->second.compare("chuncked"))
		throw "501";
	if (!this->method.compare("POST") && itr2 == this->req_headers.end())
		throw "400";
	//check if request body large than client max body
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
		if (find == itr->npos)
			throw "400";
		itr->erase(itr->length() - 1);
		if (itr->back() == '\r')
			itr->erase(itr->length() - 1);
		std::pair<std::string, std::string>	var(itr->substr(0, find), \
				this->substr_sp(itr->substr(find + 1, itr->length() - 1), ' '));
		for (std::string::iterator itr = var.first.begin(); itr != var.first.end(); itr++) {
			if (isspace(*itr))
				throw "400";
		}
		this->req_headers.insert(var);
	}
	this->check_header_variables();
}

//this one will get fixed later
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

//Request uri checking if valid
void	Request::fix_uri_slashes(std::string src, std::string &dest) {
	std::vector<std::string>	hld;

	hld = this->split(this->substr_sp(src, '/'), '/');
	for (std::vector<std::string>::iterator itr = hld.begin(); itr != hld.end(); itr++) {
		dest.append("/");
		dest.append(*itr);
	}
	if (src.back() == '/')
		dest.append("/");
}

std::string	Request::fix_location_slashes(std::string location) {
	std::string hld = location;

	for (int i = location.length() - 1; location.length() && location[i] == '/'; i = location.length() - 1)
		location.erase(i);
	if (!location.length())
		return hld;
	return location;
}

void	Request::check_location_redirection(void) {
	//only 301 can be in return
	if (!this->location_data->get_retur().size())
		throw "404";
	//maybe this one should not throw 404 error only checks
	if (this->location_data->get_retur().begin()->first != 301)
		throw "404";
	this->res_header.insert(std::make_pair("Location", this->location_data->get_retur().begin()->second));
	throw "301";
}

void	Request::check_location_and_dir_and_red(void) {
	size_t	i;
	std::string	hld_uri;
	std::string hld_loc;
	struct stat stt;
	//bool	chk = false;

	for (i = 0; this->uri[i] && this->uri[i] == '/'; i++)
		;
	if (i == this->uri.length())
		this->normalize_uri = "/";
	else {
		this->fix_uri_slashes(this->uri, this->normalize_uri);
		if (this->normalize_uri[this->normalize_uri.length() - 1] == '/')
			hld_uri = this->normalize_uri.substr(0, this->normalize_uri.length() - 1);
		else
			hld_uri = this->normalize_uri;
	}
	//here search for the location path
	std::map<std::string, Location*>::iterator itr;
	for (itr = this->server->get_location().begin(); itr != this->server->get_location().end(); itr++) {
		hld_loc = fix_location_slashes(itr->first);
		if (hld_loc.front() != '/')
			hld_loc.insert(0, 1, '/');
		if (!hld_loc.compare(hld_uri)) {
			this->if_has_loc = true;
			this->location_data = new Default_serv(*this->server, *itr->second);
			//this one still need to take off / in the back
			this->path_root_loc = this->location_data->get_root() + this->normalize_uri;
			if (this->location_data->get_retur().size())
				this->check_location_redirection();
			//had function 7ta directories 5sni ndreb 7sabhom tsti nginx
			this->check_if_dir_has_index();
			//here when i find the location match i have to get index page to fix the uri i will check if exsists or not.
			break ;
			//return ;
			//if (this->normalize_uri.back() != '/')
			//	break ;
		}
	}
	if (!this->if_has_loc) {
		this->location_data = new Default_serv(*this->server);
		this->path_root_loc = this->location_data->get_root() + this->normalize_uri;
	}
	//here take off the last slash
	if (stat(this->path_root_loc.c_str(), &stt) == -1)
		throw "404";
	//std::cout << "path root loc= " << this->path_root_loc << std::endl;
	//maybe here take off the last /
	//std::cout << "done didn't find location" << std::endl;
}

void	Request::check_if_method_allowed(void) {
	//maybe i should not have GET method by default
	std::vector<std::string> hld = this->location_data->get_allow_methods();
	if (std::find(hld.begin(), hld.end(), this->method) == hld.end())
		throw "405";
}

//Methods handling
void	Request::auto_index_page_maker() {
	struct stat	stt1;
	struct dirent *file;
	DIR	*dir;

	dir = opendir(this->path_root_loc.c_str());
	this->res_body.append("<!DOCTYPE html>\r\n");
	this->res_body.append("<html>\r\n");
	this->res_body += "<head><title>Index of " + this->normalize_uri + "</title></head>\r\n";
	this->res_body.append("<body>\r\n");
	this->res_body += "<h1>Index of " + this->normalize_uri + "</h1>\r\n";
	this->res_body += "<pre class=\"tab\">\r\n";
	while ((file = readdir(dir))) {
		std::string hld = file->d_name;
		if (file->d_type == DT_DIR)
			hld.push_back('/');
		stat((this->path_root_loc + file->d_name).c_str(), &stt1);
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
	this->res_body += "<hr>\r\n";
	this->res_body += "<center>webserv/1.0.0</center>\r\n";
	this->res_body.append("</body>\r\n");
	this->res_body.append("</html>\r\n");
}

void	Request::get_method(void) {
	struct stat	stt;
	std::string hld_path;

	int chk;
	if (!this->if_has_loc) {
		//hld_path = this->path_root_loc;
		//if (hld_path.back() == '/')
		//	hld_path.erase(hld_path.length() - 1);
		//chk = stat(hld_path.c_str(), &stt);
		chk = stat(this->path_root_loc.c_str(), &stt);
	}
	else {
		//here logic of location only
	}

	hld_path = this->path_root_loc;
	if (hld_path.back() == '/')
		hld_path.erase(hld_path.length() - 1);
	chk = stat(hld_path.c_str(), &stt);
	if (stat(hld_path.c_str(), &stt) == -1) {

	}
	if (S_ISDIR(stt.st_mode)) {
		if (this->normalize_uri.back() != '/') {
			this->res_header.insert(std::make_pair("Location", this->normalize_uri + "/"));
			throw "301";
		}
		//this fucntion need more test
		if (this->check_if_dir_has_index()) {
			if (this->check_if_loc_has_cgi()) {
				//here send it to CGI
				//std::cout << "index exists" << std::endl;
			}
			else {
				//std::map<int, std::string> page = this->server->get_status_page();
				//std::map<int, std::string>::iterator itr = page.find(200);
				//itr->second = this->file_path_body;
				throw "200";
			}
		}
		else {
			if (!this->location_data->get_autoindex())
				throw "403";
			//check if empty means the body is full if not read the file
			this->auto_index_page_maker();
			throw "200";
		}
	}
	//here it will take symbolic link and more
	else if (S_ISREG(stt.st_mode)) {
		this->file_path_body = this->path_root_loc;
		throw "200";
	}
	//in this case if it was a non file or dir throw an error
	//here sock and other
	else {
		//maybe here throw 500 ineternal server error
	}
}


void	Request::handle_request_and_response(int &chk) {
	try {
		if (chk == 1) {
			this->location_data = this->server;
			this->parse_request_line();
			this->parse_header();
			chk++;
		}
		if (!this->parse_body())
			return ;
		this->check_location_and_dir_and_red();
		this->check_if_method_allowed();
		if (!this->method.compare("GET"))
			this->get_method();
		else if (!this->method.compare("POST"))
			this->post_method();
		else if (!this->method.compare("DELETE"))
			this->delete_method();
	}
	catch (const char *status) {
		this->send_the_response(status);
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
