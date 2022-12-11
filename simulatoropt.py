# Exemple per executar:
#   python simulate.py Stonks Stonks Stonks2 Stonks2 100

import subprocess
import random
import os
import sys
from scipy.optimize import minimize

from multiprocessing import Pool
from collections import defaultdict

N_THREADS = 6  # Recomanació: 2*(nombre de nuclis) - 1 o 2*(nombre de nuclis) - 2


def player(x):
    pi1, pi2, pi3, pi4, seed = x
    command = f"./Game -s {seed} {pi1} {pi2} {pi3} {pi4} --input default.cnf --output /dev/null"
    res = subprocess.run(list(command.split(" ")), capture_output=True)
    for line in res.stderr.decode().splitlines():
        if "got top score" in line:
            for p in (pi1, pi2, pi3, pi4):
                if p in line.split():
                    return (p, seed)


def play(list1):
    with open('stamps.txt', 'w') as f:
        s = "\n".join(map(str, list1))
        f.write(s)
    with Pool(processes=N_THREADS) as pool:
        result = pool.map(player, ((p1, p2, p3, p4, i)
                                   for i in range(37, 137)))
        res = defaultdict(list)
        for i, j in result:
            res[i].append(j)
        print(-(len(res["Napoleonop"])))
        return -(len(res["Napoleonop"]))


if __name__ == '__main__':
    global p1, p2, p3, p4, N_PARTIDES
    p1 = 'Dummy'
    p2 = "Dummy"
    p3 = "Dummy"
    p4 = 'Napoleonop'
    N_PARTIDES = 100
    x0 = [20, -5, 2, 1.25, 1.5, 2.5, 0.1, 0.05, 0.01,
          10, -5, 1, -1, -0.4, -0.25, -0.1, -0.05, -0.01,
          0.03, 0.02, 0.01, 0, 0, 0, 0, 0, 0,
          -1, -0.5, -0.25, 0, 0, 0, 0, 0, 0,
          10, 5, 2.5, 1, 0.4, 0.25, 0.1, 0.05, 0.01]
    x1 = 1
    res = minimize(play, x0, method='nelder-mead', options={'disp': True})
    with open('resulting_stamp.txt', 'w') as f:
        f.write(res)
    print(res)
