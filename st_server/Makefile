CXX		= gcc
DEBUG	= -g
CXXFLAGS	= -Wall $(DEBUG)

OBJS = gf_handler.o http_server.o 
DEPS = gf_handler.h http_server.h 

%.o: %.c $(DEPS)
	echo $(CXXFLAGS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(OBJS) 
	$(CXX) -c -o http_server_main.o http_server_main.c $(CXXFLAGS)
	$(CXX) -o http_server_main http_server_main.o $(OBJS) $(CXXFLAGS)

clean:
	rm http_server_main http_server_main.o $(OBJS) 

