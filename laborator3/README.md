# Tema 3 - PAOO

Proiect realizat pentru laboratorul de PAOO.

## Ce contine aplicatia
Aplicatia implementeaza o clasa simpla `VectorDinamic` in C++ pentru a demonstra conceptele cerute la laborator:
* Incapsularea datelor.
* Alocarea memoriei in constructor si eliberarea ei in destructor.
* **Copy Constructor** (realizeaza *deep copy* pentru a preveni erorile de tip double-free).
* **Move Constructor** (pentru mutarea eficienta a resurselor).
* Mesaje de tip „proof of life” in constructori si destructori.

## De ce e necesar Copy Constructor-ul

Fara el, compilatorul copiaza doar pointerul (shallow copy), iar `v1` si `v2`
ar arata spre aceeasi memorie de pe heap. La final, ambele destructoare fac
`delete[]` pe ea, deci apare double free si programul crapa. Varianta mea
aloca memorie noua si copiaza valorile (deep copy).

## Move Constructor

Preia pointerul obiectului sursa si ii pune `nullptr`, ca sa nu se
elibereze aceeasi memorie de doua ori.

## Compilare si rulare (in WSL)

Pentru a compila si rula proiectul folosind CMake, folosim urmatoarele comenzi in terminal:

```bash
mkdir build
cd build
cmake ..
make
./Tema3PAOO
