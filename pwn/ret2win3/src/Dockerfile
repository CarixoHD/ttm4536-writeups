FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y socat

WORKDIR /challenge

COPY ./ret2win .
COPY ./flag .


RUN chmod +x ret2win
RUN chown -R root:root /challenge/*
RUN chmod -R o-w /challenge/*
RUN chown -R root:root /challenge/flag
RUN chmod -R o-w /challenge/flag

RUN useradd ctf
USER ctf

ENTRYPOINT socat TCP4-LISTEN:2727,fork,reuseaddr EXEC:/challenge/ret2win
