import copy
import sys

def mexit(message):
  print("***",message)
  sys.exit(0)
#endmexit


def get_sizes(Ng):
  # Ng = 2^k, fNv = 2*k, gNv = 3*k
    if Ng == 4:
      fNv = 4
      gNv = 6
    elif Ng == 8:
      fNv = 6
      gNv = 9
    elif Ng == 16:
      fNv = 8
      gNv = 12
    elif Ng == 32:
      fNv = 10
      gNv = 15
    else:
      mexit('wrong size of the input graph')
    return fNv, gNv


def compute_delta(w,x,fNv,P):
  res = 1
  for i in range(fNv):
    if w[i] == 1:
      res = res * x[i]
      res = res % P
    else:
      res = res * (1-x[i])
      res = res % P
  return(res)
#end compute_delta


def checkA(A,Ng):
  for a in range(Ng):
    if A[a][a] == 1:
      mexit("checkA error1")
    for b in range(a):
      if A[a][b] != A[b][a]:
        mexit("checkA error2")
  print("A OK")
#end checkA


def f(x,A,Ng):
  fNv,gNv =get_sizes(Ng)
  if fNv == 4:
    a = 2*x[0]+x[1]
    b = 2*x[2]+x[3]
  elif fNv == 6:
    a = 4*x[0]+2*x[1]+x[2]
    b = 4*x[3]+2*x[4]+x[5]
  elif fNv == 8:
    a = 8*x[0]+4*x[1]+2*x[2]+x[3]
    b = 8*x[4]+4*x[5]+2*x[6]+x[7]
  else: #fNv == 10
    a = 16*x[0]+8*x[1]+4*x[2]+2*x[3]+x[4]
    b = 16*x[5]+8*x[6]+4*x[7]+2*x[8]+x[9]
  return(A[a][b])
#end

def F(x,A,Ng,P):
  fNv, gNv = get_sizes(Ng)
  w = [0]
  for i in range(1,fNv):
    w.append(0)
  res = 0
  while True:
    if (f(w,A,Ng) == 1):
      r = compute_delta(w,x,fNv,P)
      r = (r % P)
      res = res + r
      res = (res % P)
    found = False
    for i in range(fNv-1,-1,-1):
      if w[i] == 0:
        found = True
        w[i] = 1
        for j in range(i+1,fNv):
          w[j] = 0
        break #forloop
    #endforloop
    
    if not found:
      break #whileloop
  #endwhile
  return res
#end F


def checkF(A,Ng,P):
  fNv,gNv = get_sizes(Ng)
  x = [0]
  for i in range(1,fNv):
    x.append(0)
  while True:
    r1 = F(x,A,Ng,P)
    r2 = f(x,A,Ng)
    if r1 != r2:
      mexit("problem on "+str(x))
    found = False
    for i in range(fNv-1,-1,-1):
      if x[i] == 0:
        found = True
        x[i] = 1
        for j in range(i+1,fNv):
          x[j] = 0
        break #forloop
    #endforloop
    if not found:
      break #whileloop
  #endwhiloop
  print("F OK");
#endcheckF


def g(x,A,Ng,P):
  fNv,gNv = get_sizes(Ng)
  b = fNv//2
  y = [x[0]]
  #
  for i in range(1,fNv):
    y.append(x[i])
  res = F(y,A,Ng,P)
  if res == 0:
    return res
  #
  for i in range(b,fNv):
    y[i] = x[i+b]
  r = F(y,A,Ng,P)
  r = (r % P)
  if r == 0:
    return 0
  res = res * r
  res = (res % P)
  #
  for i in range(fNv):
    y[i] = x[i+b]
  r = F(y,A,Ng,P)
  r = (r % P)
  if r == 0:
    return 0
  res = res * r
  res = (res % P)
  return res
#end g


def eval_g(r,A,Ng,P):
  fNv,gNv = get_sizes(Ng)
  rlen = len(r)
  if rlen == 0:
    y = [0]
  else:
    y = [r[0]]
  for i in range(1,gNv):
    if i < rlen:
      y.append(r[i])
    else:
      y.append(0)
  res = 0
  while True:
    r = g(y,A,Ng,P)
    res = res + r
    res = (res % P)
    found = False
    for i in range(gNv-1,rlen-1,-1):
      if y[i] == 0:
        found = True
        y[i] = 1
        for j in range(i+1,gNv):
          y[j] = 0
        break #forloop
    #endforloop
    if not found:
      break #whileloop
  #endwhile
  return res
#end eval_g
    

Ng = 4
fNv,gNv = get_sizes(Ng)
A = []
for i in range(Ng):
  a = [0]
  for j in range(1,Ng):
    a.append(0)
  A.append(a)
A[0][1] = A[1][2] = A[2][3] = A[3][2] = A[2][1] = A[1][0] = A[0][3] = A[3][0] = A[3][1] = A[1][3] = 1
checkA(A,Ng)
P = 524287
checkF(A,Ng,P)
y = []
print("G =",eval_g(y,A,Ng,P))
y = [0]
print("g1(0) =",eval_g(y,A,Ng,P))
y = [1]
print("g1(1) =",eval_g(y,A,Ng,P))
y = [2]
print("g1(2) =",eval_g(y,A,Ng,P))
y = [70000,70000,70000,70000,0]
print("g1(17,17,0) =",eval_g(y,A,Ng,P))
y = [70000,70000,70000,70000,1]
print("g1(17,17,1) =",eval_g(y,A,Ng,P))
y = [70000,70000,70000,70000,2]
print("g1(17,17,2) =",eval_g(y,A,Ng,P))

print("DONE")
