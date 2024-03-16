/**
 * @file main.cpp
 * @author {Eva Wu} ({gudautd@gmail.com})
 * @brief A co-routine based interactive proof protocol solving counting
 * triangle number in a graph (size limited to 4, 8, 16, 32)
 * @version 0.1
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <iostream>
#include <random>
#include <string>
#include <vector>

using std::string;
using std::vector;

#define P (524287)

/**
 * @class Prover
 * @brief Represents the prover in an interactive proof system.
 *
 * This class contains methods to perform computations required by the prover
 * to interact with the verifier.
 */
class Prover {
private:
  vector<vector<int>> A; ///< Adjacency matrix of the graph
  int Ng;                ///< Number of graph nodes
  int k;                 ///< Binary length of a node index
  vector<int> r;         ///< Record of all random values chosen by the verifier
  int compute_delta(const vector<int> &w, const vector<int> &xy) {
    long long res = 1, fNv = k + k;
    for (int i = 0; i < fNv; i++)
      if (w[i] == 1)
        res = res * xy[i] % P;
      else
        res = res * (1 - xy[i]) % P;
    res = (res % P + P) % P;
    return int(res);
  }
  int f(const vector<int> &xy) {
    int a, b;
    if (k == 2) {
      a = 2 * xy[0] + xy[1];
      b = 2 * xy[2] + xy[3];
    } else if (k == 3) {
      a = 4 * xy[0] + 2 * xy[1] + xy[2];
      b = 4 * xy[3] + 2 * xy[4] + xy[5];
    } else if (k == 4) {
      a = 8 * xy[0] + 4 * xy[1] + 2 * xy[2] + xy[3];
      b = 8 * xy[4] + 4 * xy[5] + 2 * xy[6] + xy[7];
    } else { // k == 5
      a = 16 * xy[0] + 8 * xy[1] + 4 * xy[2] + 2 * xy[3] + xy[4];
      b = 16 * xy[5] + 8 * xy[6] + 4 * xy[7] + 2 * xy[8] + xy[9];
    }
    return A[a][b];
  }
  int F(const vector<int> &xy) {
    int fNv = k + k; // F(X, Y) so fNv = 2 * k
    vector<int> w = vector<int>(fNv, 0);
    long long res = 0;
    while (true) {
      if (f(w)) {
        res = res + compute_delta(w, xy);
        res = (res % P + P) % P;
      }
      bool found = false;
      for (int i = fNv - 1; i >= 0; i--)
        if (w[i] == 0) {
          found = true;
          w[i] = 1;
          for (int j = i + 1; j < fNv; j++)
            w[j] = 0;

          break;
        }
      if (!found)
        break;
    }
    return int(res);
  }
  int g(const vector<int> &xyz) {
    int fNv = k + k;
    long long res = 0;
    vector<int> X(xyz.begin(), xyz.begin() + fNv);
    res = F(X) % P;
    if (res == 0)
      return 0;
    std::copy(xyz.begin() + fNv, xyz.end(), X.begin() + k);
    int r = F(X) % P;
    if (r == 0) {
      return 0;
    }
    res = (res * r) % P;
    std::copy(xyz.begin() + k, xyz.end(), X.begin());
    r = F(X) % P;
    if (r == 0) {
      return 0;
    }
    res = (res * r) % P;
    return int(res);
  }
  int eval_g(const vector<int> &xyz) {
    int gNv = k + k + k, rlen = xyz.size();
    long long res = 0;
    vector<int> X = vector<int>(gNv, 0);
    std::copy(xyz.begin(), xyz.end(), X.begin());
    while (true) {
      res = res + g(X);
      res = (res % P + P) % P;
      bool found = false;
      for (int i = gNv - 1; i > rlen - 1; i--) {
        if (X[i] == 0) {
          found = true;
          X[i] = 1;
          for (int j = i + 1; j < gNv; j++)
            X[j] = 0;
          break;
        }
      }
      if (!found)
        break;
    }
    return int(res);
  }

public:
  Prover(vector<vector<int>> A_, int Ng_, int k_) {
    A = A_;
    Ng = Ng_;
    k = k_;
    r = {};
  }

  /**
   * Receives a message from the verifier and processes it.
   *
   * Based on the round and the content of the message, performs necessary
   * computations and returns a response.
   *
   * @param message A vector containing the message from the verifier.
   * @return A vector containing the prover's response.
   */
  vector<int> Receive(const vector<int> &message) {
    // message: round flag, the new chosen value r_new from Verifier(empty at 0
    // and 1st round) res: round flag, {G (first round); gi(0), gi(1), gi(2)
    // (other rounds)}
    int round_flag = message[0];
    if (round_flag == 0) // compute G and return
      return {round_flag, eval_g({})};
    // add the new chosen value to r
    if (round_flag > 1)
      r.push_back(message[1]);
    // compute gi at 0, 1, 2
    vector<int> res = {round_flag};
    for (int i = 0; i < 3; i++) {
      r.push_back(i);
      res.push_back(eval_g(r));
      r.pop_back();
    }
    return res;
  }
};

/**
 * @class Verifier
 * @brief Represents the verifier in the interactive proof system.
 *
 * This class contains methods for the verifier to generate challenges and
 * verify the prover's responses. It includes functions for random value
 * generation, Lagrange polynomial evaluation, and oracle access to the total
 * number of triangles in graph A.
 */
class Verifier {
private:
  vector<vector<int>> A;
  int Ng;
  int k;
  int G_last; // value of G_i from last round (with self-chosen random value)
  vector<int> r;
  void log(const string &msg) { std::cout << "Verifier " << msg << "\n"; }
  int randValue(const int &low, const int &high) {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(low, high); // define the range
    return distr(gen);
  }
  inline int LagrangeEval(const int &r_new, const int &g0, const int &g1,
                          const int &g2) {
    long res = (long(g0) * (r_new - 1) * (r_new - 2) >> 1) % P -
               (long(g1) * r_new * (r_new - 2)) % P +
               (long(g2) * r_new * (r_new - 1) >> 1) % P;
    return int(res % P + P) % P;
  }
  int oracle() { return eval_g(r); }
  int compute_delta(const vector<int> &w, const vector<int> &xy) {
    long long res = 1, fNv = k + k;
    for (int i = 0; i < fNv; i++)
      if (w[i] == 1)
        res = res * xy[i] % P;
      else
        res = res * (1 - xy[i]) % P;
    res = (res % P + P) % P;
    return int(res);
  }
  int f(const vector<int> &xy) {
    int a, b;
    if (k == 2) {
      a = 2 * xy[0] + xy[1];
      b = 2 * xy[2] + xy[3];
    } else if (k == 3) {
      a = 4 * xy[0] + 2 * xy[1] + xy[2];
      b = 4 * xy[3] + 2 * xy[4] + xy[5];
    } else if (k == 4) {
      a = 8 * xy[0] + 4 * xy[1] + 2 * xy[2] + xy[3];
      b = 8 * xy[4] + 4 * xy[5] + 2 * xy[6] + xy[7];
    } else { // k == 5
      a = 16 * xy[0] + 8 * xy[1] + 4 * xy[2] + 2 * xy[3] + xy[4];
      b = 16 * xy[5] + 8 * xy[6] + 4 * xy[7] + 2 * xy[8] + xy[9];
    }
    return A[a][b];
  }
  int F(const vector<int> &xy) {
    int fNv = k + k; // F(X, Y) so fNv = 2 * k
    vector<int> w = vector<int>(fNv, 0);
    long long res = 0;
    while (true) {
      if (f(w)) {
        res = res + compute_delta(w, xy);
        res = (res % P + P) % P;
      }
      bool found = false;
      for (int i = fNv - 1; i >= 0; i--)
        if (w[i] == 0) {
          found = true;
          w[i] = 1;
          for (int j = i + 1; j < fNv; j++)
            w[j] = 0;

          break;
        }
      if (!found)
        break;
    }
    return int(res);
  }
  int g(const vector<int> &xyz) {
    int fNv = k + k;
    long long res = 0;
    vector<int> X(xyz.begin(), xyz.begin() + fNv);
    res = F(X) % P;
    if (res == 0)
      return 0;
    std::copy(xyz.begin() + fNv, xyz.end(), X.begin() + k);
    int r = F(X) % P;
    if (r == 0) {
      return 0;
    }
    res = (res * r) % P;
    std::copy(xyz.begin() + k, xyz.end(), X.begin());
    r = F(X) % P;
    if (r == 0) {
      return 0;
    }
    res = (res * r) % P;
    return int(res);
  }
  int eval_g(const vector<int> &xyz) {
    int gNv = k + k + k, rlen = xyz.size();
    long long res = 0;
    vector<int> X = vector<int>(gNv, 0);
    std::copy(xyz.begin(), xyz.end(), X.begin());
    while (true) {
      res = res + g(X);
      res = (res % P + P) % P;
      bool found = false;
      for (int i = gNv - 1; i > rlen - 1; i--) {
        if (X[i] == 0) {
          found = true;
          X[i] = 1;
          for (int j = i + 1; j < gNv; j++)
            X[j] = 0;
          break;
        }
      }
      if (!found)
        break;
    }
    return int(res);
  }

public:
  Verifier(vector<vector<int>> A_, int Ng_, int k_) {
    A = A_;
    Ng = Ng_;
    k = k_;
    r = {};
  }
  vector<int> Receive(const vector<int> &message) {
    // message: round flag, {G (first round); gi(0), gi(1), gi(2) (other
    // rounds)} res: round flag, the new chosen value(empty at 0 round).
    int round_flag = message[0];
    if (round_flag == 0) {
      G_last = message[1];
      log("round " + std::to_string(round_flag) + ": G = " +
          std::to_string(G_last) + ", G/6 = " + std::to_string(G_last / 6));
      return {++round_flag};
    }
    int r_new = randValue(round_flag, P);
    r.push_back(r_new);
    // check if new evaluation match previous
    int g0 = message[1], g1 = message[2];
    // check if match last round: return negative round_flag to indicate a
    // failure
    if (((g0 + g1) % P) != G_last)
      return {-round_flag, G_last, g0, g1};
    log("round " + std::to_string(round_flag) + ": " + std::to_string(g0) +
        " + " + std::to_string(g1) + " %= " + std::to_string(G_last));
    // check if last round; if so, use oracle access to check G(r)
    G_last = LagrangeEval(r_new, message[1], message[2], message[3]);
    if (round_flag == k + k + k) {
      int G_oracle = oracle();
      if (G_oracle == G_last) {
        log("round " + std::to_string(round_flag) +
            ": Oracle value matches at: " + std::to_string(G_oracle));
        return {0};
      }
      return {-round_flag, G_oracle, G_last};
    }

    return {++round_flag, r_new};
  }
};

/**
 * @class IP
 * @brief Wrapper class for grouping the process together using Prover and
 * Verifier.
 *
 * This class encapsulates the interaction between a prover and a verifier to
 * validate properties of a given graph. It includes methods for setting up
 * the proof system, creating helper matrices, and conducting the interactive
 * proof rounds.
 */
class IP {
private:
  int Ng; // size of graph (number of nodes)
  int k;  // length of binary representation of one node (2^k = Ng)
  vector<vector<int>> A;
  void mexit(string message) {
    std::cout << ("***" + message);
    exit(0);
  }
  void checkA() {
    for (int a = 0; a < Ng; a++) {
      if (A[a][a] == 1)
        mexit("checkA error1: 1 on diagonal");
      for (int b = 0; b < a; b++)
        if (A[a][b] != A[b][a])
          mexit("checkA error2: transpose not equal");
    }
    std::cout << "A OK\n";
  }
  bool setA(vector<vector<int>> matrix) {
    A = matrix;
    return true;
  }
  void setSize() {
    Ng = A.size();
    k = int(log2(Ng));
  }
  void checkVerifierMsg(const vector<int> &msg) {
    // msg: round flag, the new chosen value.
    if (msg[0] < 0) {
      if (msg.size() == 3) // oracle fail
        mexit("Verifier: values fail to match. Supposed to be " +
              std::to_string(msg[1]) + ", got " + std::to_string(msg[2]) + ".");
      else
        mexit("Verifier: values fail to match. Supposed to be " +
              std::to_string(msg[1]) + ", got " + std::to_string(msg[2]) +
              " + " + std::to_string(msg[3]) + ".");
    }
    if (msg[0] == 0)
      mexit("End of protocol.");
    std::cout << "Round " << msg[0] - 1 << ": chose new random value " << msg[1]
              << ".\n";
  }

public:
  /**
   * @brief The function for coordinating the IP
   *
   * @param A_ adjacency matrix representing the graph
   */
  void triangle(vector<vector<int>> A_) {
    // set up
    A = A_;
    setSize();
    checkA();

    Prover prover = Prover(A, Ng, k);
    Verifier verifier = Verifier(A, Ng, k);
    // round 0: give G
    vector<int> message = {0};           // initial message
    message = prover.Receive(message);   // {0, G}
    message = verifier.Receive(message); // {1}
    // start 3k round of interactive proof
    for (int round = 1; round <= k + k + k; round++) {
      message = prover.Receive(message);
      message = verifier.Receive(message);
      // check message from verifier
      checkVerifierMsg(message);
    }
  }

  /**
   * @brief Helper function to create a full adjacency matrix
   *
   * @param matrix reference to the adjacency matrix this function is to modify
   */
  void createAHelper_full(vector<vector<int>> &matrix) {
    int length = matrix.size();
    for (int i = 0; i < length - 1; i++) {
      for (int j = i + 1; j < length; j++) {
        matrix[i][j] = 1;
        matrix[j][i] = 1;
      }
    }
  }
};

int main(int argc, char *argv[]) {
  vector<vector<int>> A = vector<vector<int>>(32, vector<int>(32, 0));
  IP ip = IP();
  ip.createAHelper_full(A);
  ip.triangle(A);
}