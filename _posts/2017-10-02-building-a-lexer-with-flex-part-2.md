---
layout: post
title:  "Building a Lexer with Flex, Part 2"
cover:  empty.jpg
date:   2017-10-02 00:00:00 ET
categories: compilers lexers
published: false
---

[The previous post]({% post_url 2017-09-24-building-a-lexer-with-flex-part-1 %}) provided an introduction to Flex, describing how Flex-generated lexers tokenize an input stream.  In this post, we'll discuss Flex's pattern syntax -- how to write a regular expression describing a particular type of token.  We'll also discuss two important Flex warnings, and why you shouldn't ignore them.

## Flex's Pattern Syntax

In [the previous post]({% post_url 2017-09-24-building-a-lexer-with-flex-part-1 %}), we discussed the format of a Flex input file.  The input file consists of three sections, separated by `%%`.  The second section consists of a list of **rules**, where each rule consists of a **pattern** followed by an **action**.  We saw several examples of patterns: <tt>[\t\v\f \n\r]\*</tt> matched whitespace; <tt>"break"</tt> matched that word exactly; and <tt>.</tt> matched any character except newline.  Now, we're ready to explore Flex's full pattern syntax.

Flex patterns are *regular expressions*.  Since regular expressions are used throughout Unix and are built into nearly every major programming language, I'll assume you've seen them before.  If not, there are plenty of books and online tutorials.

Unfortunately, the *exact* syntax of regular expressions tends to vary.  The regular expressions supported by C++'s <tt>std::regex</tt> are not exactly the same as the regular expressions in Vim.  Flex is no different: Some of its patterns will be familiar to you, but some other familiar patterns may be missing.

The following patterns are the most commonly used, and they're the ones we'll use in future blog posts.  For a full list of patterns, see the [Flex Manual](ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html).

|:----------------|:--
| `"break"`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       | Matches `break`.  A double-quoted string matches that string exactly.  Characters can be escaped using backslashes, as in C/C++ (e.g., `\r\n`).
| `break`         | Matches `break`.  Double-quotes are unnecessary if none of the characters have have special meanings in regular expressions.  If the string contains only letters, numbers, and escaped characters, it is safe to omit the double quotes.
| `[abc]`         | Matches `a`, `b`, or `c`.  A **character class** matches exactly one character; the allowable characters are listed between the square brackets.
| `[A-Za-z0-9_$]` | Matches any letter, or a decimal digit, or an underscore, or a dollar sign.  A hyphen in a character classes specifies a **range** of characters.  `A-Z` matches any character between `A` (ASCII 65) and `Z` (ASCII 90), which effectively matches any uppercase letter.
| `[^/*]`         | Matches any character **except** slash or asterisk.  <tt>[^</tt>&nbsp;&nbsp;<tt>]</tt> denotes a **negated character class**, which matches any character *except* those listed.  Negated character classes can contain ranges, too; `[^0-9]` matches any character that is not a decimal digit.

Patterns can be combined using the following operators:

|:------------------------|:--
| `(`*pattern*`)`         | Parentheses can be used for grouping.  `(x)` matches the same strings as `x`.
| *pattern*`*`            | Zero or more repetitions of *pattern*.
| *pattern*`+`            | One or more repetitions of *pattern*.
| *pattern*`?`            | Zero or one occurrences of *pattern* (i.e., something that is optional).
| *pattern*`{2,10}`       | Between 2 and 10 occurrences of *pattern*.
| *pattern\\(_1\\)pattern\\(_2\\)*      | *pattern\\(_1\\)* followed by *pattern\\(_2\\)* (concatenation).
| *pattern\\(_1\\)*`|`*pattern\\(_2\\)* | *pattern\\(_1\\)* or *pattern\\(_2\\)*

The operators above are listed from highest to lowest precedence.  For example, `*` has higher precedence than concatenation, so `cat*` is interpreted as `ca(t*)`, which matches `ca`, `cat`, `catt`, `cattt`, etc.  As another example, concatenation has higher precedence than `|`, so `cat|dog` is interpreted as `(cat)|(dog)`, which matches `cat` or `dog`.

Examples:
* `cat|[Dd]oggie` matches `cat`, `Doggie`, or `doggie`
* `ca(t|[Dd])og` matches `catog`, `caDog`, or `cadog`
* `ca*t` matches `ct`, `cat`, `caat`, `caaat`, `caaaat`, `caaaaat`, etc.
* `ca+t` matches `cat`, `caat`, `caaat`, `caaaat`, `caaaaat`, etc. (but not `ct`)
* `ca{2,4}t` matches `caat`, `caaat`, or `caaaat`
* `c(at)*` matches `c`, `cat`, `catat`, `catatat`, etc.
* `c(a|b)*` matches `c`, `ca`, `cb`, `caa`, `cab`, `cba`, `cbb`, `caaa`, `caab`, `caba`, `cabb`, `cbaa`, `cbab`, `cbba`, `cbbb`, etc.

Some people find the last pattern confusing: `c(a|b)*`.  How can it match the string `cbab`?  Think of it like this:
* *Step 1.* Match the letter `c`.
* *Step 2.* Repeat zero or more times, as long as possible:
  * Match either `a` or `b`.

In other words, it's the same as matching `c` or `c(a|b)` or `c(a|b)(a|b)` or `c(a|b)(a|b)(a|b)` or ...

## Two examples in JavaScript

**Whitespace.** In JavaScript, spaces, tabes, vertical tabs, form feeds, newlines, and carriage returns are treated as whitespace (and skipped).  We saw a regular expression for this in the previous post: `[ \t\v\f\n\r]*`.  This is a character class matching six possible characters (note that it starts with a space), and it is repeated zero or more times to match arbitrary amounts of whitespace.

**Identifiers.** In a JavaScript identifier, (1) every character is either a letter, decimal digit, underscore, or dollar sign; (2) the first character is not a decimal digit; and (3) the string is not a reserved word like <tt>if</tt> or <tt>break</tt>).  In our Flex input, we will match identifiers using the pattern `[A-Za-z$_][A-Za-z0-9$_]*`.  This does not satisfy condition 3 -- `break` will still match our identifier pattern.  We'll talk about this later; essentially, we will avoid the problem by taking advantages of this fact: if the same string matches multiple rules, Flex will match whichever rule *is listed earliest* in the file.

## Name definitions: Demo #3

The regular expressions we've seen so far have been simple.  However, some tokens have a more complex structure, and a regular expression to describe them can become unwieldy.

In a Flex file, the first section (above the first `%%`) can contain **name definitions**, where a regular expression is given a name.  Later in the file, `{name}` can be used in place of that regular expression.  For example, if the definitions section contained the name definition `something  b|c`, then a rule could have the pattern `a{something}*d`.  This would be the same as `a(b|c)*d`.

To illustrate why name definitions are useful, consider decimal numeric literals in JavaScript.  Notice how the name definitions make this definition readable, even though it is quite complex.  (And imagine what awful regular expression would result if Flex did not have name definitions...)

{% highlight plaintext %}
{% include_relative code/flex/demo3/lexer.l %}
{% endhighlight %}

As an aside, this example uses three tricks that aren't useful in production but are good for demonstration code:
* We defined a `main` function in the user code section.  When we run the `flex` tool to generate lexer.cpp, the user code section will be copied verbatim into lexer.cpp, so `g++ lexer.cpp` will produce a useful executable.
* We didn't use `%option reentrant`, so we didn't need the code to create a `yyscan_t` like we did in the previous demos.
* Since we didn't specify a buffer to scan before calling `yylex`, the lexer defaulted to reading from standard input.
* In the lexer rules, we displayed the tokens but did not return values, so the lexer discarded the tokens.  This means the only time `yylex` will return a value is when it reaches the end of the input and returns 0.

Since this lexer reads from standard input, you'll have to enter values manually, then press Ctrl+D when you're done (Ctrl+D corresponds to ASCII 4, or "end of transmission" -- which represents end-of-file on Unix systems).  Here's an example session:

```
$ ./demo3 
1.2345
Decimal Literal: 1.2345
.6
Decimal Literal: .6
3.2e+42
Decimal Literal: 3.2e+42
      8e3
Decimal Literal: 8e3
  bad
Unexpected Character: b
Unexpected Character: a
Unexpected Character: d
```

## The default rule, and <tt>%option warn nodefault</tt>

In each of our Flex demos, we have included a <tt>.</tt> rule at the end of the file.  This was designed to match characters that no other rule could match.  Including a rule like this is a best practice.  A lexer should *always* have rules to match every possible input string.  Usually, one token type is reserved for "unexpected" characters.  It might be called T_UNEXPECTED, or T_UNKNOWN, or something like that.  When the lexer sees a character that doesn't "make sense", it returns this token.  Then the caller -- the parser, or whoever invoked the lexer -- can issue an error message or take some other corrective action.

In our previous demos, we have included the directive `%option warn nodefault`.  Let's discuss this in more detail.

The `nodefault` option disables the *default rule*.  Without this, the generated lexer implicitly includes a "default rule": if input does not match any rule listed, it is echoed to standard output.  This is almost never a good idea in production.

When the default rule is *disabled*, the lexer won't echo unexpected characters to standard output.  Instead, it will terminate the program.  This is also undesirable.

The directive `%option warn` enables Flex warnings.  This is generally a good idea, but it's especially useful when combined with `%option nodefault`.  If the user-specified rules are incomplete -- if there's any input that does not match any rule, and hence will terminate the program -- the `flex` tool will issue a warning.

For example, consider the following Flex input file.

{% highlight plaintext %}
{% include_relative code/flex/incomplete1/lexer.l %}
{% endhighlight %}

Notice the warning when it is compiled:

```
$ flex -olexer.cpp --header-file=lexer.hpp lexer.l
lexer.l:7: warning, -s option given but default rule can be matched
```

This input file only contains a rule to match letters.  Digits, punctuation, newlines, non-ASCII characters... there is no rule for any of those.

Let's add a <tt>.</tt> rule.

{% highlight plaintext %}
{% include_relative code/flex/incomplete2/lexer.l %}
{% endhighlight %}

```
flex -olexer.cpp --header-file=lexer.hpp lexer.l
lexer.l:8: warning, -s option given but default rule can be matched
```

Wait... that didn't fix it?

Remember, the <tt>.</tt> regular expression matches any character *except newline*.  So this lexer has a rule for every possible character *except newline*.  If we change the last pattern to `.|\n`, it will fix it.

## Rules that cannot be matched

Flex will also issue a warning if a rule can never be matched.  Consider the following.

{% highlight plaintext %}
{% include_relative code/flex/unnecessary1/lexer.l %}
{% endhighlight %}

```
$ flex -olexer.cpp --header-file=lexer.hpp lexer.l
lexer.l:7: warning, rule cannot be matched
```

Can you see what the problem is?

A single newline character will match both the first rule (for whitespace) and the last rule (for newline).  When the same string matches multiple rules, the one that is listed first takes precendence.  So, the last rule -- the rule for `\n` -- will never be matched.

Here's a common mistake that will produce this warning:

{% highlight plaintext %}
{% include_relative code/flex/unnecessary2/lexer.l %}
{% endhighlight %}

```
$ flex -olexer.cpp --header-file=lexer.hpp lexer.l
lexer.l:6: warning, rule cannot be matched
```

The keyword `break` matches our pattern for identifiers, and it also matches the pattern <tt>"break"</tt>.  Since the identifier pattern is listed earlier, that rule will take precedence.

If we swap the order -- list the <tt>"break"</tt> rule first -- then that rule will take precedence, so the string `break` will match that rule rather than the identifier rule.

## Up Next: Lexical States, Text, Line Numbers, and a JavaScript Lexer

We're getting close to being able to build a lexer for JavaScript.  Next, we'll talk about *lexical states*, which are useful for matching string literals.

## Download the source code

## Demo #3

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/flex/demo3/lexer.l">lexer.l</a> | 30 lines |
| **Makefile:**                | <a href="{{site.baseurl}}/_posts/code/flex/demo3/Makefile">Makefile</a>&nbsp;&nbsp; | <small>(GNU Make on Linux/macOS)</small> |

## Flex spec missing a default rule

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/flex/incomplete1/lexer.l">lexer.l</a> | 8 lines |
| **Makefile:**                | <a href="{{site.baseurl}}/_posts/code/flex/incomplete1/Makefile">Makefile</a>&nbsp;&nbsp; | <small>(GNU Make on Linux/macOS)</small> |

## References

As before, the [Flex Manual](ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html) is the recommended reference for more information on the topics in this post.
