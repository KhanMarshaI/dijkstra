#include <iostream>
#include <vector>
#include <chrono>
#include <climits>
#include <random>
using namespace std;

// Custom MinHeap implementation for Dijkstra's priority queue
// Stores pairs of {distance, node} to efficiently extract minimum distance nodes
class MinHeap {
private:
	vector<pair<int, int>> heap; //pair: {distance, node}

	// Helper functions to navigate the binary heap structure
	int left(int i) {
		return 2 * i + 1;
	}

	int right(int i) {
		return 2 * i + 2;
	}

	int parent(int i) {
		return (i - 1) / 2;
	}

	// Maintains min-heap property by moving element up the tree
	void heapifyUp(int index) {
		while (index != 0 && heap[parent(index)].first > heap[index].first) {
			swap(heap[index], heap[parent(index)]);
			index = parent(index);
		}
	}

	// Maintains min-heap property by moving element down the tree
	void heapifyDown(int index) {
		int smallest = index;
		int l = left(index);
		int r = right(index);

		// Find the smallest among parent and its children
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
	// Insert a new element and maintain heap property
	void push(pair<int, int> val) {
		heap.push_back(val);
		heapifyUp(heap.size() - 1);
	}

	// Remove the minimum element (root) and restore heap property
	void pop() {
		if (heap.empty()) return;
		heap[0] = heap.back();
		heap.pop_back();
		heapifyDown(0);
	}

	// Get the minimum element without removing it
	pair<int, int> top() {
		return heap[0];
	}

	// Check if heap is empty
	bool empty() {
		return heap.empty();
	}

};

// Node structure for adjacency list representation
// Each node represents a neighbor with destination vertex and edge weight
struct adjNode {
	int dest;           // Destination vertex ID
	int weight;         // Edge weight to destination
	struct adjNode* next;  // Pointer to next neighbor in the list

	//constructors
	adjNode(int d, int w) {
		dest = d;
		weight = w;
		next = nullptr;
	}
};

// Vertex structure containing vertex ID and pointer to its adjacency list
struct vertex {
	int ID; //which vertex are we at?
	struct adjNode* head; //adjList/neighbors - pointer to first neighbor

	//constructor
	vertex() {
		ID = -1;
		head = nullptr;
	}

	vertex(int i) {
		ID = i;
		head = nullptr;
	}
};

// Graph class using adjacency list representation
// More memory efficient for sparse graphs compared to adjacency matrix
class Graph {
public:
	vertex* vertices;  // Array of vertices
	int V; //tot vertices

	//constructor - Initialize graph with V vertices
	Graph(int v) {
		V = v;
		vertices = new vertex[V]; //array of vertices

		// Initialize each vertex with its ID
		for (int i = 0; i < V; i++) {
			vertices[i] = vertex(i); //assign ids to every vertex
		}
	}

	// Add an undirected weighted edge between src and dest
	void addEdge(int src, int dest, int w) {
		// Add edge from src to dest
		adjNode* node = new adjNode(dest, w);
		node->next = vertices[src].head;  // Insert at beginning of adjacency list
		vertices[src].head = node;

		//undirected graph - add reverse edge
		adjNode* reverseNode = new adjNode(src, w);
		reverseNode->next = vertices[dest].head;
		vertices[dest].head = reverseNode;
	}

	// Check if an edge exists between vertex v and vertex u
	bool doesEdgeExist(vertex v, int u) {
		adjNode* temp = v.head;
		while (temp) {
			if (temp->dest == u) {
				return true;
			}
			temp = temp->next;
		}
		return false;
	}

	// Print the entire graph structure for debugging
	void printGraph() {
		for (int i = 0; i < V; ++i) {
			cout << "Vertex " << i << " -> ";
			adjNode* temp = vertices[i].head;
			while (temp) {
				cout << "(" << temp->dest << ", " << temp->weight << ") ";
				temp = temp->next;
			}
			cout << endl;
		}
	}

	//deconstructor/reclaim mem - Prevent memory leaks
	~Graph() {
		// Delete all adjacency list nodes for each vertex
		for (int i = 0; i < V; i++) {
			adjNode* current = vertices[i].head; //get neighbor
			while (current) { //if neighbor exists
				adjNode* toDelete = current; //save current node for deletion
				current = current->next;
				delete toDelete;
			}
		}
		delete[] vertices;//delete the array as well
	}
};

// Generate a random connected graph with specified number of nodes and edge limit
// Uses seed for reproducible results
Graph random_graph(int nodes, int edges_limit, int seed = 0) {
	Graph g(nodes);

	srand(seed);  // Set seed for reproducible random generation

	// Track existing edges to avoid duplicates
	vector<vector<bool>> existing(nodes, vector<bool>(nodes, false));
	vector<bool> connected(nodes, false);  // Track which vertices are connected to the main component

	// Start with vertex 0 as connected
	connected[0] = true;
	int connected_count = 1;
	int edge_count = 0;

	// Phase 1: Ensure all vertices are connected (create spanning tree)
	while (connected_count < nodes && edge_count < edges_limit) {
		// Pick a random vertex that's already connected
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes;

		// Pick a random vertex that's not yet connected
		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes;

		// Add edge with weight 0 (will be assigned later)
		g.addEdge(u, v, 0);
		existing[u][v] = existing[v][u] = true;
		connected[v] = true;

		connected_count++;
		edge_count++;
	}

	// Phase 2: Add additional random edges up to the limit
	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v) continue;  // No self-loops
		if (u > v) swap(u, v); // optional: reduce symmetry
		if (existing[u][v]) continue;  // Skip if edge already exists

		g.addEdge(u, v, 0);
		existing[u][v] = existing[v][u] = true;
		edge_count++;
	}

	return g;
}

// Assign random weights to all edges in the graph
// Uses MT19937 generator for better randomness quality
Graph& random_cost_generator(Graph& g, int seed = 0, int start = 0, int end = INT_MAX) {
	mt19937 rng(seed);  // Mersenne Twister random number generator

	uniform_int_distribution<int> dist(start, end);  // Uniform distribution for edge weights

	vector<vector<bool>> assigned(g.V, vector<bool>(g.V, false)); //easier to track which edges have been assigned weights

	// Iterate through all vertices and their adjacency lists
	for (int i = 0; i < g.V; i++) {
		adjNode* current = g.vertices[i].head; //current source

		while (current) {
			int v = current->dest; //neighbor

			// Only assign weight once per edge (avoid duplicate assignment for undirected edges)
			if (!assigned[i][v]) {
				int weight = dist(rng);

				// Assign weight to edge u->v
				current->weight = weight;

				// Find and assign same weight to reverse edge v->u
				adjNode* reverseNode = g.vertices[v].head;
				//for v the current source could be anywhere in the adjacency list
				while (reverseNode) {
					if (reverseNode->dest == i) {
						reverseNode->weight = weight;
						break;
					}
					reverseNode = reverseNode->next;
				}

				assigned[i][v] = assigned[v][i] = true;
			}

			current = current->next;
		}
	}

	return g;
}

// Dijkstra's algorithm implementation for finding shortest paths from a source vertex
// Time complexity: O((V + E) log V) where V = vertices, E = edges
void dijkstra(int V, int source, Graph& g) {
	auto start = chrono::high_resolution_clock::now();  // Start timing

	// Initialize distances to infinity and source to 0
	vector<int> dist(V, INT_MAX);
	dist[source] = 0;
	vector<int> predecessor(V, -1);  // Track predecessors for path reconstruction

	// Priority queue implemented as min-heap
	MinHeap minHeap;
	minHeap.push({ 0,source });  // Start with source vertex

	// Main Dijkstra loop
	while (!minHeap.empty()) {
		pair<int, int> top = minHeap.top();
		minHeap.pop();

		int d = top.first;   // Current shortest distance
		int u = top.second;  // Current vertex

		// Explore all neighbors of current vertex
		adjNode* current = g.vertices[u].head;

		while (current) {
			int v = current->dest;      // Neighbor vertex
			int weight = current->weight; // Edge weight

			// Relaxation step: check if we found a shorter path to v
			if (dist[u] + weight < dist[v]) {
				dist[v] = dist[u] + weight;
				predecessor[v] = u;
				minHeap.push({ dist[v],v });  // Add to priority queue with new distance
			}

			current = current->next;
		}

	}

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << "Dijkstra on " << V << " vertices took " << duration.count() << "microsec" << endl;

	// Wait for user input before showing results
	cout << "Press Enter to begin path reconstruction.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get();

	// Display shortest distances from source
	cout << "Shortest distances from source " << source << ":\n";
	for (int i = 0; i < V; ++i) {
		cout << "Vertex " << i << ": Distance = ";
		if (dist[i] == INT_MAX) {
			cout << "INF, Predecessor = " << predecessor[i];
		}
		else {
			cout << dist[i] << ", Predecessor = " << predecessor[i];
		}
		cout << endl;
	}

	// Reconstruct and display shortest paths
	cout << "\nShortest paths from source " << source << ":\n";
	for (int i = 0; i < V; ++i) {
		if (dist[i] == INT_MAX) continue; // Skip unreachable vertices
		cout << "Path to " << i << ": ";
		vector<int> path;
		int v = i;
		// Trace back from destination to source using predecessors
		while (v != -1) {
			path.push_back(v);
			v = predecessor[v];
		}
		// Print path in correct order (source to destination)
		for (int j = path.size() - 1; j >= 0; --j) {
			cout << path[j];
			if (j > 0) cout << " -> ";
		}
		cout << endl;
	}

}

// Main function - Entry point of the program
int main() {
	cout << "Dijkstra with Raw AdjList approach." << endl;

	// Get user input for graph parameters
	int vertices;
	cout << "Ver? ";
	cin >> vertices;

	int edge_limit;
	cout << "Edge Limit? ";
	cin >> edge_limit;

	int seed = 1051962;  // Fixed seed for reproducible results

	// Generate random graph and measure time
	auto start = chrono::high_resolution_clock::now();
	Graph g = random_graph(vertices, edge_limit, seed);
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Random generation of " << vertices << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	// Assign random weights to edges and measure time
	start = chrono::high_resolution_clock::now();
	g = random_cost_generator(g, seed, 1, 50);  // Weights between 1 and 50
	stop = chrono::high_resolution_clock::now();

	duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Random cost generation of " << edge_limit << " edges with seed " << seed << " took " << duration.count() << "microsec" << endl;

	// Wait for user to start Dijkstra
	cout << "Press Enter to run dijkstra.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
	cin.get();

	// Run Dijkstra's algorithm from vertex 0
	dijkstra(vertices, 0, g);

	//g.printGraph();  // Uncomment to see graph structure

	// Wait before exiting
	cout << "Press Enter to exit...";
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
	cin.get();

	return 0;
}