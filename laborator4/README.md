# Lucrarea 4 - PAOO

**Student:** Buda Maria, grupa 4C.1.2

Proiect C++ realizat pentru laboratorul de PAOO. Acoperă itemii 4, 5 și 6 din cartea *Effective C++* (Scott Meyers). Fiecare item are o secțiune separată în `main.cpp`, cu clase demonstrative care afișează mesaje în consolă, astfel încât comportamentul să poată fi observat la rulare.

## Compilare și rulare

```bash
g++ -std=c++11 -Wall -Wextra main.cpp -o lab4
./lab4
```

La compilare apare un avertisment `-Wreorder`. Este intenționat: demonstrează că ordinea din lista de inițializare nu influențează ordinea reală a inițializării (vezi Item 4).

## Item 4: Make sure that objects are initialized before they're used

**Problema.** În C++, tipurile builtin (`int`, pointeri) nu sunt inițializate automat, deci citirea lor înainte de a primi o valoare dă rezultate imprevizibile.

**Ce demonstrează proiectul:**

| Concept | Clasa din cod | Observație |
|---|---|---|
| Atribuire vs. inițializare | `BadContact` / `GoodContact` | În corpul constructorului membrii sunt mai întâi creați implicit și apoi atribuiți (muncă dublă). Lista de inițializare îi construiește direct cu valoarea corectă. |
| Membri neinițializați | `BadContact::numCalls` | Conține o valoare aleatoare, spre deosebire de `GoodContact`, unde este `0`. |
| Membri `const` și referințe | `Account` | Pot fi inițializați doar în lista de inițializare. |
| Ordinea inițializării | `OrderDemo` | Membrii se inițializează în ordinea declarării în clasă, nu în ordinea din listă. |
| Obiecte statice | `theFileSystem()` | Un obiect `static` local este creat la primul apel (Singleton Meyers), ceea ce evită problema ordinii de inițializare între fișiere. |

## Item 5: Know what functions C++ silently writes and calls

**Problema.** Dacă nu le definim noi, compilatorul generează automat: constructorul implicit, destructorul, copy constructorul și `operator=` de copiere (în C++11 și operațiile de move).

**Ce demonstrează proiectul:**

| Concept | Clasa din cod | Observație |
|---|---|---|
| Funcții generate pentru o clasă goală | `Empty` | Clasa pare fără conținut, dar poate fi creată, copiată și atribuită. `sizeof(Empty)` este 1, pentru că fiecare obiect trebuie să aibă o adresă proprie. |
| Momentul apelurilor | `Verbose` | `Verbose v3 = v1;` apelează copy constructorul, nu `operator=`. |
| Copiere superficială | `ShallowBuffer` | Versiunea generată copiază doar adresa pointerului, deci două obiecte partajează același buffer (risc de *double delete*). |
| Copiere profundă | `DeepBuffer` | Aplică regula celor trei: destructor, copy constructor și `operator=` definite de utilizator, fiecare obiect având propriul buffer. |
| Limitele compilatorului | `NamedObject` | Pentru clase cu membri referință sau `const`, compilatorul nu poate genera `operator=` (linia de test este comentată în cod). |

## Item 6: Explicitly disallow the use of compiler-generated functions you do not want

**Problema.** Unele obiecte nu au sens să fie copiate (o conexiune la baza de date, un fișier deschis). Dacă nu interzicem explicit copierea, compilatorul o permite.

**Ce demonstrează proiectul:**

| Tehnică | Clasa din cod | Cum funcționează |
|---|---|---|
| `= delete` (C++11) | `Uncopyable` | Funcțiile marcate `= delete` nu pot fi apelate, iar compilatorul semnalează eroare. |
| Funcții `private` nedefinite (C++98) | `HomeForSale` | Tehnica clasică din carte: copierea este blocată la compilare sau la link. |
| Moștenire privată | `DatabaseConnection` | Clasa moștenește din `Uncopyable`, deci devine necopiabilă. |
| `= delete` pe overload-uri | `Calculator` | Interzice apelurile cu `double` sau `char`, evitând conversii nedorite. |

Liniile care ar produce erori de compilare sunt lăsate comentate (`// EROARE`). Pot fi decomentate pe rând pentru a vedea mesajul compilatorului.

## Concluzii

- Inițializarea corectă a membrilor se face prin lista de inițializare, iar ordinea reală este dată de declararea în clasă.
- Compilatorul generează în fundal mai multe funcții speciale, iar cele generate nu sunt potrivite pentru clase care gestionează resurse (pointeri alocați dinamic).
- Funcțiile pe care nu le dorim trebuie interzise explicit, de preferat cu `= delete`.
