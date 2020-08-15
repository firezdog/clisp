# after building image, run the container with interactive terminal to use the REPL
FROM gcc:4.9
COPY . /usr/src/clisp
WORKDIR /usr/src/clisp
RUN apt-get update; apt-get install -y libeditline-dev libedit-dev; bash make.sh
CMD ["./deploy/lispy"]