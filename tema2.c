// Delea Cynthia Andreea, 312AB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Resursa {
	char *nume;
	int cantitate;
} Resursa;

typedef struct Island{
	char *nume;
	int nrResurse;
	Resursa *inventarResurse;
	int nrAvioane;
	int *avioane;
	int tolAvioane;
} Island;

Island *citireInsule(FILE *in, int n) {
	// Se aloca spatiu pentru structura de insule si se citeste aceasta
	Island *insule = calloc(n, sizeof(Island));
	if (!insule) {
		exit(EXIT_FAILURE);
	}

	char buffer[100];
	int i, j;
	for (i = 0; i < n; i++) {
		fscanf(in, "%s %d", buffer, &insule[i].nrResurse);
		insule[i].nume = strdup(buffer);

		// Se aloca vectorul de resurse si se citeste acesta pt fiecare insula
		insule[i].inventarResurse = calloc(insule[i].nrResurse,
			sizeof(Resursa));
		if (!insule[i].inventarResurse) {
			exit(EXIT_FAILURE);
		}
		for (j = 0; j < insule[i].nrResurse; j++) {
			fscanf(in, "%s %d", buffer,
				&insule[i].inventarResurse[j].cantitate);
			insule[i].inventarResurse[j].nume = strdup(buffer);
		}
	}

	return insule;
}

int **citireLegaturi(FILE *in, int n) {
	// Se aloca matricea de adiacenta dintre insule si se citeste aceasta
	int **legaturi = calloc(n, sizeof(int *));
	if (!legaturi) {
		exit(EXIT_FAILURE);
	}
	int i;
	for (i = 0; i < n; i++) {
		legaturi[i] = calloc(n, sizeof(int));
		if (!legaturi[i]) {
			exit(EXIT_FAILURE);
		}
	}

	int p;
	fscanf(in, "%d", &p);

	char buffer[100], buffer2[100];
	for (i = 0; i < p; i++) {
		fscanf(in, "%s - %s", buffer, buffer2);

		// Se prelucreaza string-ul citit pentru a se obtine doar numarul de la
		// finalul acestuia
		int stg, dr;
		sscanf(buffer, "%*[^0123456789]%d", &stg);
		sscanf(buffer2, "%*[^0123456789]%d", &dr);
		stg--;
		dr--;
		fscanf(in, "%d ", &legaturi[stg][dr]);
		legaturi[dr][stg] = legaturi[stg][dr];
	}

	return legaturi;
}

void printeazaResurse(FILE *out, int n, Island *insule) {
	// se numara resursele de pe toate insulele
	int i, nr = 0;
	for (i = 0; i < n; i++) {
		nr += insule[i].nrResurse;
	}

	// se aloca un vector pentru toate resursele
	char **resurse = calloc(nr, sizeof(char *));
	if (!resurse) {
		exit(EXIT_FAILURE);
	}

	// se adauga toate resursele in vector
	int j, k = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < insule[i].nrResurse; j++) {
			resurse[k] = insule[i].inventarResurse[j].nume;
			k++;
		}
	}

	// se sorteaza alfabetic resursele
	char *aux;
	for (i = 0; i < k - 1; i++) {
		for (j = i + 1; j < k; j++) {
			if (strcmp(resurse[i], resurse[j]) > 0) {
				aux = resurse[i];
				resurse[i] = resurse[j];
				resurse[j] = aux;
			}
		}
	}

	// Se numara resursele unice
	nr = 0;
	for (i = 0; i < k; i++) {
		while (i < k - 1 && strcmp(resurse[i], resurse[i + 1]) == 0) {
			i++;
		}

		nr++;
	}

	// Se afiseaza acest numar
	fprintf(out, "%d ", nr);

	// Se afiseaza fiecare resursa unica
	for (i = 0; i < k; i++) {
		while (i < k - 1 && strcmp(resurse[i], resurse[i + 1]) == 0) {
			i++;
		}
		fprintf(out, "%s ", resurse[i]);
	}

	fprintf(out, "\n");

	free(resurse);
}

void printeazaMaxResurse(FILE *out, int n, Island *insule, char *resursa) {
	// Verifica cantitatea maxima pentru resursa dorita
	int i, max = -1, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < insule[i].nrResurse; j++) {
			if (strcmp(insule[i].inventarResurse[j].nume, resursa) == 0) {
				if (max < insule[i].inventarResurse[j].cantitate) {
					max = insule[i].inventarResurse[j].cantitate;
				}
			}
		}
	}

	// Se afiseaza toate insulele care au aceasta cantitate din respectiva
	// resursa
	for (i = 0; i < n; i++) {
		for (j = 0; j < insule[i].nrResurse; j++) {
			if (strcmp(insule[i].inventarResurse[j].nume, resursa) == 0) {
				if (max == insule[i].inventarResurse[j].cantitate) {
					fprintf(out, "Island%d ", i + 1);
				}
			}
		}
	}
	fprintf(out, "\n");
}

int distantaMinima(int n, int *distante, int *sptSet) {
	// Cauta nodul cu distanta cea mai mica ce nu a fost deja prelucrat
	int i, min = INT_MAX, indexMin;
	for (i = 0; i < n; i++) {
		if (!sptSet[i] && distante[i] <= min) {
			indexMin = i;
			min = distante[i];
		}
	}

	return indexMin;
}

void dijsktra(int n, int **legaturi, int sursa, int *parinti) {
	// Se aplica algoritmul Dijsktra pentru a se calcula caile cele mai scurte
	// de la nodul sursa la celelalte noduri. Aceste cai sunt memorate in vectorul
	// de parinti

	// Vector de distante de la nodul sursa
	int *distante = calloc(n, sizeof(int));
	if (!distante) {
		exit(EXIT_FAILURE);
	}
	// Vector ce marcheaza daca un nod a fost prelucrat sau nu
	int *vizitati = calloc(n, sizeof(int));
	if (!vizitati) {
		exit(EXIT_FAILURE);
	}

	// Initializam distantele cu valoarea maxima si parintii cu -1
	int i;
	for (i = 0; i < n; i++) {
		distante[i] = INT_MAX;
		parinti[i] = -1;
	}

	// Nodul sursa are distanta 0
	distante[sursa] = 0;

	for (i = 0; i < n - 1; i++) {
		// Se ia indexul cu distanta cea mai mica
		int u = distantaMinima(n, distante, vizitati);
		// Se marcheaza ca si prelucrat
		vizitati[u] = 1;
		int v;
		for (v = 0; v < n; v++) {
			if (!vizitati[v] && legaturi[u][v] && legaturi[u][v] != INT_MAX &&
				distante[u] != INT_MAX &&
				distante[u] + legaturi[u][v] < distante[v]) {
				parinti[v] = u;
				distante[v] = distante[u] + legaturi[u][v];
			}
		}

	}

	free(distante);
	free(vizitati);
}

void calculeazaCale(int *parinti, int *cale, int insula, int curent,
	int *lungime) {
	// Calculez recursiv calea de la nodul destinatie la cel sursa si o memorez
	if (parinti[insula] == -1) {
		// insula este nodul sursa
		cale[curent] = insula;
		*lungime = ++curent;
		return;
	}

	// Adaug nodul in cale
	cale[curent] = insula;
	// Calculez recursiv restul caii
	calculeazaCale(parinti, cale, parinti[insula], ++curent, lungime);
}

void afiseazaDrum(int n, int **legaturi, int sursa, int destinatie, FILE *out) {
	// Aplic algoritmul dijsktra, care returneaza vectorul de parinti. Apoi
	// pornesc recursiv de la nodul destinatie catre cel sursa si salvez calea
	// intr-un vector de indici, apoi afisez numele fiecaruia din vectorul de
	// clienti
	int *parinti = calloc(n, sizeof(int));
	if (!parinti) {
		exit(EXIT_FAILURE);
	}

	// Calculez vectorul de parinti
	dijsktra(n, legaturi, sursa, parinti);

	// Calculez calea catre nodul sursa de la cel destinatie
	int *cale = calloc(n, sizeof(int));
	if (!cale) {
		exit(EXIT_FAILURE);
	}
	int lungime = 0;
	calculeazaCale(parinti, cale, destinatie, 0, &lungime);

	if (lungime > 1) {
		// Afisez calea invers
		int i;
		for (i = lungime - 2; i >= 0; i--) {
			fprintf(out, "Island%d ", cale[i] + 1);
		}
		fprintf(out, "\n");
	} else {
		fprintf(out, "NO\n");
	}

	free(parinti);
	free(cale);
}

void afiseazaTimp(int n, int **legaturi, int sursa, int destinatie, FILE *out) {
	// Aplic algoritmul dijsktra, care returneaza vectorul de parinti. Apoi
	// pornesc recursiv de la nodul destinatie catre cel sursa si salvez calea
	// intr-un vector de indici, apoi afisez numele fiecaruia din vectorul de
	// clienti
	int *parinti = calloc(n, sizeof(int));
	if (!parinti) {
		exit(EXIT_FAILURE);
	}

	// Calculez vectorul de parinti
	dijsktra(n, legaturi, sursa, parinti);

	// Calculez calea catre nodul sursa de la cel destinatie
	int *cale = calloc(n, sizeof(int));
	if (!cale) {
		exit(EXIT_FAILURE);
	}
	int lungime = 0;
	calculeazaCale(parinti, cale, destinatie, 0, &lungime);

	if (lungime > 1) {
		// Calculez si afisez durata traseului
		int i, s = 0;
		for (i = lungime - 1; i > 0; i--) {
			s += legaturi[cale[i]][cale[i - 1]];
		}
		fprintf(out, "%d\n", s);
	} else {
		fprintf(out, "INF\n");
	}

	free(parinti);
	free(cale);
}

void eliberareInsule(Island *insule, int n) {
	// Eliberez memoria alocata pentru insule si resursele acestora
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < insule[i].nrResurse; j++) {
			free(insule[i].inventarResurse[j].nume);
		}

		free(insule[i].inventarResurse);
		free(insule[i].nume);
	}
	free(insule);
}

int main(int argc, char *argv[]) {
	// Pentru a evita warning-urile de compilare
	if (argc) {}

	FILE *in = fopen(argv[1], "r");

	// Citeste numarul de insule
	int n;
	fscanf(in, "%d", &n);

	Island *insule = citireInsule(in, n);

	int **legaturi = citireLegaturi(in, n);

	// Incepe prelucrarea comenzilor
	FILE *out = fopen(argv[2], "w");
	char command[100], buffer[100], buffer2[100];
	while (fscanf(in, "%s", command) != EOF) {
		if (strcmp(command, "conexiune") == 0) {
			// Se verifica in matricea de adiacenta daca exista conexiune intre
			// cele doua insule
			fscanf(in, "%s %s", buffer, buffer2);
			int stg, dr;
			sscanf(buffer, "%*[^0123456789]%d", &stg);
			sscanf(buffer2, "%*[^0123456789]%d", &dr);
			stg--;
			dr--;
			if (legaturi[stg][dr]) {
				fprintf(out, "OK\n");
			} else {
				fprintf(out, "NO\n");
			}
		} else if (strcmp(command, "legatura") == 0) {
			// Se afiseaza toate insulele care au legatura cu cea dorita,
			// verificand daca exista valori in matricea de adiacenta
			fscanf(in, "%s", buffer);

			int nr;	
			sscanf(buffer, "%*[^0123456789]%d", &nr);
			nr--;
		
			int i;
			for (i = 0; i < n; i++) {
				if (legaturi[nr][i]) {
					fprintf(out, "Island%d ", i + 1);
				}
			}
			fprintf(out, "\n");	
		} else if (strcmp(command, "adauga_zbor") == 0) {
			// Se adauga o noua legatura intre cele doua insule
			fscanf(in, "%s %s", buffer, buffer2);

			int stg, dr;
			sscanf(buffer, "%*[^0123456789]%d", &stg);
			sscanf(buffer2, "%*[^0123456789]%d", &dr);
			stg--;
			dr--;
			fscanf(in, "%d ", &legaturi[stg][dr]);
			legaturi[dr][stg] = legaturi[stg][dr];
		} else if (strcmp(command, "anulare_zbor") == 0) {
			// Se sterge o legatura intre doua insule
			fscanf(in, "%s %s", buffer, buffer2);
			int stg, dr;
			sscanf(buffer, "%*[^0123456789]%d", &stg);
			sscanf(buffer2, "%*[^0123456789]%d", &dr);
			stg--;
			dr--;
			legaturi[dr][stg] = legaturi[stg][dr] = 0;
		} else if (strcmp(command, "max_resurse") == 0) {
			// Se afiseaza toate resursele unice din arhipelag
			printeazaResurse(out, n, insule);
		} else if (strcmp(command, "max_cantitate") == 0) {
			// Se afiseaza insulele cu cantitatea maxima din resursa dorita
			fscanf(in, "%s", buffer);
			printeazaMaxResurse(out, n, insule, buffer);
		} else if (strcmp(command, "drum_zbor") == 0) {
			// Se afiseaza drumul de zbor optim dintre doua insule
			fscanf(in, "%s %s", buffer, buffer2);

			int stg, dr;
			sscanf(buffer, "%*[^0123456789]%d", &stg);
			sscanf(buffer2, "%*[^0123456789]%d", &dr);
			stg--;
			dr--;
			afiseazaDrum(n, legaturi, stg, dr, out);
		} else if (strcmp(command, "timp_zbor") == 0) {
			// Se afiseaza timpul de zbor optim dintre doua insule
			fscanf(in, "%s %s", buffer, buffer2);

			int stg, dr;
			sscanf(buffer, "%*[^0123456789]%d", &stg);
			sscanf(buffer2, "%*[^0123456789]%d", &dr);
			stg--;
			dr--;
			afiseazaTimp(n, legaturi, stg, dr, out);
		}
	}

	// Se elibereaza memoria utilizata in cadrul programului
	eliberareInsule(insule, n);
	int i;
	for (i = 0; i < n; i++) {
		free(legaturi[i]);
	}
	free(legaturi);
	return 0;
}
