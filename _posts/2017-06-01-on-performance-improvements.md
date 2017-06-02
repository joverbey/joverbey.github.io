---
layout: post
title:  "On Performance Improvements"
cover:  empty.jpg
date:   2017-06-01 11:59:59 ET
categories: performance statistics
---

Every computer science student ends up taking an undergrad statistics course (I think), but for some reason, no one ever mentions that it's actually useful.

When I took statistics as an undergrad, out textbook was obsessed with corn.  "Suppose you plant two rows of corn, and want to find out..."  Um, no.  I have never planted corn, I have no intention of ever planting corn, and I have no interest in finding out anything about this hypothetical corn that -- did I mention? -- I will never plant.  Your example is irrelevant.

Yes, I went to school in the midwest.

## A problem that does not involve corn

I wish someone would have told me that statistics could be useful for something I *do* deal with on a regular basis: improving the performance of software.  Every software engineer has to make performance improvements at some point.  The problem is that, in many cases, refactoring code to improve performance makes it harder to maintain.  Of the two, I would prefer maintainable code any day.  So the question becomes: *how can I tell if a performance improvement actually matters?*

## How to measure performance wrong

As a simple example, let's compare two ways to clear a large buffer in C.  First we'll measure a simple loop.  Then we'll use `memset`.

{% highlight c %}
{% include_relative code/performance/v1.c %}
{% endhighlight %}

We'll compile with -O2 optimization...

```
gcc -fopenmp -O2 -ov1 v1.c
```

...and then run it.
```
setBuf:  0.004712 seconds
memset:  0.000239 seconds
```

So, our simple loop (`setBuf`) is 20 times slower.  It's terrible, just like we expect.

Done!

Oh, wait, except for one thing.  Let's try them in the opposite order.

{% highlight c %}
{% include_relative code/performance/v2.excerpt %}
{% endhighlight %}

Again, compile with O2 optimization...

```
gcc -fopenmp -O2 -ov2 v2.c
```

...and then run it.
```
memset:  0.002223 seconds
setBuf:  0.002382 seconds
```

Umm... what?  Now they're basically the same.

## You're measuring something, but it's not your code

So here's rule #1: take several measurements.  Always.

Let's modify the code to take more measurements.

{% highlight c %}
{% include_relative code/performance/v3.excerpt %}
{% endhighlight %}

```
setBuf:  0.000834 0.000373 0.000347 0.000347 0.000347 seconds
memset:  0.000054 0.000046 0.000053 0.000043 0.000043 seconds
```

If we swap the order and measure `memset` first, we get this:

```
memset:  0.000756 0.000060 0.000051 0.000051 0.000052 seconds
setBuf:  0.000549 0.000515 0.000563 0.000559 0.000559 seconds
```

Clearly, the first run takes significantly longer than the others, no matter whether `setBuf` or `memset` is used.  This is probably due to the buffer not being in the cache, but I haven't verified this.  At any rate, the first run is clearly an outlier.  That performance measurement is affected by factors other than our choice of algorithm -- factors that we do not want to measure.

Let's modify the code to ignore the first run.  We'll do this for both functions, so (hopefully) any effects due to hardware features (e.g., instruction and data caches) will be minimized.

{% highlight c %}
{% include_relative code/performance/v5.excerpt %}
{% endhighlight %}

Now the results look a bit more reasonable:

```
setBuf:  0.000373 0.000369 0.000390 0.000437 0.000438 seconds
memset:  0.000054 0.000056 0.000060 0.000061 0.000060 seconds
```

Before you trust those numbers too much, let's consider another factor: How good is your timer?

## Know your timer's resolution

Let's add one line to the above C code:
{% highlight c %}
printf("Timer resolution is %0.15f seconds\n", omp_get_wtick());
{% endhighlight %}

This produces
```
Timer resolution is 0.010000000000000 seconds
```

In case you're wondering what resolution means, exactly, it works like this.  Suppose you look out your window every Monday at 8 am -- i.e., exactly once per week.  One week, a building is starting to be constructed.  60 weeks later, it's done.  You can say for sure that it took somewhere between 59 and 60 weeks to construct, but if you only look out your window once a week, you can't give a more accurate description than that.  What if someone demands to know how long it took in days?  Pick a number between 413 and 420.  You can't say for sure, because you weren't observing it on a daily basis.

The same applies here.  The function `omp_get_wtick` claims that the timer is only accurate to 0.01 seconds, or 10 milliseconds.

This is interesting.  I suspect `omp_get_wtick` is lying, and the timer's resolution is actually much better than it claims.  However, if it's *not* lying, that means that we should round all of our measurements to the nearest 0.01 seconds, because our measurements are no more accurate than that.  If we do that, all of our measurements are 0, so we don't have useful data.

## Getting non-worthless measurements

For the purpose of illustration, let's assume that `omp_get_wtick` is correct, and our measurements are only accurate to two decimal places (1/100 of a second).  Let's raise the buffer size to 256 MB and report the timing measurements only to two decimal places.

{% highlight c %}
{% include_relative code/performance/v6.excerpt %}
{% endhighlight %}

```
setBuf:  0.09 0.12 0.12 0.10 0.09 seconds
memset:  0.03 0.03 0.03 0.03 0.03 seconds
Timer resolution is 0.010000000000000 seconds
```

Now, it's fairly obvious that `memset` is faster, but there's a better way to make that argument.

## Why use statistics?

Let's put aside this example momentarily and consider some fake data.

Suppose we measure the amount of time Function A and Function B take to process an input, and we get these results:

| Function A: | 1, 2, 3, 2, 3, 2 |
| Function B: | 8, 8, 7, 7, 8, 9 |

Assume the numbers represent execution time in seconds.  Obviously, Function A is faster.  A lot faster.

What about these?

| Function C: | 1, 7, 1, 3, 8, 5 |
| Function D: | 4, 5, 4, 4, 4, 5 |

Now, there is not an obvious winner.  Function C's average is slightly lower, but its measurements vary widely.  If we took more measurements of both functions, a few more high readings for Function C could easily make it the worse option.

This is when it's helpful to have statistics.  If we took more measurements, and they had similar characteristics as the ones we already took, would Function C or Function D conclusively appear to be faster than the other?

## Mr. T

Let's return to the last example: determining if `setBuf` or `memset` is faster.

On average, `setBuf` takes (0.09 + 0.12 + 0.12 + 0.10 + 0.09) / 5 = 0.104 seconds, while `memset` takes 0.03 seconds.  What we really want to know is this: *Is the average time taken by `setBuf` significantly different from the average time taken by `memset`?*  In this context, we mean "statistically significant."

To answer this question, we'll use Welch's t-test.  You can do it easily in Python:

{% highlight python %}
{% include_relative code/performance/ttest1.py %}
{% endhighlight %}

```
p = 0.000400683895549
Are means significantly different (p < 0.05)?  True
```

Since `memset`'s average is smaller, and the t-test produced a value of *p* less than 0.05, we can conclude that `memset` is (statistically) significantly faster than `setBuf`.  (We're using a 95% confidence interval.  You may want to use a 90% or 99% confidence interval instead -- i.e., a *p*-value of 0.1 or 0.01, respectively -- depending on the situation.)

In a more realistic scenario, you would probably have a larger number of measurements.  To perform a t-test, you do not need the individual measurements; as long as you know how many measurements were taken, their mean, and their standard deviation, you can perform a t-test based solely on that data.

|---------------------:|:---------|:---------|
|                      | `setBuf` | `memset` |
|----------------------|----------|----------|
|                      |   0.09   |   0.03   |
|                      |   0.12   |   0.03   |
|                      |   0.12   |   0.03   |
|                      |   0.10   |   0.03   |
|                      |   0.09   |   0.03   |
|----------------------|----------|----------|
| Average:&nbsp;&nbsp; |   0.104  |   0.03   |
|   Stdev:             |   0.012  |     0    |
|   Count:             |    5     |     5    |
|----------------------|----------|----------|

Use `scipy.stats.ttest_ind_from_stats` to perform a t-test from summary statistics rather than the sample data:

{% highlight python %}
{% include_relative code/performance/ttest2.py %}
{% endhighlight %}

```
p = 0.000160299988656
Are means significantly different (p < 0.05)?  True
```

## Another example: -O3 optimization

As a final example, let's compile this same example with -O3 optimization, and let's retain all 6 decimal places in the measurements (supposing `omp_get_wtick` was wrong, and the timer actually has microsecond resolution).

```
setBuf:  0.030776 0.031715 0.030640 0.030457 0.030453 seconds
memset:  0.034914 0.032352 0.030277 0.030767 0.030747 seconds
```

Now this is more interesting.  The times look similar; it's not clear if either is faster.  Let's run a t-test:

{% highlight python %}
{% include_relative code/performance/ttest3.py %}
{% endhighlight %}

```
p = 0.311610406138
Are means significantly different (p < 0.05)?  False
```

So it turns out that `memset` is *not* significantly different from our simple loop when -O3 optimization is enabled.  The compiler's optimizer has managed to translate our naive loop into native code that is indistinguishable from `memset` in its performance.

Now, this does not mean that the two are exactly the same.  It simply means that any improvement or degradation in runtime is indistinguishable from normal variability and measurement error.  In other words, a third-party observer probably wouldn't be able to tell the difference simply by measuring their execution times as we did.

So there you go.  There's something useful you can do with statistics.  Now go plant some corn.
