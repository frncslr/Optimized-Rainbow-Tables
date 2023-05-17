from scipy.optimize import minimize, NonlinearConstraint

N = 2**24
t = 1000
r = 20
alpha = r/(r+1)
m0 = 2*r*N/(t+2)
mt = 2*alpha*N/(t+2)
gamma = 2*N/m0

nh = 1 # number of hasing nodes
vh = 1 # number of hash reductions per second
nf = 1 # number of filtrating nodes
vf = 1 # number of filtrations per second
do = 0 # average overhead time per point
dc = 0 # average nodes communication time

c0 = 0
ca = t


def mci(i):
    return 2*N/(i+gamma)

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

def positions(amax):
    nb_filters = 0
    time = N*t
    filters = []
    for a in range(1, amax+1):
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
    return list(map(lambda x: int(x), filters))
 
def export(filters, filename):
    with open(filename, "wb") as file:
        for filter in filters:
            file.write(filter.to_bytes(4, "little"))
           
def check(filename):
    filters = []
    with open(filename, "rb") as file:
        nb_filters = int.from_bytes(file.read(4), "little")
        content = file.read(nb_filters*4)
    for i in range(nb_filters):
        filters.append(int.from_bytes(content[4*i:4*(i+1)], "little"))
    return nb_filters, filters

if __name__ == "__main__": 
    amax = 38
    # filters = positions(amax)
    # print(f"filters : {filters}")
    # export(filters, "configTestPositions.dat")
    
    nb, f = check("configTestPositions.dat")
    print(nb)
    print(f)