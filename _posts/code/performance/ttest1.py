import scipy.stats
setBuf = [0.09, 0.12, 0.12, 0.10, 0.09]
memset = [0.03, 0.03, 0.03, 0.03, 0.03]
t, p = scipy.stats.ttest_ind(setBuf, memset, equal_var=False)
print('p = {}'.format(p))
print('Are means significantly different (p < 0.05)?  {}'.format(p < 0.05))
