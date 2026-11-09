// Lucrarea 4 - PAOO
// Effective C++ (Scott Meyers): Item 4, Item 5, Item 6
// Compilare: g++ -std=c++11 -Wall -Wextra main.cpp -o lab4
//Buda Maria, Gr 1.2C
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

// Functie ajutatoare pentru separarea sectiunilor in output
void titlu(const string& t) {
    cout << "\n==================== " << t << " ====================\n";
}

// =====================================================================
// ITEM 4: Make sure that objects are initialized before they're used
// =====================================================================

// Clasa care "vorbeste": ne arata cand se apeleaza fiecare functie speciala
class PhoneNumber {
public:
    PhoneNumber(const string& n = "") : number(n) {
        cout << "  [PhoneNumber] constructor (" << number << ")\n";
    }
    PhoneNumber(const PhoneNumber& o) : number(o.number) {
        cout << "  [PhoneNumber] copy constructor\n";
    }
    PhoneNumber& operator=(const PhoneNumber& o) {
        cout << "  [PhoneNumber] operator=\n";
        number = o.number;
        return *this;
    }
private:
    string number;
};

// VARIANTA GRESITA: valorile sunt ATRIBUITE in corpul constructorului.
// Intai membrii sunt initializati implicit, apoi li se da o valoare
// (munca dubla). Mai grav: daca uitam un tip builtin, ramane cu gunoi.
class BadContact {
public:
    BadContact(const string& n, const PhoneNumber& p) {
        name = n;       // atribuire, nu initializare
        phone = p;      // default ctor + operator= (2 operatii)
        // numCalls NU e initializat -> valoare nedefinita
    }
    int getNumCalls() const { return numCalls; }
private:
    string name;
    PhoneNumber phone;
    int numCalls;
};

// VARIANTA CORECTA: lista de initializare a constructorului.
// Fiecare membru e initializat o singura data, direct cu valoarea corecta.
class GoodContact {
public:
    GoodContact(const string& n, const PhoneNumber& p)
        : name(n), phone(p), numCalls(0) {}   // doar copy ctor pentru phone
    int getNumCalls() const { return numCalls; }
private:
    string name;
    PhoneNumber phone;
    int numCalls;
};

// Membrii const si referintele TREBUIE initializati in lista
// (nu pot fi atribuiti in corpul constructorului).
class Account {
public:
    Account(int id, int& counterRef) : id(id), counter(counterRef) {
        counter++;
    }
    int getId() const { return id; }
private:
    const int id;
    int& counter;
};

// Ordinea de initializare = ordinea DECLARARII membrilor in clasa,
// nu ordinea din lista de initializare!
class Tracer {
public:
    Tracer(const string& n) : nume(n) { cout << "  initializez " << nume << "\n"; }
private:
    string nume;
};

class OrderDemo {
public:
    // In lista e scris b inainte de a, dar a e declarat primul
    OrderDemo() : b("b"), a("a") {}
private:
    Tracer a;
    Tracer b;
};

// Ordinea initializarii obiectelor non-locale statice din fisiere diferite
// este nedefinita. Solutia: obiecte statice LOCALE intr-o functie
// (Singleton Meyers) -> se initializeaza la prima folosire.
class FileSystem {
public:
    FileSystem() { cout << "  FileSystem creat\n"; }
    void ceva() const { cout << "  FileSystem folosit\n"; }
};

FileSystem& theFileSystem() {
    static FileSystem fs;   // initializat la primul apel
    return fs;
}

void demoItem4() {
    titlu("ITEM 4: initializare inainte de folosire");

    PhoneNumber tel("0722123456");

    cout << "\n-- BadContact (atribuire in corp):\n";
    BadContact bad("Ana", tel);
    cout << "  numCalls (neinitializat, gunoi!): " << bad.getNumCalls() << "\n";

    cout << "\n-- GoodContact (lista de initializare):\n";
    GoodContact good("Ana", tel);
    cout << "  numCalls: " << good.getNumCalls() << "\n";

    cout << "\n-- const si referinta in lista de initializare:\n";
    int contor = 0;
    Account acc(7, contor);
    cout << "  id=" << acc.getId() << ", contor=" << contor << "\n";

    cout << "\n-- Ordinea de initializare (urmeaza declararea, nu lista):\n";
    OrderDemo od;

    cout << "\n-- Statice locale (Meyers singleton):\n";
    cout << "  inainte de primul apel\n";
    theFileSystem().ceva();
    theFileSystem().ceva();   // nu se mai creeaza a doua oara
}

// =====================================================================
// ITEM 5: Know what functions C++ silently writes and calls
// =====================================================================
// Pentru o clasa, compilatorul genereaza (daca e nevoie si nu le definim noi):
//   constructor implicit, destructor, copy constructor, copy assignment
//   (+ move constructor si move assignment in C++11)

// Clasa "goala" - compilatorul tot ii genereaza functiile
class Empty {};
// e echivalenta cu:
// class Empty {
// public:
//     Empty() {}
//     ~Empty() {}
//     Empty(const Empty&) {}
//     Empty& operator=(const Empty&) { return *this; }
// };

// Definim noi toate, ca sa VEDEM cand sunt apelate
class Verbose {
public:
    Verbose()                          { cout << "  Verbose(): constructor implicit\n"; }
    Verbose(const Verbose&)            { cout << "  Verbose(const Verbose&): copy ctor\n"; }
    Verbose& operator=(const Verbose&) { cout << "  operator=: copy assignment\n"; return *this; }
    ~Verbose()                         { cout << "  ~Verbose(): destructor\n"; }
};

// Probleme cu versiunea generata: copierea membru-cu-membru e SUPERFICIALA.
// Pentru un pointer se copiaza adresa, nu continutul -> 2 obiecte
// impart acelasi buffer (la destructor: double delete!).
class ShallowBuffer {
public:
    ShallowBuffer(const char* s) {
        data = new char[strlen(s) + 1];
        strcpy(data, s);
    }
    // Nu definim copy ctor / operator= / destructor -> se folosesc cele generate
    const char* ptr() const { return data; }
    // (nu punem destructor ca sa nu crape programul in demo; ar fi leak,
    //  iar cu destructor ar fi double delete)
private:
    char* data;
};

// Solutia: Regula celor trei - definim copy ctor, operator= si destructor
class DeepBuffer {
public:
    DeepBuffer(const char* s) {
        data = new char[strlen(s) + 1];
        strcpy(data, s);
    }
    DeepBuffer(const DeepBuffer& o) {                // copie profunda
        data = new char[strlen(o.data) + 1];
        strcpy(data, o.data);
    }
    DeepBuffer& operator=(const DeepBuffer& o) {
        if (this != &o) {                            // protectie la auto-atribuire
            char* nou = new char[strlen(o.data) + 1];
            strcpy(nou, o.data);
            delete[] data;
            data = nou;
        }
        return *this;
    }
    ~DeepBuffer() { delete[] data; }
    const char* ptr() const { return data; }
private:
    char* data;
};

// Cand compilatorul REFUZA sa genereze operator=:
// clase cu membri referinta sau const.
class NamedObject {
public:
    NamedObject(string& n, int v) : nameRef(n), value(v) {}
private:
    string& nameRef;       // referinta - nu poate fi re-legata
    const int value;       // const - nu poate fi modificat
};

void demoItem5() {
    titlu("ITEM 5: functiile generate silentios");

    cout << "\n-- Clasa Empty: compilatorul ii genereaza functiile\n";
    Empty e1;             // constructor implicit
    Empty e2(e1);         // copy constructor
    e2 = e1;              // copy assignment
    cout << "  sizeof(Empty) = " << sizeof(Empty) << " (nu e 0, obiectele trebuie sa aiba adresa)\n";
    // destructorul se apeleaza la sfarsitul functiei

    cout << "\n-- Verbose: cand se apeleaza fiecare functie\n";
    {
        Verbose v1;               // constructor
        Verbose v2(v1);           // copy ctor
        Verbose v3 = v1;          // tot copy ctor (NU operator=)
        v2 = v1;                  // copy assignment
        cout << "  (la acolada se distrug v3, v2, v1)\n";
    }

    cout << "\n-- ShallowBuffer: copierea generata copiaza doar pointerul\n";
    ShallowBuffer s1("salut");
    ShallowBuffer s2(s1);
    cout << "  s1.ptr()=" << static_cast<const void*>(s1.ptr())
         << "\n  s2.ptr()=" << static_cast<const void*>(s2.ptr())
         << "\n  -> aceeasi adresa = copie superficiala (periculos)\n";

    cout << "\n-- DeepBuffer: copie profunda (regula celor trei)\n";
    DeepBuffer d1("salut");
    DeepBuffer d2(d1);
    cout << "  d1.ptr()=" << static_cast<const void*>(d1.ptr())
         << "\n  d2.ptr()=" << static_cast<const void*>(d2.ptr())
         << "\n  -> adrese diferite, fiecare obiect are bufferul lui\n";

    cout << "\n-- NamedObject (referinta + const)\n";
    string p1 = "Ana", p2 = "Ion";
    NamedObject n1(p1, 1), n2(p2, 2);
    // n1 = n2;   // EROARE de compilare: compilatorul nu poate genera
    //            // operator= pentru clase cu membri referinta/const
    (void)n1; (void)n2;
    cout << "  n1 = n2 nu compileaza (decomentati linia ca sa verificati)\n";
}

// =====================================================================
// ITEM 6: Explicitly disallow the use of compiler-generated functions
//         you do not want
// =====================================================================

// Exemplu: un obiect unic, care nu are sens sa fie copiat
// (ex: o conexiune la baza de date, un fisier deschis, un mutex).

// Varianta C++11: "= delete"
class Uncopyable {
public:
    Uncopyable() {}
    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;
};

// Varianta veche (C++98), cea din carte: declarate private si NEdefinite.
// Eroare la compilare daca le foloseste cineva din exterior,
// eroare la link daca le folosesc membrii/friend-ii.
class HomeForSale {
public:
    HomeForSale() {}
private:
    HomeForSale(const HomeForSale&);             // fara implementare
    HomeForSale& operator=(const HomeForSale&);  // fara implementare
};

// Solutia generala din carte: mostenire dintr-o clasa de baza Uncopyable
class DatabaseConnection : private Uncopyable {
public:
    DatabaseConnection(const string& s) : conn(s) {}
    void query() const { cout << "  query pe " << conn << "\n"; }
private:
    string conn;
};

// "= delete" merge si pentru a interzice conversii nedorite
class Calculator {
public:
    void process(int x)    { cout << "  process(int): " << x << "\n"; }
    void process(double)   = delete;   // interzice apelul cu double
    void process(char)     = delete;   // interzice apelul cu char
};

void demoItem6() {
    titlu("ITEM 6: interzicerea functiilor generate");

    Uncopyable u1;
    // Uncopyable u2(u1);   // EROARE: use of deleted function
    // Uncopyable u3; u3 = u1; // EROARE: use of deleted function
    (void)u1;
    cout << "  Uncopyable: copierea e interzisa la compilare\n";

    HomeForSale h1;
    // HomeForSale h2(h1);  // EROARE: copy ctor e private
    (void)h1;
    cout << "  HomeForSale: copierea e privata (stilul C++98)\n";

    DatabaseConnection db1("localhost:5432");
    db1.query();
    // DatabaseConnection db2(db1);  // EROARE: baza Uncopyable nu se poate copia
    // db2 = db1;                    // EROARE
    cout << "  DatabaseConnection: mosteneste Uncopyable -> necopiabila\n";

    Calculator c;
    c.process(5);
    // c.process(3.14);   // EROARE: process(double) e sters
    // c.process('a');    // EROARE: process(char) e sters
    cout << "  process(double) si process(char) sunt sterse cu = delete\n";
}

// =====================================================================
int main() {
    demoItem4();
    demoItem5();
    demoItem6();
    cout << "\n";
    return 0;
}
