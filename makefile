BINDIRTEST = debug

ALL: $(SRCDIR)/Scanner.cpp $(HEADERDIR)/Scanner.h 
	gcc -g  -c -Wall GameOfLife.c -o GameOfLife
	
clean:
	rm -rf $(BINDIR)/*