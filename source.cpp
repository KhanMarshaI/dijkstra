#include <iostream>
#include <vector>
#include <climits>
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

vector<vector<pair<int, int>>>& initGraph(int V) {

	vector<vector<pair<int, int>>> adjList(V);

	int n;
	int neighbour, weight;
	for (int i = 0; i < V; i++) {

		cout << "How many neighbours of " << i << "?" << endl;
		cin >> n;

		for (int j = 0; j < n; j++) {
			cin >> neighbour >> weight;

			adjList[i].push_back({ neighbour,weight });
			adjList[neighbour].push_back({ i,weight });
		}

		printGraph(adjList, V);
	}

	return adjList;
}

int main() {
	int V;

	cout << "Vertices? ";
	cin >> V;

	vector<vector<pair<int, int>>>& adjList = initGraph(V);

	return 0;
}