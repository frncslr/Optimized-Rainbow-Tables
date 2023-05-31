import pylab
from scipy import optimize

################################################################################

def T(t, ell):
    T = 0
    Csum = 0
    pf = 2.0/(t +1) # = m / N
    for k in range(1 , t +1):
        q = 1 -(t -k -1)*(t - k)/ float((t)*(t +1))
        C = k +(t - k +1)* q
        for j in range(ell):
            Csum += C
            p = pf *(1 - pf)**(ell *(k -1)+ j)
            T += p * Csum
    return T

def T_heterog_intlv(ts):
    ell = len(ts)
    T = 0
    pnot = 1
    Csum = 0
    ks = [1]* ell
    stops =[False]* ell
    while not all(stops):
        metrics = [0]* ell
        ps = [0]* ell
        Cs = [0]* ell
        for j in range(ell):
            if stops[j]:
                continue
            q = 1 -(ts[j]- ks[j]-1)*(ts[j]- ks[j])/ float((ts[j])*(ts[j]+1))
            Cs[j]= ks[j]+(ts[j]- ks[j]+1)* q
            ps[j]= 2.0/(ts[j]+1) # =[m]j / N
            metrics[j]= ps[j]/ Cs[j]
        best = metrics.index(max(metrics))
        Csum += Cs[best]
        T += Csum * ps[best]* pnot
        pnot *= 1 - ps[best]
        ks[best]+= 1
        if ks[best]> ts[best]:
            stops[best]= True
    return T

def W_heterog(ts):
    W = 0
    for j in range(ell):
        for k in range(1 , ts[j]+1):
            q = 1 -(ts[j]-k -1)*(ts[j]- k)/ float((ts[j])*(ts[j]+1))
            C = k +(ts[j]- k +1)* q
            W += C
    return W

################################################################################

N = 2**40
t = 10000
m = 2* N / float(t +1)
ell = 4


Thomog = T(t , ell)
# The optimal order of search for homogeneous tables is parallel
assert abs(Thomog - T_heterog_intlv([t]* ell))/ Thomog < 0.0001
# Initial guess for faster convergence(doesn ’t have to meet the constraints)
init_ts = pylab.linspace(t /2 , 2* t , ell)
# Minimization
cons =({'type': 'ineq' , 'fun': lambda ts :((m * ell) - sum([2* N / float(tt +1) for tt in ts]))} ,)
bnds = [(1 , 10* t)]* ell
res = optimize.minimize(T_heterog_intlv , init_ts , method = 'SLSQP' , bounds = bnds ,
constraints = cons , options ={ 'maxiter' : 100 , 'ftol': Thomog /1000.})
ts =[int(tt) for tt in res.x]
ms =[int(2* N / float(tt +1)) for tt in ts]
Theterog = T_heterog_intlv(ts)
# The total memory used in both cases is virtually equal
print(abs(m * ell - sum(ms))/(m * ell))
assert abs(m * ell - sum(ms))/(m * ell) < 0.0001
# Results
print("homogeneous :")
print("t : ", t)
print("M : ", m * ell)
print("T : ", Thomog)
print("W : ", W_heterog([t]* ell))
print()
print("heterogeneous : ")
print("ts : " , ts)
print("M : ", sum(ms))
print("T : ", Theterog , "(speedup : ", Thomog / Theterog , ")")
print("W : ", W_heterog(ts) , "(slowdown : ", W_heterog(ts)/ W_heterog([t]* ell) , ")")