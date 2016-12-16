---
layout: post
title:  "256 Lines or Less (the joverblog?)"
cover:  gophamily.png
date:   2016-12-15 12:00:00 ET
categories: blog
---

What can you do with 256 lines of code?

A lot, actually.

I figured it's time to start a blog with that goal: every post will show how to build something interesting with 256 lines of code or fewer.<font color="gray"><sup>1,2</sup></font>

The first post will show how to build a small x86 assembler in C.  I'm not sure exactly what will come after that.  Anything in the realm of compilers, parallel programming, algorithms, data structures, and software engineering is fair game.  You can build a red-black tree in F# with surprisingly little code... and it's actually comprehensible.  If you like Go (er, golang), building a [Go Doctor](http://gorefactor.org) plug-in is fun.  You can use the C interop facilities in Fortran 2008 to build a GTK+ application, but you probably shouldn't.  Maybe I'll try to describe the Fast Fourier Transform.

At some point, I'll probably take advantage of the fact that 0 is less than 256, which gives me a remarkable amount of flexibility.

Stay tuned.  The first "real" post -- an x86 (subset) assembler in 256 LOC -- goes live on January 1.

<small><font color="gray"><sup>1</sup> Why 256 lines?  The <a target="_blank" href="https://www.eclipse.org/legal/EclipseLegalProcessPoster.pdf">Eclipse Foundation Legal Process</a> used to define 250 lines of code as the upper limit for a "small" contribution that could be committed without a full IP review.  (That limit is now 1000 lines.)  I was going to call the blog "250 Lines or Less," but <a target="_blank" href="http://blog.mattrbianchi.com/">Matt Bianchi</a> reminded me that 256 was obviously a better choice.  Also, this length corresponds to about four printed pages, which seems like "not a lot."</font></small>

<small><font color="gray"><sup>2</sup> If you're bothered by the fact that the name of this blog isn't the gramatically correct "256 lines or fewer," you're welcome.</font></small>

{% highlight shell %}
# Install Jekyll on Ubuntu 16 LTS
sudo apt install ruby-dev zlib1g-dev    # make and gcc already installed
sudo gem install jekyll bundler github-pages git
{% endhighlight %}
