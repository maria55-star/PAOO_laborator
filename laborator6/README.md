# Tema 6 - PAOO

Proiect realizat pentru laboratorul de PAOO (Effective C++, Item 13-14).

## Ce contine aplicatia

Aplicatia demonstreaza gestionarea resurselor cu ajutorul obiectelor:

* Diferenta dintre `delete` manual si `unique_ptr` (Item 13).
* `unique_ptr` (un singur proprietar) si `shared_ptr` (proprietari multipli).
* Clasa `Lock` pentru un mutex, ca exemplu de resursa care nu e memorie (Item 14).
* Doua variante de copiere pentru `Lock`: copiere interzisa si numarare de referinte.
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

## RAII: resursa gestionata de un obiect

```cpp
void cuRAII(bool eroare) {
    unique_ptr<Investment> p(createInvestment("raii"));
    if (eroare)
        return;     // destructorul lui p sterge Investment
}
```

Ideea (Resource Acquisition Is Initialization): resursa e data imediat unui obiect, iar destructorul obiectului o elibereaza. Destructorul ruleaza indiferent cum se iese din bloc (final normal, return sau exceptie), deci nu mai poate aparea leak.

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
2. **Numar referintele.** Copiile impart acelasi mutex, iar ultima copie face unlock. Am implementat in `LockRC`, cu `shared_ptr` si un deleter propriu (`unlock`).
3. **Copie profunda.** Se copiaza si resursa. Merge la memorie (ca la `std::string`), dar nu la un mutex.
4. **Transfer de proprietate.** Un singur proprietar, iar mutarea e explicita (ca la `unique_ptr`).

```cpp
Lock(const Lock&) = delete;
Lock& operator=(const Lock&) = delete;
```

La `LockRC`, `unlock` se apeleaza o singura data, cand iese din scope ultima copie. In `main` se vede ca `copii` ajunge la 2 si apoi la 1.

## Cum se ruleaza

Se compileaza cu CMake (C++17) si se ruleaza executabilul `laborator6`. Mesajele afisate arata in ce ordine se creeaza si se distrug obiectele.
