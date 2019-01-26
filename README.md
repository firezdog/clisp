# About
This repository contains code exercises meant to build towards a LISP interpreter written in C.  The following are various discussions and notes concerning topics that have come up in the course of completing those exercises.

## Languages
The syntax of a programming language (and of linguistic structures more generally) is defined recursively: atoms are elements of the language.  Atoms combined in accordance with the syntactical rules are also elements of the language.  For example,

> 3 + 5 == 7

is a statement of some programming language.  It is also a rule that if _p_ is a statement of a programming language and _q_ is a statement of the programming language, then

> if (p) { q }

is a statement of that programming language.  From this it follows that 

> if (3 + 5 == 7) { 3 + 5 == 7 }

is a statement of the programming language, and also

> if (3 + 5 == 7) { if (3 + 5 == 7) { 3 + 5 == 7 } }

is a statement of the programming language, and so on _ad infinitum_.

This treatment can be used to define the syntax of all sorts of "statements":

### Numerals

Numerals are strings of characters that symbolize numbers.  The basic numerals are _0_, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, and _9_.  In an unrestricted sense, if _X_ is a numeral and _Y_ is a numeral, we can also say that _X_+_Y_, the result of concatenating _X_ and _Y_, is a numeral.  In an unrestricted sense, then, _1_, _10_, and _01_ are all numerals.  (From this exercise we can also see how much different it is to define a syntax for numerals than it is to define a semantics for numerals.  It is a very easy matter to define rules from which it will immediately follow that _1_, _01_, and _001_ are all numerals.  It is a little bit harder to define rules from which it will follow that _1_, _01_, and _001_ all stand for the same number.)

In a restricted sense, the basic numerals are the same, but we only allow concatenation of basic numerals _X_ and _Y_ when _X_ is not _0_.  We want to allow, on the other hand, that _1_ and _0004_ form a numeral (_1004_).  We can say, then, that if _X_ is a numeral in the restricted sense, and _Y_ is a numeral either in the restricted or unrestricted sense, then _X_ concatenated with _Y_ is also a numeral (in both the restricted and unrestricted senses).

### Decimals

Decimals are formed when we concatenate a numeral in the restricted sense with _._ and another numeral in the unrestricted sense.  (This definition allows the decimal _1.000000_.)  We could also define decimals in an unrestricted sense, which would be any concatenation of numerals in the unrestricted sense with _._ in between.  Note that the result of concatenating decimals with decimals is not a well-formed decimal (_1.05.1.32_ is obviously not a decimal, even though both _1.05_ and _1.32_ are).

### URLs
Currently an exercise for the reader (and writer).

### Simple English sentences.
Currently an exercise for the reader (and writer).

### JSON
Currently an exercise for the reader (and writer).
