from typing import Tuple
from ast import literal_eval
import matplotlib.pyplot as plt
import matplotlib as mpl
import geopandas as gp
import numpy as np
from shapely.geometry import Point
from astar import search


#Helper function to determine threshold
def getThreshold(pct, arr):
    return arr[int(len(arr) * (pct / 100))]

#Helper function to create custom cmap for matplotlib
def getCmap(thresVal):
    cmap = plt.cm.jet
    cmaplist = ['navy', 'yellow']
    cmap = mpl.colors.LinearSegmentedColormap.from_list('Custom cmap', cmaplist, cmap.N)
    bounds = [thresVal]
    norm = mpl.colors.BoundaryNorm(bounds, cmap.N)
    return cmap, norm

#Main function which plots the path and point
def plot(binSize, thresholdPct, start, end):
    shape = gp.read_file('Assignment1/crime_dt.shp')
    bounds = shape.total_bounds
    [xmin, ymin, xmax, ymax] = bounds
    dim = int(np.ceil((xmax - xmin) / binSize))
    X = []
    Y = []
    bins = [dim, dim]

    for point in shape.geometry:
        X.append(point.x)
        Y.append(point.y)

    hist = plt.hist2d(X, Y, bins)
    val = hist[0]
    xE = hist[1]
    yE = hist[2]

    thres = getThreshold(thresholdPct, sorted(hist[0].flatten()))
    cmap, norm = getCmap(thres)

    hist = plt.hist2d(X, Y, bins, cmap=cmap, norm=norm)

    #converting bins to points on bottom left
    xedges = hist[1]
    yedges = hist[2]
    nx = len(xedges)
    ny = len(yedges) 
    start_x = xmin
    start_y = ymin
    coords1d = []
    coords2d = np.empty((nx, ny), dtype=object)
    for i in range(0, nx):
        current_x = xedges[i]
        for j in range(0, ny):
            current_y = yedges[j]
            coords1d.append((current_x, current_y))
            coords2d[i][j] = (current_x, current_y)

    #adding values on the plot
    for (x, xi) in zip(xE, range(0, len(xE) - 1)):
        for (y, yi) in zip(yE, range(0, len(yE) - 1)):
            ax = plt.gca()
            ax.text(x + binSize / 2, y + binSize / 2, str(val[xi][yi]), fontdict=dict(fontsize=5, ha='center', va='center'))

    ax = plt.gca()
    ax.set_title(f'Threshold_pct: {thresholdPct} Threshold_val: {thres} '.format(thres, thresholdPct))

    path = search(hist, start, end, thres)
    path_as_points = [coords2d[xi][yi] for (xi, yi) in path]

    plt.plot([p[0] for p in path_as_points], [p[1] for p in path_as_points], color='white')
    plt.hist2d(X, Y, bins, cmap=cmap, norm=norm)

    plt.show()


if __name__ == "__main__":
    print("Please input the bin size: ")
    bin = input()
    print("Please input the threshold pct (%): ")
    pct = input()
    print("Please input the start index [x,y]: ")
    start = input()
    print("Please input the end index [x,y]: ")
    end = input()
    print("Generating path...")
    plot(float(bin), int(pct), literal_eval(start), literal_eval(end))
    print("Bye!")

    #plot(0.002, 70, [19, 0], [6, 18])
