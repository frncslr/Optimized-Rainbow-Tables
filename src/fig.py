import numpy as np
import matplotlib.pyplot as plt
from struct import unpack
from filtration import importf

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
    
# draw_coverage()

def read_results(filename, count):
    with open(filename, 'rb') as file:
        return list(unpack('d' * count, file.read(8 * count)))

# print(read_results("hSpeeds.dat", 21))

def draw_hSpeeds():
    count = 200
    y = read_results("hSpeeds.dat", count)
    average = sum(y) / count
    fig, ax = plt.subplots()
    ax.plot(y,'o',color='red',label='Hash speeds')
    ax.axhline(y=average, color='b', linestyle='--',label=f'Average speed ({average})')
    # plt.xticks(np.arange(1, count+1, 1), fontsize=16)
    plt.yticks(np.arange(1500000, 1600000, 10000), fontsize=16)

    ax.set_title(f'Hash speed variation on {count} mesures')
    plt.xlabel("Mesure number ",fontsize=16)
    plt.ylabel("Hash speed (M/s)",fontsize=16)
    plt.legend(loc="upper left")
    plt.show()
    # fig.savefig(f'hSpeeds', bbox_inches='tight',dpi=500)

# draw_hSpeeds()

def draw_cSpeeds():
    count = 100
    y = read_results("cSpeeds.dat", count)
    average = sum(y) / count
    fig, ax = plt.subplots()
    ax.plot(y,'o',color='red',label='Clean speeds')
    ax.axhline(y=average, color='b', linestyle='--',label=f'Average speed ({average})')
    # plt.xticks(np.arange(1, count+1, 1), fontsize=16)
    plt.yticks(np.arange(15000000, 18000000, 500000), fontsize=16)

    ax.set_title(f'Clean speed variation on {count} mesures')
    plt.xlabel("Mesure number ",fontsize=16)
    plt.ylabel("Clean speed (M/s)",fontsize=16)
    plt.legend(loc="upper left")
    
    plt.show()
    fig.savefig(f'cSpeeds', bbox_inches='tight',dpi=500)
    
# draw_cSpeeds()

def draw_filters():
    filters = importf("./configs/config_opti.dat")
    nb_filters = filters[0]
    y = filters[1:]
    fig, ax = plt.subplots()
    ax.plot(y,'-',color='red',label='Optimized positions')
    
    filters = importf("./configs/config_mini.dat")
    nb_filters = filters[0]
    y = filters[1:]
    ax.plot(y,'+',color='blue',label='Minimized positions')

    plt.xticks(np.arange(0, nb_filters+1, 5), fontsize=16)
    plt.yticks(np.arange(0, 1200, 200), fontsize=16)

    ax.set_title(f'Positions of the {nb_filters} filters')
    plt.xlabel("",fontsize=16)
    plt.ylabel("",fontsize=16)
    plt.legend(loc="upper left")
    
    plt.show()
    fig.savefig(f'./configs/configs', bbox_inches='tight',dpi=500)
    
draw_filters()