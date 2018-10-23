#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>
#include<set>

using namespace std;

static const int MAX_LIMIT = 500000; // 10^5

static set<int> prime;

static int nrPrime = 0;

mutex m;

// varianta secventiala - genereaza toate nr prime intre 1 si limita
void generarePrime(set<int> &prime, int limitaInferioara, int limitaSuperioara) {
	for (int i = limitaInferioara; i < limitaSuperioara; i++) {
		bool estePrim = true;
		for (int j = 2; j <= i / 2; j++) {
			if (i%j == 0) {
				estePrim = false;
				break;
			}
		}
		if (estePrim) prime.insert(i);
	}
}

static int NR_CURENT = 0;
static int NR_CURENT2 = 1;

void verificaPrim(set<int> &prime, int &nrCurent, int limitaSuperioara) {
	while (nrCurent < limitaSuperioara) {
		nrCurent += 1;
		int valoare = nrCurent;
			bool estePrim = true;
			for (int i = 2; i <= limitaSuperioara / 2; i++) {
				if (valoare % i == 0) {
					estePrim = false;
					break;
				}
			}
			if (estePrim) prime.insert(valoare);
	}
}

int main() {
	chrono::system_clock::time_point tStart = chrono::system_clock::now();

	//thread t1(generarePrime, ref(prime), 2, MAX_LIMIT / 2);
	//thread t2(generarePrime, ref(prime), MAX_LIMIT / 2, MAX_LIMIT);

	//t1.join();
	//t2.join();

	//generarePrime(prime, MAX_LIMIT / 2, MAX_LIMIT );

	thread t1(verificaPrim, ref(prime), ref(NR_CURENT), MAX_LIMIT);
	thread t2(verificaPrim, ref(prime), ref(NR_CURENT2), MAX_LIMIT);

	t1.join();
	t2.join();

	chrono::system_clock::time_point tFinal = chrono::system_clock::now();

	cout << endl << "Numere prime: " << prime.size() << endl 
		<< "Timp procesare(ms): " << chrono::duration_cast<chrono::milliseconds>(tFinal - tStart).count();

	return 0;
}
