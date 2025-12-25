CFLAGS += -Wall -Wextra -O2 -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
LDFLAGS = -lm -lnotcurses -lnotcurses-core

SRC = main.c globals.c input.c ncapi.c board.c logic.c

game: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o game
