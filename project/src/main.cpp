#include <iostream>
#include <vector>
#include <string>
#include <random>

using std::string;
using std::vector;


#define P (524287)



class Prover{
private:
    vector<vector<int>> A;
    int Ng;
    int k;
    vector<int> r;  // record all the chosen value by verifier
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
    int eval_g(const vector<int>& xyz) {
        int gNv = k + k + k, res = 0, rlen = xyz.size();
        vector<int> X = vector<int>(gNv, 0);
        std::copy(xyz.begin(), xyz.end(), X.begin());

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
        r = {};
    }
    vector<int> Receive(const vector<int>& message){
        // message: round flag, the new chosen value from Verifier(empty at first round).
        // res: round flag, {G (first round); gi(0), gi(1), gi(2) (other rounds)}
        int round_flag = message[0];
        // compute G and return 
        if(round_flag == 0)
            return {round_flag, eval_g({})};
        // add the new chosen value to r
        r.push_back(message[1]);
        // compute gi at 0, 1, 2
        vector<int> res = {round_flag};
        for(int i = 0;i < 3;i++){
            r.push_back(i);
            res.push_back(eval_g(r));
            r.pop_back();
        }
        return res;
    }
};

class Verifier{
private:
    int k;
    int G_last;  // value of G_i from last round (with self-chosen random value)
    vector<int> r;
    int randValue(const int& low, const int& high){
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(low, high); // define the range
        return distr(gen);
    }
    inline int LagrangeEval(const int& r_new, const int& g0, const int& g1, const int& g2){
        return int(0.5*g0*(r_new-1)*(r_new-2))%P - int(g1*r_new*(r_new-2))%P + int(0.5*g2*r_new*(r_new-1))%P;
    }
    int oracle(){
        return eval_g(r);
    }
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
    int eval_g(const vector<int>& xyz) {
        int gNv = k + k + k, res = 0, rlen = xyz.size();
        vector<int> X = vector<int>(gNv, 0);
        std::copy(xyz.begin(), xyz.end(), X.begin());

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
    Verifier(const int& k_){
        k = k_;
    }
    vector<int> Receive(const vector<int>& message){
        // message: round flag, {G (first round); gi(0), gi(1), gi(2) (other rounds)}
        // res: round flag, the new chosen value.
        int round_flag = message[0], r_new = randValue(round_flag, P);
        r.push_back(r_new);
        if(round_flag == 0)
            G_last = message[1];        
        else{
            // check if new evaluation match previous
            int g0 = LagrangeEval(0, message[1], message[2], message[3]);
            int g1 = LagrangeEval(1, message[1], message[2], message[3]);
            // check if match last round: return negative round_flag to indicate a failure
            if(((g0 + g1) % P) != G_last)
                return {-round_flag};
            // check if last round; if so, use oracle access to check G(r)
            G_last = LagrangeEval(r_new, message[1], message[2], message[3]);
            if(round_flag == k + k + k){
                if(oracle() == G_last)
                    return {0}; 
                return {-round_flag};
            }
        }
        return {++round_flag, r_new};
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
