# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c11
LDFLAGS := 
LDLIBS := 

# Directories
SRCDIR := src
INCDIR := include
LIBDIR := lib
OBJDIR := obj
BUILDDIR := build

# Files
SERVER_SRCS := $(shell find $(SRCDIR)/server -name '*.c')
CLIENT_SRCS := $(shell find $(SRCDIR)/client -name '*.c')
COMMON_SRCS := $(shell find $(SRCDIR)/common -name '*.c')

SERVER_OBJS := $(SERVER_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CLIENT_OBJS := $(CLIENT_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
COMMON_OBJS := $(COMMON_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

SERVER_DEPS := $(SERVER_OBJS:.o=.d)
CLIENT_DEPS := $(CLIENT_OBJS:.o=.d)
COMMON_DEPS := $(CLIENT_OBJS:.o=.d)

INC := -I$(INCDIR)

# Targets
SERVER_EXEC := $(BUILDDIR)/server
CLIENT_EXEC := $(BUILDDIR)/client

.PHONY: all clean server client

all: server client

server: $(SERVER_EXEC)

client: $(CLIENT_EXEC)

$(SERVER_EXEC): $(COMMON_OBJS) $(SERVER_OBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(COMMON_OBJS) $(SERVER_OBJS) -o $@ $(LDLIBS)

$(CLIENT_EXEC): $(COMMON_OBJS) $(CLIENT_OBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(COMMON_OBJS) $(CLIENT_OBJS) -o $@ $(LDLIBS)

# Include the dependency files
-include $(SERVER_DEPS)
-include $(CLIENT_DEPS)
-include $(COMMON_DEPS)

# Rule to compile source files and generate dependency files for server
$(OBJDIR)/server/%.o: $(SRCDIR)/server/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@

# Rule to compile source files and generate dependency files for client
$(OBJDIR)/client/%.o: $(SRCDIR)/client/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@

# Rule to compile source files and generate dependency files for common
$(OBJDIR)/common/%.o: $(SRCDIR)/common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@

debug: CFLAGS += -DDEBUG -g
debug: all

clean:
	$(RM) -r $(OBJDIR) $(SERVER_EXEC) $(CLIENT_EXEC)

locate_server:
	@echo $(SERVER_EXEC)

locate_client:
	@echo $(CLIENT_EXEC)
