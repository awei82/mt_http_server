CXX		= gcc
DEBUG	= -g
CXXFLAGS	= -Wall $(DEBUG) -pthread

OBJS = gf_handler.o http_server.o 
DEPS = gf_handler.h http_server.h 

%.o: %.c $(DEPS)
	echo $(CXXFLAGS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(OBJS) 
	$(CXX) -c -o gf_server_main.o gf_server_main.c $(CXXFLAGS)
	$(CXX) -o gf_server_main gf_server_main.o $(OBJS) $(CXXFLAGS)

clean:
	rm gf_server_main gf_server_main.o $(OBJS) 

