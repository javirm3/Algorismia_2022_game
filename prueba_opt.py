from scipy.optimize import minimize


def rosen(x):
    """The Rosenbrock function"""
    return -(-3*x**2+12*x-8)


x0 = 2
res = minimize(rosen, x0, method='nelder-mead',
               options={'xatol': 1e-8, 'disp': True})
