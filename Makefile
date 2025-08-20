NAME = webserv
TEST_NAME = test_webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = include
TEST_SRC_DIR = tests/src
TEST_INC_DIR = tests/include

GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
MAGENTA = \033[0;35m
RESET = \033[0m

INC_SUBDIRS = $(shell find $(INC_DIR) -type d)
INC= $(addprefix -I, $(INC_DIR) $(INC_SUBDIRS))
TEST_INC_SUBDIRS = $(shell find $(TEST_INC_DIR) -type d)
TEST_INC= $(addprefix -I, $(TEST_INC_DIR) $(TEST_INC_SUBDIRS))

SRC = $(shell find $(SRC_DIR) -name "*.cpp")
TEST_SRC = $(shell find $(TEST_SRC_DIR) -name "*.cpp")

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)
OBJ_NO_MAIN = $(filter-out $(OBJ_DIR)/src/main.o, $(OBJ))
ONLY_TEST_OBJ = $(TEST_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJ = $(OBJ_NO_MAIN) $(ONLY_TEST_OBJ)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)/$(dir $<)
	@echo "$(BLUE)Compiling:$(RESET) $<"
	@$(CC) $(CFLAGS) $(INC) $(TEST_INC) -c $< -o $@

all: $(NAME)
test: $(TEST_NAME)

$(NAME): $(OBJ)
	@echo "$(YELLOW)Linking $(NAME)... $(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) is ready!$(RESET)"

$(TEST_NAME): $(TEST_OBJ)
	@echo "$(YELLOW)Linking $(TEST_NAME)... $(RESET)"
	@$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_NAME)
	@echo "$(GREEN)$(TEST_NAME) is ready!$(RESET)"

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)Clean complete!$(RESET)"

fclean:
	@echo "$(YELLOW)Full cleaning...$(RESET)"
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)
	@echo "Removing executable..."
	@rm -f $(NAME)
	@rm -f $(TEST_NAME)
	@echo "$(GREEN)Full clean complete!$(RESET)"

re: fclean all

.PHONY: all clean fclean re test
