# Makefile for Study Buddy Backend

CC = gcc
CFLAGS = -Wall -Wextra -O2 -lm

all: study_buddy_backend

study_buddy_backend: backend.c
	$(CC) $(CFLAGS) -o study_buddy_backend backend.c -lm

clean:
	rm -f study_buddy_backend