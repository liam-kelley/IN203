

# TP3 de KELLEY Liam

## lscpu

Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              8
On-line CPU(s) list: 0-7
Thread(s) per core:  2
Core(s) per socket:  4
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               142
Model name:          Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz
Stepping:            10
CPU MHz:             700.019
CPU max MHz:         3400,0000
CPU min MHz:         400,0000
BogoMIPS:            3600.00
Virtualization:      VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            6144K
NUMA node0 CPU(s):   0-7


## Produit scalaire 

***

OMP_NUM    | samples=1024 | avec c++ threads
-----------|--------------|----------
séquentiel |   0.00202407
1          |   0.00285671  0.0247764
2          |   0.00280896  0.0337895
3          |   0.00243048  0.035688
4          |   0.00245616  0.0506767
8          |   0.00719699  0.111949

Threads c++ sont très lents!
Pas d'optimisation, programme memory bound.


## Produit matrice-matrice

1023 1.5s
1024 4.5s ??
1025 1.5s

Explication: /


### Permutation des boucles q2.2

`make TestProductMatrix.exe && ./TestProductMatrixNaif.exe 1024`
//.exe renommé entre temps

//copié depuis la correc.

  ordre           | time    | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|---------|----------------|----------------|---------------
i,j,k (origine)   | 2.73764 | 782.476 |                |                | 
j,i,k             | 3.72883 | 575.913 |                |                |
i,k,j             | 12.0426 | 172.571 |                |                |
k,i,j             | 12.0755 | 177.838 |                |                |
j,k,i             | 0.45099 | 4761.65 | 3511.59        | 5454.68        | 3416.2
k,j,i             | 0.46301 | 4638.07 | 1928.87        | 6311.08        | 1775.2

Les deux meilleurs permutations sont celles qui mettent l'indice i au plus bas niveau. (i opère sur les rows!)
Cette permutation est celle qui exploite au mieux la mémoire cache.

On remarque aussi que les performances baissent quand la taille des matrices augmente : 
les petites matrices tiennent entièrement dans le cache. 



### OMP sur la meilleure boucle q2.3

`make TestProductMatrix.exe && OMP_NUM_THREADS=8 ./TestProductMatrixOpenMP.exe 1024`
//.exe renommés entre temps.

  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 |  |
2                 |  |
3                 |  |
4                 |  |
5                 |  |
6                 |  |
7                 |  |
8                 |  |

Test parallélisation: On a une nette accélération! (test entre Naif et OpenMP)


### Produit par blocs q2.5


  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    |  |
32                |  |
64                |  |
128               |  |
256               |  |
512               |  | 
1024              |  |




### Bloc + OMP q2.7



  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
---------------|---------|---------|------------------------------------------------
A.nbCols       |  1      |         | 
512            |  8      |         | 



