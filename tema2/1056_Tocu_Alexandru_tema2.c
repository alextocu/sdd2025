#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct Masina {
    char* marca;
    char* model;
    char* culoare;
    int an;
    int pret;
    int kilometraj;
    float consum;
}masina;

typedef struct NodSimplu {
    masina info;
    struct NodSimplu* next;
}nodSimplu;

typedef struct NodDublu {
    masina info;
    struct NodDublu* next;
    struct NodDublu* prev;
}nodDublu;

masina creareMasina(const char* marca, const char* model, const char* culoare, int an, int pret, int km, float consum) {
    masina m;
    m.marca = (char*)malloc(sizeof(char) * (strlen(marca) + 1));
    strcpy(m.marca, marca);
    m.model = (char*)malloc(sizeof(char) * (strlen(model) + 1));
    strcpy(m.model, model);
    m.culoare = (char*)malloc(sizeof(char) * (strlen(culoare) + 1));
    strcpy(m.culoare, culoare);
    m.an = an;
    m.pret = pret;
    m.kilometraj = km;
    m.consum = consum;
    return m;
}

void afisareMasina(masina m) {
    printf("Masina: %s %s, %s, An: %d, Pret: %d, Km: %d, Consum: %.2f\n",
        m.marca, m.model, m.culoare, m.an, m.pret, m.kilometraj, m.consum);
}

masina copieMasina(masina orig) {
    return creareMasina(orig.marca, orig.model, orig.culoare, orig.an, orig.pret, orig.kilometraj, orig.consum);
}

void dezalocareMasina(masina* m) {
    if (m->marca) {
        free(m->marca);
        m->marca = NULL;
    }
    if (m->model) {
        free(m->model);
        m->model = NULL;
    }
    if (m->culoare) {
        free(m->culoare);
        m->culoare = NULL;
    }
}

nodSimplu* adaugaSimplu(nodSimplu* cap, masina m) {
    nodSimplu* nou = (nodSimplu*)malloc(sizeof(nodSimplu));
    nou->info = copieMasina(m);
    nou->next = cap;
    return nou;
}

nodSimplu* citireDinFisier(const char* numeFisier) {
    nodSimplu* cap = NULL;
    FILE* f = fopen(numeFisier, "r");
    if (!f) {
        printf("Eroare la deschiderea fisierului\n");
        return cap;
    }

    int nr;
    fscanf(f, "%d", &nr);

    for (int i = 0; i < nr; i++) {
        char marca[50], model[50], culoare[30];
        int an, pret, km;
        float consum;

        fscanf(f, "%s %s %s %d %d %d %f", marca, model, culoare, &an, &pret, &km, &consum);
        masina m = creareMasina(marca, model, culoare, an, pret, km, consum);
        cap = adaugaSimplu(cap, m);
        dezalocareMasina(&m);
    }

    fclose(f);
    return cap;
}

nodDublu* conversieSimplaDubla(nodSimplu* simplu) {
    if (!simplu) return NULL;

    nodDublu* cap = NULL;
    nodDublu* coada = NULL;

    while (simplu) {
        nodDublu* nou = (nodDublu*)malloc(sizeof(nodDublu));
        nou->info = copieMasina(simplu->info);
        nou->next = NULL;
        nou->prev = coada;

        if (coada) {
            coada->next = nou;
        }
        else {
            cap = nou;
        }
        coada = nou;
        simplu = simplu->next;
    }

    return cap;
}

nodDublu* inserareSfarsit(nodDublu* cap, masina m) {
    nodDublu* nou = (nodDublu*)malloc(sizeof(nodDublu));
    nou->info = copieMasina(m);
    nou->next = NULL;
    nou->prev = NULL;

    if (!cap) {
        return nou;
    }

    nodDublu* temp = cap;
    while (temp->next) {
        temp = temp->next;
    }

    temp->next = nou;
    nou->prev = temp;
    return cap;
}

nodDublu* filtrareMasini(nodDublu* cap, int pretMin) {
    nodDublu* nouCap = NULL;
    nodDublu* nouCoada = NULL;

    while (cap) {
        if (cap->info.pret > pretMin) {
            nodDublu* nou = (nodDublu*)malloc(sizeof(nodDublu));
            nou->info = copieMasina(cap->info);
            nou->next = NULL;
            nou->prev = nouCoada;

            if (nouCoada) {
                nouCoada->next = nou;
            }
            else {
                nouCap = nou;
            }
            nouCoada = nou;
        }
        cap = cap->next;
    }

    return nouCap;
}

nodDublu* transformareCirculara(nodDublu* cap) {
    if (!cap) return NULL;

    nodDublu* temp = cap;
    while (temp->next) {
        temp = temp->next;
    }

    temp->next = cap;
    cap->prev = temp;

    return cap;
}

void afisareListaDubla(nodDublu* cap) {
    if (!cap) return;

    nodDublu* temp = cap;
    while (temp) {
        afisareMasina(temp->info);
        temp = temp->next;
    }
}

void afisareListaCirculara(nodDublu* cap) {
    if (!cap) return;

    nodDublu* temp = cap;
    do {
        afisareMasina(temp->info);
        temp = temp->next;
    } while (temp != cap);
}

nodDublu* stergereCirculara(nodDublu* cap, int anCautat) {
    if (!cap) return NULL;

    nodDublu* temp = cap;

    do {
        if (temp->info.an == anCautat) {
            if (temp->next == temp) {
                dezalocareMasina(&temp->info);
                free(temp);
                return NULL;
            }

            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            if (temp == cap) {
                cap = temp->next;
            }

            dezalocareMasina(&temp->info);
            free(temp);
            return cap;
        }
        temp = temp->next;
    } while (temp != cap);

    return cap;
}

void dezalocareListaSimpla(nodSimplu** cap) {
    while (*cap) {
        nodSimplu* temp = *cap;
        *cap = (*cap)->next;
        dezalocareMasina(&temp->info);
        free(temp);
    }
}

void dezalocareListaDubla(nodDublu** cap) {
    while (*cap) {
        nodDublu* temp = *cap;
        *cap = (*cap)->next;
        dezalocareMasina(&temp->info);
        free(temp);
    }
}

void dezalocareListaCirculara(nodDublu** cap) {
    if (!*cap) return;

    nodDublu* temp = *cap;
    (*cap)->prev->next = NULL;

    while (temp) {
        nodDublu* next = temp->next;
        dezalocareMasina(&temp->info);
        free(temp);
        temp = next;
    }
    *cap = NULL;
}

void main() {
    printf("Citire lista simpla din fisier : \n");
    nodSimplu* listaSimpla = citireDinFisier("masini.txt");
    nodSimplu* temp = listaSimpla;
    while (temp) {
        afisareMasina(temp->info);
        temp = temp->next;
    }
    printf("\n");

    printf("Conversie simpla -> dubla:\n");
    nodDublu* listaDubla = conversieSimplaDubla(listaSimpla);
    afisareListaDubla(listaDubla);
    printf("\n");

    printf("Inserare la sfarsit:\n");
    masina nouaMasina = creareMasina("Mercedes", "C-Class", "Negru", 2023, 45000, 15000, 7.5);
    listaDubla = inserareSfarsit(listaDubla, nouaMasina);
    afisareListaDubla(listaDubla);
    dezalocareMasina(&nouaMasina);
    printf("\n");

    printf("Lista filtrata(pret > 25000) :\n");
    nodDublu* listaFiltrata = filtrareMasini(listaDubla, 25000);
    afisareListaDubla(listaFiltrata);
    printf("\n");

    printf("Transformare in lista circulara:\n");
    nodDublu* listaCirculara = transformareCirculara(listaDubla);
    printf("Lista circulara (primele 10 elemente):\n");
    nodDublu* tempCirc = listaCirculara;
    for (int i = 0; i < 10 && tempCirc; i++) {
        afisareMasina(tempCirc->info);
        tempCirc = tempCirc->next;
        if (tempCirc == listaCirculara && i < 9) break;
    }
    printf("\n");

    printf("Stergere masina din anul 2020:\n");
    listaCirculara = stergereCirculara(listaCirculara, 2020);
    printf("Lista dupa stergere:\n");
    afisareListaCirculara(listaCirculara);

    dezalocareListaSimpla(&listaSimpla);
    dezalocareListaDubla(&listaFiltrata);
    dezalocareListaCirculara(&listaCirculara);
    printf("Lista dupa dezalocare:\n");
    afisareListaCirculara(listaCirculara);
}