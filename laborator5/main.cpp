#include <iostream>
#include <string>
using namespace std;

// Clasa simpla ca la tema 3: un vector alocat dinamic
class VectorDinamic {
private:
    int n;
    int* v;

public:
    // constructor
    VectorDinamic(int n = 0, int val = 0) {
        this->n = n;
        v = new int[n];
        for (int i = 0; i < n; i++)
            v[i] = val;
        cout << "Constructor: am alocat " << n << " elemente\n";
    }

    // copy constructor (deep copy)
    VectorDinamic(const VectorDinamic& alt) {
        n = alt.n;
        v = new int[n];
        for (int i = 0; i < n; i++)
            v[i] = alt.v[i];
        cout << "Copy constructor\n";
    }

    // move constructor: iau pointerul, nu mai copiez nimic
    VectorDinamic(VectorDinamic&& alt) noexcept {
        n = alt.n;
        v = alt.v;
        alt.n = 0;
        alt.v = nullptr;   // alt nu mai detine nimic
        cout << "Move constructor\n";
    }

    // Item 10 + 11: operator= returneaza *this ca sa mearga a = b = c
    // Copiez INTAI, sterg DUPA, ca sa mearga si la a = a
    // (daca stergeam intai, la a = a pierdeam datele)
    VectorDinamic& operator=(const VectorDinamic& alt) {
        cout << "Operator= (copy)\n";
        int* nou = new int[alt.n];
        for (int i = 0; i < alt.n; i++)
            nou[i] = alt.v[i];
        delete[] v;        // acum pot sterge vechiul vector
        v = nou;
        n = alt.n;
        return *this;
    }

    // move assignment: obiectul are deja memorie, deci o sterg intai
    // altfel ar fi memory leak
    VectorDinamic& operator=(VectorDinamic&& alt) noexcept {
        cout << "Operator= (move)\n";
        if (this != &alt) {
            delete[] v;
            v = alt.v;
            n = alt.n;
            alt.v = nullptr;
            alt.n = 0;
        }
        return *this;
    }

    // destructor
    ~VectorDinamic() {
        cout << "Destructor: eliberez " << n << " elemente\n";
        delete[] v;
    }

    void set(int i, int val) { v[i] = val; }

    void afiseaza() const {
        cout << "[ ";
        for (int i = 0; i < n; i++)
            cout << v[i] << " ";
        cout << "]\n";
    }

    int getN() const { return n; }
};

// Item 12: clasa derivata. Trebuie copiate TOATE partile,
// inclusiv partea din clasa de baza
class VectorEtichetat : public VectorDinamic {
private:
    string eticheta;

public:
    VectorEtichetat(int n, int val, string eticheta)
        : VectorDinamic(n, val), eticheta(eticheta) {}

    // copy constructor: apelez explicit copy constructorul din baza
    VectorEtichetat(const VectorEtichetat& alt)
        : VectorDinamic(alt), eticheta(alt.eticheta) {}

    // operator=: apelez explicit operator= din baza
    // (daca uitam, se copia doar eticheta, nu si vectorul)
    VectorEtichetat& operator=(const VectorEtichetat& alt) {
        VectorDinamic::operator=(alt);
        eticheta = alt.eticheta;
        return *this;
    }

    void afiseaza() const {
        cout << eticheta << ": ";
        VectorDinamic::afiseaza();
    }
};

int main() {
    cout << "--- Item 10: atribuire inlantuita ---\n";
    VectorDinamic a(3, 1), b(2, 2), c(4, 3);
    a = b = c;
    a.afiseaza();

    cout << "\n--- Item 11: auto-atribuire ---\n";
    c.set(0, 99);
    c = c;
    c.afiseaza();   // datele au ramas intacte

    cout << "\n--- Move assignment ---\n";
    VectorDinamic d(2, 7);
    d = move(c);
    d.afiseaza();
    cout << "c dupa move are " << c.getN() << " elemente\n";

    cout << "\n--- Item 12: copiem toate partile ---\n";
    VectorEtichetat x(3, 5, "x"), y(1, 0, "y");
    y = x;
    y.afiseaza();   // s-au copiat si vectorul, si eticheta

    cout << "\n--- Sfarsit program ---\n";
    return 0;
}
