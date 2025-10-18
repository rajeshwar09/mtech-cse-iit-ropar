#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <iomanip>

using namespace std;

const int UNVISITED = -1;
const int DIRECTED = 1;
const int UNDIRECTED = 0;
const int INF = 999999;

// Structures
typedef struct BFS_data {
  vector<int> distance;
  vector<int> parent;
  vector<int> levels;
} BFS_data;

typedef struct Edges_data {
  int tree = 0;
  int forward = 0;
  int backward = 0;
  int cross = 0;
} Edges_data;

typedef struct DFS_data {
  vector<int> discovery_time;
  vector<int> finish_time;
  vector<int> parent;
} DFS_data;


// HELPER FUNCTIONS
// Create adjacency matrix
void create_adjacency_matrix(vector<vector<int>>& matrix, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      cin >> matrix[i][j];
    }
  }
}

int find_max(const vector<int>& array) {
  auto max_it = max_element(array.begin(), array.end());
  return *max_it;
}

//--------------------------------------------------------------------------
// 1. BFS + supported functions

BFS_data bfs_levels(const vector<vector<int>>& matrix, int s, int N) {
  vector<int> distance(N + 1, UNVISITED);
  vector<int> parent(N + 1, -1);
  vector<int> levels;
  queue<int> q;
  distance[s] = 0;
  q.push(s);

  while (!q.empty()) {
    int u = q.front();
    q.pop();

    for (int v = 1; v <= N; v++) {
      if (matrix[u-1][v-1] == 1 && distance[v] == UNVISITED) {
        distance[v] = distance[u] + 1;
        parent[v] = u;
        q.push(v);
      }
    }
  }

  int max_level = find_max(distance);
  for (int d = 1; d <= max_level+1; d++) {
    int count = 0; 
    for (auto i: distance) {
      if (d == i) count++;
    }
    levels.push_back(count);
    if (count == 0) break; 
  }

  return BFS_data{distance, parent, levels};
}

Edges_data bfs_classify_edges(const vector<vector<int>>& matrix, int N, int D, const BFS_data& bfs) {
  Edges_data edges;
  int tree_edges = 0;

  for (int v = 1; v <= N; v++) {
    if (bfs.parent[v] != -1) {
      tree_edges++;
    }
  }
  
  if (D == DIRECTED) {
    for (int u = 1; u <= N; u++) {
      if (bfs.distance[u] == UNVISITED) continue;

      for (int v = 1; v <= N; v++) {
        if (u == v) continue; // self edge
        if (matrix[u-1][v-1] != 1) continue; // must be edge
        if (bfs.distance[v] == UNVISITED) continue; // unreachable v
        if (bfs.parent[v] == u) continue; // tree edge (already counted)
        
        bool v_ancestor_u = false;
        for (int curr = u; curr != -1; curr = bfs.parent[curr]) {
          if (curr == v) {
            v_ancestor_u = true;
            break;
          }
        }

        if (v_ancestor_u == true) {
          edges.backward++;
          continue;
        }

        bool u_ancestor_v = false;
        for (int curr = v; curr != -1; curr = bfs.parent[curr]) {
          if (curr == u) {
            u_ancestor_v = true;
            break;
          }
        }

        if (u_ancestor_v == true) {
          edges.forward++;
        } else {
          edges.cross++;
        }
      }
    }
    edges.tree = tree_edges;
  } else {
    for (int u = 1; u <= N; u++) {
      for (int v = u + 1; v <= N; v++) {
        if (matrix[u-1][v-1] != 1) continue; 
        if (bfs.distance[u] == UNVISITED || bfs.distance[v] == UNVISITED) continue;
        if (bfs.parent[u] == v || bfs.parent[v] == u) edges.tree++;
        else edges.cross++;
      }
    }
  }

  return edges;
}

void bfs_solution(const vector<vector<int>>& matrix, int s, int N, int D) {
  BFS_data bfs = bfs_levels(matrix, s, N);
  Edges_data edges = bfs_classify_edges(matrix, N, D, bfs);

  for (int i = 0; i < bfs.levels.size(); i++) {
    if (i) cout << " ";
    cout << bfs.levels[i];
  }

  if (D == DIRECTED) {
    cout << " " << edges.tree
         << " " << edges.backward
         << " " << edges.forward
         << " " << edges.cross;
  } else {
    cout << " " << edges.tree
         << " " << edges.cross;
  }

  cout << "\n";
}

//--------------------------------------------------------------------------
// 2. DFS + supported functions
void dfs_visit(const vector<vector<int>>& matrix, int u, int& time, DFS_data& dfs, int N) {
  dfs.discovery_time[u] = ++time;

  for (int v = 1; v <= N; v++) {
    if (matrix[u-1][v-1] == 0) continue;
    
    if (dfs.discovery_time[v] == 0) {
      dfs.parent[v] = u;
      dfs_visit(matrix, v, time, dfs, N);
    }
  }

  dfs.finish_time[u] = ++time;
}

DFS_data dfs_run(const vector<vector<int>>& matrix, int s, int N) {
  DFS_data dfs;
  dfs.discovery_time.assign(N + 1, 0);
  dfs.finish_time.assign(N + 1, 0);
  dfs.parent.assign(N + 1, UNVISITED);
  
  int time = 0;

  dfs_visit(matrix, s, time, dfs, N);

  return dfs;
}

Edges_data dfs_classify_edges(const vector<vector<int>>& matrix, int N, int D, const DFS_data& dfs) {
  Edges_data edges;
  
  if (D == DIRECTED) {
    for (int u = 1; u <= N; u++) {
      if (dfs.discovery_time[u] == 0) continue;
      for (int v = 1; v <= N; v++) {
        if (u == v) continue;
        if (matrix[u-1][v-1] == 0) continue;
        if (dfs.discovery_time[v] == 0) continue;

        if (dfs.parent[v] == u) {
          edges.tree++;
          continue;
        }

        if (dfs.discovery_time[v] < dfs.discovery_time[u] && dfs.finish_time[u] < dfs.finish_time[v]) edges.backward++;
        else if (dfs.discovery_time[u] < dfs.discovery_time[v] && dfs.finish_time[v] < dfs.finish_time[u]) edges.forward++;
        else edges.cross++;
      }
    }
  } else {
    for (int u = 1; u <= N; u++) {
      for (int v = u + 1; v <= N; v++) {
        if (matrix[u-1][v-1] == 0) continue;

        if (dfs.discovery_time[u] == 0 || dfs.discovery_time[v] == 0) continue;

        if (dfs.parent[v] == u || dfs.parent[u] == v) edges.tree++;
        else edges.backward++;
      }
    }
  }

  return edges;
}

void dfs_solution(const vector<vector<int>>& matrix, int s, int N, int D) {
  DFS_data dfs = dfs_run(matrix, s, N);
  Edges_data edges = dfs_classify_edges(matrix, N, D, dfs);

  cout << dfs.finish_time[s];
  
  if (D == DIRECTED) {
    cout << " " << edges.tree
         << " " << edges.backward
         << " " << edges.forward
         << " " << edges.cross;
  } else {
    cout << " " << edges.tree
         << " " << edges.backward;
  }

  cout << "\n";
}

//--------------------------------------------------------------------------
// 3. Topological Sorting
vector<int> topological_sort(const vector<vector<int>>& matrix, int N) {
  vector<int> in_degrees(N, 0);
  vector<int> order;

  for (int v = 0; v < N; v++) {
    for (int u = 0; u < N; u++) {
      if (matrix[u][v] == 1) in_degrees[v]++;
    }
  }

  priority_queue<int, vector<int>, greater<int>> min_heap;

  for (int v = 0; v < N; v++) {
    if (in_degrees[v] == 0) {
      min_heap.push(v);
    }
  }

  while (!min_heap.empty()) {
    int u = min_heap.top();
    min_heap.pop();
    order.push_back(u);

    for (int v = 0; v < N; v++) {
      if (matrix[u][v] == 1) {
        in_degrees[v]--;
        if (in_degrees[v] == 0) min_heap.push(v);
      }
    }
  }

  if (order.size() != N) {
    return {};
  }

  return order;
}

void topological_solution(const vector<vector<int>>& matrix, int N) {
  vector<int> order = topological_sort(matrix, N);
  if (order.empty()) {
    cout << -1 << "\n";
  } else {
    for (int i = 0; i < N; i++) {
      if (i) cout << " ";
      cout << order[i] + 1;
    }
    cout << "\n";
  }
}

//--------------------------------------------------------------------------
// 4. MST using Prim's and adjacency list
vector<vector<pair<int, int>>> create_adjacency_list(const vector<vector<int>>& matrix, int N) {
  vector<vector<pair<int, int>>> adj_list(N + 1);

  for (int i = 1; i <= N; i++) {
    for (int j = i + 1; j <= N; j++) {
      int weight = matrix[i-1][j-1];
      if (i != j && weight != INF) {
        adj_list[i].push_back({j, weight});
        adj_list[j].push_back({i, weight});
      }
    }
  }

  return adj_list;
}

pair<vector<int>, int> prim_run(const vector<vector<pair<int, int>>>& adj_list, int N, int s) {
  vector<int> key(N + 1, INF);
  vector<bool> in_MST(N + 1, false);

  priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

  key[s] = 0;
  pq.push({0, s});
  int picked = 0;

  while (!pq.empty()) {
    int k = pq.top().first;
    int u = pq.top().second;
    
    pq.pop();
    if (in_MST[u]) continue;
    in_MST[u] = true;
    picked++;

    for (auto [v, w]: adj_list[u]) {
      if (!in_MST[v] && w < key[v]) {
        key[v] = w;
        pq.push({key[v], v});
      }
    }
  }

  return {key, picked};
}

void prim_solution(const vector<vector<int>>& matrix, int N, int D, int s) {
  if (D == 1) {
    cout << -1 << "\n";
    return;
  }

  // check for symeetricty
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (i == j) continue;
      if (matrix[i][j] != matrix[j][i]) {
        cout << -1 << '\n';
        return;
      }
    }
  }

  vector<vector<pair<int, int>>> adj = create_adjacency_list(matrix, N);
  pair<vector<int>, int> prims = prim_run(adj, N, s);

  vector<int> key = prims.first;
  int picked = prims.second;

  if (picked != N) {
    cout << -1 << "\n";
    return;
  }

  long total_cost = 0;
  for (int i = 1; i <= N; i++) {
    total_cost += key[i];
  }

  cout << total_cost << " ";
  for (int i = 1; i <= N; i++) {
    if (i > 1) cout << " ";
    cout << key[i];
  }

  cout << "\n";
}

//--------------------------------------------------------------------------
// 5. DFS validation
bool dfs_validate(const vector<vector<int>>& matrix, int N, const vector<int>& discovery_time, const vector<int>& finish_time) {
  bool validate = true;

  // check 1
  if (validate) {
    vector<int> seen(2*N + 1, 0);

    for (int u = 0; u < N; u++) {
      if (discovery_time[u] < 1 || finish_time[u] < 1 || discovery_time[u] >= finish_time[u] || finish_time[u] > 2*N) {
        validate = false;
        break;
      }
      if (++seen[discovery_time[u]] != 1) {
        validate = false;
        break;
      }
      if (++seen[finish_time[u]] != 1) {
        validate = false;
        break;
      }
    }

    if (validate) {
      for (int i = 1; i <= 2 * N; i++) {
        if (seen[i] != 1) {
          validate = false;
          break;
        }
      }
    }
  }

  // check 2
  if (validate) {
    for (int u = 0; u < N && validate; u++) {
      for (int v = u+1; v < N; v++) {
        bool u_in_v = (discovery_time[v] < discovery_time[u] && finish_time[u] < finish_time[v]);
        bool v_in_u = (discovery_time[u] < discovery_time[v] && finish_time[v] < finish_time[u]);
        bool disjoint = (finish_time[u] < discovery_time[v] || finish_time[v] < discovery_time[u]);

        if (!(u_in_v || v_in_u || disjoint)) {
          validate = false;
          break;
        }
      }
    }
  }

  //check 3
  if (validate) {
    for (int u = 0; u < N; u++) {
      for (int v = 0; v < N; v++) {
        if (u == v || matrix[u][v] == 0) continue;
        
        bool tree_forward = (discovery_time[u] < discovery_time[v] && finish_time[v] < finish_time[u]);
        bool back = (discovery_time[v] < discovery_time[u] && finish_time[u] < finish_time[v]);
        bool cross = (finish_time[v] < discovery_time[u]);

        if (!(tree_forward || back || cross)) {
          validate = false;
          break;
        }
      }
    }
  }

  //check 4
  if (validate) {
    for (int v = 0; v < N && validate; v++) {
      bool contain = false;

      for (int u = 0; u < N; u++) {
        if (u == v) continue;
        if (discovery_time[u] < discovery_time[v] && finish_time[v] < finish_time[u]) {
          contain = true;
          break;
        }
      }

      if (!contain) continue;

      bool from_ancestor = false;
      for (int u = 0; u < N; u++) {
        if (u == v) continue;
        if (matrix[u][v] == 0) continue;
        if (discovery_time[u] < discovery_time[v] && finish_time[v] < finish_time[u]) {
          from_ancestor = true;
          break;
        }
      }

      if (!from_ancestor) {
        validate = false;
        break;
      }
    }
  }

  return validate;
}

//--------------------------------------------------------------------------
// 6. Floydd Warshall (All pair shortest path)
vector<vector<long>> floyd_warshall(const vector<vector<int>>& matrix, int N) {
  vector<vector<long>> distance(N, vector<long>(N, INF));

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (i == j) distance[i][j] = 0;
      else if (matrix[i][j] == INF) distance[i][j] = INF;
      else distance[i][j] = matrix[i][j];
    }
  }

  for (int k = 0; k < N; k++) {
    for (int i = 0; i < N; i++) {
      if (distance[i][k] == INF) continue;
      for(int j = 0; j < N; j++) {
        if (distance[k][j] == INF) continue;
        if (distance[i][k] + distance[k][j] < distance[i][j])
          distance[i][j] = distance[i][k] + distance[k][j];
      }
    }
  }

  return distance;
}

void floyd_warshall_solution(const vector<vector<int>>& matrix, int N) {
  vector<vector<long>> distance = floyd_warshall(matrix, N);

  for (int i = 0; i < N; i++) {
    if (distance[i][i] < 0) {
      cout << -1 << "\n";
      return;
    }
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (j) cout << " ";
      cout << (distance[i][j] == INF ? INF : distance[i][j]);
    }
    cout << "\n";
  }
}

// MAIN
int main(int argc, char const *argv[])
{
  int T;
  cin >> T;

  for (int t = 1; t <= T; t++) {
    int query_id;
    cin >> query_id;

    switch (query_id) {
      case 1: {
        // Breadth First Search
        int N, D, s;
        cin >> N >> D >> s;

        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);
        // print_adj_matrix(adj_matrix);
        bfs_solution(adj_matrix, s, N, D);
      break;
      }

      case 2: {
        // Depth First Search
        int N, D, s;
        cin >> N >> D >> s;

        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);
        dfs_solution(adj_matrix, s, N, D);
        break;
      }

      case 3: { 
        // Topological Sort
        int N;
        std::cin >> N;
        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);
        topological_solution(adj_matrix, N);
        break;
      }

      case 4: {
        // MST using Prim's and adjacenty list representation
        int N, D, s;
        cin >> N >> D >> s;

        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);
        prim_solution(adj_matrix, N, D, s);
        break;
      }

      case 5: {
        // Validating DFS
        int N;;
        cin >> N;

        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);
        
        vector<int> discovery_time(N);
        vector<int> finish_time(N);

        for (int i = 0; i < N; i++) cin >> discovery_time[i];
        for (int i = 0; i < N; i++) cin >> finish_time[i];

        bool validate = dfs_validate(adj_matrix, N, discovery_time, finish_time);

        if (validate) cout << "YES\n";
        else cout << "NO\n";

        break;
      }

      case 6: {
        // Floyd Warshall for All pair shortest path (DP)
        int N, D;
        cin >> N >> D;

        vector<vector<int>> adj_matrix(N, vector<int>(N));
        create_adjacency_matrix(adj_matrix, N);  
        floyd_warshall_solution(adj_matrix, N);
        break;
      }
      
      default:
      break;
    }
  }

  return 0;
}
