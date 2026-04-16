NAME = ircserv
BOT_NAME = bot

INC_DIR = INC
SRC_DIR = SRC
CMD_DIR = CMD
BONUS_DIR = Bonus

SRC = $(SRC_DIR)/Auth.cpp \
      $(SRC_DIR)/Channel.cpp \
      $(SRC_DIR)/Parser.cpp \
      $(SRC_DIR)/Server.cpp \
      $(SRC_DIR)/Client.cpp \
      $(CMD_DIR)/INVITE.cpp \
      $(CMD_DIR)/JOIN.cpp \
      $(CMD_DIR)/KICK.cpp \
      $(CMD_DIR)/MODE.cpp \
      $(CMD_DIR)/PRIVMSG.cpp \
      $(CMD_DIR)/TOPIC.cpp \
      main.cpp 

BOT_SRC = $(BONUS_DIR)/bot.cpp

HEADERS = $(INC_DIR)/Channel.hpp \
          $(INC_DIR)/IClient.hpp \
          $(INC_DIR)/Client.hpp \
          $(INC_DIR)/Parser.hpp \
          $(INC_DIR)/Server.hpp

OBJ = $(SRC:.cpp=.o)
BOT_OBJ = $(BOT_SRC:.cpp=.o)

C++ = c++
C++FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(C++) $(C++FLAGS) $(OBJ) -o $@

bonus: $(BOT_NAME)

$(BOT_NAME): $(BOT_OBJ)
	$(C++) $(C++FLAGS) $(BOT_OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(C++) $(C++FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BOT_OBJ)

fclean: clean
	rm -f $(NAME) $(BOT_NAME)

re: fclean all