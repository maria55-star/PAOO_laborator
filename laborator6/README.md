# Tema 6 - PAOO

Proiect realizat pentru laboratorul de PAOO (Effective C++, Item 13-14).

## Ce contine aplicatia

Aplicatia demonstreaza gestionarea resurselor cu ajutorul obiectelor:

* Diferenta dintre `delete` manual si `unique_ptr`, inclusiv in prezenta exceptiilor (Item 13).
* `unique_ptr` (un singur proprietar) si `shared_ptr` (proprietari multipli).
* Un mutex ca exemplu de resursa care nu e memorie (Item 14).
* Cele patru variante de copiere din Item 14, fiecare cu clasa ei: `Lock` (copiere interzisa), `LockRC` (numarare de referinte), `Mesaj` (copiere profunda) si `LockMutabil` (transfer de proprietate).
* Mesaje de tip „proof of life" pentru a vedea cand se creeaza si cand se distrug obiectele.

## De ce nu e bun delete-ul manual (Item 13)

```cpp
void manual(bool eroare) {
    Investment* p = createInvestment("manual");
    if (eroare)
        return;     // delete nu mai ruleaza
    delete p;
}
```

Daca functia iese mai devreme (return, exceptie), `delete p` nu se mai executa si apare memory leak. In `main` se vede ca la `manual(true)` nu apare mesajul „Investment distrus".

Leak-ul din `manual(true)` este intentionat, doar pentru demonstratie. Un tool de memorie (valgrind, AddressSanitizer) il va raporta.

## RAII: resursa gestionata de un obiect

```cpp
void cuRAII(bool eroare) {
    unique_ptr<Investment> p(createInvestment("raii"));
    if (eroare)
        return;     // destructorul lui p sterge Investment
}
```

Ideea (Resource Acquisition Is Initialization): resursa e data imediat unui obiect, iar destructorul obiectului o elibereaza. Destructorul ruleaza indiferent cum se iese din bloc (final normal, return sau exceptie), deci nu mai poate aparea leak. Functia `cuExceptie` arata exact acest caz: se arunca o exceptie, iar `Investment` se distruge totusi.

## unique_ptr si shared_ptr

* **unique_ptr** are un singur proprietar. Nu se poate copia (`auto p2 = p1;` da eroare), doar muta cu `std::move`, iar dupa mutare sursa devine `nullptr`.
* **shared_ptr** are proprietari multipli. Tine un contor de referinte (`use_count()`), iar obiectul se sterge cand dispare ultimul proprietar.

In carte se foloseste `auto_ptr`, dar a fost eliminat in C++17, deci in proiect folosesc `unique_ptr`.

## De ce conteaza copierea obiectelor RAII (Item 14)

Clasa `Lock` face `lock` in constructor si `unlock` in destructor. Daca as lasa compilatorul sa genereze copierea:

```cpp
Lock ml1(&m);
Lock ml2(ml1);   // ambele retin acelasi Mutex*
```

ambele obiecte ar face `unlock` pe acelasi mutex, deci s-ar apela de doua ori. Trebuie deci sa decid ce inseamna o copie. Optiunile sunt:

1. **Interzic copierea.** O copie a unui lock nu are sens, asa ca sterg copy constructor-ul si operator=. E varianta aleasa pentru clasa `Lock`.

   ```cpp
   Lock(const Lock&) = delete;
   Lock& operator=(const Lock&) = delete;
   ```

2. **Numar referintele.** Copiile impart acelasi mutex, iar ultima copie face unlock. Am implementat in `LockRC`, cu `shared_ptr` si un deleter propriu (`unlock`). In `main` se vede ca `copii` ajunge la 2 si apoi la 1, iar `unlock` se apeleaza o singura data.

3. **Copie profunda.** Se copiaza si resursa. Merge la memorie (ca la `std::string`), dar nu la un mutex. Am implementat in clasa `Mesaj`, cu copy constructor si `operator=` proprii (alocare inainte de `delete`, protectie la auto-atribuire). Fara ele, copia ar pastra acelasi pointer si ar urma un double delete. In `main` se verifica ca adresele sunt diferite.

4. **Transfer de proprietate.** Un singur proprietar, iar mutarea e explicita (ca la `unique_ptr`). Am implementat in `LockMutabil`: copierea e interzisa, iar move constructor-ul lasa obiectul vechi gol (`m = nullptr`), asa ca `unlock` se face o singura data, de catre noul proprietar.

## Cum se ruleaza

Se compileaza cu un compilator C++17:

```bash
g++ -std=c++17 -Wall -o laborator6 main.cpp
./laborator6
```

Sau cu CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Mesajele afisate arata in ce ordine se creeaza si se distrug obiectele.
