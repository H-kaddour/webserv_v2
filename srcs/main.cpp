#include "../include/Data.hpp"
#include "../include/Parsing.hpp"

Parsing	&return_parsing_obj(int ac, char *av)
{
	if (ac == 2)
	{
		Parsing	*parse = new Parsing(av);
		parse->parse_file();
		return (*parse);
	}
	Parsing	*parse = new Parsing;
	parse->make_default_server();
	return (*parse);
}

void	l()
{
	system("leaks webserv");
}

int	main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Error: wrong argument" << std::endl;
		return (1);
	}
	Parsing	*parse = &return_parsing_obj(ac, av[1]);
	Data data(*parse);
	delete parse;
	data.server_execution();
	return (0);
}
