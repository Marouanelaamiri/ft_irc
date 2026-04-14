NAME = ircserv
BOT_NAME = bot

INC_DIR = INC
SRC_DIR = SRC
CMD_DIR = CMD

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

HEADERS = $(INC_DIR)/Channel.hpp \
          $(INC_DIR)/IClient.hpp \
          $(INC_DIR)/Client.hpp \
          $(INC_DIR)/Parser.hpp \
          $(INC_DIR)/Server.hpp

OBJ = $(SRC:.cpp=.o)

C++ = c++
C++FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

all: $(NAME) $(BOT_NAME)

$(NAME): $(OBJ) 
	@$(C++) $(C++FLAGS) $(OBJ) -o $@
	@echo "Compilation complete. Executable created: $(NAME)"

$(BOT_NAME): $(SRC_DIR)/bot.cpp
	@$(C++) $(C++FLAGS) $(SRC_DIR)/bot.cpp -o $(BOT_NAME)
	@echo "Compilation complete. Executable created: $(BOT_NAME)"

%.o: %.cpp $(HEADERS)
	@$(C++) $(C++FLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

clean:
	@rm -f $(OBJ)
	@echo "Cleaned object files."

fclean: clean
	@rm -f $(NAME) $(BOT_NAME)
	@echo "Cleaned executables."

re: fclean all
	@echo "Recompiled everything."