from numpy import append
from scipy.optimize import minimize

################################################################################

def m(i):
    return 2*r*N / (r*i+t+2)

def c(i):
    return round(gamma * ((t+gamma-1) / gamma) ** (i/a_opti) - gamma + 1)

def T(f):
    H = F = prev = 0
    fs = sorted(append(f, [t]))
    for curr in fs:
        H += m(prev)*(curr-prev)
        F += m(prev)
        prev = curr
    return H/(vh*nh) + F/(vf*nf)

def P(a):
    return 2*N*a * (((t+gamma-1) / gamma) ** (1/a) - 1)

################################################################################

def exportf(filters, nb_filters, filename):
    filters.append(t)
    with open(filename, "wb") as file:
        file.write(nb_filters.to_bytes(4, "little")) 
        for filter in filters:
            file.write(filter.to_bytes(4, "little")) 

def importf(filename):
    with open(filename, "rb") as file:
        nb_filters = int.from_bytes(file.read(4), "little")
        filters = [nb_filters]
        content = file.read(nb_filters*4)
    for i in range(nb_filters):
        filters.append(int.from_bytes(content[4*i:4*(i+1)], "little"))
    return filters

################################################################################

# problem size
N = 2**24
# table width
t = 1000
# maximum number of unique endpoints
mtmax = 2*N/(t+2)
# maximum efficiency ratio
r = 20
# maximality factor
alpha = r/(r+1)
# initial number of startpoints
m0 = r*mtmax
# objective number of unique endpoints
mt = alpha*mtmax
# mi constant
gamma = 2*N/m0
# number of hasing nodes
nh = 1
# number of hash reductions per second
vh = 1556599
vh = 11882475
# number of filtrating nodes
nf = 1
# number of filtrations per second
vf = 16879286
vf = 22000576
# average overhead time per point
do = 0
# average nodes communication time
dc = 0

################################################################################

if __name__ == "__main__":
    
    # Boundaries
    # lower bound of number of intermediary filters
    alb = 20
    # upper bound of number of intermediray filters
    aub = 40

    # Minimization
    # time when ultimate filtration only
    time = T([])
    # intermediary filters positions
    f_mini = []
    # number of intermediary filters
    a_mini = 0
    # searching between boundaries
    for a in range(alb,aub+1):
        # initial guess
        f0 = [t*float(i)/a for i in range(a)]
        # filters positions bounds
        bnds = [(0, t-1)]*a
        # minimizing the objective
        res = minimize(T, f0, method='TNC', options={'disp':False, 'maxfun':10000, 'eps':0.1, 'ftol':0.0001}, bounds=bnds)
        # checking for improvement and updating if so
        if res.success and time - res.fun > 0.1:
            time = res.fun
            f = res.x
            a_mini = a
    f = list(map(round, sorted(f)))

    # Optimization
    # hash operations when no intermediary filtrations
    hash = m0 * t
    # intermediary filters positions
    f_opti = []
    # number of intermediary filters
    a_opti = a_mini + 1
    # computing filters positions for the optimal number
    for i in range(1,a_opti):
        f_opti.append(c(i))

    # Results
    print("boundaries :")
    print(f"lower : {alb}")
    print(f"upper : {aub}")
    print()
    print("minimization :")
    print(f"T : {T(f)}")
    print(f"a : {a_mini}")
    print(f"f : {f}")
    print()
    print("optimization :")
    print(f"T : {P(a_opti)/(vh*nh)}")
    print(f"a : {a_opti-1}")
    print(f"f : {f_opti}")

    # Export of configurations    
    exportf(f, a_mini+1, "data/configs/config_mini.dat")
    exportf(f_opti, a_opti, "data/configs/config_opti.dat")
