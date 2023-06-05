from math import ceil
from scipy.optimize import minimize, NonlinearConstraint
from numpy import linspace

N = 2**24
t = 1000
r = 20
alpha = r/(r+1)
m0 = 2*r*N/(t+2)
mt = 2*alpha*N/(t+2)
gamma = 2*N/m0

nh = 1 # number of hasing nodes
vh = 1577142 # number of hash reductions per second
# vh = 1555509
# vh = 1562115
nf = 1 # number of filtrating nodes
vf = 16879286 # number of filtrations per second
do = 0 # average overhead time per point
dc = 0 # average nodes communication time

c0 = 0
ca = t


def mci(i):
    return ceil(2*N/(i+gamma))

def precompH(filters):
    result = 0;
    for i in range(1, len(filters), 1):
        result += mci(filters[i-1])*(filters[i]-filters[i-1])
    return result / (vh*nh)

def precompF(filters):
    result = 0
    for i in range(1, len(filters), 1):
        result += mci(filters[i-1])
    return result / (vf*nf)

def precompO(filters):
    result = 0
    for i in range(1, len(filters), 1):
        result += mci(filters[i-1])
    return do*result

def precompC(filters):
    result = 0
    for i in range(1, len(filters), 1):
        result += mci(filters[i-1])
    return dc*result/nh 

def precompT(filters):
    return precompH(filters) + precompF(filters)

def cstr_ineq(a):
    list_cstr = []
    for i in range(2,a+1):
        def f(filters, i=i):
            return filters[i]-filters[i-1]-1
        list_cstr.append(NonlinearConstraint(f, 0, 1000))
    return list_cstr

def positions(amin, amax):
    nb_filters = 0
    time = N*t
    filters = []
    for a in range(amin, amax+1):
        x0 = [0]*(a+1)
        bnds = ((c0,c0),)+((1,ca),)*(a-1)+((ca,ca),)
        cstr = cstr_ineq(a)
        result = minimize(precompT, x0, method='trust-constr', bounds=bnds, constraints=cstr)
        if result.fun < time:
            nb_filters = a
            time = result.fun
            filters = result.x
    print(f"T : {precompT(filters)}")
    print(f"H : {precompH(filters)}")
    print(f"F : {precompF(filters)}")
    filters[0] = nb_filters
    return list(map(lambda x: round(x), filters))

def cstr_ineq2(a):
    list_cstr = []
    for i in range(2,a+1):
        def f(filters, i=i):
            return filters[i]-filters[i-1]-1
        list_cstr.append({'type':'eq', 'fun':f})
    return list_cstr

def positions2():
    a = 37
    x0 = linspace(0,1000,a+1, dtype=int)
    bnds = ((0,0),)+((1,1000),)*(a-1)+((1000,1000),)
    cstr = [{'type': 'eq', 'fun': lambda x: min([int(x[i]-x[i]) for i in range(len(x))])}]
    cstr = [{'type': 'eq', 'fun': lambda f: (min([f[i]-f[i-1]-1 for i in range(1,len(f))])-1)}]
    result = minimize(precompT, x0, method='SLSQP', bounds=bnds, constraints=cstr, options={"maxiter": 10000})
    print(result)
    print(precompH(result.x))
    print(operations(result.x[1:]))
    
def positions3(amin, amax):
    nb_filters = 0
    time = N*t
    filters = []
    for a in range(amin, amax+1):
        x0 = linspace(0,1000,a+1, dtype=int)
        bnds = ((0,0),)+((1,1000),)*(a-1)+((1000,1000),)
        cstr = [{'type': 'eq', 'fun': lambda f: (min([f[i]-f[i-1]-1 for i in range(1,len(f))])-1)}]
        result = minimize(precompT, x0, method='SLSQP', bounds=bnds, constraints=cstr, options={"maxiter": 10000})
        if result.fun < time:
            nb_filters = a
            time = result.fun
            filters = result.x
    filters[0] = nb_filters
    return list(map(lambda x: round(x), filters))
 
def export_filters(filters, filename):
    with open(filename, "wb") as file:
        for filter in filters:
            file.write(filter.to_bytes(4, "little"))

def import_filters(filename):
    with open(filename, "rb") as file:
        nb_filters = int.from_bytes(file.read(4), "little")
        filters = [nb_filters]
        content = file.read(nb_filters*4)
    for i in range(nb_filters):
        filters.append(int.from_bytes(content[4*i:4*(i+1)], "little"))
    return filters

def operations(filters):
    total = 0
    previous = 0
    for current in filters:
        total += mci(previous)*(current - previous)
        previous = current
    return ceil(total)
    
def characteristics(filters): 
    print(f"Filters : {filters[0]} = {filters[1:]}")
    print(f"Hash : {operations(filters[1:])}")
    filters[0] = 0
    print(f"T : {precompT(filters)}")
    print(f"H : {precompH(filters)}")
    print(f"F : {precompF(filters)}")

if __name__ == "__main__": 
    
    # check_export("config-36.dat")
    
    amin = 53
    amax = 55
    # filters = positions(amin, amax)
    # print(f"filters : {filters}")
    # export(filters, "config.dat")
    
    # positions2()
    
    filters = positions3(37, 37)
    characteristics(filters)
    # export_filters(filters, f"configCV-{filters[0]}.dat")
    
    characteristics(import_filters(f"configDLA.dat"))