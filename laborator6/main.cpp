// Effective C++ - Item 13: Use objects to manage resources
//                 Item 14: Think carefully about copying behavior
//                          in resource-managing classes

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
using namespace std;

// ============================================================
// Item 13: folosesc obiecte ca sa gestionez resursele (RAII)
// ============================================================

class Investment {
private:
    string nume;

public:
    Investment(const string& nume) : nume(nume) {
        cout << "Investment creat: " << nume << "\n";
    }
    ~Investment() {
        cout << "Investment distrus: " << nume << "\n";
    }
};

// functie de tip factory: cine o apeleaza primeste proprietatea
Investment* createInvestment(const string& nume) {
    return new Investment(nume);
}

// Varianta cu delete manual: gresita, exista drumuri pe care delete nu ruleaza
void manual(bool eroare) {
    Investment* p = createInvestment("manual");
    if (eroare)
        return;        // iesire anticipata: delete nu mai ruleaza => LEAK (intentionat, ca demonstratie)
    delete p;
}

// Varianta cu unique_ptr: resursa este data unui obiect care o sterge singur
void cuRAII(bool eroare) {
    unique_ptr<Investment> p(createInvestment("raii"));
    if (eroare)
        return;        // destructorul lui p sterge Investment oricum
}

// Varianta cu exceptie: delete manual nu ruleaza, RAII da
void cuExceptie() {
    unique_ptr<Investment> p(createInvestment("exceptie"));
    throw runtime_error("ceva a mers prost");
}

// ============================================================
// Item 14: comportamentul la copiere pentru clase care gestioneaza resurse
// ============================================================

// Resursa care nu e memorie: un mutex
struct Mutex {
    string nume;
};

void lock(Mutex* m)   { cout << "lock " << m->nume << "\n"; }
void unlock(Mutex* m) { cout << "unlock " << m->nume << "\n"; }

// ---- Varianta 1: interzic copierea ----
// Are cel mai mult sens pentru un lock: doua copii ar face unlock de doua ori.
class Lock {
private:
    Mutex* m;

public:
    Lock(Mutex* m) : m(m) {
        lock(m);
    }
    ~Lock() {
        unlock(m);
    }

    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
};

// ---- Varianta 2: numar referintele (reference counting) ----
// shared_ptr cu deleter propriu: unlock se apeleaza doar cand dispare ultima copie.
// Constructorul de copiere generat de compilator copiaza shared_ptr-ul, deci e corect.
class LockRC {
private:
    shared_ptr<Mutex> m;

public:
    LockRC(Mutex* pm) : m(pm, unlock) {
        lock(pm);
    }
    int copii() const { return m.use_count(); }
};

// ---- Varianta 3: copiere profunda (deep copy) ----
// Resursa e memorie alocata dinamic: fiecare obiect isi are propria copie.
// Fara constructor de copiere propriu, copia ar pastra acelasi pointer
// si ar urma un double delete.
class Mesaj {
private:
    string* text;

public:
    Mesaj(const string& t) : text(new string(t)) {
        cout << "Mesaj creat: " << *text << "\n";
    }

    Mesaj(const Mesaj& alt) : text(new string(*alt.text)) {
        cout << "Mesaj copiat (copie profunda): " << *text << "\n";
    }

    Mesaj& operator=(const Mesaj& alt) {
        if (this != &alt) {                      // protectie la auto-atribuire
            string* nou = new string(*alt.text); // alocam intai, ca sa nu pierdem nimic daca da eroare
            delete text;
            text = nou;
        }
        return *this;
    }

    ~Mesaj() {
        cout << "Mesaj distrus: " << *text << "\n";
        delete text;
    }

    void seteaza(const string& t) { *text = t; }
    const string& get() const { return *text; }
    const string* adresa() const { return text; }
};

// ---- Varianta 4: transfer de proprietate (move) ----
// Copierea e interzisa, dar proprietatea se poate muta: obiectul vechi ramane gol.
class LockMutabil {
private:
    Mutex* m;

public:
    LockMutabil(Mutex* m) : m(m) {
        lock(m);
    }
    ~LockMutabil() {
        if (m)                      // doar proprietarul curent face unlock
            unlock(m);
    }

    LockMutabil(const LockMutabil&) = delete;
    LockMutabil& operator=(const LockMutabil&) = delete;

    LockMutabil(LockMutabil&& alt) noexcept : m(alt.m) {
        alt.m = nullptr;            // alt nu mai detine nimic
    }

    bool detine() const { return m != nullptr; }
};

LockMutabil creeazaLock(Mutex* m) {
    return LockMutabil(m);          // proprietatea iese din functie spre apelant
}

// ============================================================

int main() {
    cout << "===== ITEM 13 =====\n";

    cout << "\n--- fara RAII ---\n";
    manual(true);      // Investment-ul nu se mai distruge niciodata

    cout << "\n--- cu RAII ---\n";
    cuRAII(true);      // se distruge singur

    cout << "\n--- RAII si exceptii ---\n";
    try {
        cuExceptie();
    } catch (const exception& e) {
        cout << "exceptie prinsa: " << e.what() << "\n";
    }

    cout << "\n--- unique_ptr: un singur proprietar ---\n";
    unique_ptr<Investment> p1 = make_unique<Investment>("p1");
    // unique_ptr<Investment> p2 = p1;   // eroare de compilare, nu se poate copia
    unique_ptr<Investment> p3 = move(p1);   // acum p1 e null
    cout << "p1 este null? " << (p1 == nullptr) << "\n";

    cout << "\n--- shared_ptr: mai multi proprietari ---\n";
    shared_ptr<Investment> s1 = make_shared<Investment>("s1");
    {
        shared_ptr<Investment> s2 = s1;
        cout << "use_count = " << s1.use_count() << "\n";   // 2
    }
    cout << "use_count = " << s1.use_count() << "\n";       // 1

    cout << "\n===== ITEM 14 =====\n";
    Mutex mtx{"mtx"};

    cout << "\n--- Varianta 1: Lock fara copiere ---\n";
    {
        Lock l(&mtx);
        // Lock l2(l);   // eroare de compilare
    }

    cout << "\n--- Varianta 2: Lock cu numarare de referinte ---\n";
    {
        LockRC l1(&mtx);
        {
            LockRC l2(l1);
            cout << "copii = " << l1.copii() << "\n";   // 2
        }
        cout << "copii = " << l1.copii() << "\n";       // 1
    }   // unlock se apeleaza o singura data, aici

    cout << "\n--- Varianta 3: copiere profunda ---\n";
    {
        Mesaj a("salut");
        Mesaj b = a;                     // constructor de copiere
        b.seteaza("altceva");
        cout << "a = " << a.get() << ", b = " << b.get() << "\n";
        cout << "adrese diferite? " << (a.adresa() != b.adresa()) << "\n";

        Mesaj c("temporar");
        c = a;                           // operator=
        cout << "c = " << c.get() << "\n";
    }

    cout << "\n--- Varianta 4: transfer de proprietate ---\n";
    {
        LockMutabil l1 = creeazaLock(&mtx);
        cout << "l1 detine lock-ul? " << l1.detine() << "\n";

        LockMutabil l2 = move(l1);
        cout << "dupa move: l1 detine? " << l1.detine()
             << ", l2 detine? " << l2.detine() << "\n";
    }   // unlock o singura data, de catre l2

    cout << "\n--- Sfarsit program ---\n";
    return 0;
}
