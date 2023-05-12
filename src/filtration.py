from scipy.optimize import minimize

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
d0 = 0 # average overhead time per point
dc = 0 # average nodes communication time


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
    return d0*result

def precompC(pos):
    result = 0
    for i in range(1, len(pos), 1):
        result += mci(i-1)
    return dc*result/nh 

def precompT(pos):
    return precompH(pos) + precompF(pos)

def f(x):
    return (x**2)-(12*x)+20

if __name__ == "__main__":
    # print(minimize(f, 2.0))
    print(m0)
    print(mci(0))
    print(precompH([0, t+1]))
    print(precompF([0, t+1]))
    print(precompO([0, t+1]))
    print(precompC([0, t+1]))
    print(precompT([0, t+1]))