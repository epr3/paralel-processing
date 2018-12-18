#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <omp.h>

#define NO_OF_CHARS 256 

using namespace std;

void multiplicareFisier(const char* numeFisier, int factorMultiplicare) {
	FILE *pf = fopen(numeFisier, "r");
	fseek(pf, 0, SEEK_END);
	unsigned int dimensiune = ftell(pf);
	char* continut = new char[dimensiune];
	fseek(pf, 0, SEEK_SET);
	fread(continut, dimensiune, 1, pf);

	FILE *copie = fopen("date.txt", "w");
	for (int i = 0; i < factorMultiplicare; i++)
		fwrite(continut, dimensiune, 1, copie);
	fclose(pf);
	fclose(copie);
}

char* getContinutText(const char* numeFisier) {
	FILE *pf = fopen(numeFisier, "r");
	fseek(pf, 0, SEEK_END);
	unsigned int dimensiune = ftell(pf);
	char* continut = new char[dimensiune + 1];
	fseek(pf, 0, SEEK_SET);
	fread(continut, dimensiune, 1, pf);
	fclose(pf);
	continut[dimensiune] = '\0';
	return continut;
}

// cautare naiva de subsiruri
int cautareNaivaParalela(const char* text, const char* pattern) {
	int contor = 0;
	int n = strlen(text);
	int m = strlen(pattern);

#pragma omp parallel for reduction(+:contor) num_threads(4)
	for (int i = 0; i < n; i++) {
		bool gasit = true;

		for (int j = 0; j < m; j++) {
			if (text[i + j] != pattern[j]) {
				gasit = false;
				break;
			}
		}
		if (gasit) contor += 1;
	}
	return contor;
}

void badCharHeuristic(char *str, int size,
	int badchar[NO_OF_CHARS])
{
	int i;

	// Initialize all occurrences as -1 
	for (i = 0; i < NO_OF_CHARS; i++)
		badchar[i] = -1;

	// Fill the actual value of last occurrence  
	// of a character 
	for (i = 0; i < size; i++)
		badchar[(int)str[i]] = i;
}

int boyerMooreSearch(char *txt, char *pat)
{
	int contor = 0;
	int m = strlen(pat);
	int n = strlen(txt);

	int badchar[NO_OF_CHARS];
	badCharHeuristic(pat, m, badchar);

	int s = 0;
	while (s <= (n - m))
	{
		int j = m - 1;
		while (j >= 0 && pat[j] == txt[s + j])
			j--;
		if (j < 0)
		{
			// printf("\n pattern occurs at shift = %d", s);
			contor += 1;
			s += (s + m < n) ? m - badchar[txt[s + m]] : 1;

		}

		else
		{
			int a = 1;
			int b = j - badchar[txt[s + j]];
			s += a > b ? a : b;
		}
	}
	return contor;
}

int boyerMooreParallelSearch(char* text, char* pat) {
	int NUM_THREADS = 4;
	int contor = 0;
	int chunkSize = strlen(text) / NUM_THREADS;
	int chunkRemainder = strlen(text) % NUM_THREADS;
	int m = strlen(pat);
	char* chunk;

#pragma omp parallel for reduction(+:contor) num_threads(NUM_THREADS)
	for (int i = 0; i < NUM_THREADS; i++) {
		if (i == 0) {
			chunk = new char[chunkSize + 1];
			memcpy(chunk, (char*)text, chunkSize);
			chunk[chunkSize] = '\0';
		}
		else {
			chunk = new char[chunkSize + 1 + m];
			memcpy(chunk, (char*)text + i * chunkSize - m, chunkSize + m);
			chunk[chunkSize + m] = '\0';
		}
		contor += boyerMooreSearch(chunk, pat);
	}
	return contor;
}

void main() {
	// multiplicareFisier("dickens.txt", 200);
	char* text = getContinutText("date.txt");
	int n = strlen(text);
	cout << endl << "Dimensiune text: " << n;

	char pattern[100];
	cout << endl << "Cuvantul cautat este: ";
	cin >> pattern;

	// test cautare naiva
	double tStart = omp_get_wtime();
	int rezultat = cautareNaivaParalela(text, pattern);
	double tFinal = omp_get_wtime();

	cout << endl << "Rezultat:" << rezultat << " in " << tFinal - tStart << " secunde";

	// boyer moore secvential
	tStart = omp_get_wtime();
	rezultat = boyerMooreSearch(text, pattern);
	tFinal = omp_get_wtime();

	cout << endl << "Rezultat:" << rezultat << " in " << tFinal - tStart << " secunde";

	// boyer moore paralel
	tStart = omp_get_wtime();
	rezultat = boyerMooreParallelSearch(text, pattern);
	tFinal = omp_get_wtime();

	cout << endl << "Rezultat:" << rezultat << " in " << tFinal - tStart << " secunde";

}