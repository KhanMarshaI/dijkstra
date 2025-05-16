#include <iostream>
#include <vector>
using namespace std;

class MinHeap {
private:
	vector<pair<int, int>> heap; //pair: {distance, node}

	int left(int i) {
		return 2 * i + 1;
	}

	int right(int i) {
		return 2 * i + 2;
	}

	int parent(int i) {
		return (i - 1) / 2;
	}

	void heapifyUp(int index) {
		while (index != 0 && heap[parent(index)].first > heap[index].first) {
			swap(heap[index], heap[parent(index)]);
			index = parent(index);
		}
	}

	void heapifyDown(int index) {
		int smallest = index;
		int l = left(index);
		int r = right(index);

		if (l < heap.size() && heap[l].first < heap[smallest].first)
			smallest = l;
		if (r < heap.size() && heap[r].first < heap[smallest].first)
			smallest = r;

		if (smallest != index) {
			swap(heap[index], heap[smallest]);
			heapifyDown(smallest);
		}
	}

public:
	void push(pair<int, int> val) {
		heap.push_back(val);
		heapifyUp(heap.size() - 1);
	}

	void pop() {
		if (heap.empty()) return;
		heap[0] = heap.back();
		heap.pop_back();
		heapifyDown(0);
	}

	pair<int, int> top() {
		return heap[0];
	}

	bool empty() {
		return heap.empty();
	}

};

struct adjNode {
	int dest;
	int weight;
	struct adjNode* next;

	//constructors
	adjNode(int d, int w) {
		dest = d;
		weight = w;
		next = nullptr;
	}
};

struct vertex {
	int ID; //which vertex are we at?
	struct adjNode* head; //adjList/neighbors

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

class Graph {
	int V; //tot vertices
	vertex* vertices;

public:
	//constructor
	Graph(int v) {
		V = v;
		vertices = new vertex[V]; //array of vertices

		for (int i = 0; i < V; i++) {
			vertices[i] = vertex(i); //assign ids to every vertex
		}
	}

	void addEdge(int src, int dest, int w) {
		adjNode* node = new adjNode(dest, w);
		node->next = vertices[src].head;
		vertices[src].head = node;

		//undirected graph
		adjNode* reverseNode = new adjNode(src, w);
		reverseNode->next = vertices[dest].head;
		vertices[dest].head = reverseNode;
	}

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

	//deconstructor/reclaim mem
	~Graph() {
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


Graph random_graph(int nodes, int edges_limit, int seed = 0) {
	Graph g(nodes);

	srand(seed);

	vector<vector<bool>> existing(nodes, vector<bool>(nodes, false));
	vector<bool> connected(nodes, false);

	connected[0] = true;
	int connected_count = 1;
	int edge_count = 0;

	while (connected_count < nodes && edge_count < edges_limit) {
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes;

		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes;

		g.addEdge(u, v, 0);
		existing[u][v] = existing[v][u] = true;
		connected[v] = true;

		connected_count++;
		edge_count++;
	}

	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v) continue;
		if (u > v) swap(u, v); // optional: reduce symmetry
		if (existing[u][v]) continue;

		g.addEdge(u, v, 0);
		existing[u][v] = existing[v][u] = true;
		edge_count++;
	}

	return g;
}

int main() {
	int vertices;
	cout << "Ver? ";
	cin >> vertices;

	int edge_limit;
	cout << "Edge Limit? ";
	cin >> edge_limit;

	int seed = 1051962;

	Graph g = random_graph(vertices, edge_limit, seed);

	g.printGraph();

	return 0;
}