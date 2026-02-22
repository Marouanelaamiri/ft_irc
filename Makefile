NAME = ircserv

INC_DIR = INC
SRC_DIR = SRC
CMD_DIR = CMD

SRC = $(SRC_DIR)/Auth.cpp \
      $(SRC_DIR)/Channel.cpp \
      $(SRC_DIR)/Parser.cpp \
      $(CMD_DIR)/INVITE.cpp \
      $(CMD_DIR)/JOIN.cpp \
      $(CMD_DIR)/KICK.cpp \
      $(CMD_DIR)/MODE.cpp \
      $(CMD_DIR)/PRIVMSG.cpp \
      $(CMD_DIR)/TOPIC.cpp

HEADERS = $(INC_DIR)/Channel.hpp \
          $(INC_DIR)/IClient.hpp \
          $(INC_DIR)/Parser.hpp \
          $(INC_DIR)/Fakeserver.hpp

OBJ = $(SRC:.cpp=.o)

C++ = c++
C++FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(C++) $(C++FLAGS) $(OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(C++) $(C++FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
