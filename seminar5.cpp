#include <iostream>
#include <omp.h>

using namespace std;

void main() {
	const int N = 100000;
	int valori[N];

	for (int i = 0; i < N; i++) {
		valori[i] = 1;
	}

	long long suma = 0;

	int nrT = 4;
	long long** rezultate = new long long*[nrT];

	for (int i = 0; i < nrT; i++) {
		rezultate[i] = new long long[1];
		rezultate[i][0] = 0;
	}

	double tStart = omp_get_wtime();


#pragma omp parallel shared(rezultate) num_threads(nrT)
	{
		int id = omp_get_thread_num();
		int nrFire = omp_get_num_threads();

		int limitaSuperioara = N * (id + 1) / nrFire;

		for (int i = 0; i < N/nrFire; i++) {
			for (int j = 0; j < N; j++) {
				rezultate[id][0] += valori[i];
			}
		}
	}
	

	double tFinal = omp_get_wtime();

	cout << endl << "Durata este: " << tFinal - tStart;
	for (int i = 0; i < nrT; i++) {
		suma += rezultate[i][0];
	}
	cout << endl << "suma este " << suma;
}
