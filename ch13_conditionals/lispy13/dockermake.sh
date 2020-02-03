gcc \
  -std=c99 \
  builtins.c \
  environment.c \
  evaluation.c \
  grammar.c \
  io.c \
  lispy.c \
  lval.c \
  mpc_library/mpc.c \
  -leditline \
  -g