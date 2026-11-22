#include <iostream>
#include <memory>
#include <string>
using namespace std;

class Investment {
private:
    string nume;

public:
    Investment(string nume) {
        this->nume = nume;
        cout << "Investment creat: " << nume << "\n";
    }
    ~Investment() {
        cout << "Investment distrus: " << nume << "\n";
    }
};

Investment* createInvestment(string nume) {
    return new Investment(nume);   // cine apeleaza trebuie sa faca delete
}

// Item 13: varianta cu delete manual
void manual(bool eroare) {
    Investment* p = createInvestment("manual");
    if (eroare)
        return;        // ies mai devreme, delete nu mai ruleaza: LEAK
    delete p;
}

// Item 13: varianta cu unique_ptr (RAII)
void cuRAII(bool eroare) {
    unique_ptr<Investment> p(createInvestment("raii"));
    if (eroare)
        return;        // destructorul lui p sterge Investment oricum
}

// Item 14: resursa care nu e memorie, un mutex
struct Mutex {
    string nume;
};

void lock(Mutex* m) { cout << "lock " << m->nume << "\n"; }
void unlock(Mutex* m) { cout << "unlock " << m->nume << "\n"; }

// Varianta 1: interzic copierea (are cel mai mult sens pentru un lock)
class Lock {
private:
    Mutex* m;

public:
    Lock(Mutex* m) {
        this->m = m;
        lock(m);
    }
    ~Lock() {
        unlock(m);
    }

    // fara copiere, altfel s-ar face unlock de doua ori
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
};

// Varianta 2: numar cate copii exista, shared_ptr cu deleter propriu
// unlock se apeleaza doar cand dispare ultima copie
class LockRC {
private:
    shared_ptr<Mutex> m;

public:
    LockRC(Mutex* pm) : m(pm, unlock) {
        lock(pm);
    }
    int copii() const { return m.use_count(); }
};

int main() {
    cout << "--- Item 13: fara RAII ---\n";
    manual(true);      // Investment-ul nu se mai distruge niciodata

    cout << "\n--- Item 13: cu RAII ---\n";
    cuRAII(true);      // se distruge singur

    cout << "\n--- unique_ptr: un singur proprietar ---\n";
    unique_ptr<Investment> p1 = make_unique<Investment>("p1");
    // unique_ptr<Investment> p2 = p1;   // eroare, nu se poate copia
    unique_ptr<Investment> p3 = move(p1);   // acum p1 e null
    cout << "p1 este null? " << (p1 == nullptr) << "\n";

    cout << "\n--- shared_ptr: mai multi proprietari ---\n";
    shared_ptr<Investment> s1 = make_shared<Investment>("s1");
    {
        shared_ptr<Investment> s2 = s1;
        cout << "use_count = " << s1.use_count() << "\n";   // 2
    }
    cout << "use_count = " << s1.use_count() << "\n";       // 1

    cout << "\n--- Item 14: Lock fara copiere ---\n";
    Mutex mtx{"mtx"};
    {
        Lock l(&mtx);
        // Lock l2(l);   // eroare de compilare
    }

    cout << "\n--- Item 14: Lock cu numarare de referinte ---\n";
    {
        LockRC l1(&mtx);
        {
            LockRC l2(l1);
            cout << "copii = " << l1.copii() << "\n";   // 2
        }
        cout << "copii = " << l1.copii() << "\n";       // 1
    }   // unlock se apeleaza o singura data, aici

    cout << "\n--- Sfarsit program ---\n";
    return 0;
}
