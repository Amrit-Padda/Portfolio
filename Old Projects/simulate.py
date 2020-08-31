import QSTK.qstkutil.qsdateutil as du
import QSTK.qstkutil.tsutil as tsu
import QSTK.qstkutil.DataAccess as da

import datetime as dt
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import random 

def simulate(start, end, symbols, allocation):		##Function that simulates the portfolio return for a given list of stocks
	dt_timeofday = dt.timedelta(hours = 16)
	ldt_timestamps = du.getNYSEdays(start, end, dt_timeofday)

	ls_keys = ['close']
	c_dataobj = da.DataAccess('Yahoo')
	ldf_data = c_dataobj.get_data(ldt_timestamps, symbols, ls_keys)
	d_data = dict(zip(ls_keys, ldf_data))
	
	temp = d_data['close'].values.copy()
	d_normal = temp / temp[0,:]
	alloc = np.array(allocation).reshape(4,1)
	portVal = np.dot(d_normal, alloc)

	dailyVal = portVal.copy()
	tsu.returnize0(dailyVal)

	daily_ret = np.mean(dailyVal)
	vol = np.std(dailyVal)
	sharpe = np.sqrt(252) * daily_ret / vol 
	cum_ret = portVal[portVal.shape[0] -1][0]

	return vol, daily_ret, sharpe, cum_ret





def print_simulate(start, end, symbols, allocation):
	vol, daily_ret, sharpe, cum_ret = simulate( start, end, symbols, allocation)
	print "Start date: ", start
	print "End Date: ", end
	print "Symbols", symbols
	print "Optimal Allocations: ", allocation
	print "Sharpe Ratio: ", sharpe
	print "Volatility (stdev): ", vol
	print "Average Daily Return: ", daily_ret
	print "Cumulative Return: ", cum_ret


def optimal_allocation_4( start, end, symbols ):

        max_sharpe = -1
        max_alloc = [0.0, 0.0, 0.0, 0.0]
        for i in range(0,11):
                for j in range(0,11-i):
                        for k in range(0,11-i-j):
                                for l in range (0,11-i-j-k):
                                        if (i + j + l + k) == 10:
                                                alloc = [float(i)/10, float(j)/10, float(k)/10, float(l)/10]
                                                vol, daily_ret, sharpe, cum_ret = simulate( start, end, symbols, alloc )
                                                if sharpe > max_sharpe:
                                                        max_sharpe = sharpe
                                                        max_alloc = alloc

        return max_alloc



def optimal_allocation_genetic( start, end, symbols ):		#An attempt at a Genetic Algorithm to determine the allocations of the stocks in the portfolio
	max_sharpe = -1
	max_alloc1 = [0.0, 1.0, 0.0, 0.0]
	max_alloc2 = [1.0, 0.0, 0.0, 0.0]	
	possible_values = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
	max_alloc_rand = []
	final_array = []
	
	while True : 
		max_alloc_rand.append(possible_values[random.randint(0, 9)])
	
		if len(max_alloc_rand) == 4 :
			for value in max_alloc_rand:	
				final_array.append(value / sum(max_alloc_rand))
			max_alloc1 = final_array
			vol, daily_ret, sharpe, cum_ret = simulate( start, end, symbols, max_alloc1)
    		if sharpe > max_sharpe:
    			max_sharpe = sharpe
    			max_alloc = final_array

    	return max_alloc

    			








start = dt.datetime(2010, 1, 1)
end = dt.datetime(2010, 12, 31)
symbols = ['C', 'GS', 'IBM', 'HNZ']
allocation = [0.4, 0.4, 0.0, 0.2]
max_alloc = optimal_allocation_genetic( start, end, symbols)
print_simulate( start, end, symbols, max_alloc)