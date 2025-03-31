# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: apintus <apintus@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/17 14:48:41 by apintus           #+#    #+#              #
#    Updated: 2025/03/31 18:39:27 by apintus          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

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

NAME			=	WebServ

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
				
				
# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRCS 			=	main.cpp \
					srcs/LogManager/LogManager.cpp \
					srcs/Config/Config.cpp \
					srcs/Config/BlocServer.cpp \
					srcs/Config/BlocLocation.cpp \
					srcs/Utils/Utils.cpp \
					srcs/Server/Socket.cpp \
					srcs/Config/Listen.cpp \

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
	@$(RM) $(NAME)
	@echo "${YELLOW}> Cleaning of $(NAME) has been done ❌${END}\n"

re: clean all

-include $(DEPS)

.PHONY: all clean re fclean
