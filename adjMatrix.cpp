#include <iostream>
#include <random>
#include <climits>
#include <chrono>
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

void deleteGraph(bool** graph, int V) {
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

int** random_graph(int nodes, int edges_limit, int seed = 0) {
	int** graph = new int* [nodes]; //VxV matrix
	//each idx points to an array of int (row/neighbor of current vertex)

	for (int i = 0; i < nodes; i++) {
		graph[i] = new int[nodes]; //point to an array of integers (row)
		for (int j = 0; j < nodes; j++) {
			graph[i][j] = -1; //sentinel value (no connection)
		}
	}

	//for tracking generated edges
	bool** existing = new bool*[nodes];
	for (int i = 0; i < nodes; i++) {
		existing[i] = new bool[nodes];
		for (int j = 0; j < nodes; j++) {
			existing[i][j] = false;
		}
	}

	bool* connected = new bool[nodes];
	for (int i = 0; i < nodes; i++) {
		connected[i] = false;
	}

	srand(seed);
	
	//spanning tree
	connected[0] = true;
	int connected_count = 1;
	int edge_count = 0;

	while (connected_count < nodes && edge_count < edges_limit) {
		//connected u vertex
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes;

		//unconnected v vertex
		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes;

		graph[u][v] = graph[v][u] = 0;
		existing[u][v] = existing[v][u] = true;
		connected[v] = true;

		connected_count++;
		edge_count++;
	}

	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v || existing[u][v]) continue;
		if (u > v) swap(u, v);

		graph[u][v] = graph[v][u] = 0;
		existing[u][v] = existing[v][u] = 0;
		edge_count++;
	}

	deleteGraph(existing, nodes);
	delete[] connected;

	return graph;
}

int main() {
	int V;
	cout << "Vertices? ";
	cin >> V;

	int edges;
	cout << "How many edges do you want to add? ";
	cin >> edges;

	int seed = 1051962;

	auto start = chrono::high_resolution_clock::now();
	int** graph = random_graph(V, edges, seed);
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Random generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	//populateGraph(graph, V, edges);
	//printGraph(graph, V);
	deleteGraph(graph, V);
	
	return 0;
}