#include <cstdio>
#include <algorithm>
#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpz_mod_polyxx.h>

using namespace std;
using namespace flint;

// 2^160 - 47
#define P "1461501637330902918203684832716283019655932542929"
#define STR_FAIL "FAIL"
#define STR_SUCC "SUCC"

int main(int argc, char* argv[])
{
    int n;

    // Read length of vector
    cin >> n;
    if (n < 2) {
        cout << "Input vector too short." << endl;
        return EXIT_FAILURE;
    }

    // Prime, polynomial and factorization
    fmpzxx p(P);
    fmpz_mod_polyxx poly(p);
    fmpz_mod_poly_factorxx factors;
    factors.fit_length(n);

    // Integer arrays
    fmpzxx *s = new fmpzxx[n];
    fmpzxx *coeff = new fmpzxx[n];
    fmpzxx *inv = new fmpzxx[n];
    fmpzxx *roots = new fmpzxx[n];

    // Precompute inverses
    for (int i = 0; i < n; i++) {
        inv[i] = -(i+1);
        inv[i] = inv[i].invmod(p);
    }

    // Read own message from stdin
    fmpzxx m;
    m.read();

    // Read power sums from stdin
    for (int i = 0; i < n; i++) {
        s[i].read();
        coeff[i] = s[i];
    }

    // Set lead coefficient
    poly.set_coeff(n, 1);

    // Compute other coeffients
    for (int i = 0; i < n; i++) {
        int k = 0;
        for (int j = i-1; j >= 0; j--) {
            coeff[i] += coeff[k] * s[j];
            k++;
        }
        coeff[i] *= inv[i];
        poly.set_coeff(n-i-1, coeff[i]);
    }

#ifdef DEBUG
    cout << "Polynomial: "; print(poly); cout << endl;
#endif

    // Check if our message is a root
    if (poly(m) != 0) {
        cout << STR_FAIL << " ";
        cout << "Message missing." << endl;
        return 0;
    }

    // Factor
    factors.set_factor_kaltofen_shoup(poly);

#ifdef DEBUG
    cout << "Factors: "; print(factors); cout << endl;
#endif

    int n_roots = 0;
    for (int i = 0; i < factors.size(); i++) {
        if (factors.p(i).degree() != 1 || factors.p(i).lead() != 1) {
            cout << STR_FAIL << " ";
            cout << "Non-monic factor." << endl;
            return 0;
        }
        n_roots += factors.exp(i);
    }
    if (n_roots != n) {
        cout << STR_FAIL << " ";
        cout << "Not enough roots" << endl;
        return 0;
    }

    // Extract roots
    int k = 0;
    bool found = false;
    for (int i = 0; i < factors.size(); i++) {
        for (int j = 0; j < factors.exp(i); j++) {
            roots[k] = factors.p(i).get_coeff(0).negmod(p);
            found |= roots[k] == m;
            k++;
        }
    }

    // Sanity check
    if (!found) {
        cout << "Bug: Our message is a root but not in list of roots." << endl;
        return EXIT_FAILURE;
    }

    sort(roots, roots + n);

    cout << STR_SUCC << " ";
    for (int i = 0; i < n; i++) {
        cout << roots[i] << " ";
    }
    cout << endl;

    return 0;
}

