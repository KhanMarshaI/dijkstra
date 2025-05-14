#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
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

vector<vector<pair<int, int>>> random_graph(int nodes, int edges_limit, int seed = 0, int start = 0, int end = INT_MAX) {

	vector<vector<pair<int, int>>> adjList(nodes);


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
	int V;

	cout << "Vertices? ";
	cin >> V;

	vector<vector<pair<int, int>>> adjList = initGraph(V);

	auto start = chrono::high_resolution_clock::now();
	dijkstra(V, 0, adjList);
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << duration.count() << "ms" << endl;

	return 0;
}