---
layout: post
title:  "Lexical Analysis"
cover:  empty.jpg
date:   2017-09-01 11:59:59 ET
categories: compilers lexers
---

The first stage in a compiler (or interpreter) is called **lexical analysis**.  In this post, I'll briefly describe what lexical analysis is and why it's useful.

## Lexical Analysis

A compiler reads source code in a high-level language and translates it into an equivalent program in a lower-level language -- usually machine language.  This is a complex process, so like any complex piece of software, a compiler is divided into many different components, each of which is responsible for a single, well-defined task.

[//]: <> (This means that each component can be tested and reasoned about independently.)

If you are writing a compiler from scratch, the lexical analyzer -- also called the **lexer** or **scanner** -- is probably the component you will write first.  It is the first "phase" in the compilation process -- it reads the source code and converts it into input for the next phase: the parser.  To understand what a lexer does, consider this small fragment of JavaScript.

```javascript
var something = /* comment */
  fn()<<100.5;
```

The lexer **partitions** the input as follows:

$$
\underset{\color{gray}{1}}{\texttt{var}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{2}}{\texttt{ }}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{3}}{\texttt{something}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{4}}{\texttt{ }}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{5}}{\texttt{=}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{6}}{\texttt{ }}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{7}}{\texttt{/* comment */}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{8}}{\texttt{\\n}\ \ \ \ }\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{9}}{\texttt{fn}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{10}}{\texttt{(}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{11}}{\texttt{)}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{12}}{\texttt{<<}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{13}}{\texttt{100.5}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{14}}{\texttt{;}}
$$

The first three letters form the keyword <tt>var</tt>.  The next character is whitespace.  The letters <tt>something</tt> form an identifier (a name).  Later in the input, the two-character sequence <tt>&lt;&lt;</tt> forms the left-shift operator.

The lexer **discards whitespace and comments**, so its output is really more like this:

$$
\fbox{var}\,
\fbox{something}\,
\fbox{=}\,
\fbox{fn}\,
\fbox{(}\,
\fbox{)}\,
\fbox{<<}\,
\fbox{100.5}\,
\fbox{;}
$$

Each of these strings is called a **token** or **lexeme**.  (This is not what a "lexeme" is in linguistics, but the term is used this way in computer science, for better or worse.)

Now, from this example, it should be clear that there are many different "kinds" of tokens, including keywords like <tt>var</tt>, operators like <tt>&lt;&lt;</tt>, identifiers like <tt>something</tt> and <tt>fn</tt>, and literals like <tt>100.5</tt>.  Let's associate a symbolic name with each token, indicating what "kind" of token it is.  This is shown in the "Token Type" column below.

|-------------------|--------------------|
| Token Type        | Lexeme             |
|-------------------|--------------------|
| T_VAR             | <tt>var</tt>       |
| T_IDENTIFIER      | <tt>something</tt> |
| T_EQ              | <tt>=</tt>         |
| T_IDENTIFIER      | <tt>fn</tt>        |
| T_LPAREN          | <tt>(</tt>         |
| T_RPAREN          | <tt>)</tt>         |
| T_LT_LT           | <tt>&lt;&lt;</tt>  |
| T_NUMERIC_LITERAL | <tt>100.5</tt>     |
| T_SEMICOLON       | <tt>;</tt>         |

Note that the lexer identifies both <tt>x</tt> and <tt>fn</tt> as T_IDENTIFIER tokens.  In general, for every type of token, there is a **pattern** that describes all of the corresponding lexemes.  For example, in JavaScript, an _identifier_ is a string of one or more characters, where (1) every character is either a letter, decimal digit, underscore, or dollar sign; (2) the first character is not a decimal digit; and (3) the string is not a reserved word (<tt>if</tt>, <tt>while</tt>, etc.).  Any string that matches this pattern will be labeled as a T_IDENTIFIER token.  While there are many different strings that correspond to T_IDENTIFIER, many token types correspond to only one lexeme (e.g., T_EQ always corresponds to `=`, and T_VAR always corresponds to `var`).

So what is the output of the lexer?  It's reasonably close to what's shown in the table above.  For each token in the input, the lexer outputs its token type (such as <tt>T_IDENTIFIER</tt>) together with any "interesting" information about that token (e.g., its source position, its text, etc.).  These "interesting" pieces of information are called the **attributes** of that token.  Sometimes, this output -- a token type together with any attributes -- is also called a "token."

## Summary so far

Notice that we have defined "token" twice now.  At first, we said a "token" and a "lexeme" are the same thing.  In the last paragraph, we said that a "token" consists of a token type and attributes.  Unfortunately, the word is used both ways, depending on context.  When a string is tokenized (divided into tokens), the substrings (lexemes) are often called "tokens."  When a lexical analyzer tokenizes its input, it produces a sequence of "tokens" that are fed to the parser; these usually consist of a token type together with attributes.  Fortunately, it's usually clear from context which meaning is intended.  Either way, a "token" refers to a substring of the input, or information about such a string.

To summarize the terminology so far:
* The **lexical analyzer** (also called the **lexer**, **scanner**, or **tokenizer**) is the first phase of a compiler, which reads the source text and outputs a stream of tokens.
* The lexer partitions the input text into substrings.  Substrings for whitespace and comments are usually discarded.  Each remaining substring is called a **token** or **lexeme**.
* For each lexeme in the input, the lexer outputs (1) the token's classification (token type, such as <tt>T_IDENTIFIER</tt>) and (2) any **attributes** (text, source position, etc.) that the parser requires to process that token.  Sometimes, the word **token** does not refer to the lexeme itself but rather to this \\(\langle\\)token type, attributes\\(\rangle\\) pair that is produced by the lexer.
* Typically, for each type of token, there is a **pattern** that describes all of the lexemes corresponding to that type of token.

## The lexer's API

We've discussed what a lexer does in theory.  But what does this look like in code?

For now, let's not worry about *how* a lexer tokenizes the input stream.  Instead, let's focus on what API the lexer provides to the parser.

Of course, the exact API provided by the lexer various from one compiler to the next.  However, a straightforward object-oriented API might look something like this.

```cpp
enum TokenType { T_VAR, T_IDENTIFIER, T_LPAREN, ... };

class Token
{
public:
    TokenType getTokenType();
    char *getText();
    unsigned long getLineNumber();
};

class Lexer
{
public:
    Lexer(istream & in);
    Token readNextToken();
};
```

When a `Lexer` is constructed, is is passed an input stream to tokenize.  The `readNextToken` function is called repeatedly; each call identifies the next token in the input, until the entire stream has been exhausted.  The `Token` object returned from this call provides the token's type (e.g., <tt>T_VAR</tt>), text, and line number (which the parser can use in error messages).

In the next post, we'll discuss Flex, which generates lexers.  Its generated API is not nearly this clean -- mostly for efficiency reasons.  Nevertheless, the code above is a good model for understanding lexers conceptually.

## Why lexers exist

At this point, it's probably worth discussing why lexers exist.  Again, the role of the lexer is to tokenize the input and produce a stream of tokens to serve as input for the parser.

Why is this necessary?  Why not have the parser read the source text directly?

The main answer is **simplicity**.  Imagine you want to parse a simplified JavaScript <tt>var</tt> declaration: <tt>var </tt>_identifier_<tt>;</tt>.

*Option 1.* Suppose you have a lexer, so the parser's input is a token stream.  To recognize such a <tt>var</tt> declaration, you must (1) match a <tt>T_VAR</tt> token, then (2) match a <tt>T_IDENTIFIER</tt> token, then (3) match a <tt>T_SEMICOLON</tt> token.

*Option 2.* Now, suppose you do not have a lexer, so the parser's input is a stream of characters.  To recognize such a <tt>var</tt> declaration, you must (1) match the letter <tt>v</tt>; (2) match the letter <tt>a</tt>; (3) match the letter <tt>r</tt>; (4) match zero or more whitespace characters and/or comments; (5) match an identifier, which means matching a letter, underscore, or dollar sign, followed by zero or more letters, digits, underscores, and/or dollar signs; (6) match zero or more whitespace characters and/or comments; then (7) match a semicolon.

Perhaps the most annoying part of Option 2 is ensuring that whitespace and comments are allowed in every permissible location.

Another argument against Option 2 is that most parsing algorithms simply cannot handle it.  Most parsing algorithms have a step which is something like, "Look at the next symbol in the input, and make a decision."  If the parser is reading tokens, <tt>for</tt> and <tt>fortune</tt> are completely different tokens -- <tt>T_FOR</tt> vs. <tt>T_IDENTIFIER</tt>.  If the parser is reading individual characters, <tt>for</tt> and <tt>fortune</tt> look very similar -- the first three characters are identical -- so the parser needs to see the fourth character before it can distinguish them.  Without going into detail, the need to "look ahead" by several symbols is problematic for many parsing algorithms.

Another argument for using a token stream is **efficiency**.  Lexers can use specialized string tokenization algorithms, which can be blazingly fast.  Parsing algorithms generally involve pushing symbols onto a stack, which is slower.  Tokenization reduces the number of symbols the parser has to process, which improves performance overall.

## What are the tokens?

Earlier, we assigned each lexeme a "token type," like <tt>T_VAR</tt>, <tt>T_IDENTIFIER</tt>, etc.  In the API code above, I suggested that the lexer could contain an <tt>enum</tt> of every possible token type.

So, what *are* the token types?  The exact set depends on what language is being compiled, but generally it's something like this:
* One token type for each keyword: <tt>for</tt>, <tt>if</tt>, <tt>while</tt>, etc.
* One token type for each kind of literal: numeric literal, string literal, etc.
* One token type for identifiers (names of functions, variables, etc.)
* One token type for each operator or symbol: <tt>+</tt> <tt>-</tt> <tt>\*</tt> <tt>=</tt> <tt>==</tt> <tt>?</tt> <tt>.</tt> etc.
* At least one token type for words and symbols that should never appear in a valid program

### Example: Tokens for JavaScript

To make this more concrete, let's enumerate all the token types for an early version of JavaScript (ECMAScript 2.0, 1998).  The names (<tt>T_VAR</tt>, <tt>T_IDENTIFIER</tt>, etc.) are arbitrary -- these are my names, and they are likely to vary from one JavaScript implementation to the next.  Nevertheless, the set of tokens in any JavaScript processor is likely to be fairly close.

#### Keywords

  |------------|------------|----------|---------|
  | T_BREAK    | T_FOR      | T_NEW    | T_VAR   |
  | T_CONTINUE | T_FUNCTION | T_RETURN | T_VOID  |
  | T_DELETE   | T_IF       | T_THIS   | T_WHILE |
  | T_ELSE     | T_IN       | T_TYPEOF | T_WITH  |

#### Literals

  |-------------------|
  | T_NULL            |
  | T_TRUE            |
  | T_FALSE           |
  | T_NUMERIC_LITERAL |
  | T_STRING_LITERAL  |

#### Identifiers

  |--------------|
  | T_IDENTIFIER |

#### Punctuators

  |------------|---------------|---------------|---------------|
  | T_EQ       | T_PERIOD      | T_PERCENT     | T_LT_LT_EQ
  | T_GT       | T_AND_AND     | T_LT_LT       | T_GT_GT_EQ
  | T_LT       | T_VBAR_VBAR   | T_GT_GT       | T_GT_GT_GT_EQ
  | T_EQ_EQ    | T_PLUS_PLUS   | T_GT_GT_GT    | T_LPAREN
  | T_LT_EQ    | T_MINUS_MINUS | T_PLUS_EQ     | T_RPAREN
  | T_GT_EQ    | T_PLUS        | T_MINUS_EQ    | T_LBRACE
  | T_EXCL_EQ  | T_MINUS       | T_ASTERISK_EQ | T_RBRACE
  | T_COMMA    | T_ASTERISK    | T_SLASH_EQ    | T_LBRACKET
  | T_EXCL     | T_SLASH       | T_AND_EQ      | T_RBRACKET
  | T_TILDE    | T_AND         | T_VBAR_EQ     | T_SEMICOLON
  | T_QUESTION | T_VBAR        | T_CARET_EQ    |
  | T_COLON    | T_CARET       | T_PERCENT_EQ  |

#### Invalid

  |--------------|--
  | T_RESERVED   | Future reserved words
  | T_UNEXPECTED | Unexpected character

### Additional considerations

In the list of tokens above, three things are worth noting.

1. In JavaScript, keywords like <tt>for</tt> and <tt>if</tt> are **reserved words**.  That is, they cannot be used as identifiers; you cannot have a variable named `if`.  This makes lexing easier, since <tt>if</tt> will always tokenize as a <tt>T_IF</tt> token and never as a <tt>T_IDENTIFIER</tt> token.  Some languages, including Fortran, do not have reserved words: `if (if < 5) print *, "Whoa"` is a valid Fortran statement, which compares the value of the variable named `if` to 5.  The lack of reserved words can make lexing and parsing Fortran quite difficult.

2. JavaScript does not allow a space between the equal signs in `if (a == 3)`.  We can enforce this in the lexer by defining <tt>==</tt> to be its own token (<tt>T_EQ_EQ</tt>).  The two equal signs in `if (a = = 3)` would tokenize as two separate <tt>T_EQ</tt> tokens, while the <tt>==</tt> in `if (a == 3)` would become a single <tt>T_EQ_EQ</tt> token.

3. All names -- variable names, function names, property names, etc. -- become <tt>T_IDENTIFIER</tt> tokens.  Generally speaking, the lexer does not try to determine whether a name refers to a variable, function, etc.; this is handled by a later stage of the compiler.

The last point is not always true.  For example, lexical analyzers for C and C++ must distinguish between identifiers and names that have been typedef'ed (see [link](https://pdos.csail.mit.edu/archive/l/c/roskind.html) for details).  This makes lexing and parsing messy.  Designers of other languages including Go have specifically tried to avoid introducing such complications ([link](https://talks.golang.org/2012/splash.article)).

## Up Next: Flex

So far, we have discussed *what* lexers do, but not *how* they do it.  Writing a lexer is not especially difficult, but there are tools that make it even easier.  In the next post, I'll discuss Flex, a tool that generates fast lexical analyzers from regular expressions.

## References

Lexical analysis is covered in detail in the two "standard" compiler textbooks:

* Aho, Lam, Sethi, and Ullman.  _Compilers: Principles, Techniques, and Tools_, Second Edition, Section 1.2.1, Section 2.6, and Chapter 3.

* Cooper and Torczon.  _Engineering a Compiler_.  Chapter 2.

It is also covered in the classic research reference:

* Aho and Ullman.  _The Theory of Parsing, Translation, and Compiling: Volume 1: Parsing_.  Section 1.2.2.

Lexical analysis is also covered in most introductory programming languages textbooks, including the "standard" texts:

* Scott.  _Programming Language Pragmatics_, Fourth Edition.  Sections 1.6.1 and 2.2.

* Friedman, Wand, and Haynes.  _Essentials of Programming Languages_.  Chapter 11.

Aho, Lam, Sethi, and Ullman refer to substrings of the input exclusively as "lexemes," reserving the word "token" to refer to \\(\langle\\)token type, attributes\\(\rangle\\) pairs.  However, most authors do not make this distinction.  Aho and Ullman (the same authors, in an earlier text) note that "It is the job of the lexical analyzer to group together certain terminal characters into single syntactic entities, called _tokens_.  ...  We shall call the pair (token type, pointer) a "token" also, when there is no source of confusion."  Likewise, Friedman et al. do not distinguish between lexemes and tokens: "_Scanning_ is the process of analyzing a sequence of characters into larger units, called _lexical items, lexemes,_ or _tokens_." (p. 375).

Cooper and Torczon do not define tokens and lexemes; they discuss lexers as recognizing "words."

Aho et al. observe that the words "scanner" and "lexer" are usually used interchangeably, although some compilers make a distinction: the scanner performs simple tasks like removing whitespace and comments, while the lexer recognizes and categorizes tokens.  Most modern compilers for recent languages do not separate these tasks, so most compiler writers today use "scanner" and "lexer" synonymously.  Cooper and Torczon prefer the word "scanner" over the word "lexer" as a name for the compiler component that performs lexical analysis.
