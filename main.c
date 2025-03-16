#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structura care începe cu litera A (prima literă din prenumele Alexandru)
typedef struct Articol {
    int id;                 // Identificator unic
    char* denumire;         // Pointer către denumire (string)
    float pret;             // Preț
    int cantitate;          // Cantitate disponibilă
    char* categorie;        // Pointer către categorie (string)
    char* producator;       // Pointer către producător (string)
    int anFabricatie;       // Anul fabricației
} Articol;

// Structura pentru nodurile listei
typedef struct Nod {
    Articol* info;          // Pointer către informația nodului (structura Articol)
    struct Nod* next;       // Pointer către următorul element
} Nod;

// Funcție pentru citirea unei structuri Articol din fișier
Articol* citireArticol(FILE* f) {
    Articol* a = (Articol*)malloc(sizeof(Articol));
    if (!a) return NULL;

    // Citire date
    fscanf(f, "%d", &a->id);

    // Citire denumire
    char buffer[100];
    fscanf(f, "%s", buffer);
    a->denumire = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->denumire) {
        free(a);
        return NULL;
    }
    strcpy(a->denumire, buffer);

    // Citire preț și cantitate
    fscanf(f, "%f %d", &a->pret, &a->cantitate);

    // Citire categorie
    fscanf(f, "%s", buffer);
    a->categorie = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->categorie) {
        free(a->denumire);
        free(a);
        return NULL;
    }
    strcpy(a->categorie, buffer);

    // Citire producător
    fscanf(f, "%s", buffer);
    a->producator = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->producator) {
        free(a->categorie);
        free(a->denumire);
        free(a);
        return NULL;
    }
    strcpy(a->producator, buffer);

    // Citire an fabricație
    fscanf(f, "%d", &a->anFabricatie);

    return a;
}

// Funcție pentru citirea vectorului de articole din fișier
Articol** citireVectorArticole(const char* numeFisier, int* n) {
    FILE* f = fopen(numeFisier, "r");
    if (!f) return NULL;

    fscanf(f, "%d", n); // Citim numărul de articole

    Articol** vectorArticole = (Articol**)malloc(*n * sizeof(Articol*));
    if (!vectorArticole) {
        fclose(f);
        return NULL;
    }

    for (int i = 0; i < *n; i++) {
        vectorArticole[i] = citireArticol(f);
        if (!vectorArticole[i]) {
            // Eliberăm memoria alocată până acum în caz de eroare
            for (int j = 0; j < i; j++) {
                free(vectorArticole[j]->denumire);
                free(vectorArticole[j]->categorie);
                free(vectorArticole[j]->producator);
                free(vectorArticole[j]);
            }
            free(vectorArticole);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return vectorArticole;
}

// 3. Inserare la început în lista simplu înlănțuită
Nod* inserareInceput(Nod* cap, Articol* articol) {
    Nod* nou = (Nod*)malloc(sizeof(Nod));
    if (!nou) return cap;

    nou->info = articol;
    nou->next = cap;

    return nou;
}

// 4. Ștergere ultimul element din lista simplu înlănțuită
Nod* stergereUltim(Nod* cap) {
    if (!cap) return NULL; // Lista e goală

    if (!cap->next) { // Lista are un singur element
        free(cap->info->denumire);
        free(cap->info->categorie);
        free(cap->info->producator);
        free(cap->info);
        free(cap);
        return NULL;
    }

    // Căutăm penultimul nod
    Nod* curent = cap;
    while (curent->next->next) {
        curent = curent->next;
    }

    // Eliberăm memoria pentru ultimul nod
    free(curent->next->info->denumire);
    free(curent->next->info->categorie);
    free(curent->next->info->producator);
    free(curent->next->info);
    free(curent->next);
    curent->next = NULL;

    return cap;
}

// 5. Verificare dacă un element există în listă
int cautareElement(Nod* cap, int id, Articol** gasit) {
    Nod* curent = cap;
    int pozitie = 0;

    while (curent) {
        if (curent->info->id == id) {
            if (gasit) *gasit = curent->info;
            return pozitie;
        }
        curent = curent->next;
        pozitie++;
    }

    if (gasit) *gasit = NULL;
    return -1; // Nu s-a găsit elementul
}

// 6. Afișarea tuturor elementelor listei
void afisareLista(Nod* cap) {
    Nod* curent = cap;
    int nr = 0;

    printf("\nElementele listei sunt:\n");
    while (curent) {
        printf("Pozitia %d: ID=%d, Denumire=%s, Pret=%.2f, Cantitate=%d, Categorie=%s, Producator=%s, An=%d\n",
            nr++, curent->info->id, curent->info->denumire, curent->info->pret,
            curent->info->cantitate, curent->info->categorie, curent->info->producator,
            curent->info->anFabricatie);
        curent = curent->next;
    }

    if (nr == 0) {
        printf("Lista este goala.\n");
    }
}

// 7. Conversie de la listă simplu înlănțuită la vector
Articol** listaLaVector(Nod* cap, int* dimensiune) {
    // Determinăm dimensiunea listei
    int n = 0;
    Nod* curent = cap;
    while (curent) {
        n++;
        curent = curent->next;
    }

    *dimensiune = n;
    if (n == 0) return NULL;

    // Alocăm memoria pentru vector
    Articol** vector = (Articol**)malloc(n * sizeof(Articol*));
    if (!vector) return NULL;

    // Copiem pointerii către articole în vector
    curent = cap;
    for (int i = 0; i < n; i++) {
        vector[i] = curent->info;
        curent = curent->next;
    }

    return vector;
}

// Funcție pentru eliberarea memoriei alocate pentru un vector de articole
void eliberareVectorArticole(Articol** vector, int n) {
    if (!vector) return;

    for (int i = 0; i < n; i++) {
        if (vector[i]) {
            free(vector[i]->denumire);
            free(vector[i]->categorie);
            free(vector[i]->producator);
            free(vector[i]);
        }
    }

    free(vector);
}

// Funcție pentru eliberarea memoriei alocate pentru lista
void eliberareLista(Nod* cap) {
    Nod* curent = cap;

    while (curent) {
        Nod* temp = curent;
        curent = curent->next;
        // Nu eliberăm memoria pentru Articol deoarece acesta va fi eliberat prin vectorul inițial
        free(temp);
    }
}

// Funcția main pentru testare
int main() {
    const char* numeFisier = "articole.txt";
    int n;

    // 2. Citirea din fișier a vectorului de articole
    Articol** vectorArticole = citireVectorArticole(numeFisier, &n);
    if (!vectorArticole) {
        printf("Eroare la citirea din fisier!\n");
        return 1;
    }

    printf("S-au citit %d articole din fisier.\n", n);

    // Creăm lista simplu înlănțuită
    Nod* cap = NULL;
    for (int i = 0; i < n; i++) {
        cap = inserareInceput(cap, vectorArticole[i]);
    }

    // 6. Afișăm lista inițială
    printf("\nLista initiala:");
    afisareLista(cap);

    // 3. Testăm inserarea la început
    printf("\nTestam inserarea la inceput a unui nou articol:\n");
    Articol* articolNou = (Articol*)malloc(sizeof(Articol));
    articolNou->id = 999;
    articolNou->denumire = (char*)malloc(20 * sizeof(char));
    strcpy(articolNou->denumire, "ArticolNou");
    articolNou->pret = 99.99;
    articolNou->cantitate = 10;
    articolNou->categorie = (char*)malloc(20 * sizeof(char));
    strcpy(articolNou->categorie, "CategorieNoua");
    articolNou->producator = (char*)malloc(20 * sizeof(char));
    strcpy(articolNou->producator, "ProducatorNou");
    articolNou->anFabricatie = 2023;

    cap = inserareInceput(cap, articolNou);
    afisareLista(cap);

    // 5. Testăm căutarea unui element
    int idCautat = 999;
    Articol* articolGasit = NULL;
    int pozitie = cautareElement(cap, idCautat, &articolGasit);

    printf("\nCautam articolul cu ID=%d:\n", idCautat);
    if (pozitie != -1) {
        printf("Articolul a fost gasit la pozitia %d: ID=%d, Denumire=%s\n",
            pozitie, articolGasit->id, articolGasit->denumire);
    }
    else {
        printf("Articolul cu ID=%d nu a fost gasit.\n", idCautat);
    }

    // 4. Testăm ștergerea ultimului element
    printf("\nStergem ultimul element din lista:\n");
    cap = stergereUltim(cap);
    afisareLista(cap);

    // 7. Testăm conversia de la listă la vector
    int dimVector;
    Articol** vectorDinLista = listaLaVector(cap, &dimVector);

    printf("\nVectorul obtinut din lista are %d elemente:\n", dimVector);
    for (int i = 0; i < dimVector; i++) {
        printf("Pozitia %d: ID=%d, Denumire=%s\n", i, vectorDinLista[i]->id, vectorDinLista[i]->denumire);
    }

    // 8. Eliberăm memoria pentru a evita memory leaks
    free(vectorDinLista); // Eliberăm doar vectorul, nu și articolele (acestea sunt în lista)
    eliberareLista(cap);  // Eliberăm nodurile listei, nu și articolele
    free(vectorArticole); // Eliberăm vectorul inițial, nu și articolele

    // La final, trebuie să eliberăm memoria pentru articole
    for (int i = 0; i < n; i++) {
        free(vectorArticole[i]->denumire);
        free(vectorArticole[i]->categorie);
        free(vectorArticole[i]->producator);
        free(vectorArticole[i]);
    }

    // Eliberăm și articolul nou creat
    free(articolNou->denumire);
    free(articolNou->categorie);
    free(articolNou->producator);
    free(articolNou);

    return 0;
}