FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y \
    socat 

WORKDIR /challenge

COPY ./licence-checker .
COPY ./flag .

RUN chown -R root:root /challenge/*
RUN chmod -R o-w /challenge/*
RUN chown root:root /challenge/flag
RUN chmod 500 /challenge/flag
RUN chmod 4755 /challenge/licence-checker

RUN useradd ctf
USER ctf

ENTRYPOINT socat TCP4-LISTEN:2727,fork,reuseaddr EXEC:/challenge/licence-checker
