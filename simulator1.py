# Exemple per executar: 
#   python simulate.py Stonks Stonks Stonks2 Stonks2 100

import subprocess
import random
import os
import sys

from multiprocessing import Pool
from collections import defaultdict

N_THREADS = 16 # Recomanació: 2*(nombre de nuclis) - 1 o 2*(nombre de nuclis) - 2

def player(x):
    pi1,pi2,pi3,pi4,seed = x
    command = f"./Game -s {seed} {pi1} {pi2} {pi3} {pi4} --input default.cnf --output /dev/null"
    res = subprocess.run(list(command.split(" ")), capture_output=True)
    pls = [0,0,0,0]
    i = 0
    for line in res.stderr.decode().splitlines():
        if "got score" in line:
        	pls[i] = int(line.split()[-1])
        	i = i + 1
    for i in range(0, len(pls)):
    	imax = 0
    	for j in range(0, len(pls)):
    		if pls[imax] < pls[j]:
    			imax = j
    	pls[imax] = -1-i;
    return (pls, seed)

if __name__ == '__main__':
    assert(len(sys.argv) >= 6)
    
    p1,p2,p3,p4, N_PARTIDES = sys.argv[1:]
    
    N_PARTIDES = int(N_PARTIDES)
    
    pos1 = [0, 0, 0, 0]
    pos2 = [0, 0, 0, 0]
    pos3 = [0, 0, 0, 0]
    pos4 = [0, 0, 0, 0]
    
    with Pool(processes=N_THREADS) as pool:
      result = pool.map(player, ((p1,p2,p3,p4,i) for i in random.choices(range(99999), k=N_PARTIDES)))
      for pos, j in result: 
          pos1[-1-pos[0]] += 1
          pos2[-1-pos[1]] += 1
          pos3[-1-pos[2]] += 1
          pos4[-1-pos[3]] += 1
          
          
      print(f"Player {p1:12} results:          ", [x*100/N_PARTIDES for x in pos1], "%")
      print(f"Player {p2:12} results:          ", [x*100/N_PARTIDES for x in pos2], "%")
      print(f"Player {p3:12} results:          ", [x*100/N_PARTIDES for x in pos3], "%")
      print(f"Player {p4:12} results:          ", [x*100/N_PARTIDES for x in pos4], "%")
      
      
