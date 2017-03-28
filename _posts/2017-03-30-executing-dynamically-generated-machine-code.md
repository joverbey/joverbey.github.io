---
layout: post
title: "Executing Dynamically Generated Machine Code: The Start of a JIT"
cover: empty.jpg
date:   2017-03-30 11:59:59 ET
categories: assembly compilers jit
---

Just-in-time compilers (JITs) have to do something that most application programs never do: generate and execute machine code at runtime.  In a previous post, I showed how to [build an x86 assembler]({% post_url 2017-01-15-x86-assembler %}).  Something like that can be used to construct machine code.  But how can you *execute* that code?  If you have an array of bytes, and you want the processor to execute them as machine code, how can you do that?

## An example: 6 bytes of x86/x64 machine language

We need some machine code to execute.  Let's pick something simple.

The C function
{% highlight c %}
uint32_t function() {
    return 0x12345678;
}
{% endhighlight %}
can be translated into assembly language as
```
mov eax, 12345678h
ret
```
which translates to the six-byte sequence
```
b8 78 56 34 12 c3
```
in machine code.  (This is so simple it works on both x86 and x64 under Linux, Windows, and macOS.)

## The right idea, which doesn't quite work

Let's say we have the six-byte array `b8 78 56 34 12 c3` somewhere in memory.  If we can treat those bytes of machine code as a function, we should be able to call it, and it should return 0x12345678.

In C, that's not hard to do: take a pointer to the machine code, cast it as a function pointer, and call it.

{% highlight c %}
{% include_relative code/executing-dynamically-generated-machine-code/broken.c %}
{% endhighlight %}

The only problem with this code is that it doesn't work.
```
Segmentation fault (core dumped)
```

## The operating system is protecting you from yourself

The code above has the right idea, but it doesn't work because *the operating system will try to prevent you from executing data*.  This is typically enforced by a hardware feature sometimes called an [NX bit](https://en.wikipedia.org/wiki/NX_bit) (*n*o e*x*ecute), which is enabled in macOS, [Linux](https://wiki.ubuntu.com/Security/Features#nx), and [Windows](https://support.microsoft.com/en-us/kb/875352).  This feature first became [available to consumers](http://www.zdnet.com/article/amd-intel-put-antivirus-tech-into-chips/) in 2004.  At that time, buffer overflow vulnerabilities plagued the software industry.  The NX bit was introduced to make it more difficult for attackers to execute arbitrary code after exploiting such vulnerabilities.

Of course, a JIT is one of the rare cases where a program *wants* to write data to memory and then execute it.  To continue our small example, we need to convince the OS to let us do that.

## Allocating memory and making it executable

The MX bit is part of the page table, which means that memory protections are usually set on a per-page basis.  Memory protections for a particular page are changed using the [mprotect(2)](https://linux.die.net/man/2/mprotect) system call on Linux and macOS, and they're changed using [VirtualProtect](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366898.aspx) on Windows.

To store machine code in memory and then execute it:
1. **Allocate a new page of memory, setting its protections to allow write access.**  This is done via the [mmap(2)](https://linux.die.net/man/2/mmap) system call on Linux/macOS and [VirtualAlloc](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887.aspx) on Windows.  These system calls allocate full pages of memory, and the returned pointer is guaranteed to be page-aligned, suitable for passing to mprotect.
2. **Write the machine code into the newly allocated page.**
3. **Change the protections for the newly allocated page to read+execute.**  Generally speaking, a page not be writable if it is executable.
4. **On Windows, flush the instruction cache** by invoking [FlushInstructionCache](https://msdn.microsoft.com/en-us/library/windows/desktop/ms679350.aspx).  Your code will probably work without it, but the documentation for VirtualProtect requires it.
5. **Set a function pointer to an address in the newly allocated page, then invoke the function at that address.**
6. **When you are finished, free the page** using [munmap(2)](https://linux.die.net/man/2/munmap) or [VirtualFree](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366892.aspx).

Ultimately, this results in the following code.

{% highlight c linenos %}
{% include_relative code/executing-dynamically-generated-machine-code/execute.c %}
{% endhighlight %}

| **Source Code:** &nbsp;&nbsp; | <a href="{{site.baseurl}}/_posts/code/executing-dynamically-generated-machine-code/execute.c">execute.c</a> |
| **Makefiles:**    | <a href="{{site.baseurl}}/_posts/code/executing-dynamically-generated-machine-code/GNUmakefile">GNUmakefile</a> <small>(GNU Make on Linux/macOS)</small> |
|                  | <a href="{{site.baseurl}}/_posts/code/executing-dynamically-generated-machine-code/Makefile">Makefile</a> <small>(NMAKE on Windows)</small> |
