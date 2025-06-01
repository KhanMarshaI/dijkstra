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
		delete[] graph[i]; // Delete each row
	}

	delete[] graph; // Delete array of pointers
}

void deleteGraph(bool** graph, int V) {
	for (int i = 0; i < V; i++) {
		delete[] graph[i]; // Delete each row
	}

	delete[] graph; // Delete array of pointers
}

void printGraph(int** graph, int V) {
	//print
	for (int i = 0; i < V; i++) {
		for (int j = 0; j < V; j++) {
			cout << '(' << i << ',' << j << ',' << graph[i][j] << "),"; // Print (source, destination, weight)
		}
		cout << endl;
	}
}

void populateGraph(int** graph, int V, int edges) {
	cout << "Insert edge u v w: ";
	int u, v, w;
	for (int i = 0; i < edges; i++) {
		cin >> u >> v >> w;
		graph[u][v] = w; // Add directed edge with weight
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
	bool** existing = new bool* [nodes];
	for (int i = 0; i < nodes; i++) {
		existing[i] = new bool[nodes];
		for (int j = 0; j < nodes; j++) {
			existing[i][j] = false; // Initialize to no existing edges
		}
	}

	bool* connected = new bool[nodes];
	for (int i = 0; i < nodes; i++) {
		connected[i] = false; // Initialize all vertices as unconnected
	}

	srand(seed); // Set seed for reproducible results

	//spanning tree
	connected[0] = true; // Start with vertex 0 connected
	int connected_count = 1;
	int edge_count = 0;

	while (connected_count < nodes && edge_count < edges_limit) {
		//connected u vertex
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes; // Find connected vertex

		//unconnected v vertex
		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes; // Find unconnected vertex

		graph[u][v] = graph[v][u] = 0; // Add bidirectional edge with weight 0
		existing[u][v] = existing[v][u] = true; // Mark edge as existing
		connected[v] = true; // Mark vertex as connected

		connected_count++;
		edge_count++;
	}

	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v || existing[u][v]) continue; // Skip self-loops and existing edges
		if (u > v) swap(u, v); // Ensure u < v for consistency

		graph[u][v] = graph[v][u] = 0; // Add bidirectional edge with weight 0
		existing[u][v] = existing[v][u] = 0; // Should be true, likely a bug
		edge_count++;
	}

	deleteGraph(existing, nodes); // Clean up tracking matrix
	delete[] connected; // Clean up connected array

	return graph;
}

int** random_cost_generation(int** graph, int V, int seed = 0, int start = 0, int end = INT_MAX) {
	mt19937 rng(seed); // Mersenne Twister random number generator

	uniform_int_distribution<int> dist(start, end); // Uniform distribution for edge costs

	for (int i = 0; i < V; i++) {
		for (int j = i + 1; j < V; j++) { // Only check upper triangle to avoid duplicates

			if (graph[i][j] == 0) { // If edge exists (weight 0)
				int cost = dist(rng); // Generate random cost
				graph[i][j] = cost; // Set cost for i->j
				graph[j][i] = cost; // Set same cost for j->i (undirected)
			}

		}
	}

	return graph;
}

struct Node {
	int vertex;
	int dist; // Distance from source
};

class MinHeap {
private:
	Node* heap; // Array to store heap elements
	int capacity; // Maximum capacity of heap
	int size; // Current number of elements

	int parent(int i) { return (i - 1) / 2; } // Parent index in binary heap
	int left(int i) { return 2 * i + 1; } // Left child index
	int right(int i) { return 2 * i + 2; } // Right child index

	void swap(Node& a, Node& b) {
		Node temp = a;
		a = b;
		b = temp;
	}

	void heapifyUp(int idx) {
		// Move element up until heap property is satisfied
		while (idx != 0 && heap[parent(idx)].dist > heap[idx].dist) {
			swap(heap[idx], heap[parent(idx)]);
			idx = parent(idx);
		}
	}

	void heapifyDown(int idx) {
		int smallest = idx;
		int l = left(idx);
		int r = right(idx);

		// Find smallest among parent and children
		if (l < size && heap[l].dist < heap[smallest].dist) smallest = l;
		if (r < size && heap[l].dist < heap[smallest].dist) smallest = r; // Bug: should be heap[r]

		// If smallest is not the parent, swap and continue heapifying
		if (smallest != idx) {
			swap(heap[idx], heap[smallest]);
			heapifyDown(smallest);
		}
	}

public:
	MinHeap(int c) {
		capacity = c;
		size = 0;
		heap = new Node[capacity]; // Allocate heap array
	}

	~MinHeap() {
		delete[] heap; // Destructor to free memory
	}

	bool empty() {
		return size == 0;
	}

	void push(int v, int d) {
		if (size == capacity) return; // Heap is full

		heap[size] = { v,d }; // Add new element at end
		heapifyUp(size); // Restore heap property
		size++;
	}

	Node pop() {
		if (empty()) return { -1,-1 }; // Return invalid node if empty

		Node top = heap[0]; // Store minimum element
		heap[0] = heap[size - 1]; // Move last element to root
		size--;
		heapifyDown(0); // Restore heap property from root

		return top;
	}
};

void dijkstra(int V, int source, int** graph) {
	auto start = chrono::high_resolution_clock::now(); // Start timing

	int* dist = new int[V]; // Distance array
	bool* visited = new bool[V]; // Visited array
	int* predecessor = new int[V]; // Predecessor array for path reconstruction

	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX; // Initialize distances to infinity
		visited[i] = false; // Initialize all vertices as unvisited
		predecessor[i] = -1; // Initialize predecessors to -1
	}

	dist[source] = 0; // Distance to source is 0

	MinHeap minHeap(V * V); // Create heap with large capacity
	minHeap.push(source, 0); // Add source to heap

	while (!minHeap.empty()) {
		Node top = minHeap.pop(); // Get minimum distance vertex

		int u = top.vertex;

		if (visited[u]) continue; // Skip if already visited
		visited[u] = true; // Mark as visited

		int d = top.dist;

		int* neighbours = graph[u]; // Get adjacency row for vertex u

		for (int v = 0; v < V; v++) { // Check all possible neighbors
			int weight = neighbours[v]; // Get edge weight

			if (graph[u][v] != -1 && !visited[v]) { // If edge exists and vertex not visited

				if (dist[u] + weight < dist[v]) { // Relaxation step
					dist[v] = dist[u] + weight; // Update shorter distance
					predecessor[v] = u; // Update predecessor for path reconstruction
					minHeap.push(v, dist[v]); // Add to heap with new distance
				}

			}


		}

	}

	auto stop = chrono::high_resolution_clock::now(); // End timing
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration
	cout << "Dijkstra on " << V << " vertices and " << " took " << duration.count() << " microsec" << endl;

	cout << "Press Enter to begin path reconstruction.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input

	for (int i = 0; i < V; ++i) {
		cout << "Vertex " << i << ": ";
		if (dist[i] == INT_MAX) // Check if vertex is unreachable
			cout << "Distance = INF, Predecessor = " << predecessor[i] << "\n";
		else
			cout << "Distance = " << dist[i] << ", Predecessor = " << predecessor[i] << "\n";
	}

	for (int i = 0; i < V; ++i) {
		if (dist[i] == INT_MAX) continue; // Skip unreachable vertices
		cout << "Path to " << i << ": ";
		int v = i;
		vector<int> path; // Store path vertices
		while (v != -1) { // Backtrack using predecessors
			path.push_back(v); // Add vertex to path
			v = predecessor[v]; // Move to predecessor
		}
		for (int j = path.size() - 1; j >= 0; --j) { // Print path in reverse order
			cout << path[j];
			if (j > 0) cout << " -> ";
		}
		cout << "\n";
	}

	delete[] dist; // Clean up distance array
	delete[] visited; // Clean up visited array
	delete[] predecessor; // Clean up predecessor array
}

int main() {
	cout << "Dijkstra Adjacency Matrix Implementation." << endl;;

	int V;
	cout << "Vertices? ";
	cin >> V;

	int edges;
	cout << "How many edges do you want to add? ";
	cin >> edges;

	int seed = 1051962; // Fixed seed for reproducible results

	auto start = chrono::high_resolution_clock::now(); // Start timing graph generation
	int** graph = random_graph(V, edges, seed);
	auto stop = chrono::high_resolution_clock::now(); // End timing

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration

	cout << "Random generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	start = chrono::high_resolution_clock::now(); // Start timing cost generation
	graph = random_cost_generation(graph, V, seed, 1, 50); // Generate costs between 1 and 50
	stop = chrono::high_resolution_clock::now(); // End timing

	duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration

	cout << "Random cost generation of " << edges << " edges with seed " << seed << " took " << duration.count() << "microsec" << endl;

	cout << "Press Enter to run dijkstra.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input

	dijkstra(V, 0, graph); // Run Dijkstra from vertex 0

	//populateGraph(graph, V, edges); // Optional: manual graph population
	//printGraph(graph, V); // Optional: print graph matrix
	deleteGraph(graph, V); // Clean up graph memory

	cout << "Press Enter to exit...";
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input before exit

	return 0;
}