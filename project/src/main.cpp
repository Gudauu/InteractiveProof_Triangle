#include <iostream>
#include <vector>
#include <string>

using std::string;
using std::vector;


#define P (524287)



class Prover{
private:
    vector<vector<int>> A;
    int Ng;
    int k;
    int compute_delta(const vector<int>& w, const vector<int>& xy) {
        int res = 1, fNv = k + k;
        for(int i = 0; i < k; i++)
            if (w[i] == 1)
                res = res * xy[i];
            else
                res = res * (1-xy[i]);
        res = res % P;
        return res;
    }
    int f(const vector<int>& xy) {
        int a, b; 
        if (k == 2) {
            a = 2*xy[0]+xy[1];
            b = 2*xy[2]+xy[3];
        }else if (k == 3) {
            a = 4*xy[0]+2*xy[1]+xy[2];
            b = 4*xy[3]+2*xy[4]+xy[5];
        }else if (k == 4) {
            a = 8*xy[0]+4*xy[1]+2*xy[2]+xy[3];
            b = 8*xy[4]+4*xy[5]+2*xy[6]+xy[7];
        }else{  // k == 5
            a = 16*xy[0]+8*xy[1]+4*xy[2]+2*xy[3]+xy[4];
            b = 16*xy[5]+8*xy[6]+4*xy[7]+2*xy[8]+xy[9];
        }
        return A[a][b];
    }
    int F(const vector<int>& xy) {
        vector<int> w = vector<int>(k, 0);
        int res = 0;
        while(true) {
            if (f(w)) {
                res = res + compute_delta(w,xy);
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
    int g(const vector<int>& xyz) {
        int fNv = k + k, res = 0;
        vector<int> X = xyz;
        res = F(X) % P;
        if (res == 0)
            return 0;
        std::copy(xyz.begin() + fNv, xyz.end() + fNv, X.begin() + k);
        int r = F(X) % P;
        if (r == 0) {
            return 0;
        }
        res = (res * r) % P;
        std::copy(xyz.begin() + k, xyz.end() + fNv, X.begin());
        r = F(X) % P;
        if (r == 0) {
            return 0;
        }
        res = (res * r) % P;
        return res;
    }
    int eval_g(const vector<int>& xyz,int rlen) {
        int gNv = k + k + k, res = 0;
        vector<int> X = vector<int>(gNv, 0);
        std::copy(xyz.begin(), xyz.begin() + rlen, X.begin());

        while(true) {
            res = res + g(X);
            res = (res % P);
            bool found = false;
            for(int i = gNv-1; i > rlen-1; i--) {
                if (X[i] == 0) {
                    found = true;
                    X[i] = 1;
                    for(int j = i+1; j < gNv; j++) 
                        X[j] = 0;
                    break;
                }
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
    void mexit(string message) {
        std::cout << ("***" + message);
        exit(0);
    }
    void checkA() {
        for (int a = 0; a < Ng; a++) {
            if (A[a][a] == 1)
                mexit("checkA error1");
            for (int b = 0; b < a; b++) 
                if (A[a][b] != A[b][a])
                    mexit("checkA error2");
        }
        std::cout << "A OK\n";
    }
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
