FROM ubuntu:20.04
MAINTAINER qgjyf2001
RUN apt-get update
RUN apt-get install -y wget
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN dpkg --add-architecture i386
RUN apt-get install -y build-essential
RUN apt-get update
RUN apt-get install -y gcc-9-multilib
RUN apt-get install -y gcc
COPY perflab-handout ./perflab-handout
