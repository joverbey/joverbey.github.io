import scipy.stats
setBuf = [0.030776, 0.031715, 0.030640, 0.030457, 0.030453]
memset = [0.034914, 0.032352, 0.030277, 0.030767, 0.030747]
t, p = scipy.stats.ttest_ind(setBuf, memset, equal_var=False)
print('p = {}'.format(p))
print('Are means significantly different (p < 0.05)?  {}'.format(p < 0.05))
