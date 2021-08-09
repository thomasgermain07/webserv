# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tboisnie <tboisnie@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/12/15 15:20:28 by tboisnie          #+#    #+#              #
#    Updated: 2021/03/01 19:01:39 by tboisnie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
TITLE = WebServ

# ---------------------------------------
# Compiler
# ---------------------------------------

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
DEBUG = -g3 -fsanitize=address

# ---------------------------------------
# Files
# ---------------------------------------

INC =	-I./\
		-I./config/\
		-I./http/\
		-I./logger/\
		-I./utils/

SRCS = 			main.cpp\
				WebServer.cpp
OBJS = $(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))

HTTP_DIR = 	http
HTTP_SRCS = 	Authenticator.cpp\
				CGI.cpp\
				Connection.cpp \
				ConnectionManager.cpp\
				Mime.cpp\
				Request.cpp\
				RequestInterpreter.cpp\
				RequestParser.cpp\
				Resource.cpp\
				Response.cpp\
				Socket.cpp\
				URI.cpp
HTTP_OBJS = $(addprefix $(OBJSDIR)/, $(HTTP_SRCS:.cpp=.o))

CONFIG_DIR = config
CONFIG_SRCS = 	Config.cpp\
				ConfigParser.cpp\
				Location.cpp\
				Server.cpp
CONFIG_OBJS = $(addprefix $(OBJSDIR)/, $(CONFIG_SRCS:.cpp=.o))

LOGGER_DIR = logger
LOGGER_SRCS = 	Console.cpp
LOGGER_OBJS = $(addprefix $(OBJSDIR)/, $(LOGGER_SRCS:.cpp=.o))

UTILS_DIR = utils
UTILS_SRCS = 	Base64.cpp\
				Network.cpp\
				String.cpp\
				System.cpp\
				Time.cpp
UTILS_OBJS = $(addprefix $(OBJSDIR)/, $(UTILS_SRCS:.cpp=.o))

OBJSDIR = .objs

ALL_OBJS = $(OBJS) $(HTTP_OBJS) $(CONFIG_OBJS) $(LOGGER_OBJS) $(UTILS_OBJS)

DPDCS = $(ALL_OBJS:.o=.d)

# ---------------------------------------
# Create the exec
# ---------------------------------------

opti :
	@-make -j all

-include $(DPDCS)

all: $(NAME)

$(NAME): $(ALL_OBJS)
	@-echo "$(TITLE) : Creation."
	@-$(CXX) $(CXXFLAGS) $(INC) $(ALL_OBJS) -o $(NAME)

fg : $(ALL_OBJS)
	@-echo "$(TITLE) : Creation debug mode."
	@-$(CXX) $(CXXFLAGS) $(DEBUG) $(INC) $(ALL_OBJS) -o $(NAME)

# ---------------------------------------
# Create all objects
# ---------------------------------------

$(OBJSDIR)/%.o : %.cpp | $(OBJSDIR)
	@-echo "Compiling -> $^"
	@-$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

$(OBJSDIR)/%.o : $(HTTP_DIR)/%.cpp | $(OBJSDIR)
	@-echo "Compiling -> $^"
	@-$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

$(OBJSDIR)/%.o : $(CONFIG_DIR)/%.cpp | $(OBJSDIR)
	@-echo "Compiling -> $^"
	@-$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

$(OBJSDIR)/%.o : $(LOGGER_DIR)/%.cpp | $(OBJSDIR)
	@-echo "Compiling -> $^"
	@-$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

$(OBJSDIR)/%.o : $(UTILS_DIR)/%.cpp | $(OBJSDIR)
	@-echo "Compiling -> $^"
	@-$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

$(OBJSDIR) :
	@(mkdir -p .objs)

# ---------------------------------------
# Cleaning rules
# ---------------------------------------

clean:
	@-echo "$(TITLE) : Suppression."
	@-rm -rf $(OBJSDIR)

fclean: clean
	@-echo "$(TITLE) : Annihilation."
	@-rm -f $(NAME)

re: fclean all
