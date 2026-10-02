DEBUG_MODE_RUN ?= 0

all:
	gcc -g vigenere-encrypt.c -o executable-exe -DDEBUG_MODE_RUN=$(DEBUG_MODE_RUN) 
	./executable-exe

clean:
	rm executable-exe
