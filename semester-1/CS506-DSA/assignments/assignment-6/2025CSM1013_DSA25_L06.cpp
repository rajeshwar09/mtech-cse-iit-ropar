#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <deque>

using namespace std;

const long long INF = 1e15;

struct Edge {
  int u, v;
  long long w;
};

struct Arborescence_Findings {
  long long total_cost = 0;
  vector<int> parent;
  vector<int> reachable;
  vector<vector<pair<int, long long>>> adj_tree;
};

// HELPERS
// To find weight of edge (u, v)
long long get_edge_weight(const vector<Edge>& edges, int u, int v) {
  for (auto i = 0; i < edges.size(); i++) {
    if (edges[i].u == u && edges[i].v == v) {
      return edges[i].w;
    }
  }

  return -1;
}

// Function to clean repeated edges between u and v and keep the smallest
vector<Edge> get_minimum_edge_set(const vector<Edge>& edges, int N) {
  map<pair<int, int>, long long> min_edges;

  // Keeping the smallest weight for each edge (u, v)
  for (auto e: edges) {
    pair<int, int> key = { e.u, e.v };
    if (!min_edges.count(key) || e.w < min_edges[key]) {
      min_edges[key] = e.w;
    }
  }

  // Convert above map entries back to list of edges
  vector<Edge> updated_edges;
  updated_edges.reserve(min_edges.size());

  for (auto key_val: min_edges) {
    int u = key_val.first.first;
    int v = key_val.first.second;
    long long w = key_val.second;

    updated_edges.push_back({u, v, w});
  }

  return updated_edges;
}

// Fucntion to return cost, parent and adjaceny tree
Arborescence_Findings min_arborescence(const vector<Edge>& edges, int N, int s) {
  // Get updated edges
  vector<Edge> updated_edges = get_minimum_edge_set(edges, N);

  // removing self loops and out-of-bound edges
  vector<Edge> valid_edges;
  valid_edges.reserve(edges.size());

  for (auto e: updated_edges) {
    if (e.u >= 1 && e.u <= N && e.v >= 1 && e.v <= N && e.u != e.v) {
      valid_edges.push_back(e);
    }
  }

  updated_edges.swap(valid_edges);

  Arborescence_Findings findings;
  findings.parent.assign(N+1, -1);
  findings.reachable.assign(N+1, 0);
  findings.adj_tree.assign(N+1, {});

  findings.parent[s] = 0;

  if (updated_edges.empty()) {
    findings.reachable[s] = 1;
    return findings;
  }

  // Using the edmonds algorithm till no more cycles are detected
  int n = N;
  vector<Edge> E = updated_edges;
  int source = s;
  long long total_cost = 0;

  while (true) {
    // choosing min incoming edge for every node
    vector<long long> in_weight(n+1, INF);
    vector<int> predecessor(n+1, -1);

    for (auto e: E) {
      if (e.u == e.v) continue;
      // tie break rule : selecting small one
      if (e.w < in_weight[e.v] || (e.w == in_weight[e.v] && (predecessor[e.v] == -1 || e.u < predecessor[e.v]))) {
        in_weight[e.v] = e.w;
        predecessor[e.v] = e.u;
      }
    }

    in_weight[source] = 0;
    predecessor[source] = 0;

    // cycle detection in incoming edges
    int cycle_count = 0;
    vector<int> component_id(n+1, -1);
    vector<int> visited(n+1, -1);

    for (int v = 1; v <= n; v++) {
      total_cost += (in_weight[v] == INF ? 0 : in_weight[v]);
      int u = v;

      while (visited[u] != v && component_id[u] == -1 && u != source && predecessor[u] != -1) {
        visited[u] = v;
        u = predecessor[u];
      }

      if (u != source && predecessor[u] != -1 && component_id[u] == -1 && visited[u] == v) {
        cycle_count++;
        for (int i = predecessor[u]; i != u; i = predecessor[i]) {
          component_id[i] = cycle_count;
        }

        component_id[u] = cycle_count;
      }
    }

    // when no cycles left
    if (cycle_count == 0) break;

    // assigning component id to nodes with no cycle
    for (int v = 1; v <= n; v++) {
      if (component_id[v] == -1) {
        component_id[v] = ++cycle_count;
      }
    }

    // Creating new set of edges with new weights
    vector<Edge> new_E;
    new_E.reserve(E.size());

    for (auto e: E) {
      int new_u = component_id[e.u];
      int new_v = component_id[e.v];

      if (new_u != new_v) {
        long long new_w = e.w - (in_weight[e.v] == INF ? 0 : in_weight[e.v]);
        new_E.push_back({new_u, new_v, new_w});
      }
    }

    // updateing variables for next iteration
    source = component_id[source];
    n = cycle_count;
    E.swap(new_E);
  }

  // selecting parent and storing in final edges
  vector<long long> in_weight(N+1, INF);
  vector<int> parent(N+1, -1);

  in_weight[s] = 0;
  parent[s] = 0;

  for (auto e: updated_edges) {
    if (e.v == s) continue;
    if (e.w < in_weight[e.v] || (e.w == in_weight[e.v] && (parent[e.v] == -1 || e.u < parent[e.v]))) {
      in_weight[e.v] = e.w;
      parent[e.v] = e.u;
    }
  }

  // keeping only vertex whose path reaches to root without loop
  vector<vector<pair<int, long long>>> adj_tree(N+1);

  for (int v = 1; v <= N; v++) {
    if (v == s) continue;
    if (parent[v] == -1) continue;

    int u = v;
    unordered_set<int> seen;
    bool reached_source = false;

    while (true) {
      if (u == s) {
        reached_source = true;
        break;
      }
      if (parent[u] == -1) {
        reached_source = false;
        break;
      }
      if (seen.count(u)) {
        reached_source = false;
        break;
      }

      seen.insert(u);
      u = parent[u];
    }

    if (!reached_source) {
      parent[v] = -1;
      continue;
    }

    long long w = get_edge_weight(updated_edges, parent[v], v);

    if (w >= 0) {
      total_cost += w;
      adj_tree[parent[v]].push_back({v, w});
    } else {
      parent[v] = -1;
    }
  }

  // distances from source (s) along the chosen path/cost
  vector<long long> distance(N+1, -1);
  vector<int> reachable(N+1, 0);

  distance[s] = 0;
  reachable[s] = 1;

  deque<int> dq;
  dq.push_back(s);

  while (!dq.empty()) {
    int u = dq.front();
    dq.pop_front();

    for (auto at: adj_tree[u]) {
      int v = at.first;
      int w = at.second;
      distance[v] = (distance[u] < 0 ? -1 : distance[u] + w);
      reachable[v] = (distance[v] >= 0);
      dq.push_back(v);
    }
  }

  // collecting result to return
  findings.total_cost = total_cost;
  findings.parent = std::move(parent);
  findings.reachable = std::move(reachable);
  findings.adj_tree = std::move(adj_tree);
  findings.parent[s] = 0;

  return findings;
}

// MAIN
int main() {
  int query_id;
  cin >> query_id;

  switch (query_id)
  {
  case 1: {
    int T;
    cin >> T;

    for (int t = 0; t < T; t++) {
      int N, M, s;
      cin >> N >> M >> s;

      vector<Edge> edges;
      edges.reserve(M);

      bool has_negative = false;

      for (int i = 0; i < M; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;

        if (w < 0) has_negative = true;
        
        edges.push_back({u, v, w});
      }

      if (has_negative) {
        cout << -1 << "\n";
        continue;
      }

      Arborescence_Findings af = min_arborescence(edges, N, s);

      // calculating distances from S;
      vector<long long> distance(N+1, -1);
      distance[s] = 0;
      deque<int> dq;
      dq.push_back(s);

      while(!dq.empty()) {
        int u = dq.front();
        dq.pop_front();

        for (auto t : af.adj_tree[u]) {
          int v = t.first;
          int w = t.second;
          distance[v] = (distance[u] < 0 ? -1 : distance[u] + w);
          dq.push_back(v);
        }
      }

      cout << af.total_cost;
      for (int i = 1; i <= N; i++) {
        cout << " " << distance[i];
      }
      cout << " #";
      for (int i = 1; i<= N; i++) {
        cout << " " << af.parent[i];
      }
      cout << "\n";
    }
    break;
  }
  
  default:
    break;
  }
}