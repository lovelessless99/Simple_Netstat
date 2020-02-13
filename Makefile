INCLUDEPATH = include
OBJPATH = obj
SRCPATH = src
EXECPATH = .

CC = gcc
CFLAGS = -g -Wall -c

SRCS = $(wildcard $(SRCPATH)/*.c)
OBJS = $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(SRCS))
EXEC = $(EXECPATH)/simple_ns

all: dir build

dir:
	mkdir -p $(OBJPATH)

build: $(OBJS) $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -g -o $@ $(OBJS)
	sudo chown root $@
	sudo chmod 4775 $@

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) $(CFLAGS) $< -I $(INCLUDEPATH) -o $@

clean:
	rm -r $(OBJPATH)
	rm $(EXEC)
