NAME = webserv
CC = c++
FLAG = -Wall -Wextra -Werror -std=c++98 -g
HEADER = $(addprefix include/, Data.hpp Parsing.hpp Default_serv.hpp Server.hpp \
				 Location.hpp Request.hpp)

SRCS = $(addprefix srcs/, main.cpp parsing/Parsing.cpp parsing/tokenizer.cpp \
			 parsing/lexer.cpp parsing/Data.cpp parsing/Server.cpp parsing/Default_serv.cpp \
			 parsing/Location.cpp parsing/save_data_server.cpp parsing/parse_mime_types.cpp \
			 Request/Request.cpp)

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $^ -o $@

%.o: %.cpp $(HEADER)
	$(CC) $(FLAG) -c $< -o $@

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
