#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Produs {
	char* nume;
	char* categorie;
	char* producator;
	int id;
	int pret;
	int stoc;
	float rating;
}produs;

produs creareProdus(const char* nume, const char* cat, const char* prod, int id, int pret, int stoc, float rating) {
	produs p;
	p.nume = (char*)malloc(strlen(nume) + 1);
	strcpy(p.nume, nume);
	p.categorie = (char*)malloc(strlen(cat) + 1);
	strcpy(p.categorie, cat);
	p.producator = (char*)malloc(strlen(prod) + 1);
	strcpy(p.producator, prod);
	p.id = id;
	p.pret = pret;
	p.stoc = stoc;
	p.rating = rating;
	return p;
}

void afisareProdus(produs p) {
	printf("ID: %d, %s (%s) - %s, Pret: %d, Stoc: %d, Rating: %.2f\n", p.id, p.nume, p.categorie, p.producator, p.pret, p.stoc, p.rating);
}

typedef struct Nod {
	produs info;
	struct Nod* next;
}nod;

nod* creareNod(produs p, nod* next) {
	nod* nou = (nod*)malloc(sizeof(nod));
	nou->info = creareProdus(p.nume, p.categorie, p.producator, p.id, p.pret, p.stoc, p.rating);
	nou->next = next;
	return nou;
}

nod* inserareFinal(nod* cap, produs p) {
	nod* nou = creareNod(p, NULL);
	if (cap) {
		nod* temp = cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
	else {
		cap = nou;
	}
	return cap;
}

nod* dezalocareLista(nod* cap) {
	while (cap) {
		nod* temp = cap;
		cap = cap->next;
		free(temp->info.nume);
		free(temp->info.categorie);
		free(temp->info.producator);
		free(temp);
	}
	return NULL;
}

void afisareLista(nod* cap) {
	while (cap) {
		afisareProdus(cap->info);
		cap = cap->next;
	}
}

typedef struct Hashtable {
	int dim;
	nod** vector;
}hashtable;

hashtable initHashtable(int dim) {
	hashtable h;
	h.dim = dim;
	h.vector = (nod**)malloc(sizeof(nod*) * dim);
	for (int i = 0; i < dim; i++) {
		h.vector[i] = NULL;
	}
	return h;
}

int hashcode(hashtable h, int id) {
	return (id * 7 + 13) % h.dim;
}

int inserareHashtable(hashtable h, produs p) {
	int poz = hashcode(h, p.id);
	h.vector[poz] = inserareFinal(h.vector[poz], p);
	return poz;
}

void afisareHashtable(hashtable h) {
	for (int i = 0; i < h.dim; i++) {
		if (h.vector[i]) {
			printf("Bucket %d:\n", i);
			afisareLista(h.vector[i]);
		}
	}
}

hashtable conversieListaHash(nod* cap, int dim) {
	hashtable h = initHashtable(dim);
	while (cap) {
		inserareHashtable(h, cap->info);
		cap = cap->next;
	}
	return h;
}

void stergereHashtableStocMic(hashtable h, int prag) {
	for (int i = 0; i < h.dim; i++) {
		nod** p = &h.vector[i];
		while (*p) {
			if ((*p)->info.stoc < prag) {
				nod* temp = *p;
				*p = (*p)->next;
				free(temp->info.nume);
				free(temp->info.categorie);
				free(temp->info.producator);
				free(temp);
			}
			else {
				p = &(*p)->next;
			}
		}
	}
}

void dezalocareHashtable(hashtable h) {
	for (int i = 0; i < h.dim; i++) {
		h.vector[i] = dezalocareLista(h.vector[i]);
	}
	free(h.vector);
	h.vector = NULL;
	h.dim = 0;
}

nod* citireLista(const char* numeFisier) {
	nod* lista = NULL;
	FILE* f = fopen(numeFisier, "r");
	if (f) {
		int n;
		fscanf(f, "%d", &n);
		for (int i = 0; i < n; i++) {
			char nume[50], cat[30], prod[50];
			int id, pret, stoc;
			float rating;
			fscanf(f, "%d %s %s %s %d %d %f", &id, nume, cat, prod, &pret, &stoc, &rating);
			produs p = creareProdus(nume, cat, prod, id, pret, stoc, rating);
			lista = inserareFinal(lista, p);
			free(p.nume);
			free(p.categorie);
			free(p.producator);
		}
		fclose(f);
	}
	return lista;
}

void main() {
	nod* lista = citireLista("produse.txt");
	printf("Lista produse:\n");
	afisareLista(lista);
	printf("\n");

	hashtable h = conversieListaHash(lista, 10);
	printf("Hashtable:\n");
	afisareHashtable(h);
	printf("\n");

	produs pNou = creareProdus("Tableta", "Electronice", "Samsung", 106, 800, 25, 4.6);
	inserareHashtable(h, pNou);
	free(pNou.nume);
	free(pNou.categorie);
	free(pNou.producator);
	printf("Hashtable dupa inserare:\n");
	afisareHashtable(h);
	printf("\n");

	printf("Hashtable dupa stergere produse cu stoc < 20:\n");
	stergereHashtableStocMic(h, 20);
	afisareHashtable(h);

	lista = dezalocareLista(lista);
	dezalocareHashtable(h);
}

// nota 10
