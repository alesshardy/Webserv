# **************************************************************************** #
#                                  VARIABLES                                   #
# **************************************************************************** #

END				=	\033[0m

# COLORS

GREY			=	\033[0;30m
RED				=	\033[0;31m
GREEN			=	\033[0;32m
YELLOW			=	\033[0;33m
BLUE			=	\033[0;34m
PURPLE			=	\033[0;35m
CYAN			=	\033[0;36m
WHITE			=	\033[0;37m


# **************************************************************************** #
#                                   PROGRAM                                    #
# **************************************************************************** #

NAME			=	webserv

# **************************************************************************** #
#                                   COMPILER                                   #
# **************************************************************************** #

CC				=	c++
RM				=	rm -rf
CFLAGS			+=	-Wall -Wextra -Werror -std=c++98

# **************************************************************************** #
#                                    PATHS                                     #
# **************************************************************************** #

OBJ_PATH		=	.obj
INC			=	-I./srcs/	\
				-I./srcs/LogManager	\
				-I./srcs/Server	\
				-I./srcs/Request \
				-I./srcs/Response \
				-I./srcs/Config \
				-I./srcs/Utils \
				-I./srcs/Client \
				-I./srcs/CommandLineParser \
				
				
TMP_PATH		=	./www/main/cgi-bin/tmp				
# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRCS 			=	srcs/main.cpp \
					srcs/LogManager/LogManager.cpp \
					srcs/Config/Config.cpp \
					srcs/Config/BlocServer.cpp \
					srcs/Config/BlocLocation.cpp \
					srcs/Config/Listen.cpp \
					srcs/Utils/Utils.cpp \
					srcs/Server/Socket.cpp \
					srcs/Server/Server.cpp \
					srcs/Client/Client.cpp \
					srcs/CommandLineParser/CommandLineParser.cpp \
					srcs/Request/Request.cpp \
					srcs/Request/RequestBody.cpp \
					srcs/Request/CgiRequest.cpp \
					srcs/Response/Response.cpp \
					srcs/Response/ErrorPage.cpp \

OBJS			=	$(SRCS:%.cpp=$(OBJ_PATH)/%.o)

DEPS			=	$(OBJS:.o=.d)

# **************************************************************************** #
#                                    RULES                                     #
# **************************************************************************** #


all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(INC) -o $(NAME)
	@echo "\n${GREEN}> $(NAME) was successfuly compiled ${END}"

$(OBJ_PATH)/%.o: %.cpp
			@mkdir -p $(dir $@)
			@echo "${BLUE}>Generating $(NAME) objects... %-33.33s\r${END}" $@
			@$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@$(RM) $(OBJ_PATH)
	@echo "${YELLOW}> Cleaning $(NAME)'s objects has been done ❌${END}\n"

fclean:	clean
	@$(RM) $(NAME) $(TMP_PATH)
	@echo "${YELLOW}> Cleaning of $(NAME) has been done ❌${END}\n"

re: clean all

-include $(DEPS)

.PHONY: all clean re fclean
