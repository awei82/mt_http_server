# Simple multithreaded HTTP GET Server

Simple multithreaded HTTP GET server written entirely in C, using the pthreads library.  
This is a simple HTTP web/file server that hosts a simplified version of the HTTP GET and HEAD methods.

This project is expanded from Gatech's GIOS GETFILE server project.  
The design is roughly based off of Python's [http.server](https://docs.python.org/3/library/http.server.html) module.

## Why build an HTTP server in C?
Partly due to self interest, and partly due to an obsession in low level programming.  
Since the basic framework for an HTTP server was already built out for my OS course, I thought I'd flesh it out into a fully functional GET HTTP server.  
This project was a good opportunity for me to brush up on my socket and multithreading programming, as well as dig into the HTTP specs.

## Building + running the server
Just run `make` to build the executable.  
Run `./getfile_server -h` to view help instructions.

### Example:  
In one terminal, run `./getfile_server`  
From a web browser, navigate to `http://localhost:6200/README.md`

## Running with Docker
Build the image: `docker build -t getfile_server:latest .`  
Run the server: `docker run -p 6200:6200 getfile_server -p 6200`
