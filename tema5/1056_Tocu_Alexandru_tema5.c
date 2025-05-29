#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct Angajat {
    char* nume;
    char* departament;
    char* pozitie;
    int salariu;
    int aniExperienta;
    int codAngajat;
    float rating;
}angajat;

typedef struct NodGraf {
    angajat info;
    struct NodGraf** vecini;
    int nrVecini;
    int capacitate;
}nodGraf;

typedef struct Graf {
    nodGraf** noduri;
    int nrNoduri;
    int capacitate;
}graf;

typedef struct NodAVL {
    angajat info;
    struct NodAVL* stanga;
    struct NodAVL* dreapta;
    int inaltime;
}nodAVL;

typedef struct NodLista {
    angajat info;
    struct NodLista* next;
}nodLista;

angajat creareAngajat(const char* nume, const char* dept, const char* poz, int sal, int ani, int cod, float rat) {
    angajat a;
    a.nume = (char*)malloc(sizeof(char) * (strlen(nume) + 1));
    strcpy(a.nume, nume);
    a.departament = (char*)malloc(sizeof(char) * (strlen(dept) + 1));
    strcpy(a.departament, dept);
    a.pozitie = (char*)malloc(sizeof(char) * (strlen(poz) + 1));
    strcpy(a.pozitie, poz);
    a.salariu = sal;
    a.aniExperienta = ani;
    a.codAngajat = cod;
    a.rating = rat;
    return a;
}

void afisareAngajat(angajat a) {
    printf("Angajat: %s, Dept: %s, Pozitie: %s, Salariu: %d, Ani: %d, Cod: %d, Rating: %.2f\n",
        a.nume, a.departament, a.pozitie, a.salariu, a.aniExperienta, a.codAngajat, a.rating);
}

angajat copieAngajat(angajat orig) {
    return creareAngajat(orig.nume, orig.departament, orig.pozitie, orig.salariu, orig.aniExperienta, orig.codAngajat, orig.rating);
}

void dezalocareAngajat(angajat* a) {
    if (a->nume) {
        free(a->nume);
        a->nume = NULL;
    }
    if (a->departament) {
        free(a->departament);
        a->departament = NULL;
    }
    if (a->pozitie) {
        free(a->pozitie);
        a->pozitie = NULL;
    }
}

graf initGraf() {
    graf g;
    g.noduri = NULL;
    g.nrNoduri = 0;
    g.capacitate = 0;
    return g;
}

void adaugaNodGraf(graf* g, angajat a) {
    if (g->nrNoduri == g->capacitate) {
        g->capacitate = (g->capacitate == 0) ? 2 : g->capacitate * 2;
        g->noduri = (nodGraf**)realloc(g->noduri, sizeof(nodGraf*) * g->capacitate);
    }
    nodGraf* nou = (nodGraf*)malloc(sizeof(nodGraf));
    nou->info = copieAngajat(a);
    nou->vecini = NULL;
    nou->nrVecini = 0;
    nou->capacitate = 0;
    g->noduri[g->nrNoduri] = nou;
    g->nrNoduri++;
}

void adaugaMuchie(graf* g, int i, int j) {
    if (i < g->nrNoduri && j < g->nrNoduri) {
        nodGraf* nod = g->noduri[i];
        if (nod->nrVecini == nod->capacitate) {
            nod->capacitate = (nod->capacitate == 0) ? 2 : nod->capacitate * 2;
            nod->vecini = (nodGraf**)realloc(nod->vecini, sizeof(nodGraf*) * nod->capacitate);
        }
        nod->vecini[nod->nrVecini] = g->noduri[j];
        nod->nrVecini++;
    }
}

int maxim(int a, int b) {
    return (a > b) ? a : b;
}

int inaltimeAVL(nodAVL* nod) {
    if (nod == NULL) return 0;
    return nod->inaltime;
}

int getBalance(nodAVL* nod) {
    if (nod == NULL) return 0;
    return inaltimeAVL(nod->stanga) - inaltimeAVL(nod->dreapta);
}

nodAVL* rotireDreapta(nodAVL* y) {
    nodAVL* x = y->stanga;
    nodAVL* T2 = x->dreapta;
    x->dreapta = y;
    y->stanga = T2;
    y->inaltime = maxim(inaltimeAVL(y->stanga), inaltimeAVL(y->dreapta)) + 1;
    x->inaltime = maxim(inaltimeAVL(x->stanga), inaltimeAVL(x->dreapta)) + 1;
    return x;
}

nodAVL* rotireStanga(nodAVL* x) {
    nodAVL* y = x->dreapta;
    nodAVL* T2 = y->stanga;
    y->stanga = x;
    x->dreapta = T2;
    x->inaltime = maxim(inaltimeAVL(x->stanga), inaltimeAVL(x->dreapta)) + 1;
    y->inaltime = maxim(inaltimeAVL(y->stanga), inaltimeAVL(y->dreapta)) + 1;
    return y;
}

nodAVL* creareNodAVL(angajat a) {
    nodAVL* nou = (nodAVL*)malloc(sizeof(nodAVL));
    nou->info = copieAngajat(a);
    nou->stanga = NULL;
    nou->dreapta = NULL;
    nou->inaltime = 1;
    return nou;
}

nodAVL* inserareAVL(nodAVL* nod, angajat a) {
    if (nod == NULL)
        return creareNodAVL(a);

    if (a.salariu < nod->info.salariu)
        nod->stanga = inserareAVL(nod->stanga, a);
    else if (a.salariu > nod->info.salariu)
        nod->dreapta = inserareAVL(nod->dreapta, a);
    else
        return nod;

    nod->inaltime = 1 + maxim(inaltimeAVL(nod->stanga), inaltimeAVL(nod->dreapta));

    int balance = getBalance(nod);

    if (balance > 1 && a.salariu < nod->stanga->info.salariu)
        return rotireDreapta(nod);

    if (balance < -1 && a.salariu > nod->dreapta->info.salariu)
        return rotireStanga(nod);

    if (balance > 1 && a.salariu > nod->stanga->info.salariu) {
        nod->stanga = rotireStanga(nod->stanga);
        return rotireDreapta(nod);
    }

    if (balance < -1 && a.salariu < nod->dreapta->info.salariu) {
        nod->dreapta = rotireDreapta(nod->dreapta);
        return rotireStanga(nod);
    }

    return nod;
}

nodAVL* minValueNode(nodAVL* nod) {
    nodAVL* current = nod;
    while (current->stanga != NULL)
        current = current->stanga;
    return current;
}

nodAVL* stergereAVL(nodAVL* root, int salariu) {
    if (root == NULL)
        return root;

    if (salariu < root->info.salariu)
        root->stanga = stergereAVL(root->stanga, salariu);
    else if (salariu > root->info.salariu)
        root->dreapta = stergereAVL(root->dreapta, salariu);
    else {
        if ((root->stanga == NULL) || (root->dreapta == NULL)) {
            nodAVL* temp = root->stanga ? root->stanga : root->dreapta;
            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            else {
                dezalocareAngajat(&root->info);
                *root = *temp;
            }
            free(temp);
        }
        else {
            nodAVL* temp = minValueNode(root->dreapta);
            dezalocareAngajat(&root->info);
            root->info = copieAngajat(temp->info);
            root->dreapta = stergereAVL(root->dreapta, temp->info.salariu);
        }
    }

    if (root == NULL)
        return root;

    root->inaltime = 1 + maxim(inaltimeAVL(root->stanga), inaltimeAVL(root->dreapta));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->stanga) >= 0)
        return rotireDreapta(root);

    if (balance > 1 && getBalance(root->stanga) < 0) {
        root->stanga = rotireStanga(root->stanga);
        return rotireDreapta(root);
    }

    if (balance < -1 && getBalance(root->dreapta) <= 0)
        return rotireStanga(root);

    if (balance < -1 && getBalance(root->dreapta) > 0) {
        root->dreapta = rotireDreapta(root->dreapta);
        return rotireStanga(root);
    }

    return root;
}

void traversareInordine(nodAVL* root) {
    if (root != NULL) {
        traversareInordine(root->stanga);
        afisareAngajat(root->info);
        traversareInordine(root->dreapta);
    }
}

void adaugaInLista(nodLista** cap, angajat a) {
    nodLista* nou = (nodLista*)malloc(sizeof(nodLista));
    nou->info = copieAngajat(a);
    nou->next = *cap;
    *cap = nou;
}

nodLista* filtreazaAngajati(nodAVL* root, int salariuMin) {
    if (root == NULL) return NULL;

    nodLista* lista = NULL;
    nodLista* temp = filtreazaAngajati(root->stanga, salariuMin);
    while (temp) {
        nodLista* next = temp->next;
        temp->next = lista;
        lista = temp;
        temp = next;
    }

    if (root->info.salariu > salariuMin) {
        adaugaInLista(&lista, root->info);
    }

    temp = filtreazaAngajati(root->dreapta, salariuMin);
    while (temp) {
        nodLista* next = temp->next;
        temp->next = lista;
        lista = temp;
        temp = next;
    }

    return lista;
}

void afisareLista(nodLista* cap) {
    while (cap) {
        afisareAngajat(cap->info);
        cap = cap->next;
    }
}

void calculMedieRating(graf g) {
    if (g.nrNoduri == 0) return;
    float suma = 0;
    for (int i = 0; i < g.nrNoduri; i++) {
        suma += g.noduri[i]->info.rating;
    }
    printf("Media rating-urilor: %.2f\n", suma / g.nrNoduri);
}

int calculSumaConditionala(nodAVL* root, int aniMin, float ratingMin) {
    if (root == NULL) return 0;

    int suma = 0;
    suma += calculSumaConditionala(root->stanga, aniMin, ratingMin);

    if (root->info.aniExperienta >= aniMin && root->info.rating >= ratingMin) {
        suma += root->info.salariu;
    }

    suma += calculSumaConditionala(root->dreapta, aniMin, ratingMin);
    return suma;
}

void parcurgereLatimeGraf(graf g) {
    if (g.nrNoduri == 0) return;

    int* vizitat = (int*)calloc(g.nrNoduri, sizeof(int));
    int* coada = (int*)malloc(sizeof(int) * g.nrNoduri);
    int front = 0, rear = 0;

    printf("Parcurgere in latime graf:\n");
    coada[rear++] = 0;
    vizitat[0] = 1;

    while (front < rear) {
        int current = coada[front++];
        afisareAngajat(g.noduri[current]->info);

        for (int i = 0; i < g.noduri[current]->nrVecini; i++) {
            for (int j = 0; j < g.nrNoduri; j++) {
                if (g.noduri[j] == g.noduri[current]->vecini[i] && !vizitat[j]) {
                    vizitat[j] = 1;
                    coada[rear++] = j;
                    break;
                }
            }
        }
    }

    free(vizitat);
    free(coada);
}

void traversareGraf(graf g) {
    printf("Toate elementele grafului:\n");
    for (int i = 0; i < g.nrNoduri; i++) {
        printf("Nodul %d: ", i);
        afisareAngajat(g.noduri[i]->info);
    }
}

nodAVL* grafLaAVL(graf g) {
    nodAVL* root = NULL;
    for (int i = 0; i < g.nrNoduri; i++) {
        root = inserareAVL(root, g.noduri[i]->info);
    }
    return root;
}

graf citireGrafDinFisier(const char* numeFisier) {
    graf g = initGraf();
    FILE* f = fopen(numeFisier, "r");
    if (!f) {
        printf("Eroare la deschiderea fisierului\n");
        return g;
    }

    int nrAngajati;
    fscanf(f, "%d", &nrAngajati);

    for (int i = 0; i < nrAngajati; i++) {
        char nume[100], dept[100], poz[100];
        int sal, ani, cod;
        float rating;

        fscanf(f, "%s %s %s %d %d %d %f", nume, dept, poz, &sal, &ani, &cod, &rating);
        angajat a = creareAngajat(nume, dept, poz, sal, ani, cod, rating);
        adaugaNodGraf(&g, a);
        dezalocareAngajat(&a);
    }

    int nrMuchii;
    fscanf(f, "%d", &nrMuchii);
    for (int i = 0; i < nrMuchii; i++) {
        int x, y;
        fscanf(f, "%d %d", &x, &y);
        adaugaMuchie(&g, x, y);
    }

    fclose(f);
    return g;
}

void dezalocareAVL(nodAVL** root) {
    if (*root) {
        dezalocareAVL(&(*root)->stanga);
        dezalocareAVL(&(*root)->dreapta);
        dezalocareAngajat(&(*root)->info);
        free(*root);
        *root = NULL;
    }
}

void dezalocareGraf(graf* g) {
    for (int i = 0; i < g->nrNoduri; i++) {
        dezalocareAngajat(&g->noduri[i]->info);
        if (g->noduri[i]->vecini) {
            free(g->noduri[i]->vecini);
        }
        free(g->noduri[i]);
    }
    if (g->noduri) {
        free(g->noduri);
    }
    g->noduri = NULL;
    g->nrNoduri = 0;
    g->capacitate = 0;
}

void dezalocareLista(nodLista** cap) {
    while (*cap) {
        nodLista* temp = *cap;
        *cap = (*cap)->next;
        dezalocareAngajat(&temp->info);
        free(temp);
    }
}

void main() {
    graf g = citireGrafDinFisier("angajati.txt");


    printf("Graf citit din fisier:\n");
    traversareGraf(g);
    printf("\n");

    printf("Media rating-urilor (caracteristica din graf):\n");
    calculMedieRating(g);
    printf("\n");

    printf("Convertire graf la AVL(ordonat dupa salariu) :\n");
    nodAVL* avl = grafLaAVL(g);
    traversareInordine(avl);
    printf("\n");

    printf("Lista angajati cu salariu > 4000:\n");
    nodLista* lista = filtreazaAngajati(avl, 4000);
    afisareLista(lista);
    printf("\n");

    printf("Suma salarii (ani >= 3 si rating >= 4.0):\n");
    int suma = calculSumaConditionala(avl, 3, 4.0);
    printf("Suma totala: %d\n\n", suma);

    parcurgereLatimeGraf(g);
    printf("\n");

    printf("Traversare AVL (inordine):\n");
    traversareInordine(avl);
    printf("\n");

    printf("Stergere angajat cu salariul 5500:\n");
    avl = stergereAVL(avl, 5500);
    printf("AVL dupa stergere:\n");
    traversareInordine(avl);

    dezalocareLista(&lista);
    dezalocareAVL(&avl);
    dezalocareGraf(&g);
}

// nota 10
