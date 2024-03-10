#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mexit(const char* message) {
  printf("***%s\n",message);
  exit(0);
}//end mexit


void get_sizes(int Ng,int& gNv,int& fNv) {
  // Ng = 2^k, fNv = 2*k, gNv = 3*k
  if (Ng == 4) {
    fNv = 4;
    gNv = 6;
  }else if (Ng == 8) {
    fNv = 6;
    gNv = 9;
  }else if (Ng == 16) {
    fNv = 8;
    gNv = 12;
  }else if (Ng == 32) {
    fNv = 10;
    gNv = 15;
  }else
    mexit("wrong size of the input graph\n");
}//end get_sizes


int compute_delta(int w[],int x[],int fNv,int P) {
  int res = 1;
  for(int i = 0; i < fNv; i++)
    if (w[i] == 1)
      res = res * x[i];
    else
      res = res * (1-x[i]);
  res = (res % P);
  return(res);
}//#end compute_delta


void checkA(int** A,int Ng) {
  for (int a = 0; a < Ng; a++) {
    if (A[a][a] == 1)
      mexit("checkA error1");
    for (int b = 0; b < a; b++) 
      if (A[a][b] != A[b][a])
        mexit("checkA error2");
   }
  printf("A OK\n");
}//#end checkA


int f(int x[],int** A,int Ng) {
  int gNv, fNv, a, b; 
  get_sizes(Ng,gNv,fNv);
  if (fNv == 4) {
    a = 2*x[0]+x[1];
    b = 2*x[2]+x[3];
  }else if (fNv == 6) {
    a = 4*x[0]+2*x[1]+x[2];
    b = 4*x[3]+2*x[4]+x[5];
  }else if (fNv == 8) {
    a = 8*x[0]+4*x[1]+2*x[2]+x[3];
    b = 8*x[4]+4*x[5]+2*x[6]+x[7];
  }else{  // fNv == 10
    a = 16*x[0]+8*x[1]+4*x[2]+2*x[3]+x[4];
    b = 16*x[5]+8*x[6]+4*x[7]+2*x[8]+x[9];
  }
  return(A[a][b]);
}// #end f


int F(int x[],int** A,int Ng,int P) {
  int fNv, gNv;
  get_sizes(Ng,gNv,fNv);
  int* w = new int[fNv];
  for(int i = 0; i < fNv; i++)
    w[i] = 0;
  int res = 0;
  while(true) {
    if (f(w,A,Ng) == 1) {
      res = res+compute_delta(w,x,fNv,P);
      res = (res % P);
    }
    bool found = false;

    for(int i = fNv-1; i >= 0; i--) {
      if (w[i] == 0) {
        found = true;
        w[i] = 1;
        for(int j = i+1; j < fNv; j++) 
          w[j] = 0;
        break; // top forloop
      }  
    }// endforloop
    if (! found) 
      break; //whileloop
  }//endwhile
  delete[] w;
  return res;
}//#end F


void checkF(int** A,int Ng,int P) {
  int fNv, gNv, r1, r2;
  get_sizes(Ng,gNv,fNv);
  int* x = new int[fNv];
  for(int i = 0; i < fNv; i++)
    x[i] = 0;
  while(true) {
    r1 = F(x,A,Ng,P);
    r2 = f(x,A,Ng);
    if (r1 != r2) {
      printf("problem on ");
      for (int i = 0; i < fNv; i++)
        printf("%d",x[i]);
      mexit("");
    }
    bool found = false;
    for (int i = fNv-1; i >= 0; i--) {
      if (x[i] == 0) {
        found = true;
        x[i] = 1;
        for(int j = i+1; j < fNv; j++) 
          x[j] = 0;
        break; //forloop
      }
    }//endforloop
    if (! found)
      break; //whileloop
  }//#endwhiloop
  delete[] x;
  printf("F OK\n");
}//#endcheckF


int g(int x[],int** A,int Ng,int P) {
  int fNv, gNv, b, res;
  get_sizes(Ng,gNv,fNv);
  b = fNv/2;
  int *y = new int[fNv];
  for(int i = 0; i < fNv; i++)
    y[i] = x[i];
  res = F(y,A,Ng,P);
  if (res == 0)
    return 0;
  for(int i = b; i < fNv; i++)
    y[i] = x[i+b];
  int r = F(y,A,Ng,P);
  r = (r % P);
  if (r == 0) {
    delete[] y;
    return 0;
  }
  res = res * r;
  res = (res % P);
  for(int i = 0; i < fNv; i++)
    y[i] = x[i+b];
  r = F(y,A,Ng,P);
  r = (r % P);
  if (r == 0) {
    delete[] y;
    return 0;
  }
  res = res * r;
  res = (res % P);
  delete[] y;
  return res;
}//end g

int eval_g(int* r,int rlen, int** A,int Ng,int P) {
  int fNv, gNv, res;
  int * y;
  get_sizes(Ng,gNv,fNv);
  y = new int[gNv];
  for(int i = 0; i < gNv; i++) {
    if (i < rlen)
      y[i] = r[i];
    else
      y[i] = 0;
  }
  res = 0;
  while(true) {
    res = res+g(y,A,Ng,P);
    res = (res % P);
    bool found = false;
    for(int i = gNv-1; i > rlen-1; i--) {
      if (y[i] == 0) {
        found = true;
        y[i] = 1;
        for(int j = i+1; j < gNv; j++) 
          y[j] = 0;
        break; //forloop
      }
    }//endforloop
    if (! found)
      break; //whileloop
  }//endwhile
  delete[] y;
  return res;
}//end eval_g


int** createA(int Ng) {
  int** A = new int*[Ng];
  for(int i = 0; i < Ng; i++) {
    A[i] = new int[Ng];
    for(int j = 0; j < Ng; j++) 
      A[i][j] = 0;
  }
  return A;
}

void deleteA(int** A,int Ng) {
  for(int i = 0; i < Ng; i++)
    delete[] A[i];
  delete[] A;
}


int main() {
  int P = 524287;
  int Ng = 4;
  int fNv, gNv;
  get_sizes(Ng,gNv,fNv);
  int** A = createA(Ng);
  A[0][1] = A[1][0] = A[0][2] = A[2][0] = A[1][2] = A[2][1] = 1;
  checkA(A,Ng);
  checkF(A,Ng,P);
  int* y = new int[1];
  int rlen = 0;
  printf("G = %d\n",eval_g(y,rlen, A, Ng, P));
  y[0] = 0;
  rlen = 1;
  printf("g1(0)=%d\n",eval_g(y,rlen,A,Ng,P));
  y[0] = 1;
  printf("g1(1)=%d\n",eval_g(y,rlen,A,Ng,P));
  y[0] = 2;
  printf("g1(2)=%d\n",eval_g(y,rlen,A,Ng,P));
  delete[] y;
  y = new int[2];
  y[0] = 3;
  y[1] = 4;
  rlen = 2;
  printf("g1(3,4)=%d\n",eval_g(y,rlen,A,Ng,P));
  deleteA(A,Ng);
  delete[] y;
  printf("\nDONE\n");
  getchar();
  return 0;
}
