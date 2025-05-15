#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
#include <random> //cost range
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

void printGraph(vector<vector<pair<int, int>>>& graph, int V) {
	
	for (int j = 0; j < V; j++) {
		cout << j << "--> ";

		for (auto edge : graph[j]) {
			cout << "(" << edge.first << ", " << edge.second << ")";
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

		printGraph(adjList, V);
	}

	return adjList;
}

vector<vector<pair<int, int>>> random_graph(int nodes, int edges_limit, int seed = 0) {
	
	vector<vector<pair<int, int>>> adjList(nodes);
	
	srand(seed);
	
	vector<vector<bool>> existing(nodes, vector<bool>(nodes, false));

	vector<bool> connected(nodes, false);
	connected[0] = true;
	int connected_count = 1;
	int edge_count = 0;
	
	//partial spanning tree
	while (connected_count < nodes && edge_count < edges_limit) {
		int u = rand() % nodes;
		while (!connected[u]) u = rand() % nodes;

		int v = rand() % nodes;
		while (connected[v] || u == v) v = rand() % nodes;

		adjList[u].push_back({ v,0 });
		adjList[v].push_back({ u,0 });
		
		existing[u][v] = existing[v][u] = true;
		connected[v] = true;

		connected_count++;
		edge_count++;

	}

	//denser graph (higher chance of all nodes being connected)
	while (edge_count < edges_limit) {
		int u = rand() % nodes;
		int v = rand() % nodes;

		if (u == v) continue;
		if (u > v) swap(u, v); // Ensure u < v for consistency
		if (existing[u][v]) continue;

		adjList[u].push_back({ v, 0 });
		adjList[v].push_back({ u, 0 });

		existing[u][v] = true;
		edge_count++;
	}

	return adjList;
}

vector<vector<pair<int, int>>> random_cost_generator(vector<vector<pair<int, int>>> adjList, int seed = 0, int start = 0, int end = INT_MAX) {
	
	mt19937 rng(seed);
	
	uniform_int_distribution<int> dist(start, end);

	for (int i = 0; i < adjList.size(); i++) {

		for (int j = 0; j < adjList[i].size(); j++) {
			
			int cost = dist(rng);
			adjList[i][j].second = cost;

		}

	}

	return adjList;
}

void dijkstra(int V, int source, vector<vector<pair<int, int>>>& adjList) {
	vector<int> dist(V, INT_MAX);
	dist[source] = 0;

	MinHeap minHeap;
	minHeap.push(make_pair(0,source)); //weight,vertices

	while (!minHeap.empty()) {
		pair<int, int> top = minHeap.top();
		minHeap.pop();

		int d = top.first; //dist
		int u = top.second; //vertices

		if (d > dist[u]) continue;

		vector<pair<int, int>>& neighbours = adjList[u]; //get neighbour of current vertices

		for (size_t i = 0; i < neighbours.size(); ++i) {
			int v = neighbours[i].first; //neighbour vertices
			int weight = neighbours[i].second;

			if (dist[u] + weight < dist[v]) {
				dist[v] = dist[u] + weight;

				minHeap.push(make_pair(dist[v], v));
			}

		}

	}

	for (int i = 0; i < V; ++i) {
		cout << "Distance from " << source << " to " << i << ": ";
		if (dist[i] == INT_MAX)
			cout << "INF\n";
		else
			cout << dist[i] << "\n";
	}

}

int main() {
	int V,E;

	cout << "Vertices? ";
	cin >> V;

	cout << "Edge limit? ";
	cin >> E;

	int seed = 1051962;

	auto start = chrono::high_resolution_clock::now();
	vector<vector<pair<int, int>>> adjList = random_graph(V,E,seed);
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Random generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;

	start = chrono::high_resolution_clock::now();
	adjList = random_cost_generator(adjList, seed, 1, 50);
	stop = chrono::high_resolution_clock::now();

	duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << "Random cost generation of " << V << " vertices with seed " << seed << " took " << duration.count() << "microsec" << endl;


	start = chrono::high_resolution_clock::now();
	dijkstra(V, 0, adjList);
	stop = chrono::high_resolution_clock::now();

	duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Dijsktra for " << V << " vertices took " << duration.count() << "microsec" << endl;

	return 0;
}