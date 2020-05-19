FROM debian:latest

RUN apt-get update && apt-get install -y gcc make
COPY ./ /app
WORKDIR /app
RUN make

ENTRYPOINT ["./getfile_server"]