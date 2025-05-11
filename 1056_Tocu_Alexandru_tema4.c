#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct Restaurant {
    int id;
    char* nume;
    char* adresa;
    float rating;
    int capacitate;
    char* specific;
    int anInfiintare;
} Restaurant;

Restaurant creareRestaurant(int id, const char* nume, const char* adresa, float rating,
    int capacitate, const char* specific, int anInfiintare) {
    Restaurant r;
    r.id = id;
    r.nume = (char*)malloc(sizeof(char) * (strlen(nume) + 1));
    strcpy(r.nume, nume);
    r.adresa = (char*)malloc(sizeof(char) * (strlen(adresa) + 1));
    strcpy(r.adresa, adresa);
    r.rating = rating;
    r.capacitate = capacitate;
    r.specific = (char*)malloc(sizeof(char) * (strlen(specific) + 1));
    strcpy(r.specific, specific);
    r.anInfiintare = anInfiintare;
    return r;
}

void afisareRestaurant(Restaurant r) {
    printf("\n Restaurant [ID: %d] %s", r.id, r.nume);
    printf("\n  Adresa: %s", r.adresa);
    printf("\n  Rating: %.1f", r.rating);
    printf("\n  Capacitate: %d locuri", r.capacitate);
    printf("\n  Specific: %s", r.specific);
    printf("\n  An infiintare: %d", r.anInfiintare);
}

void dezalocareRestaurant(Restaurant* r) {
    if (r->nume) {
        free(r->nume);
        r->nume = NULL;
    }
    if (r->adresa) {
        free(r->adresa);
        r->adresa = NULL;
    }
    if (r->specific) {
        free(r->specific);
        r->specific = NULL;
    }
}

Restaurant copieRestaurant(Restaurant r) {
    return creareRestaurant(r.id, r.nume, r.adresa, r.rating, r.capacitate, r.specific, r.anInfiintare);
}

typedef struct NodArbore {
    Restaurant info;
    struct NodArbore* stanga;
    struct NodArbore* dreapta;
} nodArbore;

nodArbore* creareNodArbore(Restaurant r) {
    nodArbore* nou = (nodArbore*)malloc(sizeof(nodArbore));
    nou->info = copieRestaurant(r);
    nou->stanga = NULL;
    nou->dreapta = NULL;
    return nou;
}

nodArbore* inserareArbore(nodArbore* rad, Restaurant r) {
    if (rad == NULL) {
        return creareNodArbore(r);
    }

    if (r.id < rad->info.id) {
        rad->stanga = inserareArbore(rad->stanga, r);
    }
    else if (r.id > rad->info.id) {
        rad->dreapta = inserareArbore(rad->dreapta, r);
    }

    return rad;
}

nodArbore* citireArboreDinFisier(const char* numeFisier) {
    FILE* f = fopen(numeFisier, "r");
    nodArbore* rad = NULL;

    if (f == NULL) {
        printf("Eroare la deschiderea fisierului %s\n", numeFisier);
        return NULL;
    }

    int id, capacitate, anInfiintare;
    float rating;
    char buffer[1024];
    char nume[256], adresa[256], specific[256];

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        sscanf(buffer, "%d,%[^,],%[^,],%f,%d,%[^,],%d",
            &id, nume, adresa, &rating, &capacitate, specific, &anInfiintare);

        Restaurant r = creareRestaurant(id, nume, adresa, rating, capacitate, specific, anInfiintare);
        rad = inserareArbore(rad, r);
        dezalocareRestaurant(&r);
    }

    fclose(f);
    return rad;
}

void traversareInordine(nodArbore* rad) {
    if (rad) {
        traversareInordine(rad->stanga);
        afisareRestaurant(rad->info);
        traversareInordine(rad->dreapta);
    }
}

int numarFrunzeIdMaiMare(nodArbore* rad, int valoare) {
    if (rad == NULL) {
        return 0;
    }

    if (rad->stanga == NULL && rad->dreapta == NULL && rad->info.id > valoare) {
        return 1;
    }

    return numarFrunzeIdMaiMare(rad->stanga, valoare) + numarFrunzeIdMaiMare(rad->dreapta, valoare);
}

typedef struct NodHeap {
    Restaurant info;
    int prioritate;
} nodHeap;

typedef struct CoadaPrioritati {
    nodHeap* elemente;
    int dimensiune;
    int capacitateMaxima;
} coadaPrioritati;

coadaPrioritati initCoadaPrioritati(int capacitateMaxima) {
    coadaPrioritati cp;
    cp.elemente = (nodHeap*)malloc(sizeof(nodHeap) * capacitateMaxima);
    cp.dimensiune = 0;
    cp.capacitateMaxima = capacitateMaxima;
    return cp;
}

void interschimba(nodHeap* a, nodHeap* b) {
    nodHeap temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(coadaPrioritati* cp, int index) {
    int parinte = (index - 1) / 2;

    if (index > 0 && cp->elemente[parinte].prioritate < cp->elemente[index].prioritate) {
        interschimba(&cp->elemente[parinte], &cp->elemente[index]);
        heapifyUp(cp, parinte);
    }
}

void inserareCoadaPrioritati(coadaPrioritati* cp, Restaurant r, int prioritate) {
    if (cp->dimensiune >= cp->capacitateMaxima) {
        printf("\nCoada de prioritati este plina!");
        return;
    }

    nodHeap nod;
    nod.info = copieRestaurant(r);
    nod.prioritate = prioritate;

    cp->elemente[cp->dimensiune] = nod;
    cp->dimensiune++;

    heapifyUp(cp, cp->dimensiune - 1);
}

void heapifyDown(coadaPrioritati* cp, int index) {
    int maxIndex = index;
    int stanga = 2 * index + 1;
    int dreapta = 2 * index + 2;

    if (stanga < cp->dimensiune && cp->elemente[stanga].prioritate > cp->elemente[maxIndex].prioritate) {
        maxIndex = stanga;
    }

    if (dreapta < cp->dimensiune && cp->elemente[dreapta].prioritate > cp->elemente[maxIndex].prioritate) {
        maxIndex = dreapta;
    }

    if (maxIndex != index) {
        interschimba(&cp->elemente[index], &cp->elemente[maxIndex]);
        heapifyDown(cp, maxIndex);
    }
}

Restaurant extrageMaxCoadaPrioritati(coadaPrioritati* cp) {
    if (cp->dimensiune <= 0) {
        printf("\nCoada de prioritati este goala!");
        return creareRestaurant(-1, "", "", 0, 0, "", 0);
    }

    Restaurant rezultat = copieRestaurant(cp->elemente[0].info);
    dezalocareRestaurant(&cp->elemente[0].info);

    cp->elemente[0] = cp->elemente[cp->dimensiune - 1];
    cp->dimensiune--;

    heapifyDown(cp, 0);

    return rezultat;
}

void convertesteArboreLaCoadaPrioritati(nodArbore* rad, coadaPrioritati* cp) {
    if (rad) {
        inserareCoadaPrioritati(cp, rad->info, (int)(rad->info.rating * 10));

        convertesteArboreLaCoadaPrioritati(rad->stanga, cp);
        convertesteArboreLaCoadaPrioritati(rad->dreapta, cp);
    }
}

coadaPrioritati filtrareCoadaPrioritati(coadaPrioritati cp, int pragCapacitate) {
    coadaPrioritati rezultat = initCoadaPrioritati(cp.capacitateMaxima);

    for (int i = 0; i < cp.dimensiune; i++) {
        if (cp.elemente[i].info.capacitate > pragCapacitate) {
            inserareCoadaPrioritati(&rezultat, cp.elemente[i].info, cp.elemente[i].prioritate);
        }
    }

    return rezultat;
}

int comparaRestauranteId(const void* a, const void* b) {
    Restaurant* r1 = (Restaurant*)a;
    Restaurant* r2 = (Restaurant*)b;
    return r1->id - r2->id;
}

void adaugaInVector(nodArbore* nod, Restaurant* vector, int* dimensiune, int capacitateMaxima, const char* specific) {
    if (nod) {
        if (strcmp(nod->info.specific, specific) == 0) {
            if (*dimensiune < capacitateMaxima) {
                vector[*dimensiune] = copieRestaurant(nod->info);
                (*dimensiune)++;
            }
        }
        adaugaInVector(nod->stanga, vector, dimensiune, capacitateMaxima, specific);
        adaugaInVector(nod->dreapta, vector, dimensiune, capacitateMaxima, specific);
    }
}

Restaurant* restauranteCuSpecific(nodArbore* rad, const char* specific, int* dimensiune) {
    int capacitateMaxima = 100;
    Restaurant* vector = (Restaurant*)malloc(sizeof(Restaurant) * capacitateMaxima);
    *dimensiune = 0;

    adaugaInVector(rad, vector, dimensiune, capacitateMaxima, specific);

    qsort(vector, *dimensiune, sizeof(Restaurant), comparaRestauranteId);

    return vector;
}

void traversareCoadaPrioritati(coadaPrioritati cp) {
    printf("\n====== TRAVERSARE COADA DE PRIORITATI ======");
    for (int i = 0; i < cp.dimensiune; i++) {
        printf("\nPrioritate: %d", cp.elemente[i].prioritate);
        afisareRestaurant(cp.elemente[i].info);
    }
}

void stergereElementeDupaRating(coadaPrioritati* cp, float ratingMinim) {
    coadaPrioritati temp = initCoadaPrioritati(cp->capacitateMaxima);

    while (cp->dimensiune > 0) {
        Restaurant r = extrageMaxCoadaPrioritati(cp);

        if (r.rating >= ratingMinim) {
            inserareCoadaPrioritati(&temp, r, (int)(r.rating * 10));
        }

        dezalocareRestaurant(&r);
    }

    while (temp.dimensiune > 0) {
        Restaurant r = extrageMaxCoadaPrioritati(&temp);
        inserareCoadaPrioritati(cp, r, (int)(r.rating * 10));
        dezalocareRestaurant(&r);
    }

    free(temp.elemente);
}

nodArbore* dezalocareArbore(nodArbore* rad) {
    if (rad) {
        dezalocareArbore(rad->stanga);
        dezalocareArbore(rad->dreapta);
        dezalocareRestaurant(&rad->info);
        free(rad);
    }
    return NULL;
}

void dezalocareCoadaPrioritati(coadaPrioritati* cp) {
    for (int i = 0; i < cp->dimensiune; i++) {
        dezalocareRestaurant(&cp->elemente[i].info);
    }
    free(cp->elemente);
    cp->elemente = NULL;
    cp->dimensiune = 0;
    cp->capacitateMaxima = 0;
}

void dezalocareVectorRestaurante(Restaurant* vector, int dimensiune) {
    for (int i = 0; i < dimensiune; i++) {
        dezalocareRestaurant(&vector[i]);
    }
    free(vector);
}

void main() {
    FILE* f = fopen("restaurante.txt", "w");
    if (f) {
        fprintf(f, "1,La Mama,Str. Victoriei nr. 12,4.5,120,italian,2005\n");
        fprintf(f, "3,Grain,Str. Dacia nr. 7,4.8,80,fusion,2018\n");
        fprintf(f, "2,Caru' cu Bere,Str. Stavropoleos nr. 5,4.2,200,romanesc,1879\n");
        fprintf(f, "5,Casa Boema,Str. Popa Tatu nr. 24,4.0,60,mediteranean,2015\n");
        fprintf(f, "4,Beijing Garden,Bd. Timisoara nr. 56,4.1,100,chinezesc,2010\n");
        fprintf(f, "7,Sushi Time,Calea Dorobanti nr. 67,4.6,50,japonez,2016\n");
        fprintf(f, "6,Casa Latina,Str. Academiei nr. 28,3.9,90,mexican,2012\n");
        fprintf(f, "8,La Placinte,Str. Buzesti nr. 14,4.3,70,moldovenesc,2008\n");
        fclose(f);
    }

    nodArbore* rad = citireArboreDinFisier("restaurante.txt");
    printf("\n====== ARBORE CITIT DIN FISIER (traversare inordine) ======");
    traversareInordine(rad);

    int valoarePrag = 4;
    int nrFrunze = numarFrunzeIdMaiMare(rad, valoarePrag);
    printf("\n\nNumarul de frunze cu ID mai mare decat %d: %d", valoarePrag, nrFrunze);

    coadaPrioritati cp = initCoadaPrioritati(100);
    convertesteArboreLaCoadaPrioritati(rad, &cp);
    printf("\n\n====== COADA DE PRIORITATI CREATA DIN ARBORE ======");
    traversareCoadaPrioritati(cp);

    int pragCapacitate = 80;
    coadaPrioritati cpFiltrata = filtrareCoadaPrioritati(cp, pragCapacitate);
    printf("\n\n====== COADA DE PRIORITATI FILTRATA (capacitate > %d) ======", pragCapacitate);
    traversareCoadaPrioritati(cpFiltrata);

    const char* specificCautat = "italian";
    int dimensiune;
    Restaurant* vectorRestaurante = restauranteCuSpecific(rad, specificCautat, &dimensiune);
    printf("\n\n====== RESTAURANTE CU SPECIFIC %s (sortate dupa ID) ======", specificCautat);
    for (int i = 0; i < dimensiune; i++) {
        afisareRestaurant(vectorRestaurante[i]);
    }

    printf("\n\n====== TRAVERSARE ARBORE (inordine) ======");
    traversareInordine(rad);

    float ratingMinim = 4.2;
    stergereElementeDupaRating(&cp, ratingMinim);
    printf("\n\n====== COADA DE PRIORITATI DUPA STERGERE (rating >= %.1f) ======", ratingMinim);
    traversareCoadaPrioritati(cp);

    rad = dezalocareArbore(rad);
    dezalocareCoadaPrioritati(&cp);
    dezalocareCoadaPrioritati(&cpFiltrata);
    dezalocareVectorRestaurante(vectorRestaurante, dimensiune);

    printf("\n\nToate structurile de date au fost dezalocate!\n");

    _CrtDumpMemoryLeaks();
}

// Nota 9.5