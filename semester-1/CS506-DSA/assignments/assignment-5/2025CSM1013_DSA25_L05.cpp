#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// safe side taken large value
const long long INF = 1000000000000000000LL;

struct Edge {
  int u, v;
  long long weight;
};

struct Adj_Node {
  int to_node;
  long long weight;
};

// HELPER FUNCTIONS
// Create adjacency matrix
void create_adjacency_matrix(vector<vector<long long>>& matrix, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      cin >> matrix[i][j];
    }
  }
}

// Convert adjacency matrix to adjacency list
vector<Edge> adj_mat_to_adj_list(const vector<vector<long long>>& matrix, int N) {
  vector<Edge> edges;

  for (int u = 0; u < N; u++) {
    for (int v = 0; v < N; v++) {
      if (u == v) continue;
      if (matrix[u][v] != 0) {
        edges.push_back({u, v, matrix[u][v]});
      }
    }
  }

  return edges;
}

// Reweighting edges for johnson's algorithm
vector<vector<Adj_Node>> reweighting(const vector<vector<long long>>& matrix, int N, const vector<long long>& h) {
  vector<vector<Adj_Node>> adj_node_list(N);
  for (int u = 0; u < N; u++) {
    for (int v = 0; v < N; v++) {
      if (u == v) continue;
      if (matrix[u][v] != 0) {
        long long new_weight = matrix[u][v] + h[u] - h[v];
        adj_node_list[u].push_back({v, new_weight});
      }
    }
  }

  return adj_node_list;
}

// Print johnson's matrix solution
void print_johnson_solution(const vector<vector<long long>>& distance, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (j) cout << ' ';
      if (distance[i][j] == INF) cout << 'X';
      else cout << distance[i][j];
    }
    cout << '\n';
  }
}

// Convert Adjacency matrix to undirected edges
vector<Edge> matrix_to_undirected_edges(const vector<vector<long long>>& matrix, int N) {
  vector<Edge> edges;

  for (int i = 0; i < N; i++) {
    for (int j = i+1; j < N; j++) {
      if (matrix[i][j] != 0) {
        edges.push_back({i, j, matrix[i][j]});
      }
    }
  }

  return edges;
}

// Print kruskal's solution
void print_kruskal_solution(const vector<vector<long long>>& matrix, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (j) cout << ' ';
      cout << matrix[i][j];
    }
    cout << '\n';
  }
}

//-----------------------------------------------------------------
// 1. Topological Sort
vector<int> topological_sort(const vector<vector<long long>>& matrix, int N) {
  vector<int> in_degrees(N, 0);
  vector<int> order;

  for (int v = 0; v < N; v++) {
    for (int u = 0; u < N; u++) {
      if (matrix[u][v] == 1) in_degrees[v]++;
    }
  }


  // UNCOMMENT LINE TO VERIFY THE CODE
  // BY DEFAULT: taking min-heap (to get lexographically smallest)
  // priority_queue that pops the smallest label first (as per instruction of assignment):
  priority_queue<int, vector<int>, greater<int>> pq;
  // priority_queue that pops the largest label first (as per input/ouput given in assignment):
  // priority_queue<int> pq;

  for (int v = 0; v < N; v++) {
    if (in_degrees[v] == 0) {
      pq.push(v);
    }
  }

  while (!pq.empty()) {
    int u = pq.top();
    pq.pop();
    order.push_back(u);

    for (int v = 0; v < N; v++) {
      if (matrix[u][v] == 1) {
        in_degrees[v]--;
        if (in_degrees[v] == 0) pq.push(v);
      }
    }
  }

  return order;
}

void topological_solution(const vector<vector<long long>>& matrix, int N) {
  vector<int> order = topological_sort(matrix, N);

  for (int i = 0; i < N; i++) {
    if (i) cout << " ";
    cout << order[i];
  }
  cout << "\n";
}

//-----------------------------------------------------------------
// 2. Johnson's Algorithm

// Dijkstra's Algorithm
vector<long long> dijkstra_algorithm(const vector<vector<Adj_Node>>& list, int N, int s) {
  vector<long long> distance(N, INF);

  distance[s] = 0;

  priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;

  pq.push({0, s});

  while (!pq.empty()) {
    auto dist_u = pq.top().first;
    auto u = pq.top().second;
    pq.pop();

    if (dist_u != distance[u]) continue;

    for (auto e: list[u]) {
      int v = e.to_node;
      long long w = e.weight;

      if (distance[v] > distance[u] + w) {
        distance[v] = distance[u] + w;
        pq.push({distance[v], v});
      }
    }
  }

  return distance;
}

// Bellman Ford Algorithm
vector<long long> bellman_ford_algorithm(const vector<Edge>& edges, int N, int s) {
  vector<long long> distance(N, INF);
  distance[s] = 0;

  for (int i = 1; i <= N - 1; i++) {
    bool changed = false;

    for (auto& e: edges) {
      if (distance[e.u] == INF) continue;
      if (distance[e.v] > distance[e.u] + e.weight) {
        distance[e.v] = distance[e.u] + e.weight;
        changed = true;
      }
    }

    if (changed == false) break;
  }

  for (auto& e: edges) {
    if (distance[e.u] == INF) continue;
    if (distance[e.v] > distance[e.u] + e.weight) {
      return { -1 };
    }
  }

  return distance;
}

// Johnson's Algorithm
vector<vector<long long>> johnson_algorithm(const vector<vector<long long>>& matrix, int N) {
  vector<Edge> edges = adj_mat_to_adj_list(matrix, N);

  int s = N;
  int new_N = N + 1;
  vector<Edge> updated_edges = edges;
  updated_edges.reserve(updated_edges.size() + N);

  for (int v = 0; v < N; v++) {
    updated_edges.push_back({s, v, 0});
  }

  // Applying Bellman-Ford
  vector<long long> updated_distance = bellman_ford_algorithm(updated_edges, new_N, s);

  // Reweighting the edges
  vector<long long> h(N, 0);
  for (int v = 0; v < N; v++) {
    h[v] = updated_distance[v];
  }

  vector<vector<Adj_Node>> reweighted_adj_list = reweighting(matrix, N, h);

  // Applying Dijkstra on upded adjacency list
  vector<vector<long long>> distance(N, vector<long long>(N, INF));
  for (int u = 0; u < N; u++) {
    vector<long long> distance_dijkstra = dijkstra_algorithm(reweighted_adj_list, N, u);
    for (int v = 0; v < N; v++) {
      if (distance_dijkstra[v] == INF) {
        distance[u][v] = INF;
      } else {
        distance[u][v] = distance_dijkstra[v] - h[u] + h[v];
      }
    }
  }

  return distance;
}

//-----------------------------------------------------------------
// 3. Kruskal Algorithm

// Disjoint Set Union
void make_set(vector<int>& parent, vector<int>& rank, int N) {
  parent.resize(N);
  rank.assign(N, 0);
  
  for (int i = 0; i < N; i++) {
    parent[i] = i;
  }
}

int find_set(vector<int>& parent, int vertex) {
  if (vertex == parent[vertex]) return vertex;
  parent[vertex] = find_set(parent, parent[vertex]);
  return parent[vertex];
}

bool union_sets(vector<int>& parent, vector<int>& rank, int a, int b) {
  a = find_set(parent, a);
  b = find_set(parent, b);

  if (a == b) return false;

  if (rank[a] < rank[b]) {
    int temp = a;
    a = b;
    b = temp;
  }

  parent[b] = a;

  if (rank[a] == rank[b]) rank[a]++;

  return true;
}

// Kruskal Algorithm
vector<vector<long long>> kruskal_algorithm(const vector<vector<long long>>& matrix, int N) {
  vector<Edge> edges = matrix_to_undirected_edges(matrix, N);

  sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
    return a.weight < b.weight;
  });

  // Initialize DSU
  vector<int> parent, rank;
  make_set(parent, rank, N);

  vector<vector<long long>> mst(N, vector<long long>(N, 0));

  int edge_picked = 0;

  for (auto e: edges) {
    if (union_sets(parent, rank, e.u, e.v)) {
      mst[e.u][e.v] = e.weight;
      mst[e.v][e.u] = e.weight;

      if (++edge_picked == N - 1) {
        break;
      }
    }
  }

  return mst;
}

// MAIN function
int main() {
  int option;
  int T;
  int N;

  cin >> option;
  cin >> T;

  for (int t = 0; t < T; t++) {
    cin >> N;

    vector<vector<long long>> adj_matrix(N, vector<long long>(N));
    create_adjacency_matrix(adj_matrix, N);

    switch (option) {
      case 1:
        // Topological Sort
        topological_solution(adj_matrix, N);
      break;

      case 2: {
        // Johnson's Algorithm
        vector<vector<long long>> distance = johnson_algorithm(adj_matrix, N);
        print_johnson_solution(distance, N);
        break;
      }

      case 3: {
        // Kruskal Algorithm
        auto mst = kruskal_algorithm(adj_matrix, N);
        print_kruskal_solution(mst, N);
        break;
      }

      default:
      break;
    }
  }
}