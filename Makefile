CC = clang 
CFLAGS = -Wall -Wextra -Werror -Wpedantic 
OBJFILES = encode.o io.o trie.o
OBJFILES2 = io.o word.o decode.o
LFLAGS = -lm

all		:	encode decode
encode		:	$(OBJFILES)
			$(CC) encode.o -o encode $(LFLAGS)
decode		:	$(OBJFILES2)
			$(CC) decode.o -o decode $(LFLAGS)

encode.o	:	encode.o
			$(CC) $(CFLAGS) -c encode.c

decode.o	:	decode.o
			$(CC) $(CFLAGS) -c decode.c
clean		:
			rm -rf $(OBJFILES) encode infer-out 
			rm -rf $(OBJFILES2) decode infer-out
infer		:		
			make clean; infer-capture -- make; infer-analyze --make
