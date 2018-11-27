#include <iostream>
#include <omp.h>

using namespace std;

void merge(int* a, int i1, int j1, int i2, int j2)
{
	int* temp = new int[(j1 + 1 - i1) * 2 + 1];    //array used for merging
	int i, j, k;
	i = i1;    //beginning of the first list
	j = i2;    //beginning of the second list
	k = 0;

	while (i <= j1 && j <= j2)    //while elements in both lists
	{
		if (a[i] < a[j])
			temp[k++] = a[i++];
		else
			temp[k++] = a[j++];
	}

	while (i <= j1)    //copy remaining elements of the first list
		temp[k++] = a[i++];

	while (j <= j2)    //copy remaining elements of the second list
		temp[k++] = a[j++];

	//Transfer elements from temp[] back to a[]
	for (i = i1, j = 0; i <= j2; i++, j++)
		a[i] = temp[j];

	delete[] temp;
}

void mergesort(int* a, int i, int j)
{
	int mid;

	if (i < j)
	{
		mid = (i + j) / 2;
		mergesort(a, i, mid);        //left recursion
		mergesort(a, mid + 1, j);    //right recursion
		merge(a, i, mid, mid + 1, j);    //merging of two sorted sub-arrays
	}
}

void mergesort_parallel(int* a, int i, int j)
{
	int mid;
#pragma omp parallel shared(mid) num_threads(2)
	if (i < j)
	{
#pragma omp sections
		{
			mid = (i + j) / 2;
#pragma omp section
			mergesort(a, i, mid);        //left recursion
#pragma omp section
			mergesort(a, mid + 1, j);
		} //right recursion
			#pragma omp barrier
			merge(a, i, mid, mid + 1, j);    //merging of two sorted sub-arrays
	}
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void odd_even_sort(int* v, int N) {
	for (int it = 0; it < N; it++) {
		if (it % 2 == 1) {
			for (int i = 2; i < N; i += 2) {
				if (v[i - 1] > v[i]) swap(&v[i - 1], &v[i]);
			}
		}
		else {
			for (int i = 1; i < N; i += 2) {
				if (v[i - 1] > v[i]) swap(&v[i - 1], &v[i]);
			}
		}
	}
}

void odd_even_sort_parallel(int* v, int N) {
	for (int it = 0; it < N; it++) {
		if (it % 2 == 1) {
			#pragma omp parallel for
			for (int i = 2; i < N; i += 2) {
				if (v[i - 1] > v[i]) swap(&v[i - 1], &v[i]);
			}
		}
		else {
			#pragma omp parallel for
			for (int i = 1; i < N; i += 2) {
				if (v[i - 1] > v[i]) swap(&v[i - 1], &v[i]);
			}
		}
	}
}

void printVector(int* v, int N) {
	cout << "Vector: " << endl;

	for (int i = 0; i < N; i++) cout << v[i] << " ";
}


void main() {
	const long N = 1 * 1e5;
	int valori[N];

	// generare de vector initial
	for (int i = 0; i < N; i++) valori[i] = N - i;

	// test odd even sort - secvential
	double tStart = omp_get_wtime();

	//odd_even_sort(valori, N);
	odd_even_sort_parallel(valori, N);

	double tFinal = omp_get_wtime();

	cout << "Durata: " << tFinal - tStart << " s" << endl;

	printVector(valori, 100);

	// test sortare cu succes
	long sum = 0;
	for (int i = 1; i < N; i++) {
		sum += (valori[i] - valori[i - 1]);
	}

	cout << "Suma diferente: " << sum << endl;

	//test sortare merge - secvential
	for (int i = 0; i < N; i++) valori[i] = N - i;

	tStart = omp_get_wtime();

	mergesort(valori, 0, N - 1);

	tFinal = omp_get_wtime();

	cout << "Durata: " << tFinal - tStart << " s" << endl;

	printVector(valori, 100);

	// test sortare cu succes
	sum = 0;
	for (int i = 1; i < N; i++) {
		sum += (valori[i] - valori[i - 1]);
	}

	cout << "Suma diferente: " << sum << endl;

	// testare merge sort paralel
	for (int i = 0; i < N; i++) valori[i] = N - i;

	tStart = omp_get_wtime();

	mergesort_parallel(valori, 0, N - 1);

	tFinal = omp_get_wtime();

	cout << "Durata: " << tFinal - tStart << " s" << endl;

	printVector(valori, 100);

	// test sortare cu succes
	sum = 0;
	for (int i = 1; i < N; i++) {
		sum += (valori[i] - valori[i - 1]);
	}

	cout << "Suma diferente: " << sum << endl;
}