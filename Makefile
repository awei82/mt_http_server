CXX		= gcc
DEBUG	= -g
CXXFLAGS	= -Wall $(DEBUG) -pthread

OBJS = gf_handler.o http_server.o 
DEPS = src/gf_handler.h src/http_server.h 

%.o: src/%.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(OBJS) 
	$(CXX) -c -o getfile_server.o src/getfile_server.c $(CXXFLAGS)
	$(CXX) -o getfile_server getfile_server.o $(OBJS) $(CXXFLAGS)
	rm *.o

clean:
	rm getfile_server getfile_server.o $(OBJS) 

