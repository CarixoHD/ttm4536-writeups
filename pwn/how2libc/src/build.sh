docker run -it -v $(pwd):/usr/src/myapp -w /usr/src/myapp gcc:9.5 gcc main.c -no-pie -fno-stack-protector -o how2libc

id=$(docker create ubuntu:20.04)
docker cp $id:/lib/x86_64-linux-gnu/libc-2.31.so .
docker rm -v $id
