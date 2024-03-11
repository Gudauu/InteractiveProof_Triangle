#include <iostream>
#include <vector>

#define P (524287)

using std::vector;


class Prover{
private:
    vector<vector<int>> A;
    int Ng;
    int k;
    int compute_delta(const vector<int>& w, const vector<int>& x) {
        int res = 1;
        for(int i = 0; i < k; i++)
            if (w[i] == 1)
                res = res * x[i];
            else
                res = res * (1-x[i]);
        res = res % P;
        return res;
    }
    int f(const vector<int>& x) {
        int a, b; 
        if (k == 2) {
            a = 2*x[0]+x[1];
            b = 2*x[2]+x[3];
        }else if (k == 3) {
            a = 4*x[0]+2*x[1]+x[2];
            b = 4*x[3]+2*x[4]+x[5];
        }else if (k == 4) {
            a = 8*x[0]+4*x[1]+2*x[2]+x[3];
            b = 8*x[4]+4*x[5]+2*x[6]+x[7];
        }else{  // k == 5
            a = 16*x[0]+8*x[1]+4*x[2]+2*x[3]+x[4];
            b = 16*x[5]+8*x[6]+4*x[7]+2*x[8]+x[9];
        }
        return A[a][b];
    }
    int F(const vector<int>& x) {
        vector<int> w = vector<int>(k, 0);
        int res = 0;
        while(true) {
            if (f(w)) {
                res = res + compute_delta(w,x);
                res = (res % P);
            }
            bool found = false;
            int fNv = k + k; // F(X, Y) so fNv = 2 * k
            for(int i = fNv-1; i >= 0; i--) 
                if (w[i] == 0) {
                    found = true;
                    w[i] = 1;
                    for(int j = i+1; j < fNv; j++) 
                        w[j] = 0;
                    break;
                }  
            if (!found) 
                break;
        }
        return res;
    }
public:
    Prover(vector<vector<int>> A_, int Ng_, int k_){
        A = A_;
        Ng = Ng_;
        k = k_;
    }
    vector<int> Receive(){

    }
    

};

class Verifier{
public:
    vector<int> Receive(){
    }
};



class IP{
private:


public:
    int Ng;  // size of graph (number of nodes) 
    int k;  // length of binary representation of one node (2^k = Ng)
    vector<vector<int>> A;
    bool setA(vector<vector<int>> matrix){
        // validity check?
        A = matrix;
        return true;
    }
    void setSize(){
        Ng = A.size();
    }
    void triangle(){
        Prover prover = Prover(A, Ng, k);
        Verifier verifier = Verifier();
        // initial message to verifier
        verifier.Receive();
        // start 3k round of interactive proof
        vector<int> message;
        for(int round = 0;round < k+k+k;round++){
            message = prover.Receive();
            message = verifier.Receive();
        } 
    }

};

int main(int argc, char *argv[])
{
    std::cout << "Hello world!" << std::endl;
}
