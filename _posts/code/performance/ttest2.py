import scipy.stats
setBufMean  = 0.104
setBufStdev = 0.012
setBufCount = 5
memsetMean  = 0.03
memsetStdev = 0
memsetCount = 5
t, p = scipy.stats.ttest_ind_from_stats(
        setBufMean, setBufStdev, setBufCount,
        memsetMean, memsetStdev, memsetCount,
        equal_var=False)
print('p = {}'.format(p))
print('Are means significantly different (p < 0.05)?  {}'.format(p < 0.05))
