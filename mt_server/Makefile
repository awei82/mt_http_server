CXX		= gcc
DEBUG	= -g
CXXFLAGS	= -Wall $(DEBUG) -pthread

OBJS = gf_handler.o http_server.o 
DEPS = gf_handler.h http_server.h 

%.o: %.c $(DEPS)
	echo $(CXXFLAGS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(OBJS) 
	$(CXX) -c -o getfile_server.o getfile_server.c $(CXXFLAGS)
	$(CXX) -o getfile_server getfile_server.o $(OBJS) $(CXXFLAGS)

clean:
	rm getfile_server getfile_server.o $(OBJS) 

