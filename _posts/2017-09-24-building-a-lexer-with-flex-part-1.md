---
layout: post
title:  "Building a Lexer with Flex, Part 1"
cover:  empty.jpg
date:   2017-09-24 00:00:00 ET
categories: compilers lexers
---

In [the previous post]({% post_url 2017-09-09-lexical-analysis %}), I described what lexical analysis is, what a token is, and at a broad level, what sort of API a lexer provides.  In this post, I'll describe how to build a lexer in C/C++ using [Flex](https://github.com/westes/flex), the Fast Lexical Analyzer Generator.

## Lex and Flex

Recall from the previous post that every token type corresponds to a pattern.  For example, in JavaScript, the T_VAR token type always corresponded to the lexeme <tt>var</tt>, while the T_IDENTIFIER token type corresponded to any identifier (a string of one or more characters, where (1) every character is either a letter, decimal digit, underscore, or dollar sign; (2) the first character is not a decimal digit; and (3) the string is not a reserved word like <tt>if</tt> or <tt>while</tt>).

In general, a lexical analyzer can be described by a list of rules, something like this.

|:---------------------------------|:---------------------|
| **When you see this pattern...** | **Take this action...**
| (whitespace)                     | Skip it
| <tt>=</tt>                       | It is a T_EQ token
| <tt>==</tt>                      | It is a T_EQ_EQ token
| <tt>break</tt>                   | It is a T_BREAK token

It's not too difficult to hand-write a lexer.  However, several decades ago, computer scientists realized that source code for a lexical analyzer could be *generated* from a description like this.  This has several advantages: the description is generally short, easy to write, and easy to understand; and the generated code can be exceptionally fast, often much faster than a hand-written lexer.

The idea of generating lexers originated with a tool called Lex, developed at Bell Labs in the 1970s.  Since then, numerous Lex-like tools have been written.  Lex has become a standard Unix development tool, and Lex-like tools have emerged for nearly every major programming language.

In this and the following blog posts, I will focus on a Lex clone called Flex.  The original Lex tool is no longer in use (it was from 1975!); Flex is a modern successor that is standard on Linux and macOS.  All of the code in these posts will be compatible with Flex 2.5.35.  That version is old, but it's still the version that ships with macOS.  Fortunately, newer versions of Flex are backward-compatible, so all of the code in this post will work with newer versions of Flex as well.

## Demo #1: Hello, Flex

The idea behind Flex is simple.  A Flex input file contains a list of regular expressions, each of which describes the pattern for a particular token type.  After that regular expression is a snippet of C (or C++) code that is executed when that pattern is matched.  Generally, it will return a value to the parser indicating what token type was matched.

The <tt>flex</tt> tool reads this input file and generates C (or C++) code for a lexical analyzer.

As a concrete example, let's start with the following Flex input file.  (A "real" Flex lexer will look quite different from this; this example is only to help explain how Flex works.)

{% highlight plaintext %}
{% include_relative code/flex/demo1/lexer.l %}
{% endhighlight %}

Before explaining what this input file means, exactly, let's turn it into a working program.  When the <tt>flex</tt> tool is run Flex on this input file...

`flex -olexer.cpp --header-file=lexer.hpp lexer.l`

...it will generate two new files: lexer.cpp and lexer.hpp.  Let's write a main program that uses the Flex-generated lexer to tokenize some strings.  (Just skim it for now; I'll explain the details later.)

{% highlight c++ %}
{% include_relative code/flex/demo1/main.cpp %}
{% endhighlight %}

Now, let's compile this into a program called `demo1`:

`g++ -odemo1 main.cpp lexer.cpp`

When we run it, we get the following output, which I will explain momentarily:

```
Tokens for "" are:
0 

Tokens for "break===

x" are:
4 3 2 1 5 0

Tokens for "=====" are:
3 3 2 0 

Tokens for "=
=break" are:
2 1 2 4 0 

Tokens for "?" are:
5 0 

```

### The C++ driver code (main.cpp)

First, let me describe the C++ driver code.

The `main` function calls `describe_tokens` on several example strings.  The `describe_tokens` function receives a string, creates a lexer, uses the lexer to tokenize the string, and displays some information about what tokens were found.

Of course, the interesting part is how we tokenize the string in the `describe_tokens` function.
* First, we create an instance of the lexer.  We do this by declaring a variable of type `yyscan_t` (remember, the words "lexer" and "scanner" are often used interchangeably) and calling `yylex_init` to initialize it.  If `yylex_init` fails, it sets the global `errno` variable, so we can invoke `perror` (from stdio.h) to display a readable error message, which is most likely "out of memory."
* Next, we need to tell the lexer what input we want to tokenize.  The text to be tokenized must be placed in a buffer that (1) can be overwritten as the input is processed and (2) ends with two NUL characters (ASCII 0).  The easiest way to create such a buffer is to call `yy_scan_string`, which creates a new buffer and copies a string into it.  If `yy_scan_string` fails (e.g., if the process is out of memory), the program terminates.  (As you can imagine, we probably won't use `yy_scan_string` in production...)
* We call the function `yylex` repeatedly to tokenize the input.  Each time `yylex` is called, the lexer identifies the next token in the input and returns an integer, which is intended to represent the token type.  When no tokens remain, `yylex` returns 0.
* Finally, we must call `yy_delete_buffer` and `yylex_destroy` to free the memory for the buffer and the scanner, respectively.

### The Flex input file (lexer.l)

Now, let's look at the Flex input file and try to understand what our lexer is doing.  Look again at the five rules in our Flex input file:

```
[ \t\v\f\n\r]*  { return 1; }
"="             { return 2; }
"=="            { return 3; }
"break"         { return 4; }
.               { return 5; }
```

Each rule describes the pattern for a particular token.  This is followed by C code in curly braces.  As we saw in the previous section, the driver calls `yylex` repeatedly to tokenize an input string; the `return` statement in curly braces indicates what value `yylex` will return for that pattern.

The first pattern matches whitespace characters: tab, vertical tab, form feed, newline, and carriage return.  The <tt>\*</tt> means, "Match a string or zero or more such characters."  For example, this pattern will match a single space, or two spaces, or <tt>\r\n</tt>, or several spaces followed by a form feed and more spaces, etc.  The next three patterns are self-explanatory: they match a particular string.  The <tt>.</tt> pattern matches any character except newline (<tt>\n</tt>, ASCII 10).

Now, when our program was run, it contained this output:

```
Tokens for "break===

x" are:
4 3 2 1 5 0
```

When our lexer tokenized the string <tt>break===\n\nx</tt>, it produced five tokens: the first matched the rule with the code `return 4`, the second matched the rule with `return 3`, etc.  (What about the 0 at the end of the output?  Remember that `yylex` returned 0 to indicate "end of input.")  Looking at the rules above, the return values 4 3 2 1 5 correspond to this tokenization:

$$
\underset{\color{gray}{4}}{\texttt{break}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{3}}{\texttt{==}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{2}}{\texttt{=}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{1}}{\texttt{\n\n}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{5}}{\texttt{x}}\,\color{gray}{\biggr\rvert}\,%
$$

It's important to realize that *Flex matches its input character by character, not line by line*.  In this example, the two newlines were grouped into a single token.  In fact, if there were a space between the word <tt>break</tt> and the newline, this token would have consisted of three characters: space, newline, newline.  In other words, there's nothing special about newline characters -- they're matched just like any other character.

Now, the tokenization we just saw -- 4 3 2 1 5 -- seems reasonable.  However, notice that the last pattern (<tt>.</tt>) matches any single, non-newline character.  Couldn't <tt>break</tt> be *five* tokens, each a single character?  Also, the second rule matches the character <tt>=</tt>, but <tt>.</tt> also matches <tt>=</tt>.  Why did the lexer choose the "=" rule instead of the <tt>.</tt> rule?  To answer these questions, we need to understand how Flex matches its input.

## How a Flex-generated lexer matches input

When `yylex` is called for the first time, it begins reading characters from the beginning of the input.  It identifies a token as follows:

* Starting from the beginning, it finds the **longest** prefix that matches **any** rule in the grammar.  Then, it executes the C code associated with that rule.
* If the longest prefix matches *more than one rule*, it chooses whichever rule appears first.

So what does the C code in curly braces do?

* A `return` statement in the C code causes the `yylex` function to return that value.  The next time `yylex` is called, it will start matching from the next character in the input.
* If the C code does not return, then the current token is discarded, and the lexer searches for a new token beginning with the next character in the input.

To make this clearer, let's work through some examples.

**Example 1**.  Continuing the example from earlier, consider how <tt>break===\n\nx</tt> is tokenized:
* The driver calls `yylex` for the first time.  The lexer tries to find the longest prefix that matches a rule.
  * <tt>break===\n\nx</tt> does not match any rule.
  * <tt>break===\n\n</tt> does not match any rule.
  * <tt>break===\n</tt> does not match any rule.
  * <tt>break===</tt> does not match any rule.
  * <tt>break==</tt> does not match any rule.
  * <tt>break=</tt> does not match any rule.
  * <tt>break</tt> matches the rule for "break", so the C code is executed, and `yylex` returns 4.
* The driver calls `yylex` again.  The remaining input is <tt>===\n\nx</tt>.  The lexer tries to find the longest prefix that matches a rule.
  * The longest prefix that matches a rule is <tt>==</tt>.  This matches the rule for "==", and the lexer returns 3.
* The driver calls `yylex` again.  The remaining input is <tt>=\n\nx</tt>.  The lexer tries to find the longest prefix that matches a rule.
  * The longest prefix that matches any rule is <tt>=</tt>.  However, <tt>=</tt> matches the rule for "=" as well as the rule for <tt>.</tt> (any non-newline character).  Since the rule for "=" is listed before the rule for <tt>.</tt>, the lexer chooses the rule for "=" and returns 2.
* The driver calls `yylex` again.  The remaining input is <tt>\n\nx</tt>.  The lexer tries to find the longest prefix that matches a rule.
  * <tt>\n\n</tt> matches the rule for <tt>[\t\v\f \n\r]\*</tt>, so `yylex` returns 1.
* The driver calls `yylex` again.  The remaining input is <tt>x</tt>.  The lexer tries to find the longest prefix that matches a rule.
  * <tt>x</tt> matches the rule for <tt>.</tt> (any non-newline character), so the lexer returns 5.
* The driver calls `yylex` again.  No input remains, so it returns 0.

Now, we have the answers to the questions I asked earlier.  Why is <tt>break</tt> not five tokens?  Flex always matches the longest possible string.  Since the first five characters (<tt>break</tt>) match a rule, it prefers that over matching just the first character, <tt>b</tt>.

**Example 2**.  Consider how <tt>=====</tt> is tokenized:
* The lexer matches <tt>==</tt> and returns 3.
* The lexer matches <tt>==</tt> and returns 3.
* The lexer matches <tt>=</tt> and returns 2.

Remember: Flex always matches the longest prefix possible.

**Example 3**.  The string <tt>breakbreak</tt> is tokenized as two <tt>break</tt> tokens.  This is fine for now.  However, in the future, we will add a rule for JavaScript identifiers; at that time, we'll want this to tokenize as a single identifier <tt>breakbreak</tt> instead of two <tt>break</tt> tokens.  But again, two <tt>break</tt> tokens is fine for now.

## Flex input files: The structure

Before going further, let's explain the format of a Flex input file.

A Flex input file is divided into three sections.  The sections are separated by the delimiter `%%` on a line by itself.

<i>Definitions</i><br/>
<tt>%%</tt><br/>
<i>Rules</i><br/>
<tt>%%</tt><br/>
<i>User Code</i><br/>

### The Definitions section

In our example above, the <i>Definitions</i> section contains two things.

The first line is a comment.  Note that this comment starts at the beginning of the line.  *Unindented* comments in the <i>Definitions</i> section are copied directly into the output file (in our case, lexer.cpp).

The next lines contain <tt>%option</tt> directives.  Each directive can specify one or more options; it doesn't matter if the options are given on the same line or separate lines, and it doesn't matter what order the options are specified.  We used the following options:

* `%option reentrant` creates a reentrant lexer, so lexers can be created and used concurrently on several threads.  This ensures that the generated lexer does not write to any global variables.
* `%option noyywrap` tells Flex that our lexer does not require a `yywrap` function.  This function is used to supply more input when the end-of-input is reached; we don't need to do this.
* `%option warn` and `%option nodefault` turn on warnings and disable the *default rule*.  Together, these options guarantee that Flex will issue a warning if the list of rules is *incomplete* -- i.e., if there are inputs that will not match any rule.  (We'll discuss this more in a later post on patterns in Flex.)

There are a few other things that can appear in the Definitions section; we'll see some of them in later posts.

### The Rules section

Each line of the rules section has the form

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>pattern</i>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>action</i>

The pattern must not be indented, and the action must begin on the same line as the pattern.  C-style comments can be included in the rules section, but they cannot appear at the beginning of a line (they must be indented).

Our example above contained the rule
```
"break"         { return 4; }
```
where, according to our terminology here, `"break"` is the pattern and `{ return 4; }` is the action.

We've seen a few examples of patterns already.  I'll give the full syntax for patterns in a later post, where I can describe them in more detail.

### The User Code section

The <i>User Code</i> section is copied verbatim into the output file (lexer.cpp).  In our example, this section was empty, although it is sometimes used to define utility functions used by the code in the Rules section.  (A <tt>main</tt> function could also be defined here.)

## Demo #2: Symbolic names for token types, and skipping whitespace

In Demo #1 above, the `yylex` function returned 1 when it matched a whitespace, 2 when it matched "=", etc.  Let's create a header file where we define symbolic names for those token types.  While we're at it, I'm going to change the token numbers: instead of 2, 3, and 4, we'll use 257, 258, and 259.  I'll explain why shortly.

{% highlight c++ %}
{% include_relative code/flex/demo2/lexer-decls.hpp %}
{% endhighlight %}

To use those names in our Flex input file, we need to make sure that header is included in the generated <tt>lexer.cpp</tt>.  In the first section of the Flex input file, we will add an <tt>#include</tt> directive, enclosed in `%{` `}%` delimeters.  Any code in such a block will inserted into the generated <tt>lexer.cpp</tt>, close to the top of the file.

While we're at it, let's make two more changes.
* We'll change the action for the whitespace rule to an empty block, `{ }`.
* We'll change the action for the <tt>.</tt> rule to `return yytext[0]`.

{% highlight plaintext %}
{% include_relative code/flex/demo2/lexer.l %}
{% endhighlight %}

Now, what have we done?

### If a pattern has an empty action, those tokens are discarded

When the rule for a token is empty, Flex will **discard** that token.  In other word, when the lexer matches that pattern, `yylex` will **not** return that token.  Instead, it will skip over it and start searching for the next token.

For example, the string <tt>break===\n\nx</tt> will now tokenize as follows:

$$
\underset{\color{gray}{T\_BREAK}}{\texttt{break}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{T\_EQ\_EQ}}{\texttt{==}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{T\_EQ}}{\texttt{=}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{(discarded)}}{\texttt{\n\n}}\,\color{gray}{\biggr\rvert}\,%
\underset{\color{gray}{'x'}}{\texttt{x}}\,\color{gray}{\biggr\rvert}\,%
$$

### yytext[0] is the first character of the token text

We changed the action for <tt>.</tt> to `return yytext[0]`.  What is this?

Inside an action, `yytext` is a `char *` pointing to the text of the current token (and `yyleng` is an `int` containing its length, although we didn't use that).  So, `return yytext[0]` means, "Return the ASCII code of the first character of this token's text."  Since the last two rules only match a single character, this means `yylex` will return that character's ASCII code.

This means (1) when a single character is matched by the last rule, `yylex` will return its (extended) ASCII code -- a value between 1 and 255 -- and (2) when "=", "==", or "break" is matched, `yylex` will return T_EQ (257), T_EQ_EQ (258), or T_BREAK (259).

This is why we gave our symbolic constants (T_EQ_EQ, etc.) values larger than 255.

(But why 257?  Later, I'll talk about Yacc, a parser generator.  When Yacc generates its token numbers, they always start at 257.  0 is the end-of-input token, 1 through 255 are used for single characters (as above), and 256 is an "error" token.  So, custom tokens are numbered from 257.  We don't have to follow this convention, since we're not using Yacc -- we could have started from 256 -- but I did anyway.)

### Updating main.cpp

Now, we can also use our symbolic names in main.cpp.  Let's add a function that returns a readable description of each token type.

{% highlight c++ %}
{% include_relative includelines filename='code/flex/demo2/main.cpp' start=14 count=17 %}
{% endhighlight %}

Now, we can change the output loop, so it will output this description rather than the token number.

{% highlight c++ %}
{% include_relative includelines filename='code/flex/demo2/main.cpp' start=45 count=9 %}
{% endhighlight %}

Now, our program produces the following output:

```
Tokens for "" are:
(end of input) 

Tokens for "break===

x" are:
break == = x (end of input) 

Tokens for "=====" are:
== == = (end of input) 

Tokens for "breakbreak" are:
break break (end of input) 

Tokens for "?" are:
? (end of input) 

```

## Up Next: Text, Line Numbers, Patterns, and a JavaScript Lexer

So far, we've seen how Flex can match some simple tokens, but we haven't seen enough to build a "real" lexer yet.  But what we have so far is a good foundation.  After just a few more posts, we'll have covered enough about Flex to build a lexer for JavaScript.

## Download the source code

### Demo #1

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/flex/demo1/lexer.l">lexer.l</a> | 15 lines |
|                               | <a href="{{site.baseurl}}/_posts/code/flex/demo1/main.cpp">main.cpp</a> | 48 lines |
|                               |                                                                            | Total: 63 lines |
| **Makefile:**                | <a href="{{site.baseurl}}/_posts/code/flex/demo1/Makefile">Makefile</a>&nbsp;&nbsp; | <small>(GNU Make on Linux/macOS)</small> |

### Demo #2

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/flex/demo2/lexer.l">lexer.l</a> | 18 lines |
|                               | <a href="{{site.baseurl}}/_posts/code/flex/demo2/lexer-decls.hpp">lexer-decls.hpp</a> | 13 lines |
|                               | <a href="{{site.baseurl}}/_posts/code/flex/demo2/main.cpp">main.cpp</a> | 69 lines |
|                               |                                                                            | Total: 100 lines |
| **Makefile:**                | <a href="{{site.baseurl}}/_posts/code/flex/demo2/Makefile">Makefile</a>&nbsp;&nbsp; | <small>(GNU Make on Linux/macOS)</small> |

## References

The [Flex Manual](ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html) is the official documentation for Flex and is quite good.

Lex was introduced in M.E. Lesk and E. Schmidt, "Lex -- A Lexical Analyzer Generator," Computing Science Technical Report 39, AT&T Bell Laboratories, Murray Hill, NJ (1975).  Available [online](http://dinosaur.compilertools.net/lex/).

POSIX-compliant operating systems are required to include a <tt>lex</tt> tool.  The POSIX.1-2008/IEEE 1003.1-2008 specification for <tt>lex</tt> is available [online](http://pubs.opengroup.org/onlinepubs/9699919799/utilities/lex.html).  (Note: The demonstration code in this blog post is Flex-specific; it will **not** work with POSIX <tt>lex</tt> unless it is modified to eliminate some Flex-specific functionality, like <tt>%option reentrant</tt>.)
