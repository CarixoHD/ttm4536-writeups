FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y \
    vim \
    nano \
    python3 \
    python3-pip \
    socat && \
    pip3 install pwntools && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /challenge

COPY ./shellcode .
COPY ./flag /

RUN useradd -s /usr/sbin/nologin owner

RUN chmod +x shellcode \
    && chown owner:owner /challenge/shellcode

RUN chown owner:owner /flag \
    && chmod 600 /flag

RUN chown -R owner:owner /challenge \
    && chmod -R go-w /challenge

RUN useradd -m -s /bin/bash ctf \
    && chown ctf:ctf /home/ctf

RUN find / -perm /6000 -type f -exec chmod a-s {} + 2>/dev/null || true

RUN chmod 4555 /challenge/shellcode

USER ctf
WORKDIR /home/ctf

ENV TERM=xterm

ENTRYPOINT socat TCP4-LISTEN:2727,fork,reuseaddr EXEC:/bin/bash,pty,stderr,setsid,sigint,sane
#ENTRYPOINT ["/bin/bash"]
