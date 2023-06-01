import numpy as np

import matplotlib.pyplot as plt

def draw_operations():
    x = [0,1,2,3]
    y = [669750000]*4
    expected = 669750000
    fig, ax = plt.subplots()
    ax.plot(x,y,'o',color='red',label='exemple plot')
    ax.axhline(y=expected, color='b', linestyle='--',label='exemple ligne')
    ax.set_title(f'Hash operations to precompute a table')
    plt.axis([0,4,669000000,670000000])
    plt.xticks(np.arange(0, 4, 1), fontsize=16)
    plt.yticks(np.arange(669700000, 669800000, 10000), fontsize=16)
    plt.yticks(fontsize=16)

    plt.xlabel("Table index ",fontsize=16)
    plt.ylabel("Hash operations",fontsize=16)
    plt.show()
    fig.savefig(f'name', bbox_inches='tight',dpi=500)

# draw_operations()

def draw_endpoints():
    x = [0,1,2,3]
    y = [31921,31882,31965,31927]
    expected = 31893
    fig, ax = plt.subplots()
    ax.plot(x,y,'o',color='red',label='exemple plot')
    ax.axhline(y=expected, color='b', linestyle='--',label='exemple ligne')
    ax.set_title(f'Number of unique endpoints in a table')
    plt.xticks(np.arange(0, 4, 1), fontsize=16)
    plt.yticks(np.arange(31500, 32500, 200), fontsize=16)

    plt.xlabel("Table index ",fontsize=16)
    plt.ylabel("Unique endpoints",fontsize=16)
    plt.show()
    fig.savefig(f'name', bbox_inches='tight',dpi=500)
    
# draw_endpoints()

def draw_coverage():
    x = [0,1,2,3]
    y = [85.11,85.07,85.15,85.13]
    expected = 85.08
    fig, ax = plt.subplots()
    ax.plot(y,'o',color='red',label='exemple plot')
    ax.axhline(y=expected, color='b', linestyle='--',label='exemple ligne')
    ax.set_title(f'Theoretical coverage of a table')
    plt.xticks(np.arange(0, 4, 1), fontsize=16)
    plt.yticks(np.arange(84.7, 85.7, 0.2), fontsize=16)

    plt.xlabel("Table index ",fontsize=16)
    plt.ylabel("Theoretical coverage (%)",fontsize=16)
    plt.show()
    fig.savefig(f'name', bbox_inches='tight',dpi=500)
    
draw_coverage()