# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#              #
#    Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = miniRT

CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c99 -g
INCLUDES = -I./includes -I./lib/minilibx-linux
MLX_DIR = ./lib/minilibx-linux
MLX_LIB = $(MLX_DIR)/libmlx.a
LIBS = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm

# Source directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRCS =	$(SRC_DIR)/main.c \
		$(SRC_DIR)/parser/parser.c \
		$(SRC_DIR)/parser/parse_elements.c \
		$(SRC_DIR)/parser/parse_objects.c \
		$(SRC_DIR)/parser/parse_validation.c \
		$(SRC_DIR)/math/vector.c \
		$(SRC_DIR)/math/vector_ops.c \
		$(SRC_DIR)/ray/intersections.c \
		$(SRC_DIR)/lighting/lighting.c \
		$(SRC_DIR)/render/render.c \
		$(SRC_DIR)/window/window.c \
		$(SRC_DIR)/utils/error.c \
		$(SRC_DIR)/utils/cleanup.c

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)
	@echo "$(GREEN)$(NAME) created successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
