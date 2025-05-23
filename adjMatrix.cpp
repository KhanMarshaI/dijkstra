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

int** random_cost_generation(int** graph, int V, int seed=0,int start=0, int end=INT_MAX) {
	mt19937 rng(seed);

	uniform_int_distribution<int> dist(start, end);

	for (int i = 0; i < V; i++) {
		for (int j = i + 1; j < V; j++) {

			if (graph[i][j] == 0) {
				int cost = dist(rng);
				graph[i][j] = cost;
				graph[j][i] = cost;
			}

		}
	}

	return graph;
}

struct Node {
	int vertex;
	int dist;
};

class MinHeap {
private:
	Node* heap;
	int capacity;
	int size;

	int parent(int i) { return (i - 1) / 2; }
	int left(int i) { return 2 * i + 1; }
	int right(int i) { return 2 * i + 2; }

	void swap(Node& a, Node& b) {
		Node temp = a;
		a = b;
		b = temp;
	}

	void heapifyUp(int idx) {
		while (idx != 0 && heap[parent(idx)].dist > heap[idx].dist) {
			swap(heap[idx], heap[parent(idx)]);
			idx = parent(idx);
		}
	}

	void heapifyDown(int idx) {
		int smallest = idx;
		int l = left(idx);
		int r = right(idx);

		if (l < size && heap[l].dist < heap[smallest].dist) smallest = l;
		if (r < size && heap[l].dist < heap[smallest].dist) smallest = r;

		if (smallest != idx) {
			swap(heap[idx], heap[smallest]);
			heapifyDown(smallest);
		}
	}

public:
	MinHeap(int c) {
		capacity = c;
		size = 0;
		heap = new Node[capacity];
	}

	~MinHeap() {
		delete[] heap;
	}

	bool empty() {
		return size == 0;
	}

	void push(int v, int d) {
		if (size == capacity) return;

		heap[size] = { v,d };
		heapifyUp(size);
		size++;
	}

	Node pop() {
		if (empty()) return { -1,-1 };

		Node top = heap[0];
		heap[0] = heap[size - 1];
		size--;
		heapifyDown(0);

		return top;
	}
};

void dijkstra(int V, int source, int** graph) {
	auto start = chrono::high_resolution_clock::now();

	int* dist = new int[V];
	bool* visited = new bool[V];
	int* predecessor = new int[V];

	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		visited[i] = false;
		predecessor[i] = -1;
	}

	dist[source] = 0;

	MinHeap minHeap(V * V);
	minHeap.push(source, 0);

	while (!minHeap.empty()) {
		Node top = minHeap.pop();

		int u = top.vertex;
		
		if (visited[u]) continue;
		visited[u] = true;

		int d = top.dist;

		int* neighbours = graph[u];

		for (int v = 0; v < V; v++) {
			int weight = neighbours[v];

			if (graph[u][v] != -1 && !visited[v]) {
				
				if (dist[u] + weight < dist[v]) {
					dist[v] = dist[u] + weight;
					predecessor[v] = u;
					minHeap.push(v, dist[v]);
				}

			}


		}

	}

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << "Dijkstra on " << V << " vertices and "  << " took " << duration.count() << " microsec" << endl;


	for (int i = 0; i < V; ++i) {
		cout << "Vertex " << i << ": ";
		if (dist[i] == INT_MAX)
			cout << "Distance = INF, Predecessor = " << predecessor[i] << "\n";
		else
			cout << "Distance = " << dist[i] << ", Predecessor = " << predecessor[i] << "\n";
	}

	for (int i = 0; i < V; ++i) {
		if (dist[i] == INT_MAX) continue; // Skip unreachable vertices
		cout << "Path to " << i << ": ";
		int v = i;
		vector<int> path;
		while (v != -1) {
			path.push_back(v);
			v = predecessor[v];
		}
		for (int j = path.size() - 1; j >= 0; --j) {
			cout << path[j];
			if (j > 0) cout << " -> ";
		}
		cout << "\n";
	}

	delete[] dist;
	delete[] visited;
	delete[] predecessor;
}

int main() {
	cout << "Dijkstra Adjacency Matrix Implementation." << endl;;

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

	start = chrono::high_resolution_clock::now();
	graph = random_cost_generation(graph, V, seed, 1, 50);
	stop = chrono::high_resolution_clock::now();

	duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Random cost generation of " << edges << " edges with seed " << seed << " took " << duration.count() << "microsec" << endl;

	dijkstra(V, 0, graph);

	//populateGraph(graph, V, edges);
	//printGraph(graph, V);
	deleteGraph(graph, V);
	
	cout << "Press Enter to exit...";
	cin.get();

	return 0;
}