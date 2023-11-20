# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jtanner <jtanner@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/19 13:11:13 by jtanner           #+#    #+#              #
#    Updated: 2023/11/20 12:59:51 by jtanner          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Ft_IRC
CC = c++
FLAGS = -Wall -Wextra -Werror -g -std=c++11 
RM = rm -f
LINKS = -I./includes -pthread -framework OpenGL \
	   	-framework Appkit

SRCS_DIR = srcs/

FILES = main \
		irc\
		Socket \
		Channel\
		handling\
		User\

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

