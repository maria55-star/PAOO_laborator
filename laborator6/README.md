# Tema 5 - PAOO

Proiect realizat pentru laboratorul de PAOO (Effective C++, Item 10-12).

## Ce contine aplicatia

Aplicatia foloseste clasa `VectorDinamic` (vector alocat dinamic) pentru a demonstra cum se scrie corect operatorul de atribuire:

* `operator=` care returneaza referinta la `*this` (Item 10).
* Protectie la auto-atribuire si exception safety (Item 11).
* Copierea tuturor partilor unui obiect, inclusiv partea din clasa de baza (Item 12).
* Move assignment (`operator=(VectorDinamic&&)`), din C++11.
* Mesaje de tip „proof of life" in constructori, operatori si destructor.

## Diferenta dintre constructor si atribuire

```cpp
VectorDinamic b(a);   // obiect NOU, se apeleaza copy constructor
b = a;                // b exista deja, se apeleaza operator=
```

Operatorul `=` lucreaza pe un obiect care are deja memorie alocata, deci trebuie sa elibereze vechea memorie. Constructorul nu are de eliberat nimic.

## De ce returnam *this (Item 10)

Tipurile predefinite permit atribuiri inlantuite:

```cpp
x = y = z = 15;   // se evalueaza de la dreapta la stanga
```

Ca sa mearga la fel si pentru clasa mea, `operator=` returneaza `VectorDinamic&` si la final face `return *this;`. In `main` am testat cu `a = b = c;`.

## De ce e periculoasa auto-atribuirea (Item 11)

Varianta gresita:

```cpp
delete[] v;               // sterg vectorul
v = new int[alt.n];       // copiez din alt
```

Daca scriu `c = c`, atunci `alt` este chiar `*this`, deci `delete[] v` sterge exact datele pe care vreau sa le copiez. Rezultatul este undefined behavior. In plus, daca `new` arunca exceptie, vectorul vechi era deja sters.

Varianta din proiect (ordine corecta):

1. aloc memorie noua si copiez datele in ea
2. sterg vectorul vechi
3. mut pointerul pe cel nou

Asa merge si pentru `c = c`, si daca `new` esueaza, obiectul ramane intact. Mai exista doua variante: testul de identitate (`if (this == &alt)`) si copy-and-swap, dar ordinea corecta rezolva ambele probleme fara cod in plus.

## De ce copiem toate partile (Item 12)

In clasa `VectorEtichetat` (derivata din `VectorDinamic`) trebuie sa apelez explicit functiile din baza:

```cpp
VectorEtichetat(const VectorEtichetat& alt)
    : VectorDinamic(alt), eticheta(alt.eticheta) {}

VectorEtichetat& operator=(const VectorEtichetat& alt) {
    VectorDinamic::operator=(alt);
    eticheta = alt.eticheta;
    return *this;
}
```

Daca uit `VectorDinamic(alt)` sau `VectorDinamic::operator=(alt)`, compilatorul nu da eroare, dar se copiaza doar eticheta, iar partea din baza ramane neschimbata (sau se construieste cu valori implicite). Daca adaug un membru nou in clasa, trebuie sa actualizez ambele functii de copiere.

## Move assignment

```cpp
d = std::move(c);
```

| | Copy | Move |
|---|---|---|
| Parametru | `const VectorDinamic&` | `VectorDinamic&&` |
| Aloca memorie noua | Da | Nu |
| Sursa dupa operatie | Neschimbata | Goala (`nullptr`) |

La move assignment, obiectul destinatie are deja memorie, deci o sterg intai cu `delete[]`, altfel ar fi memory leak. Apoi iau pointerul sursei si pun `nullptr` in sursa, ca destructorul ei sa nu stearga a doua oara aceeasi memorie. Functia e `noexcept`.

## Cum se ruleaza

Se compileaza cu CMake (C++17) si se ruleaza executabilul `laborator5`. Mesajele din constructori, operatori si destructor arata in ce ordine se apeleaza functiile.
