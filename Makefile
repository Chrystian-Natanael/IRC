#* ******************************************************************************#
#                                   NAME                                         #
#* ******************************************************************************#

NAME = irc
FILE_EXTENSION = .cpp
.DEFAULT_GOAL := all
.PHONY: all clean fclean re tests help
.SILENT:

#* ******************************************************************************#
#                                   COLORS                                       #
#* ******************************************************************************#

DEFAULT=\033[39m
BLACK=\033[30m
DARK_RED=\033[31m
DARK_GREEN=\033[32m
DARK_YELLOW=\033[33m
DARK_BLUE=\033[34m
DARK_MAGENTA=\033[35m
DARK_CYAN=\033[36m
LIGHT_GRAY=\033[37m
DARK_GRAY=\033[90m
RED=\033[91m
GREEN=\033[92m
ORANGE=\033[93m
BLUE=\033[94m
MAGENTA=\033[95m
CYAN=\033[96m
WHITE=\033[97m
RESET = \033[0m

#* ******************************************************************************#
#                                   PATH                                         #
#* ******************************************************************************#

SRCS_PATH = src/
INCS_PATH = include/
BUILD_DIR := build/
TARGET_DIR = bin/
GTEST_DIR = tests/googletest

#* ******************************************************************************#
#                                   FILES                                        #
#* ******************************************************************************#

GTEST_REPO = git@github.com:google/googletest.git
SRCS = $(wildcard $(SRCS_PATH)*$(FILE_EXTENSION)) # !Do not use wildcard (fix in future, obviously)
OBJS = $(SRCS:%$(FILE_EXTENSION)=$(BUILD_DIR)%.o)
DEPS = $(OBJS:.o=.d)

#* ******************************************************************************#
#                                    COMMANDS                                    #
#* ******************************************************************************#

MKDIR := mkdir -p
RM := rm -rf
SLEEP = sleep 0.1
COMP = c++
SHELL := /bin/bash

#* ******************************************************************************#
#                                 FLAGS E COMP                                   #
#* ******************************************************************************#

CFLAGS = -std=c++98 -Wall -Wextra -Werror
DFLAGS = -std=c++98 -Wall -Wextra -Werror -g3
LDLIBS = -ldl -lglfw -pthread
CPPFLAGS = $(addprefix -I,$(INCS_PATH)) -MMD -MP
COMP_OBJ = $(COMP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
COMP_EXE = $(COMP) $(OBJS) $(LDLIBS) -o $(TARGET_DIR)$(NAME)

#* ******************************************************************************#
#                                  FUNCTIONS                                     #
#* ******************************************************************************#

define create_dir
	$(MKDIR) $(dir $@)
endef

define comp_objs
	$(eval COUNT=$(shell expr $(COUNT) + 1))
	$(COMP_OBJ)
	$(SLEEP)
	printf "Compiling $(NAME) $(YELLOW) %d%%\r$(FCOLOR)" $$(echo $$(($(COUNT) * 100 / $(words $(SRCS)))))
endef

define comp_exe
	$(MKDIR) $(TARGET_DIR)
	$(COMP_EXE)
	printf "\n"
	printf "$(GREEN)$(NAME) ->$(RESET)$(PURPLE) Is Ready in directory '$(TARGET_DIR)'\n$(RESET)"
endef

define help
	echo "${DARK_RED}Available targets:${RESET}"
	printf "\n"
	echo "${DARK_BLUE}all:${RESET} ${LIGHT_GRAY}Build $(NAME)${RESET}"
	echo "${DARK_BLUE}both:${RESET} ${LIGHT_GRAY}Build $(NAME) and $(NAME) bonus (if aplicable)${RESET}"
	echo "${DARK_BLUE}bonus:${RESET} ${LIGHT_GRAY}Build $(NAME) bonus (if aplicable)${RESET}"
	echo "${DARK_BLUE}re:${RESET} ${LIGHT_GRAY}Rebuild the program${RESET}"
	echo "${DARK_BLUE}clean:${RESET} ${LIGHT_GRAY}Remove the object files${RESET}"
	echo "${DARK_BLUE}fclean:${RESET} ${LIGHT_GRAY}Remove the program and the object files${RESET}"
	echo "${DARK_BLUE}run:${RESET} ${LIGHT_GRAY}run the program without arguments${RESET}"
	echo "${DARK_BLUE}tests:${RESET} ${LIGHT_GRAY}build and run tests from test/ (Need format gtests)${RESET}"
endef

#* ******************************************************************************#
#                                   TARGETS                                      #
#* ******************************************************************************#

all: $(NAME)

$(BUILD_DIR)%.o: %$(FILE_EXTENSION)
	$(call create_dir)
	$(call comp_objs)

$(NAME): $(OBJS)
	$(call comp_exe)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET_DIR)
	$(RM) tests/build/
	$(RM) $(GTEST_DIR)

re: fclean all

$(GTEST_DIR):
	git clone $(GTEST_REPO) $(GTEST_DIR)

tests: $(GTEST_DIR)
	cd tests && cmake -B build && $(MAKE) -C build && ./build/run_tests

run:
	./bin/$(NAME)
help:
	$(call help)

both: all bonus

-include $(DEPS)
