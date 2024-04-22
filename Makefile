# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Wno-unknown-pragmas -ggdb3 -std=c11 -fms-extensions `pkg-config --cflags glib-2.0`
LDFLAGS := 
LDLIBS := -lglib-2.0

# Directories
SRCDIR := src
INCDIR := include
LIBDIR := lib
OBJDIR := obj
BUILDDIR := build
TESTDIR := test

# Files
SERVER_SRCS := $(shell find $(SRCDIR)/server -name '*.c')
CLIENT_SRCS := $(shell find $(SRCDIR)/client -name '*.c')
COMMON_SRCS := $(shell find $(SRCDIR)/common -name '*.c')
TEST_SRCS   := $(shell find $(TESTDIR) -name '*.c')

SERVER_OBJS := $(SERVER_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CLIENT_OBJS := $(CLIENT_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
COMMON_OBJS := $(COMMON_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TEST_OBJS   := $(TEST_SRCS:$(TESTDIR)/%.c=$(OBJDIR)/$(TESTDIR)/%.o)

SERVER_INCS := $(shell find $(INCDIR)/server -name '*.h')
CLIENT_INCS := $(shell find $(INCDIR)/client -name '*.h')
COMMON_INCS := $(shell find $(INCDIR)/common -name '*.h')
TEST_INCS   := $(shell find $(INCDIR)/test -name '*.h')

SERVER_DEPS := $(SERVER_INCS:%.h=%.d)
CLIENT_DEPS := $(CLIENT_INCS:%.h=%.d)
COMMON_DEPS := $(COMMON_INCS:%.h=%.d)
TEST_DEPS   := $(TEST_INCS:%.h=%.d)

INC := -I$(INCDIR)

# Targets
SERVER_EXEC := $(BUILDDIR)/server
CLIENT_EXEC := $(BUILDDIR)/client
TEST_EXEC   := $(BUILDDIR)/test

.PHONY: all clean server client

all: server client test

server: $(SERVER_EXEC)

client: $(CLIENT_EXEC)

test: $(TEST_EXEC)
# test:
# 	@echo $(TEST_SRCS)
# 	@echo $(TEST_OBJS)

$(SERVER_EXEC): $(COMMON_OBJS) $(SERVER_OBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(COMMON_OBJS) $(SERVER_OBJS) -o $@ $(LDLIBS)

$(CLIENT_EXEC): $(COMMON_OBJS) $(CLIENT_OBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(COMMON_OBJS) $(CLIENT_OBJS) -o $@ $(LDLIBS)

$(TEST_EXEC): $(COMMON_OBJS) $(TEST_OBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(COMMON_OBJS) $(TEST_OBJS) -o $@ $(LDLIBS)

# Include the dependency files
-include $(SERVER_DEPS)
-include $(CLIENT_DEPS)
-include $(COMMON_DEPS)
-include $(TEST_DEPS)

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

# Rule to compile source files and generate dependency files for test
$(OBJDIR)/test/%.o: $(TESTDIR)/%.c
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

locate_test:
	@echo $(TEST_EXEC)
