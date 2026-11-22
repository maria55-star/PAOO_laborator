#include <iostream>
#include <algorithm>

class VectorDinamic {
private:
    // 2. Încapsulare: datele membre sunt private (ascunse exteriorului)
    int* date;
    size_t dimensiune;

public:
    // 3. Constructor cu lista de inițializare a membrilor
    VectorDinamic(size_t dim) : dimensiune(dim) {
        date = new int[dimensiune];
        for(size_t i = 0; i < dimensiune; ++i) {
            date[i] = (int)(i + 1); // umplem cu niște valori de test
        }
        std::cout << "[PROOF OF LIFE] Constructor apelat: Alocat memorie pe heap pentru " << dimensiune << " elemente.\n";
    }

    // 5. Suprascrierea Copy Constructor-ului (Deep Copy)
    VectorDinamic(const VectorDinamic& altul) : dimensiune(altul.dimensiune) {
        date = new int[dimensiune]; // Alocăm memorie nouă separată
        for(size_t i = 0; i < dimensiune; ++i) {
            date[i] = altul.date[i]; // Copiem valorile element cu element
        }
        std::cout << "[PROOF OF LIFE] Copy Constructor apelat: S-a făcut copierea adâncă (deep copy).\n";
    }

    // 6. Crearea unui Move Constructor (Optimizare pentru rvalues/obiecte temporare)
    VectorDinamic(VectorDinamic&& altul) noexcept 
        : date(altul.date), dimensiune(altul.dimensiune) {
        // „Furăm” resursele obiectului temporar și îl lăsăm gol
        altul.date = nullptr;
        altul.dimensiune = 0;
        std::cout << "[PROOF OF LIFE] Move Constructor apelat: Resursele au fost mutate eficient!\n";
    }

    // 4. Eliberarea heap-ului în cadrul destructorului
    ~VectorDinamic() {
        delete[] date; // Prevenim memory leaks (scurgerile de memorie)
        std::cout << "[PROOF OF LIFE] Destructor apelat: Memoria de pe heap a fost eliberată.\n";
    }

    void afiseaza() const {
        std::cout << "Elemente vector: ";
        for(size_t i = 0; i < dimensiune; ++i) {
            std::cout << date[i] << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    std::cout << "--- 1. Creare obiect initial (v1) ---\n";
    VectorDinamic v1(3);
    v1.afiseaza();

    std::cout << "\n--- 2. Test Copy Constructor (v2 = v1) ---\n";
    VectorDinamic v2 = v1; // Aici se apelează constructorul de copiere
    v2.afiseaza();

    std::cout << "\n--- 3. Test Move Constructor (v3 mutat din v1) ---\n";
    VectorDinamic v3(std::move(v1)); // Aici se apelează constructorul de mutare

    std::cout << "\n--- 4. Sfarsit program (se apeleaza destructorii automat) ---\n";
    return 0;
}
