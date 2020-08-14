/* Adjectives are one or more of "wow", "many", "so", and "such" */
mpc_parser_t* Adjective = mpc_or(4,
    mpc_sym("wow"), mpc_sym("so"),
    mpc_sym("many"), mpc_sym("such")
);

/* Nouns are one or more of "language", "lisp", "book", "build", and "c" */
mpc_parser_t* Noun = mpc_or(5,
    mpc_sym("lisp"), mpc_sym("language"),
    mpc_sym("book"), mpc_sym("build"),
    mpc_sym("c")    
);

/* A phrase consists of an adjective and a noun */
mpc_parser_t* Phrase = mpc_and(2, mpcf_strfold,
    Adjective, Noun, free);

/* A sentence of doge is a series of one or more phrases */
mpc_praser_t* Doge = mpc_many(mpcf_strfold, Phrase);