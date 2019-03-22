#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>


using namespace std;

class Task {
public:
	int time; //ile trwa zadanie
	int ex; // ktore to zadanie
	int cmax_prawy;
	int cmax_lewy;
	vector<int> _order;
	Task() {
		time = 0;
		ex = 0;
		cmax_prawy = 0;
		cmax_lewy = 0;
	}
	Task(int t, int e, int cp, int cl) {
		time = t;
		ex = e;
		cmax_prawy = cp;
		cmax_lewy = cl;

	}
};
static vector<vector<Task>> graph;
void delete_task(vector<vector<Task>> &macierz, int nr) {
	int size = macierz.size() - 1; //3
	macierz[nr].clear();
	for (int i = nr; i < size; i++)
		macierz[i].swap(macierz[i + 1]);
	macierz.resize(size);
}

vector<vector<Task>> read_data(int &number_of_ex, int &n_m) {
	ifstream data("data.txt");
	data >> number_of_ex;
	data >> n_m;
	vector<vector<Task>> macierz(number_of_ex, vector<Task>(n_m));
	for (int j = 0; j < number_of_ex; j++) {
		for (int i = 0; i < n_m; i++) {
			data >> macierz[j][i].time;
			macierz[j][i].ex = j + 1;
		}
	}
	data.close();
	return macierz;
}

vector<int> sort(vector<int> sum_time) { //sortuje zadania w kolejnosc najdluzszego czasu wykonywania na wszytskich maszynach
	vector<int> order(sum_time.size());
	vector<int>::iterator it;
	int b = 0;
	for (int i = 0; i < sum_time.size(); i++) {
		it = max_element(sum_time.begin(), sum_time.end());
		order[b] = distance(sum_time.begin(), it) + 1;  //kolejnosc zadan od jedynki
		*it = 0;
		b++;
	}
	return order;
}

vector<vector<Task>> cmax(vector<vector<Task>> macierz, vector<int> order) { //oblicza cmax dla kazdego zadania i kazdej maszyny i zwraca cmax dla ostatniego zadania na ostatniej maszynie
	int n_ex = order.size() + 1;
	int n_m = macierz[0].size() + 1;
	vector<vector<Task>> Cm(n_ex + 1, vector<Task>(n_m + 1));
	vector < vector<Task>> Cm_pom(order.size(), vector<Task>(n_m - 1));


	for (int i = 0; i <= n_m; i++) {
		for (int j = 0; j <= n_ex; j++) {
			if (i == 0 || i == n_m) {
				Cm[j][0].cmax_prawy = 0; Cm[j][0].cmax_lewy = 0;
				Cm[j][n_m].cmax_prawy = 0; Cm[j][n_m].cmax_lewy = 0;
			}
			else if (j == 0 || j == n_ex) {
				Cm[0][i].cmax_prawy = 0; Cm[0][i].cmax_lewy = 0;
				Cm[n_ex][i].cmax_prawy = 0; Cm[n_ex][i].cmax_lewy = 0;
			}
			else {
				Cm[j][i].cmax_prawy = max(Cm[j][i - 1].cmax_prawy, Cm[j - 1][i].cmax_prawy) + macierz[order[j - 1] - 1][i - 1].time;
				Cm[j][i]._order = order;
			}
		}
	}

	for (int i = n_m - 1; i >= 1; i--) {
		for (int j = n_ex - 1; j >= 1; j--) {
			Cm[j][i].cmax_lewy = max(Cm[j][i + 1].cmax_lewy, Cm[j + 1][i].cmax_lewy) + macierz[order[j - 1] - 1][i - 1].time;
		}
	}
	for (int j = 0; j < Cm_pom.size(); j++)
		for (int i = 0; i < Cm_pom[0].size(); i++)
			Cm_pom[j][i] = Cm[j + 1][i + 1];
	return Cm_pom;
}
Task min_element(vector<Task> cmaxy, int size) { //wyszukuje najmniejszy cmax z vectora<param> i zwraca dla niego order
	int min = 999999999;
	vector<int> order(size + 1);
	Task min_task;
	for (int i = 0; i < size + 1; i++) {
		if (cmaxy[i].cmax_prawy < min) {
			min = cmaxy[i].cmax_prawy;
			min_task = cmaxy[i];
			for (int j = 0; j < size + 1; j++)
				order[j] = cmaxy[i]._order[j];
		}
	}
	return min_task;
}
Task max_element(vector<Task> cmaxy, int size) { //wyszukuje najmniejszy cmax z vectora<param> i zwraca dla niego order
	int max = -1;
	Task max_task;
	for (int i = 0; i < size; i++) {
		if (cmaxy[i].cmax_prawy > max) {
			max = cmaxy[i].cmax_prawy;
			max_task = cmaxy[i];
		}
	}
	return max_task;
}
Task cmax_graph(vector<vector<Task>> macierz, vector<int> order, int position) { //oblicza cmax dla kazdego zadania i kazdej maszyny i zwraca cmax dla ostatniego zadania na ostatniej maszynie
	int n_m = macierz[0].size();
	vector<Task> graph_pom(n_m);
	vector<Task> cmaxy(n_m);
	for (int i = 0; i < n_m; i++) {
		if (i > 0)
			graph_pom[i].cmax_prawy = max(graph[position - 1][i].cmax_prawy, graph_pom[i - 1].cmax_prawy) + macierz[order[position] - 1][i].time;
		else graph_pom[i].cmax_prawy = max(graph[position - 1][i].cmax_prawy, 0) + macierz[order[position] - 1][i].time;
		cmaxy[i].cmax_prawy = graph_pom[i].cmax_prawy + graph[position][i].cmax_lewy;
		cmaxy[i]._order = order;
	}
	Task cmax;
	cmax = max_element(cmaxy, cmaxy.size());
	return cmax;
}

void add_element(vector<int> &order, int element, int position) { //dodaje element do vectora na okreslonej pozycji
	vector<int> pom_order = order;
	order[position] = element;
	for (int i = position + 1; i < order.size(); i++) {
		order[i] = pom_order[i - 1];
	}
}

vector<int> sort_cmax(vector<vector<Task>> macierz, vector<int> order) { //jak zrobimy akceleracje to tylko zmniejszy o jedno obliczenie w kazdej iteracji, bo obliczony cmax mozemy wykorzystac tylko kiedy order obliczonego cmax jest na poczatku
	vector<int> order_pom = { order[0] };
	vector<Task> cmaxy(order.size());
	for (int i = 0; i < order.size(); i++) {
		vector<int> order_pompom = order_pom;
		for (int j = 0; j <= i; j++) {	//petla po pozycji
			order_pom = order_pompom;
			order_pom.resize(i + 1);
			add_element(order_pom, order[i], j);
			if (j >= 1 && j < i) {
				cmaxy[j] = cmax_graph(macierz, order_pom, j);
				cmaxy[j]._order = order_pom;
			}
			else {
				cmaxy[j] = cmax(macierz, order_pom)[order_pom.size() - 1][macierz[0].size() - 1];
				cmaxy[j]._order = order_pom;
			}
		}
		order_pom = min_element(cmaxy, i)._order;
		graph = cmax(macierz, order_pom);
	}
	return order_pom;
}
int main()
{
	chrono::time_point< std::chrono::system_clock> start = std::chrono::system_clock::now();
	int number_of_ex, n_m;
	vector<vector<Task>> macierz = read_data(number_of_ex, n_m);
	vector<int> sum_time(number_of_ex);  //suma czasow wykonania danego zadania przez wszystkie maszyny
	for (int j = 0; j < number_of_ex; j++)
		for (int i = 0; i < n_m; i++) sum_time[j] += macierz[j][i].time;

	vector<int> order = sort(sum_time);


	vector<int> order_naj = sort_cmax(macierz, order);
	cout << "min cmax: " << cmax(macierz, order_naj)[order_naj.size() - 1][macierz[0].size() - 1].cmax_prawy << endl;
	cout << "order_naj = ";
	for (int i = 0; i < order_naj.size(); i++)
		cout << order_naj[i] << " ";
	cout << endl;

	chrono::time_point< std::chrono::system_clock> end = std::chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "czas trwania: " << elapsed_seconds.count() << "s\n";
	return 0;
}

