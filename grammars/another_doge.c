mpc_parser_t* Adjective = mpc_new("adjective");
mpc_parser_t* Noun = mpc_new("noun");
mpc_parser_t* Phrase = mpc_new("phrase");
mpc_parser_t* Doge = mpc_new("doge");

mpca_lang(MPCA_DEFAULT, 
    "                                           \
        adjective   : \"wow\"   | \"many\"      \
                    | \"so\"    | \"such\";     \
        noun        : \"lisp\"  | \"build\"     \
                    | \"book\"  | \"c\"         \
                    | \"language\";             \
        phrase      : <adjective> <noun>;       \
        doge        : <phrase>*;                \
    ", 
    Adjective, Noun, Phrase, Doge);

/* parsing goes here */

mpc_cleanup(4, Adjective, Noun, Phrase, Doge);