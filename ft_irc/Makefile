# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: josephkiragu <josephkiragu@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/19 13:11:13 by jtanner           #+#    #+#              #
#    Updated: 2023/11/28 22:15:16 by josephkirag      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Ft_IRC
CC = c++
FLAGS = -Wall -Wextra -Werror -g -std=c++11 -Wno-unused-but-set-parameter
RM = rm -f
LINKS = -I./includes -pthread -framework OpenGL \
	   	-framework Appkit

SRCS_DIR = srcs/

FILES = Main \
		irc\
		Socket \
		Channel \
		UserManager

SRCS = $(addprefix $(SRCS_DIR), $(addsuffix .cpp, $(FILES)))
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME) $(LINKS)

$(OBJS): %.o : %.cpp
	c++ $(FLAGS) -c -o $@ $< 
	
clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean

