from scipy.optimize import minimize, NonlinearConstraint
from math import floor

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

# a = 31
c0 = 0
ca = t


def mci(i):
    return 2*N/(i+gamma)

def precompH(pos):
    result = 0;
    for i in range(1, len(pos), 1):
        result += mci(i-1)*(pos[i]-pos[i-1])
    return result / (vh*nh)

def precompF(pos):
    result = 0
    for i in range(1, len(pos), 1):
        result += mci(i-1)
    return result / (vf*nf)

def precompO(pos):
    result = 0
    for i in range(1, len(pos), 1):
        result += mci(i-1)
    return do*result

def precompC(pos):
    result = 0
    for i in range(1, len(pos), 1):
        result += mci(i-1)
    return dc*result/nh 

def precompT(pos):
    return precompH(pos) + precompF(pos)

def f(x):
    return (x**2)-(12*x)+20

def cons_inc(pos):
    for i in range(1, len(pos), 1):
        if not pos[i]-pos[i-1]:
            return -1
    return 0

def cons_lim(pos):
    result = 0
    if pos[0] == c0:
        result += 1
    if pos[a] == ca:
        result += 1
    return result
    # return 0 if (pos[0] == c0 and pos[a] == ca) else 1

def ineq_cstr(pos):
    for i in range(1,len(pos)):
        if pos[i]-pos[i-1]-1 < 0:
            return 1
    return 0

def cstr_ineq(a):
    list_cstr = []
    for i in range(2,a+1):
        def f(pos, i=i):
            return pos[i]-pos[i-1]-1
        list_cstr.append(NonlinearConstraint(f, 0, 1000))
    return list_cstr

def opti_filters(amax):
    nb_filters = 0
    time = N*t
    filters = []
    for a in range(1, amax+1):
        pos = [0]*(a+1)
        bnds = ((c0,c0),)+((1,ca),)*(a-1)+((ca,ca),)
        list_cstr = cstr_ineq(a)
        result = minimize(precompT, pos, method='trust-constr', bounds=bnds, constraints=list_cstr)
        if result.fun < time:
            nb_filters = a
            time = result.fun
            filters = result.x
    print(f"nb_filters : {nb_filters}")
    filters[0] = 0
    filters = list(map(lambda x: int(x), filters))
    print(f"filters : {filters}")
    print(f"time : {precompT(filters)}")
    

if __name__ == "__main__":
        
    # pos = [i for i in range(a+1)]
    # pos = [0]*(a+1)
    # print(pos)
    # bnds = ((c0,c0),)+((1,ca),)*(a-1)+((ca,ca),)
    # print(bnds)
    # # cons = ({'type':'eq', 'fun': lambda pos: pos[0]-c0},
    # #         {'type':'eq', 'fun': lambda pos: pos[a]-ca},
    # #         {'type':'ineq', 'fun': lambda pos : pos[1]-(floor(pos[0])+1)})
    # # # cons = ({'type':'eq', 'fun': cons_lim})
    # # result = minimize(precompT, pos, method='SLSQP', bounds=bnds, constraints=cons)
    # # print(result)
    # # print(result.x)
    # # print(cons_inc(result.x))
    # # print(cons_lim(result.x))
    
    #         # {'type':'eq','fun': lambda x : max([x[i]-int(x[i]) for i in range(len(x))])}
    
    # list_cstr = cstr_ineq()
    # result = minimize(precompT, pos, method='trust-constr', bounds=bnds, constraints=list_cstr)
    # print(result)
    
    
    # print(ineq_cstr([0,1,1,1000]))
    amax = 50
    opti_filters(amax)
    