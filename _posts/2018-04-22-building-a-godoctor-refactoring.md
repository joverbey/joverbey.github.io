---
layout: post
title:  "Building a Go Doctor Refactoring"
cover:  empty.jpg
date:   2018-04-22 12:00:00 ET
categories: refactoring golang godoctor
---

In this post, I'll give an overview of how to create a new refactoring for the
[Go Doctor](http://www.gorefactor.org/), which refactors
[Go](http://golang.org) source code.  We'll build a small command-line tool
that adds a copyright header to a Go source file.

## Go Doctor

The [Go Doctor](http://www.gorefactor.org/) is a refactoring tool for the [Go
programming language](http://golang.org).  It was designed to be easy to add
new refactorings, and it's equally easy to create new tools that perform source
code modifications using the Go Doctor infrastructure.  The [Go Doctor source
code](http://www.github.com/godoctor/godoctor/) is open source, and GoDoc is
available for the [Go Doctor
API](https://godoc.org/github.com/godoctor/godoctor/).  This post provides some
starter code illustrating how to start building a new refactoring.

## A Tool to Add a Copyright Header

To illustrate how a Go Doctor-based tool is built, we'll create a tool that
inserts a copyright header into a Go source file.  To start, we'll check out
the source code for the tool, compile it, and run it.  After you're comfortable
using the tool, we'll look at the source code.

### Checking Out the Source Code and Installing the Tool

Make sure your GOPATH is set, then <tt>go get</tt> the source code and install the
<tt>goaddcopyright</tt> binary:

```console
$ go get github.com/godoctor/godoctor
$ go get github.com/joverbey/goaddcopyright
$ cd $GOPATH/src/github.com/joverbey/goaddcopyright
$ go install
```

### Running the Tool

The <tt>goaddcopyright</tt> tool uses the same command-line driver as the <tt>godoctor</tt>
tool.  Here, we'll show some typical command lines.

#### Show Usage

The <tt>-help</tt> flag displays usage information.

```console
$ $GOPATH/bin/goaddcopyright -help
Add Copyright Header

Usage: goaddcopyright [<flag> ...]

Each <flag> must be one of the following:
    -complete Output entire modified source files instead of displaying a diff
    -doc      Output documentation (install, user, man, or vim) and exit
    -file     Filename containing an element to refactor (default: stdin)
    -json     Accept commands in OpenRefactory JSON protocol format
    -list     List all refactorings and exit
    -pos      Position of a syntax element to refactor (default: entire file)
    -scope    Package name(s), or source file containing a program entrypoint
    -v        Verbose: list affected files
    -vv       Very verbose: list individual edits (implies -v)
    -w        Modify source files on disk (write) instead of displaying a diff
```

#### Refactor "Hello World"

Now, let's use the tool to refactor a simple Go program. We'll use one of the
tool's test cases to illustrate how it's used.

```console
$ cd $GOPATH/src/github.com/joverbey/goaddcopyright/refactoring/testdata/addcopyright/001-helloworld
$ cat main.go
```

You should see that main.go looks like this (ignore the strange comment for now):

```go
package main

import "fmt"

func main() { //<<<<<addcopyright,5,1,5,1,Your Name Here,pass
        fmt.Println("Hello, world!")
}
```

A minimal command line to refactor a file looks like this.

```console
$ goaddcopyright -file hello.go "My Name"
Defaulting to package scope github.com/joverbey/goaddcopyright/refactoring/testdata/addcopyright/001-helloworld for refactoring (provide an explicit scope to change this)
diff -u main.go main.go
--- main.go
+++ main.go
@@ -1,3 +1,4 @@
+// Copyright 2018 My Name.  All rights reserved.
 package main
 
 import "fmt"
```

The tool prints informational messages, errors, and warnings on standard error.
If it can successfully refactor the file (i.e., without any errors), it will
print a patch file (i.e., a [unified
diff](http://www.gnu.org/software/diffutils/manual/html_node/Unified-Format.html))
on standard output and exit with code 0.

In the output above, the first line is a warning, printed to standard error,
and the remaining lines are the patch file.  (It is safe to ignore the
"Defaulting to package scope" warning for a refactoring that only changes a
single file, like our Add Copyright refactoring.  Go Doctor's Rename
refactoring may change multiple files; this is when it is important to set the
scope correctly.  See the [Go Doctor
documentation](http://gorefactor.org/doc.html) for more details.)

The command above printed a patch file, which is fine for previewing what the
refactoring will do, but it didn't actually change any source code.  To do
that, use the following commands:

```console
$ goaddcopyright -file main.go "My Name" > main.go.patch
$ patch -p0 -i main.go.patch
```

Now, if you <tt>cat main.go</tt>, you'll see that the copyright header has been added
at the top of the file:

```go
// Copyright 2018 My Name.  All rights reserved.
package main

import "fmt"

func main() { //<<<<<addcopyright,5,1,5,1,Your Name Here,pass
        fmt.Println("Hello, world!")
}
```

The file we just changed, main.go, is one of the tool's test cases.  We should
undo the refactoring so we don't break its tests!  The refactoring can be
undone by applying the patch in reverse (using the <tt>-R</tt> switch):

```console
$ patch -p0 -R -i main.go.patch
patching file main.go
```

If you don't like using patches, you can run <tt>goaddcopyright</tt> with the <tt>-w</tt>
switch to directly overwrite files.  However, there is no easy way to undo this
if something goes wrong, so make sure the project is under version control.

For previewing what the refactoring will do, you can also run <tt>goaddcopyright</tt>
with the <tt>-complete</tt> switch to output the entire file after it has been
refactored.  Sometimes, this is easier to read than a patch file.

If the <tt>goaddcopyright</tt> command line seems cumbersome, realize that most people
don't refactor code using command line tools: They refactor from inside a text
editor or IDE.  The [Go Doctor Vim
plug-in](http://gorefactor.org/install.html#install-vim) allows you to refactor
code (and undo refactorings!) from inside the Vim editor with just a few
keystrokes.  However, for the purposes of this post, we'll focus exclusively on
the command line.

#### When Refactoring Fails

After our Add Copyright tool reads in a Go program, if it finds the word
"Copyright" in a comment (with that exact capitalization), it will not add
an additional copyright header.  Instead, it will issue an error and exit.

To illustrate this, try the following commands:

```console
$ cd $GOPATH/src/github.com/joverbey/goaddcopyright/refactoring/testdata/addcopyright/003-error1
$ cat main.go
// This file already contains a Copyright comment.
package main

import "fmt" //<<<<<addcopyright,3,1,3,1,Your Name Here,fail

func main() {
        fmt.Println("Hello, world!")
}
$ goaddcopyright -file main.go "My Name"
main.go:1:33: Error: An existing copyright was found.
$ echo $?
3
```

Notice that the <tt>goaddcopyright</tt> tool exited with code 3 and did not output a
patch.

## Overview of the Code

Now that we've seen what the <tt>goaddcopyright</tt> tool does, let's look at its
source code.  The source tree looks like this:

```
$GOPATH/src/github.com/joverbey/goaddcopyright/
├── main.go                        Command line driver
└── refactoring/
    ├── addcopyright.go            Refactoring implementation
    ├── addcopyright_test.go       Unit tests
    └── testdata/
        └── addcopyright/
            ├── 001-helloworld/    Test case
            │   ├── main.go
            │   └── main.golden
            ├── 002-noname/        Test case
            │   ├── main.go
            │   └── main.golden
            ├── 003-error1/        Test case
            │   └── main.go
            └── 004-error2/        Test case
                └── main.go
```

There are only three "interesting" files: main.go (the program entrypoint),
addcopyright.go (the refactoring itself), and addcopyright_test.go (the unit
tests).  The rest of the files are in the testdata directory; as you saw above,
these are tiny Go programs used by the unit tests.

## The Refactoring: addcopyright.go

All of the interesting work is in addcopyright.go.  Skim the entire file (76
lines), then we'll describe all the pieces.

{% highlight go linenos %}
// Copyright (C) 2018 Jeffrey L. Overbey.  Use of this source code is governed
// by a BSD-style license posted at http://blog.jeff.over.bz/license/
package refactoring

import (
        "fmt"
        "strconv"
        "time"

        "github.com/godoctor/godoctor/analysis/names"
        "github.com/godoctor/godoctor/refactoring"
        "github.com/godoctor/godoctor/text"
)

var CurrentYear string = strconv.Itoa(time.Now().Year())

type AddCopyright struct {
        refactoring.RefactoringBase
}

func (r *AddCopyright) Description() *refactoring.Description {
        return &refactoring.Description{
                Name: "Add Copyright Header",
                //          ----+----1----+----2----+----3----+----4----+----5
                Synopsis:  "Add a copyright header to a file",
                Usage:     "addcopyright <text>",
                Multifile: false,
                Params: []refactoring.Parameter{ {
                        Label:        "Copyright Owner:",
                        Prompt:       "Name to insert into the copyright text.",
                        DefaultValue: ""} },
                Hidden: false,
        }
}

func (r *AddCopyright) Run(config *refactoring.Config) *refactoring.Result {
        r.Init(config, r.Description())
        r.Log.ChangeInitialErrorsToWarnings()
        if r.Log.ContainsErrors() {
                return &r.Result
        }

        extent := r.findInComments("Copyright")
        if extent != nil {
                file := r.Program.Fset.File(r.File.Package)
                startPos := file.Pos(extent.Offset)
                endPos := file.Pos(extent.OffsetPastEnd())

                r.Log.Error("An existing copyright was found.")
                r.Log.AssociatePos(startPos, endPos)
                return &r.Result
        }

        r.addCopyright(config.Args[0].(string))
        r.FormatFileInEditor()
        return &r.Result
}

func (r *AddCopyright) findInComments(text string) *text.Extent {
        occurrences := names.FindInComments(text, r.File, nil, r.Program.Fset)
        if len(occurrences) == 0 {
                return nil
        }
        return occurrences[0]
}

func (r *AddCopyright) addCopyright(name string) {
        extentToReplace := &text.Extent{0, 0}
        possibleSpace := " "
        if name == "" {
                possibleSpace = ""
        }
        text := fmt.Sprintf("// Copyright %s%s%s.  All rights reserved.\n",
                CurrentYear, possibleSpace, name)
        r.Edits[r.Filename].Add(extentToReplace, text)
}
{% endhighlight %}

Let's start from the top.

On line 15, we declare <tt>CurrentYear</tt> as a package-scope variable.  Our
refactoring inserts the current year into the copyright header, which is
problematic for unit tests since we don't want to update our tests every
year.  In the unit tests (refactoring_test.go), we set this variable to "YYYY"
so all the tests use "YYYY" rather than the current year.

On line 17, we declare a struct for our refactoring, and we embed
<tt>refactoring.RefactoringBase</tt>.
[RefactoringBase](https://godoc.org/github.com/godoctor/godoctor/refactoring#RefactoringBase)
provides most of the functionality we will need to refactor source code; we'll
discuss it more later.

In the Go Doctor infrastructure, every refactoring must implement the
[Refactoring](https://godoc.org/github.com/godoctor/godoctor/refactoring#Refactoring)
interface, which looks like this:

{% highlight go %}
type Refactoring interface {
    Description() *Description
    Run(*Config) *Result
}
{% endhighlight %}

We have defined methods on <tt>*AddCopyright</tt> so that it will implement this
interface.  The <tt>Description()</tt> method returns a
[Description](https://godoc.org/github.com/godoctor/godoctor/refactoring#Description)
of the refactoring.  The <tt>Run()</tt> method takes a
[Config](https://godoc.org/github.com/godoctor/godoctor/refactoring#Config) and
returns a
[Result](https://godoc.org/github.com/godoctor/godoctor/refactoring#Result).
The GoDoc for these objects contains all of the excruciating details, so we'll
focus on only the most important parts here.

In the <tt>Description</tt> (lines 22-33), the <tt>Synopsis</tt> and <tt>Usage</tt> strings should
be at most 50 characters long (to display properly in help messages).  The
comment <tt>----+----1----+----2--...</tt> is a reminder of the 50-character boundary.
The <tt>Usage</tt> string should contain a string in angle brackets for each
parameter.  The <tt>Params</tt> describe what arguments the refactoring expects.  For
our Add Copyright refactoring, we expect exactly one argument: the name of the
copyright holder (to be inserted into the comment).

The <tt>Run</tt> method actually performs the refactoring.  It receives a
[Config](https://godoc.org/github.com/godoctor/godoctor/refactoring#Config),
which contains two particuarly useful fields:
* <tt>Args</tt>.  This refactoring receives exactly one argument (the copyright
holder).  The text of this argument -- supplied by the user -- will be in
<tt>Args[0]</tt>.
* <tt>Selection</tt>.  The Add Copyright refactoring does not use this, but most
refactorings require the user to select a region of text in a file before
activating the refactoring.  For example, Rename requires the user to select
the identifier to rename, and Extract Function requires the user to select a
sequence of statements to extract.  The <tt>Selection</tt> field identifies the region
of text selected by the user.

The <tt>Run</tt> method returns a
[Result](https://godoc.org/github.com/godoctor/godoctor/refactoring#Result),
which contains two things:
* <tt>Log</tt>.  If the refactoring needs to provide informational messages, warnings,
or errors to the user, this is done by writing them to the <tt>Log</tt>.
* <tt>Edits</tt>.  Ultimately, a refactoring makes changes to the user's source code.
<tt>Edits</tt> is, essentially, a description of what changes are to be made.
We will use both of these fields later.

Now, let's go through the <tt>Run</tt> method line by line.

{% highlight go %}
        r.Init(config, r.Description())
{% endhighlight %}

First, the <tt>Run</tt> method invokes <tt>Init</tt>.  This does several things.  For example:
* It sets up an error/warning log (<tt>r.Log</tt>).
* It validates arguments.  In our <tt>Description</tt> object, we included one
<tt>Param</tt>, indicating that our refactoring should receive exactly one argument.
If the user supplied no arguments or more than one argument, the Init method
will log an error to <tt>r.Log</tt>.
* It parses the Go source code to be refactored.  If the source code cannot be
parsed, it will log an error to <tt>r.Log</tt>.

{% highlight go %}
        r.Log.ChangeInitialErrorsToWarnings()
        if r.Log.ContainsErrors() {
                return &r.Result
        }
{% endhighlight %}

When the Go source code is parsed, some semantic errors may be detected.  For
example, the code might reference a package that does not exist, or the user
might have mistyped a variable name.  Errors like this are logged to
<tt>r.Log</tt>.  <tt>ChangeInitialErrorsToWarnings</tt> changes them into
warnings.  For our refactoring, these are not a problem; we can safely add a
copyright header even though the Go source code might have problems.  More
complex refactorings will leave them as errors and refuse to refactor the code,
since it might be impossible to correctly analyze (and transform) the source
code.

{% highlight go %}
        extent := r.findInComments("Copyright")
        if extent != nil {
                file := r.Program.Fset.File(r.File.Package)
                startPos := file.Pos(extent.Offset)
                endPos := file.Pos(extent.OffsetPastEnd())

                r.Log.Error("An existing copyright was found.")
                r.Log.AssociatePos(startPos, endPos)
                return &r.Result
        }
{% endhighlight %}

The <tt>findInComments</tt> method is on lines 59-65.  It searches for the first
comment containing the word "Copyright" and returns a <tt>*text.Extent</tt>
describing its position, or <tt>nil</tt> if it was not found.

An [Extent](https://godoc.org/github.com/godoctor/godoctor/text#Extent) is
just an offset-length pair, where offset 0 denotes the first character of the
file's source code.

{% highlight go %}
type Extent struct {
    Offset int
    Length int
}
{% endhighlight %}

In Go, strings are UTF-8 encoded.  The <tt>Offset</tt> is a *byte* offset into the
UTF-8 encoded source code.  For example, consider the string "今日は"
(kon'nichiwa, "hello" in Japanese).  Each character is three bytes long, so
the string is 9 bytes in total.  If the first character (今) were at offset 0,
then the suffix "は" would be described by <tt>text.Extent{6, 3}</tt>.

The last lines add an error message to the log.  The <tt>AssociatePos</tt> method
takes two [token.Pos](https://godoc.org/go/token#Pos) arguments, which
determine the file, line, and column to associate with the error message.  The
first three lines create these arguments from the <tt>text.Extent</tt>.  Don't worry
too much about those details for now.

{% highlight go %}
        r.addCopyright(config.Args[0].(string))
        r.FormatFileInEditor()
        return &r.Result
{% endhighlight %}

Finally, we get to the meat of the refactoring.  The <tt>addCopyright</tt> method
(discussed momentarily) adds an edit to <tt>r.Edits</tt>.  The call to
<tt>FormatFileInEditor</tt> formats the resulting source code in the same way
as the <tt>gofmt</tt> tool.

### Changing Source Code: The <tt>addCopyright</tt> Method

{% highlight go %}
func (r *AddCopyright) addCopyright(name string) {
        extentToReplace := &text.Extent{0, 0}
        text := ... // omitted
        r.Edits[r.Filename].Add(extentToReplace, text)
}
{% endhighlight %}

The <tt>addCopyright</tt> method illustrates how a refactoring actually changes
source code:
* Create a <tt>text.Extent</tt> (offset-length pair) describing a range of text
  to replace.
* Specify what string to replace it with.
* Add an edit to <tt>r.Edits</tt>.

Given the string "abcdef", an edit with <tt>text.Extent{1, 3}</tt> would replace the
substring "bcd".

To delete text, set the replacement string to the empty string.

To insert text, create a <tt>text.Extent</tt> with a length of 0.  For example, given
the string "abcdef", an edit at <tt>text.Extent{5, 0}</tt> would represent an
insertion before the letter f.

In our case, the extent to replace is <tt>text.Extent{0, 0}</tt> -- an insertion at
the beginning of the file.

Perhaps the most surprising part of this code is that *our refactoring does not
directly change any source code!*  Instead, it builds a list of edits that
*describe* what changes it wants to make.  The list of edits is part of the
<tt>Result</tt> object that is returned from the <tt>Run</tt> method.  The
command line driver decides what to do with this list of edits; it produces a
patch file, outputs the modified source code, or overwrites the file on disk,
depending on what flags were passed on the command line.

## The Driver: main.go

{% highlight go linenos %}
// Copyright (C) 2018 Jeffrey L. Overbey.  Use of this source code is governed
// by a BSD-style license posted at http://blog.jeff.over.bz/license/
package main

import (
        "os"

        "github.com/godoctor/godoctor/engine"
        "github.com/godoctor/godoctor/engine/cli"
        "github.com/joverbey/goaddcopyright/refactoring"
)

func main() {
        engine.AddRefactoring("addcopyright", new(refactoring.AddCopyright))
        os.Exit(cli.Run("Add Copyright Header", os.Stdin, os.Stdout, os.Stderr, os.Args))
}
{% endhighlight %}

The command-line driver for our tool is simple.  We add an <tt>AddCopyright</tt>
struct to the refactoring engine, then run Go Doctor's command line interface
(CLI) driver.  The first argument (<tt>"Add Copyright Header"</tt>) is the name of
our tool (displayed in the <tt>-help</tt> output).

In the call to <tt>AddRefactoring</tt>, the first argument (<tt>"addcopyright"</tt>) is a
short name for the refactoring.  This isn't important for our tool, since it
only has one refactoring.  In contrast, the <tt>godoctor</tt> tool has five
refactorings; their short names are shown in the first column when
<tt>godoctor -list</tt> is run:

```console
$ godoctor -list
Refactoring     Description                                          Multifile?
--------------------------------------------------------------------------------
rename          Changes the name of an identifier                       true
extract         Extracts statements to a new function/method            false
var             Extracts an expression, assigning it to a variable      false
toggle          Toggles between a var declaration and := statement      false
godoc           Adds stub GoDoc comments where they are missing         false
```

When the refactoring engine has more than one refactoring, the user must supply
this short name on the command line to indicate which refactoring to perform.
For example:

```console
$ echo 'package main' | godoctor -pos 1,9:1,9 rename thisIsMyNewName
Reading Go source code from standard input...
Defaulting to file scope for refactoring (provide an explicit scope to change this)
<stdin>:1:9: Error: The "main" function in the "main" package cannot be renamed: it will eliminate the program entrypoint
```

## The Unit Tests: addcopyright_test.go

{% highlight go linenos %}
// Copyright (C) 2018 Jeffrey L. Overbey.  Use of this source code is governed
// by a BSD-style license posted at http://blog.jeff.over.bz/license/
package refactoring_test

import (
        "testing"

        "github.com/godoctor/godoctor/engine"
        "github.com/godoctor/godoctor/refactoring/testutil"
        "github.com/joverbey/goaddcopyright/refactoring"
)

func TestRefactorings(t *testing.T) {
        engine.AddRefactoring("addcopyright", new(refactoring.AddCopyright))

        refactoring.CurrentYear = "YYYY"

        const directory = "testdata/"
        testutil.TestRefactorings(directory, t)
}
{% endhighlight %}

The unit test driver adds our Add Copyright refactoring to the refactoring
engine, then transfers control to a <tt>TestRefactorings</tt> function provided
by the Go Doctor.  To see what this does, let's run the unit tests.

```console
$ cd $GOPATH/src/github.com/joverbey/goaddcopyright/refactoring
$ go test
Add Copyright Header testdata/addcopyright/001-helloworld/main.go
Add Copyright Header testdata/addcopyright/002-noname/main.go
Add Copyright Header testdata/addcopyright/003-error1/main.go
Add Copyright Header testdata/addcopyright/004-error2/main.go
PASS
ok      github.com/joverbey/goaddcopyright/refactoring    3.359s
```

Remember the structure of our testdata directory?

```
$GOPATH/src/github.com/joverbey/goaddcopyright/
└── refactoring/
    └── testdata/
        └── addcopyright/
            ├── 001-helloworld/
            │   ├── main.go
            │   └── main.golden
            ├── 002-noname/
            │   ├── main.go
            │   └── main.golden
            ├── 003-error1/
            │   └── main.go
            └── 004-error2/
                └── main.go
```

We invoked <tt>testutil.TestRefactorings("testdata/", t)</tt>.  The short name passed
to <tt>AddRefactoring</tt> (at the start of our <tt>TestRefactorings</tt> function) was
"addcopyright".  So, this function looks for the directory
<tt>testdata/addcopyright</tt>.  Each subdirectory of that directory is treated as a
test case.

Each test case must contain at least one <tt>.go</tt> file with a comment like this:
```
//<<<<<addcopyright,5,1,5,1,Your Name Here,pass
```
* "addcopyright" is the short name of the refactoring to invoke.
* The next four numbers indicate what range of text to select.  "1,2,3,4"
would mean, "select line 1, column 2 through line 3, column 4".  Here, the
first line of the file is line 1, and the first column is column 1.  Our
Add Copyright refactoring does not use the selection for anything, so it
doesn't really matter what selection we provide.
* If the refactoring takes arguments, those are next.  Our Add Copyright
refactoring takes one argument: the name of the copyright owner (to insert into
the header comment).  In this case, "Your Name Here" will be provided to the
refactoring as this argument.
* The last field must be either "pass" or "fail".
  * If the last field is "fail", the refactoring is expected to log at least
    one error.  (This is the case where the <tt>goaddcopyright</tt> command line tool
    exited with code 3 earlier.)
  * If the last field is "pass", then the test case directory must contain
    a <tt>.golden</tt> file with the same name as the <tt>.go</tt> file being refactored.
    After the <tt>.go</tt> file has been refactored, its text must match the text of
    the <tt>.golden</tt> file *exactly*.

The last point is important: The refactored program must match the <tt>.golden</tt>
file *exactly*.  If there is an extra line at the end of the file, the unit
test will fail.  If the <tt>.golden</tt> file contains spaces but the refactoring
produces tabs, the unit test will fail.

When creating a new test case, probably the easiest way to create a <tt>.golden</tt>
file is to simply run the refactoring, visually inspect its output, and then
save the result as a <tt>.golden</tt> file.

## What's Missing: Abstract Syntax Trees and Static Analysis

This post discussed the basic structure of a Go Doctor refactoring.  However,
this is only the beginning.  "Real" refactorings are more complex.
* The <tt>RefactoringBase</tt> contains a field, <tt>File</tt>, that provides an abstract
  syntax tree (AST) for the current Go file (as an
  [ast.File](https://godoc.org/go/ast#File) object).  Almost every refactoring
  begins by analyzing this AST.  In fact, the majority of the work in most
  refactorings is in traversing and analyzing ASTs; creating edits is usually
  the easy part!
* A refactoring begins by checking **preconditions**.  These check that (1) the
  input to the refactoring is valid, (2) the refactoring will not introduce
  errors into the refactored code, and (3) when the refactored code executes,
  it will have exactly the same behavior as the code before refactoring.
* If all preconditions are satisfied, edits are created describing what changes
  to make to the source code.
* Some refactorings perform a second set of checks, analyzing the code *after*
  the edits have been applied.  This can detect whether compile errors have
  been introduced by the refactoring.

Interestingly, the second step -- checking preconditions -- is almost always
the hardest part of developing a refactoring.  The goal is to a produce a
refactoring that will never introduce an error into the user's source code.
The list of preconditions for a production-quality refactoring can be painfully
complex (see [this
example](https://github.com/godoctor/godoctor/blob/master/refactoring/extractlocal.go#L59)),
but for someone interested in static analysis, designing new refactorings can
be a source of very challenging problems.  The Go Doctor includes [control and
data flow analysis](https://godoc.org/github.com/godoctor/godoctor/analysis) to
handle some of the more complex cases.

So, the Add Copyright refactoring is obviously much simpler than most
refactorings.  However, its main purpose was to serve as a template -- to
provide a useful skeleton for developing new Go Doctor refactorings.

## Exercises

1. In main.go's <tt>main</tt> function, call <tt>engine.AddDefaultRefactorings()</tt> just
   after adding our refactoring.  What does this do, and how does it affect
   how you invoke the <tt>goaddcopyright</tt> tool from the command line?

2. Change one of the <tt>.golden</tt> files in the test cases so that it is incorrect.
   What happens?

3. Choose one of the test cases.  In the <tt>.go</tt> file, find the
   `//<<<<<addcopyright` comment and change "addcopyright" to something
   invalid.  What happens?

4. Add two new test cases for the Add Copyright refactoring: one that should
   pass and another that should fail.

5. Currently, the Add Copyright refactoring raises an error if any comment
   contains the word "Copyright".  Change it to issue an error only if a
   comment is found with the *exact* copyright text that will be produced by
   the refactoring.

6. Modify the Add Copyright refactoring to insert the copyright comment at the
   end of the file, rather than the beginning.

7. Modify the Add Copyright refactoring to check if the file being refactored
   is in a Git repository, and if it is, insert a copyright header of the form
   "Copyright 2014-2018", where "2014" is the year of the first commit
   with that file and "2018" is the current year.

## Download the Source Code

The Add Copyright refactoring is in GitHub at [https://github.com/joverbey/goaddcopyright]:

|:---------------------------------------------------------------------------|---------:|
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/main.go">main.go</a> | 16 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/addcopyright.go">refactoring/addcopyright.go</a> | 76 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/addcopyright_test.go">refactoring/addcopyright_test.go</a> | 20 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/001-helloworld/main.go">refactoring/testdata/addcopyright/001-helloworld/main.go</a> | 7 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/001-helloworld/main.golden">refactoring/testdata/addcopyright/001-helloworld/main.golden</a> | 8 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/002-noname/main.go">refactoring/testdata/addcopyright/002-noname/main.go</a> | 11 lies |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/002-noname/main.golden">refactoring/testdata/addcopyright/002-noname/main.golden</a> | 12 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/003-error1/main.go">refactoring/testdata/addcopyright/003-error1/main.go</a> | 8 lines |
| <a href="https://github.com/joverbey/goaddcopyright/blob/master/refactoring/testdata/addcopyright/004-error2/main.g">refactoring/testdata/addcopyright/004-error2/main.go</a> | 12 lines |
| | Total: 170 lines |
