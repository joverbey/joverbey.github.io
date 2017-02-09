---
layout: post
title:  "Finding Machine Language Encodings"
cover:  empty.jpg
date:   2017-02-15 12:00:00 ET
categories: assembly
---

In this post, I'll show how to use the Microsoft Macro Assembler (MASM) and DUMPBIN to figure out the machine language encoding of a particular x86 assembly language instruction.

In the previous post, I showed how to [build a tiny x86 assembler]({% post_url 2017-01-15-x86-assembler %}).  I stated matter-of-factly that <tt>nop</tt> is encoded as 90h, <tt>inc&nbsp;eax</tt> is 40h, and so forth.  It's reasonable to ask

* if you wanted to test the assembler, how could you verify that the encodings it produces are correct?
* if you wanted to extend the assembler with more instructions, how could you find the encodings of those instructions?

To test the assembler, it would be helpful to have an [oracle](https://en.wikipedia.org/wiki/Oracle_(software_testing)) -- if we knew "correct" encodings for specific instructions, we could compare those encodings to the encodings produced by our assembler.

If you wanted to extend the assembler, how could you find encodings of new instructions?  The correct answer is, "Read the [Intel&reg; 64 and IA-32 Architectures Software Developer Manuals](https://software.intel.com/en-us/articles/intel-sdm)."  It is the definitive source for x86 instruction encodings.  However, it's not an easy read... so it's helpful to have a way to generate encodings for sample instructions.  (I'll admit, when I didn't understand parts of it, I looked at a few sample instruction encodings, then went back to Intel's documentation and used them to figure it out.)

So... how can we find encodings of x86 instructions?  There are lots of ways to do this, but here's one.

## The Microsoft Macro Assembler (MASM) and DUMPBIN

The good news is, we're not the first people to build an x86 assembler.  So, an easy way to find the machine language encoding of an assembly language instructions is to assemble it using another assembler, then look at the encoding that assembler produces.

The Microsoft Macro Assembler (MASM) is an industry standard assembler, and it ships with Microsoft Visual Studio (since it's the assembler underlying Microsoft Visual&nbsp;C++).  We'll use that.

We'll also use another Visual Studio command line tool called DUMPBIN (which, I assume, stands for "dump binary," although according to its startup banner, it's the "Microsoft COFF/PE Dumper").  DUMPBIN is useful for many things, but one feature will be particularly useful here: it includes a <i>disassembler</i>.

To run MASM and DUMPBIN, you'll need Visual Studio with Visual&nbsp;C++ installed.  If you don't have it yet, you can download [Visual Studio Community Edition](https://www.visualstudio.com/downloads/) for free.  After the installer starts, when prompted to choose the type of installation, select a Custom installation.  You will then be prompted to select features; expand Programming Languages and select Visual&nbsp;C++.  Finish the installation.  When you want to run the commands shown below (<tt>ml</tt> and <tt>dumpbin</tt>), you will need to start a Visual Studio Command Prompt.  In the Start menu, this will be an item labeled "VS2015 x86 Native Tools Command Prompt" or something similar.

## Finding x86 instruction encodings using MASM and DUMPBIN

Suppose, for example, that we want to find the encoding of <tt>mov&nbsp;eax,12345678h</tt>.  We can write a small procedure in assembly language with this as the first instruction:

{% highlight plaintext %}
{% include_relative code/finding-machine-language-encodings/insn.asm %}
{% endhighlight %}

If you save the above as a file named <tt>insn.asm</tt>, you can assemble it using MASM:
 
{% highlight plaintext %}
ml /c insn.asm
{% endhighlight %}

This produces an object file, <tt>insn.obj</tt>.  Now, you can <i>disassemble</i> this object file using DUMPBIN:

{% highlight plaintext %}
dumpbin /DISASM insn.obj
{% endhighlight %}

This produces the following output:

{% highlight plaintext %}
{% include_relative code/finding-machine-language-encodings/insn-dumpbin.txt %}
{% endhighlight %}

What's on each line?  From right to left:

* The last part of the line (<tt>mov eax,12345678h</tt>) is the assembly language representation of an instruction.
* Immediately before that is a sequence of hexadecimal numbers giving the machine language encoding of that instruction, in hexadecimal (<tt>B8 78 56 34 12</tt>).  Each number corresponds to one byte of the encoding.
* The first part of the line (<tt>00000000:</tt>) is a label.  The label indicates the offset of the first byte of that instruction, in hexadecimal.  If the procedure includes jump instructions, these labels are also used to specify the jump destination (e.g., the assembly language for an unconditional jump to the beginning of this procedure would have been labeled <tt>jmp 00000000</tt>).

## Show the encoding of an instruction -- showinsn.bat

If you want to use the above technique repeatedly, we can make it easier by creating a Windows batch file that combines the above steps into a single command.  We'll call it <tt>showinsn.bat</tt>.  You can use it like this:

{% highlight plaintext %}
showinsn "mov eax, 12345678h"
{% endhighlight %}

It will display only the "interesting" line from DUMPBIN's output:

{% highlight plaintext %}
  00000000: B8 78 56 34 12     mov         eax,12345678h
{% endhighlight %}

Here is the full contents of <tt>showinsn.bat</tt>.  Note that I've added a few labels for the purpose of testing jump instructions (try <tt>showinsn "jmp back"</tt> or <tt>showinsn "je l10"</tt>).  The DUP lines insert large numbers of <tt>nop</tt> bytes (90h):

{% highlight shell linenos %}
{% include_relative code/finding-machine-language-encodings/showinsn.bat %}
{% endhighlight %}

## All your instructions are belong to me -- showall.bat

Finally, we return to the original question... what if you wanted to test the x86 assembler we wrote in the previous post?  Armed with our <tt>showinsn.bat</tt> batch file, it's not difficult to generate encodings for every single instruction our assembler supports:

{% highlight plaintext linenos %}
{% include_relative code/finding-machine-language-encodings/showall.bat %}
{% endhighlight %}

This takes several minutes to run (I never said this was an <i>efficient</i> way to find instruction encodings...) and produces <a href="{{site.baseurl}}/_posts/code/finding-machine-language-encodings/test-insns.txt">1021 lines of output</a>.

## Exercise: Testing the assembler

At this point, we can produce <a href="{{site.baseurl}}/_posts/code/finding-machine-language-encodings/test-insns.txt">a long list of machine language encodings</a>... enough to test our x86 assembler from the previous post almost exhaustively.  It's an interesting exercise to try to build an automated test suite for the x86 assembler from this file.  I'll leave that to you.  (I hacked something together with a shell script, posted below.  In retrospect, I should have used Perl, but this got me by.)

## Download the source code

<i>Code from this post (finding machine language encodings):</i>

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/finding-machine-language-encodings/showinsn.bat">showinsn.bat</a> &nbsp;&nbsp; | 76 lines
|                               | <a href="{{site.baseurl}}/_posts/code/finding-machine-language-encodings/showall.bat">showall.bat</a> | 64 lines
|                               |                                                                            | Total: 140 lines |
| **Output:**                   | <a href="{{site.baseurl}}/_posts/code/finding-machine-language-encodings/test-insns.txt">test-insns.txt</a> |

<i>Solution to the exercise (testing the x86 assembler):</i>

|:------------------------------|:---------------------------------------------------------------------------|---------:|
| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/x86-assembler-tests/generate-test.sh">generate-test.sh</a> &nbsp;&nbsp; | 192 lines |
| **Output:**                   | <a href="{{site.baseurl}}/_posts/code/x86-assembler-tests/test-x86asm.c">test-x86asm.c</a> |
| **Makefiles:**                | <a href="{{site.baseurl}}/_posts/code/x86-assembler-tests/GNUmakefile">GNUmakefile</a>&nbsp;&nbsp; | <small>(GNU Make on Linux/macOS)</small> |
|                               | <a href="{{site.baseurl}}/_posts/code/x86-assembler-tests/Makefile">Makefile</a> | <small>(NMAKE on Windows)</small> |
