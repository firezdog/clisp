if [[ $1 == "clean" ]];
  then rm -r deploy
else
  mkdir -p deploy;
  gcc \
    -std=c99 \
    -g \
    lispy/builtins.c \
    lispy/environment.c \
    lispy/evaluation.c \
    lispy/grammar.c \
    lispy/io.c \
    lispy/lispy.c \
    lispy/lval/lval.c \
    lispy/lval/lval_call.c \
    lispy/mpc_library/mpc.c \
    -ledit \
    -o deploy/lispy
fi