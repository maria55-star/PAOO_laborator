# Tema 3 - PAOO

Proiect realizat pentru laboratorul de PAOO.

## Ce contine aplicatia
Aplicatia implementeaza o clasa simpla `VectorDinamic` in C++ pentru a demonstra conceptele cerute la laborator:
* Incapsularea datelor.
* Alocarea memoriei in constructor si eliberarea ei in destructor.
* **Copy Constructor** (realizeaza *deep copy* pentru a preveni erorile de tip double-free).
* **Move Constructor** (pentru mutarea eficienta a resurselor).
* Mesaje de tip „proof of life” in constructori si destructori.

## Compilare si rulare (in WSL)

Pentru a compila si rula proiectul folosind CMake, folosim urmatoarele comenzi in terminal:

```bash
mkdir build
cd build
cmake ..
make
./Tema3PAOO
