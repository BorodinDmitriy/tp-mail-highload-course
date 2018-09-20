FROM ubuntu:14.04
MAINTAINER Dmitriy Borodin
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get -y install g++
RUN apt-get -y install libboost-all-dev

ADD ./ ./

RUN g++ *.cpp -pthread -lboost_thread -lboost_filesystem -std=c++11 -lboost_system -o server.exe

EXPOSE 80

CMD ./server.exe