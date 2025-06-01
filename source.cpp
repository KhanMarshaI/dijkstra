#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
#include <random> //cost range
#include <fstream>
using namespace std;

class MinHeap {
private:
	vector<pair<int, int>> heap; //pair: {distance, node}

	int left(int i) {
		return 2 * i + 1; // Left child index in binary heap
	}

	int right(int i) {
		return 2 * i + 2; // Right child index in binary heap
	}

	int parent(int i) {
		return (i - 1) / 2; // Parent index in binary heap
	}

	void heapifyUp(int index) {
		// Move element up until heap property is satisfied
		while (index != 0 && heap[parent(index)].first > heap[index].first) {
			swap(heap[index], heap[parent(index)]);
			index = parent(index);
		}
	}

	void heapifyDown(int index) {
		int smallest = index;
		int l = left(index);
		int r = right(index);

		// Find smallest among parent and children
		if (l < heap.size() && heap[l].first < heap[smallest].first)
			smallest = l;
		if (r < heap.size() && heap[r].first < heap[smallest].first)
			smallest = r;

		// If smallest is not the parent, swap and continue heapifying
		if (smallest != index) {
			swap(heap[index], heap[smallest]);
			heapifyDown(smallest);
		}
	}

public:
	void push(pair<int, int> val) {
		heap.push_back(val); // Add element to end
		heapifyUp(heap.size() - 1); // Restore heap property
	}

	void pop() {
		if (heap.empty()) return;
		heap[0] = heap.back(); // Move last element to root
		heap.pop_back(); // Remove last element
		heapifyDown(0); // Restore heap property from root
	}

	pair<int, int> top() {
		return heap[0]; // Return minimum element (root)
	}

	bool empty() {
		return heap.empty();
	}

};

void printGraph_toFile(vector<vector<pair<int, int>>>& adjList, int V, const string& filename) {
	ofstream outFile(filename);
	if (!outFile.is_open()) {
		cerr << "Error: Could not open file " << filename << " for writing.\n";
		return;
	}
	outFile << "Edges in the graph (u, v, w):\n";
	for (int u = 0; u < V; u++) {
		for (const auto& edge : adjList[u]) {
			int v = edge.first;
			int w = edge.second;
			if (u < v) { // Print only if u < v to avoid duplicates
				outFile << "(" << u << ", " << v << ", " << w << ")\n";
			}
		}
	}
	outFile.close();
}

void printGraph(vector<vector<pair<int, int>>>& graph, int V) {

	for (int j = 0; j < V; j++) {
		cout << j << "--> ";

		for (auto edge : graph[j]) {
			cout << "(" << edge.first << ", " << edge.second << ")"; // Print (neighbor, weight)
		}
		cout << endl;
	}
}

vector<vector<pair<int, int>>> initGraph(int V) {

	vector<vector<pair<int, int>>> adjList(V);

	int n, neighbour, weight;
	for (int i = 0; i < V; i++) {
		cout << "How many neighbours of " << i << "? ";
		cin >> n;

		for (int j = 0; j < n; j++) {
			cin >> neighbour >> weight;

			if (neighbour >= V || neighbour < 0) {
				cout << "Invalid neighbour index: " << neighbour << ". Skipping.\n";
				continue;
			}

			adjList[i].push_back({ neighbour, weight });
			adjList[neighbour].push_back({ i, weight }); // since undirected
		}

		printGraph(adjList, V); // Display current state of graph
	}

	return adjList;
}

vector<vector<pair<int, int>>> random_graph(int nodes, int edges_limit, int seed = 0) {

	vector<vector<pair<int, int>>> adjList(nodes);

	srand(seed); // Set seed for reproducible results

	vector<vector<bool>> existing(nodes, vector<bool>(nodes, false)); // Track existing edges

	vector<bool> connected(nodes, false); // Track connected vertices
	connected[0] = true; // Start with vertex 0 connected
	int connected_count = 1;
	int edge_count = 0;

	//partial spanning tree
	while (connected_count < nodes && edge_count < edges_limit) {
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes; // Find connected vertex

		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes; // Find unconnected vertex

		adjList[u].push_back({ v,0 }); // Add edge with weight 0
		adjList[v].push_back({ u,0 }); // Add reverse edge for undirected graph

		existing[u][v] = existing[v][u] = true; // Mark edge as existing
		connected[v] = true; // Mark vertex as connected

		connected_count++;
		edge_count++;

	}

	//denser graph (higher chance of all nodes being connected)
	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v) continue; // Skip self-loops
		if (u > v) swap(u, v); // Ensure u < v for consistency
		if (existing[u][v]) continue; // Skip if edge already exists

		adjList[u].push_back({ v, 0 });
		adjList[v].push_back({ u, 0 }); // Add reverse edge for undirected graph

		existing[u][v] = true; // Mark edge as existing
		edge_count++;
	}

	return adjList;
}

vector<vector<pair<int, int>>> random_cost_generator(vector<vector<pair<int, int>>> adjList, int seed = 0, int start = 0, int end = INT_MAX) {

	mt19937 rng(seed); // Mersenne Twister random number generator

	uniform_int_distribution<int> dist(start, end); // Uniform distribution for edge costs

	for (int u = 0; u < adjList.size(); u++) {
		for (int i = 0; i < adjList[u].size(); i++) {
			int v = adjList[u][i].first; // Get neighbor vertex

			if (u < v) { //Only assign for one direction to avoid duplication
				int cost = dist(rng); // Generate random cost
				adjList[u][i].second = cost; // Assign cost to edge u->v

				//Find and set the reverse edge
				for (int j = 0; j < adjList[v].size(); j++) {
					if (adjList[v][j].first == u) { // Found reverse edge v->u
						adjList[v][j].second = cost; // Set same cost for reverse edge
						break;
					}
				}
			}
		}
	}


	return adjList;
}

void dijkstra(int V, int source, vector<vector<pair<int, int>>>& adjList) {
	auto start = chrono::high_resolution_clock::now(); // Start timing

	vector<int> dist(V, INT_MAX); // Initialize all distances to infinity
	vector<int> predecessor(V, -1); // Initialize all predecessors to -1
	dist[source] = 0; // Distance to source is 0

	MinHeap minHeap;
	minHeap.push(make_pair(0, source)); //weight,vertices

	while (!minHeap.empty()) {
		pair<int, int> top = minHeap.top(); // Get minimum distance vertex
		minHeap.pop();

		int d = top.first; //dist
		int u = top.second; //vertices

		if (d > dist[u]) continue; // Skip if this is an outdated entry

		vector<pair<int, int>>& neighbours = adjList[u]; //get neighbour of current vertices

		for (size_t i = 0; i < neighbours.size(); ++i) {
			int v = neighbours[i].first; //neighbour vertices
			int weight = neighbours[i].second; // Edge weight

			if (dist[u] + weight < dist[v]) { // Relaxation step
				dist[v] = dist[u] + weight; // Update shorter distance
				predecessor[v] = u; // Update predecessor for path reconstruction
				minHeap.push(make_pair(dist[v], v)); // Add to heap with new distance
			}

		}

	}

	auto stop = chrono::high_resolution_clock::now(); // End timing
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration
	cout << "Dijsktra for " << V << " vertices took " << duration.count() << "microsec" << endl;

	cout << "Press Enter to begin path reconstruction.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input

	cout << "Shortest distances from source " << source << ":\n";
	for (int i = 0; i < V; ++i) {
		cout << "Vertex " << i << ": Distance = ";
		if (dist[i] == INT_MAX) { // Check if vertex is unreachable
			cout << "INF, Predecessor = " << predecessor[i];
		}
		else {
			cout << dist[i] << ", Predecessor = " << predecessor[i];
		}
		cout << endl;
	}

	cout << "\nShortest paths from source " << source << ":\n";
	for (int i = 0; i < V; ++i) {
		if (dist[i] == INT_MAX) continue; // Skip unreachable
		cout << "Path to " << i << ": ";
		vector<int> path; // Store path vertices
		int v = i;
		while (v != -1) { // Backtrack using predecessors
			path.push_back(v); // Add vertex to path
			v = predecessor[v]; // Move to predecessor
		}
		for (int j = path.size() - 1; j >= 0; --j) { // Print path in reverse order
			cout << path[j];
			if (j > 0) cout << " -> ";
		}
		cout << endl;
	}

}

int main() {
	cout << "Dijkstra Vector AdjList approach." << endl;
	int V, E; // Number of vertices and edge limit

	cout << "Vertices? ";
	cin >> V;

	cout << "Edge limit? ";
	cin >> E;

	int seed = 1051962; // Fixed seed for reproducible results

	auto start = chrono::high_resolution_clock::now(); // Start timing graph generation
	vector<vector<pair<int, int>>> adjList = random_graph(V, E, seed);
	auto stop = chrono::high_resolution_clock::now(); // End timing

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration

	cout << "Random generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	start = chrono::high_resolution_clock::now(); // Start timing cost generation
	adjList = random_cost_generator(adjList, seed, 1, 50); // Generate costs between 1 and 50
	stop = chrono::high_resolution_clock::now(); // End timing

	duration = chrono::duration_cast<chrono::microseconds>(stop - start); // Calculate duration
	cout << "Random cost generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	cout << "Press Enter to run dijkstra.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input

	dijkstra(V, 0, adjList); // Run Dijkstra from vertex 0

	//printGraph_toFile(adjList, V, "1000v_sparse.txt"); // Optional: save graph to file

	cout << "Press Enter to exit...";
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
	cin.get(); // Wait for user input before exit

	return 0;
}