# Exercises:

1. Change the grammar to recognize decimal numbers such as 0.01, 5.21, or 10.2.
2. Change the grammar to make the operators written conventionally, between two expressions.
3. https://medium.freecodecamp.org/parsing-math-expressions-with-javascript-7e8f5572276e -- read to get an understanding of what mpc might be doing.

# Scrapped work:

## Validation pattern for decimal numbers:

>/-?[0-9]+(\.[0-9]+)?/

Replaced with:

>(\d*\.)?\d+

This is very clever -- I would have thought to break up a decimal number by requiring the left part (representing the integer) and making the right part optional.  This cuts the number the other way -- if you make the left part and the period optional, you're left with an integer-numeral.  The left part is a nonsensical expression (an integer followed by a period) which, when concatenated with an integer-numeral, becomes a decimal.

The other improvement: using "\d" is more concise than using "[0-9]".

### Tests for the pattern:

1. Integer should pass (0, 15, -22)
3. Rational should pass (-5.23, 5.23, -0.254, 0.254)
2. Edge case: (00, 001, -0005) will pass
4. Edge case: Decimals without left integer will pass (.234, -.234)
5. Edge case: integers with period will not pass (234., -234.)

###Surprise 

C regex does not allow the "d" pattern (or the \. escape? -- see below)!

We replace it with:

> ([0-9]*.)?[0-9]+ (this is wrong -- see below)

...No.  We need to escape that "." or it means "any single character".  How to escape?  Actually, you need to use two '\' -- so you get this:

> ([0-9]*\\.)?[0-9]+

A resource that explains this (sort of):

http://www.informit.com/articles/article.aspx?p=2064649&seqNum=2

Seems like it is because I'm doing this in the context of a "string literal."

## Necessary?
Seems like the program and expression definition are repeating each other?