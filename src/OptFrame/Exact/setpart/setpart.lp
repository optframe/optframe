\Problem name: IloCplex

Minimize
 obj: 10 Y(0) + 20 Y(1) + 10 Y(2) + 20 Y(3) + 5 Y(4) + id16
Subject To
 id9:  Y(0) + Y(1) + Y(3)  = 1
 id11: Y(1) + Y(4)  = 1
 id13: Y(2) + Y(3)  = 1
 id15: Y(0) + Y(1) + Y(2) + Y(3) + Y(4) <= 2
Bounds
 0 <= Y(0) <= 1
 0 <= Y(1) <= 1
 0 <= Y(2) <= 1
 0 <= Y(3) <= 1
 0 <= Y(4) <= 1
      id16 = 0
Binaries
 Y(0)  Y(1)  Y(2)  Y(3)  Y(4) 
End
