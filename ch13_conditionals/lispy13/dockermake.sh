gcc \
  -std=c99 \
  builtins.c \
  environment.c \
  evaluation.c \
  grammar.c \
  io.c \
  lispy.c \
  lval/lval.c \
  lval/lval_call.c \
  mpc_library/mpc.c \
  -leditline \
  -g