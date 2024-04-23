CC = g++
CFLAGS = -Wall -Wextra -std=c++11

all: parent child

parent: parent.cpp
$(CC) $(CFLAGS) -o parent parent.cpp

child: child.cpp
$(CC) $(CFLAGS) -o child child.cpp

clean:
rm -f parent child
