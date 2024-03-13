#include <iostream>
#include <vector>
#include <icecream.hpp>

using std::vector;


#define P (524287)

int F(const vector<int>& xy) {
    int k = 2;
    int fNv = k + k; // F(X, Y) so fNv = 2 * k
    vector<int> w = vector<int>(fNv, 0);
    int res = 0;
    while(true) {
        // IC(w);
        if (1) {
            res = res + 1;
            // res = res + compute_delta(w,xy);
            // compute_delta(w,xy);
            res = (res % P);
        }
        bool found = false;
        for(int i = fNv-1; i >= 0; i--){
            // w[i] = 1;
            if (w[i] == 0) {
                found = true;
                w[i] = 1;
                for(int j = i+1; j < fNv; j++) {
                    // IC(w, i, j);
                    w[j] = 0;
                }
                // IC(w, i);
                break;
            }  
        }
        // IC(xy, w, res);
        if (!found) 
            break;
    }
    IC(xy, w, res);
    return res;
}

int g(const vector<int>& xyz) {
    int k = 2;
    int fNv = k + k, res = 0;
    vector<int> X(xyz.begin(), xyz.begin() + fNv);
    res = F(X) % P;
    if (res == 0)
        return 0;
    std::copy(xyz.begin() + fNv, xyz.end(), X.begin() + k);
    // IC(xyz, X);
    int r = F(X) % P;
    if (r == 0) {
        return 0;
    }
    res = (res * r) % P;
    std::copy(xyz.begin() + k, xyz.end(), X.begin());
    // IC(xyz, X);
    r = F(X) % P;
    if (r == 0) {
        return 0;
    }
    res = (res * r) % P;
    return res;
}

int main(int argc, char *argv[]){
    vector<int> xyz = {0,0,0,0,0,0};
    g(xyz);
}

