#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <deque>

using namespace std;

// INF: acts like +infinity during min checks
const long long INF = 1e15;

// edge: u -> v with weight w
struct Edge {
  int u, v;
  long long w;
};

// total_cost: sum of chosen edges
// parent[i]: parent of i in the arborscence (parent[s] = 0, unreachable = -1)
// reachable[i]: 1 if reachable from s through chosen edges, else 0
// adj_tree[u]: list of (v, w) edges chosen in the final tree
struct Arborescence_Findings {
  long long total_cost = 0;
  vector<int> parent;
  vector<int> reachable;
  vector<vector<pair<int, long long>>> adj_tree;
};

// Internal edge used by DMST (0-indexed during algorithm)
// u, v: current points in the updated graph
// w: current edge weight (after updated)
// orig_u, orig_v: original point
struct DMST_Edge {
  int u, v;
  long long w;
  int orig_u, orig_v;
};


// -----------------------------------------------------------------------------
// find weight of (u -> v) from given edges
long long get_edge_weight(const vector<Edge>& edges, int u, int v) {
  for (size_t i = 0; i < edges.size(); i++) {
    if (edges[i].u == u && edges[i].v == v) {
      return edges[i].w;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
// keep only the minimum-weight edge per (u, v)
// edges: original input edges
vector<Edge> get_minimum_edge_set(const vector<Edge>& edges) {
  // min_edges[(u,v)]: smallest weight for given u,v
  map<pair<int, int>, long long> min_edges;

  // first, collect minimum weight per (u, v)
  for (const auto& e: edges) {
    pair<int, int> key = { e.u, e.v };
    if (!min_edges.count(key) || e.w < min_edges[key]) {
      min_edges[key] = e.w;
    }
  }

  // convert map back to vector of edges
  vector<Edge> updated_edges;
  updated_edges.reserve(min_edges.size());

  for (const auto& key_val: min_edges) {
    int u = key_val.first.first;
    int v = key_val.first.second;
    long long w = key_val.second;

    updated_edges.push_back({u, v, w});
  }
  // vector with one smallest edge for each pair (u, v)
  return updated_edges;
}

// -----------------------------------------------------------------------------
// dmst_edges: working edges (u,v,w) with original points kept
// returns: for each node v, index in dmst_edges of its chosen incoming edge (or -1)
vector<int> dmst(vector<DMST_Edge>& dmst_edges, int N, int s) {
  // Variables:
  // in_weight[v]: min incoming weight chosen for v
  // predecessor_label[v]: chosen predecessor label u for v
  // predecessor_edge_index[v]: index in dmst_edges of the chosen edge
  // component_id[v]: id ofcomponent v belongs to after detection
  // visited[v]: detect cycle
  // component_count: number of components formed after cycle detection
  vector<long long> in_weight(N, INF);
  vector<int> predecessor_label(N, -1);
  vector<int> predecessor_edge_index(N, -1);

  // first, pick min incoming edge for every node
  for (int i = 0; i < dmst_edges.size(); i++) {
    DMST_Edge& e = dmst_edges[i];

    if (e.u == e.v) continue;
    if (e.w < in_weight[e.v] || (e.w == in_weight[e.v] && (predecessor_label[e.v] == -1 || e.u < predecessor_label[e.v]))) {
      in_weight[e.v] = e.w;
      predecessor_label[e.v] = e.u;
      predecessor_edge_index[e.v] = i;
    }
  }

  // source has no incoming edge
  in_weight[s] = 0;
  predecessor_label[s] = -1;
  predecessor_edge_index[s] = -1;

  int component_count = 0;
  vector<int> component_id(N, -1);
  vector<int> visited(N, -1);

  // second, detect cycles among the chosen incoming edges
  for (int v = 0; v < N; v++) {
    // follow predecessor edge till souce is found, chnage visited[u] value to v
    int u = v;
    while (visited[u] != v && component_id[u] == -1 && u != s && predecessor_label[u] != -1) {
      visited[u] = v;
      u = predecessor_label[u];
    }

    // is cycle then assign same component id to all nodes in cycle
    if (u != s && predecessor_label[u] != -1 && component_id[u] == -1 && visited[u] == v) {
      for (int i = predecessor_label[u]; i != u; i = predecessor_label[i]) {
        component_id[i] = component_count;
      }
      component_id[u] = component_count++;
    }
  }

  // no cycles then return
  if (component_count == 0) return predecessor_edge_index;

  // assign id to non-cycle nodes
  for (int i = 0; i < N; i++) {
    if (component_id[i] == -1) {
      component_id[i] = component_count++;
    }
  }

  // Update source in the new graph
  int new_source = component_id[s];

  // create new graph with adjusted weights
  // new weight = original weight - in_weight[to]
  vector<DMST_Edge> new_dmst_edges;
  new_dmst_edges.reserve(dmst_edges.size());

  for (int i = 0; i < dmst_edges.size(); i++) {
    DMST_Edge e = dmst_edges[i];
    int uu = component_id[e.u];
    int vv = component_id[e.v];

    if (uu == vv) continue;

    long long ww = e.w - (in_weight[e.v] == INF ? 0 : in_weight[e.v]);

    new_dmst_edges.push_back({uu, vv, ww, e.orig_u, e.orig_v});
  }

  // performing recursion on updated graph to get the further edges
  vector<int> component_incoming_edge = dmst(new_dmst_edges, component_count, new_source);

  // Map contracted choices back to current level:
  // entry_edge_index[i]: index in dmst_edges that enters component i
  vector<int> entry_edge_index(component_count, -1);

  for (int i = 0; i < component_count; i++) {
    int new_index = component_incoming_edge[i];
    if (new_index == -1) continue;

    int uu = new_dmst_edges[new_index].u;
    int vv = new_dmst_edges[new_index].v;

    long long best_adj = (1LL << 62);

    int pick = -1;

    for (int j = 0; j < dmst_edges.size(); j++) {
      if (component_id[dmst_edges[j].u] == uu && component_id[dmst_edges[j].v] == vv) {
        long long new_w = dmst_edges[j].w - (in_weight[dmst_edges[j].v] == INF ? 0 : in_weight[dmst_edges[j].v]);

        if (new_w < best_adj) {
          best_adj = new_w;
          pick = j;
        }
      }
    }

    entry_edge_index[i] = pick;
  }

  // replaceing one edge per cycle with entering edge
  vector<int> result(N, -1);
  for (int v = 0; v < N; v++) {
    result[v] = predecessor_edge_index[v];
  }

  for (int i = 0; i < component_count; i++) {
    if (i == new_source) continue;
    int edge_index = entry_edge_index[i];

    if (edge_index == -1) continue;
    int vvv = dmst_edges[edge_index].v;

    result[vvv] = edge_index;
  }

  // return final incoming edges to node v
  return result;
}

// -----------------------------------------------------------------------------
// gives total_cost, parent[], reachable[], adj_tree[]
Arborescence_Findings min_arborescence(const vector<Edge>& edges, int N, int s) {
  // 1 calculate minimum weights for all (u, v)
  vector<Edge> min_edges_set = get_minimum_edge_set(edges);

  // 2 remove out of bound edges
  vector<Edge> tmp;
  tmp.reserve(min_edges_set.size());
  for (size_t i = 0; i < min_edges_set.size(); i++) {
    const Edge &e = min_edges_set[i];
    if (e.u >= 1 && e.u <= N && e.v >= 1 && e.v <= N && e.u != e.v) {
      tmp.push_back(e);
    }
  }

  min_edges_set.swap(tmp);

  // 3 create edge list for DMST and keep original edges
  vector<DMST_Edge> edge_list;
  edge_list.reserve(min_edges_set.size());

  for (size_t i = 0; i < min_edges_set.size(); i++) {
    const Edge &e = min_edges_set[i];
    DMST_Edge de;
    de.u = e.u - 1;
    de.v = e.v - 1;
    de.w = e.w;
    de.orig_u = e.u;
    de.orig_v = e.v;
    edge_list.push_back(de);
  }

  int n_0 = N;
  int s_0 = s - 1;

  // 4 running DMST on 0-index to chose incoming edge index for every vertex
  vector<int> chosen_index = dmst(edge_list, n_0, s_0);

  vector<int> parent(N+1, -1);
  parent[s] = 0;

  vector<vector<pair<int, long long>>> adj(N+1);
  long long total_cost = 0;

  // 5 convert chosen edges back to original graph and compute total cost
  for (int v = 0; v < n_0; v++) {
    int index = chosen_index[v];
    if (index == -1) continue;

    int a = edge_list[index].orig_u;
    int b = edge_list[index].orig_v;
    long long w = get_edge_weight(min_edges_set, a, b);

    if (w >= 0) {
      parent[b] = a;
      adj[a].push_back({b, w});
      total_cost += w;
    }
  }

  // 6 compute distances and reachability from s along tree
  vector<int> reached(N+1, 0);
  vector<long long> distance(N+1, -1);
  
  distance[s] = 0; 
  reached[s] = 1;
  deque<int> dq;
  dq.push_back(s);

  while (!dq.empty()) {
    int u = dq.front();
    dq.pop_front();

    for (size_t i = 0; i < adj[u].size(); i++) {
      int v = adj[u][i].first;
      long long w = adj[u][i].second;

      distance[v] = (distance[u] < 0 ? -1 : distance[u] + w);
      reached[v] = (distance[v] >= 0);
      dq.push_back(v);
    }
  }

  // 7 storig result to return output
  Arborescence_Findings result;
  result.total_cost = total_cost;
  result.parent = std::move(parent);
  result.reachable = std::move(reached);
  result.adj_tree = std::move(adj);
  result.parent[s] = 0;

  return result;
}

// Extra functions
vector<long long> arb_compute_distances(const vector<vector<pair<int, long long>>>& tree, int N, int s) {
  vector<long long> distance(N+1, -1);
  distance[s] = 0;
  deque<int> dq;
  dq.push_back(s);

  while(!dq.empty()) {
    int u = dq.front();
    dq.pop_front();

    for (const auto& t : tree[u]) {
      int v = t.first;
      long long w = t.second;
      distance[v] = (distance[u] < 0 ? -1 : distance[u] + w);
      dq.push_back(v);
    }
  }

  return distance;
}

void arb_print_output(const Arborescence_Findings& af, int N, int s) {
  vector<long long> distance = arb_compute_distances(af.adj_tree, N, s);

  cout << af.total_cost;
  for (int i = 1; i <= N; i++) cout << " " << distance[i];
  cout << " #";
  for (int i = 1; i <= N; i++) cout << " " << af.parent[i];
  cout << "\n";
}

void arb_solution() {
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

    arb_print_output(af, N, s);
  }
}

// -----------------------------------------------------------------------------
// MAIN
int main() {
  int query_id;
  cin >> query_id;

  switch (query_id) {
    case 1: 
      arb_solution();
    break;
  
    default:
    break;
  }
}