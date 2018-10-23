#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>

using namespace std;

mutex m;

int* generateArray(int N) {
	int* values = new int[N];
	for (int i = 0; i < N; i++) {
		values[i] = i % 100;
	}

	return values;
}

int getMinim(int* values, int iStart, int iFinal) {
	int minim = values[iStart];
	for (int i = iStart; i < iFinal; i++) {
		if (minim > values[i]) {
			minim = values[i];
		}
	}
	return minim;
}

double getAverage(int* values, int iStart, int iFinal) {
	double average = 0;
	for (int i = iStart; i < iFinal; i++) {
		average += values[i];
	}

	return average / (iFinal - iStart);
}

void getEvenCount(int* values, int iStart, int iFinal, int& count) {
	for (int i = iStart; i < iFinal; i++) {
		if (values[i] % 2 == 0) {
			{
				m.lock();
				count += 1;
				m.unlock();
			}
		}
	}
}

//functie care poate fi executata pe un alt thread
void call_from_thread() {
	std::cout << endl << "Hello World from another thread";
}

//functie cu parametri
void call_from_thread_2(int tid) {
	std::cout << "\n Hello World from another thread" << tid;
}

int main() {
	cout << endl << "Start main";
	
	//definire thread
	std::thread t1(call_from_thread);

	//lansare thread
	t1.join();

	//definire grup de thread-uri
	const int N_THREADS = 4;
	thread* list = new thread[N_THREADS];
	for (int i = 0; i < N_THREADS; i++) {
		list[i] = thread(call_from_thread_2, i);
	}

	for (int i = 0; i < N_THREADS; i++) {
		list[i].join();
	} 

	cout << endl << "***************** TEST MIN, MEDIE SI NUMERE PARE SECVENTIAL **********************";

	const int N = 300000000;
	int* values = generateArray(N);
	int nrNumerePare = 0;
	
	std::chrono::system_clock::time_point tStart = std::chrono::system_clock::now();

	int min = getMinim(values, 0, N);
	double average = getAverage(values, 0, N);
	getEvenCount(values, 0, N, nrNumerePare);

	std::chrono::system_clock::time_point tFinal = std::chrono::system_clock::now();

	cout << endl << "Min = " << min << " Average = " << average << " Even numbers = " << nrNumerePare << " Time = " << std::chrono::duration_cast<chrono::milliseconds>(tFinal - tStart).count();

	/*
		2 thread-uri pt min si average
	*/

	cout << endl << "***************** TEST MIN, MEDIE SI NUMERE PARE PARALEL **********************";

	tStart = std::chrono::system_clock::now();
	thread tMinim(getMinim, values, 0, N);
	thread tAverage(getAverage, values, 0, N);

	/*
		2 thread-uri pt numar elemente pare
	*/
	nrNumerePare = 0;

	unsigned int nrNumerePare1 = 0;
	unsigned int nrNumerePare2 = 0;
	thread tPare1(getEvenCount, values, 0, N / 2, ref(nrNumerePare1));
	thread tPare2(getEvenCount, values, N / 2, N, ref(nrNumerePare2));

	tPare1.join();
	tPare2.join();
	
	tMinim.join();
	tAverage.join();

	tFinal = std::chrono::system_clock::now();

	cout << endl << "Nr elemente pare = " << nrNumerePare1 + nrNumerePare2;
	cout << endl << "Parallel Time = " << std::chrono::duration_cast<chrono::milliseconds>(tFinal - tStart).count();

	cout << endl << "End main";
}