#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Articol {
    int id;
    char* denumire;
    float pret;
    int cantitate;
    char* categorie;
    char* producator;
    int anFabricatie;
} Articol;

typedef struct Nod {
    Articol* info;
    struct Nod* next;

Articol* citireArticol(FILE* f) {
    Articol* a = (Articol*)malloc(sizeof(Articol));
    if (!a) return NULL;

    fscanf(f, "%d", &a->id);

    char buffer[100];
    fscanf(f, "%s", buffer);
    a->denumire = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->denumire) {
        free(a);
        return NULL;
    }
    strcpy(a->denumire, buffer);

    fscanf(f, "%f %d", &a->pret, &a->cantitate);

    fscanf(f, "%s", buffer);
    a->categorie = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->categorie) {
        free(a->denumire);
        free(a);
        return NULL;
    }
    strcpy(a->categorie, buffer);

    fscanf(f, "%s", buffer);
    a->producator = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
    if (!a->producator) {
        free(a->categorie);
        free(a->denumire);
        free(a);
        return NULL;
    }
    strcpy(a->producator, buffer);

    fscanf(f, "%d", &a->anFabricatie);

    return a;
}

Articol** citireVectorArticole(const char* numeFisier, int* n) {
    FILE* f = fopen(numeFisier, "r");
    if (!f) return NULL;

    fscanf(f, "%d", n);

    Articol** vectorArticole = (Articol**)malloc(*n * sizeof(Articol*));
    if (!vectorArticole) {
        fclose(f);
        return NULL;
    }

    for (int i = 0; i < *n; i++) {
        vectorArticole[i] = citireArticol(f);
        if (!vectorArticole[i]) {
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

Nod* inserareInceput(Nod* cap, Articol* articol) {
    Nod* nou = (Nod*)malloc(sizeof(Nod));
    if (!nou) return cap;

    nou->info = articol;
    nou->next = cap;

    return nou;
}

Nod* stergereUltim(Nod* cap) {
    if (!cap) return NULL;

    if (!cap->next) {
        free(cap->info->denumire);
        free(cap->info->categorie);
        free(cap->info->producator);
        free(cap->info);
        free(cap);
        return NULL;
    }

    Nod* curent = cap;
    while (curent->next->next) {
        curent = curent->next;
    }

    free(curent->next->info->denumire);
    free(curent->next->info->categorie);
    free(curent->next->info->producator);
    free(curent->next->info);
    free(curent->next);
    curent->next = NULL;

    return cap;
}

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
    return -1;
}

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

Articol** listaLaVector(Nod* cap, int* dimensiune) {
    int n = 0;
    Nod* curent = cap;
    while (curent) {
        n++;
        curent = curent->next;
    }

    *dimensiune = n;
    if (n == 0) return NULL;

    Articol** vector = (Articol**)malloc(n * sizeof(Articol*));
    if (!vector) return NULL;

    curent = cap;
    for (int i = 0; i < n; i++) {
        vector[i] = curent->info;
        curent = curent->next;
    }

    return vector;
}

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

void eliberareLista(Nod* cap) {
    Nod* curent = cap;

    while (curent) {
        Nod* temp = curent;
        curent = curent->next;
        free(temp);
    }
}

int main() {
    const char* numeFisier = "articole.txt";
    int n;

    Articol** vectorArticole = citireVectorArticole(numeFisier, &n);
    if (!vectorArticole) {
        printf("Eroare la citirea din fisier!\n");
        return 1;
    }

    printf("S-au citit %d articole din fisier.\n", n);

    Nod* cap = NULL;
    for (int i = 0; i < n; i++) {
        cap = inserareInceput(cap, vectorArticole[i]);
    }

    printf("\nLista initiala:");
    afisareLista(cap);

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

    printf("\nStergem ultimul element din lista:\n");
    cap = stergereUltim(cap);
    afisareLista(cap);

    int dimVector;
    Articol** vectorDinLista = listaLaVector(cap, &dimVector);

    printf("\nVectorul obtinut din lista are %d elemente:\n", dimVector);
    for (int i = 0; i < dimVector; i++) {
        printf("Pozitia %d: ID=%d, Denumire=%s\n", i, vectorDinLista[i]->id, vectorDinLista[i]->denumire);
    }

    free(vectorDinLista);
    eliberareLista(cap);
    free(vectorArticole);

    for (int i = 0; i < n; i++) {
        free(vectorArticole[i]->denumire);
        free(vectorArticole[i]->categorie);
        free(vectorArticole[i]->producator);
        free(vectorArticole[i]);
    }

    free(articolNou->denumire);
    free(articolNou->categorie);
    free(articolNou->producator);
    free(articolNou);

    return 0;
}