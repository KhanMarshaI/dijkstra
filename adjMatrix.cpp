#include <iostream>
using namespace std;

int** initGraph(int V) {
	int** graph = new int* [V]; //VxV matrix
	//each idx points to an array of int (row/neighbor of current vertex)

	for (int i = 0; i < V; i++) { 
		graph[i] = new int[V]; //point to an array of integers (row)
	}

	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			graph[i][j] = 0; //init each row with 0
		}
	}

	return graph;
}

void deleteGraph(int** graph, int V) {
	for (int i = 0; i < V; i++) {
		delete[] graph[i];
	}

	delete[] graph;
}

void printGraph(int** graph, int V) {
	//print
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			cout << '(' << i << ',' << j << ',' << graph[i][j] << "),";
		}
		cout << endl;
	}
}

void populateGraph(int** graph, int V, int edges) {
	cout << "Insert edge u v w: ";
	int u, v, w;
	for (int i = 0; i < edges; i++) {
		cin >> u >> v >> w;
		graph[u][v] = w;
	}
}

int main() {
	int V;
	cout << "Vertices? ";
	cin >> V;

	int** graph = initGraph(V);

	int edges;
	cout << "How many edges do you want to add? ";
	cin >> edges;

	populateGraph(graph, V, edges);
	printGraph(graph, V);
	deleteGraph(graph, V);
	
	return 0;
}