#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <deque>
#include <string>
#include <array>
#include <limits>
#include <functional>

using namespace std;

// INF: acts like +infinity during min checks
const long long INF = 1e15;

// 1. ARBORSCENCE PROBLEM ======================================================

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

// Internal edge used by DMST (1-indexed during algorithm)
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
  vector<long long> in_weight(N+1, INF);
  vector<int> predecessor_label(N+1, -1);
  vector<int> predecessor_edge_index(N+1, -1);

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
  vector<int> component_id(N+1, -1);
  vector<int> visited(N+1, -1);

  // second, detect cycles among the chosen incoming edges
  for (int v = 1; v <= N; v++) {
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
  for (int i = 1; i <= N; i++) {
    if (component_id[i] == -1) {
      component_id[i] = component_count++;
    }
  }

  // Update source in the new graph + 1-indexing
  for (int i = 1; i <= N; i++) {
    component_id[i] = component_id[i] + 1;
  }
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
  vector<int> entry_edge_index(component_count + 1, -1);

  for (int i = 1; i <= component_count; i++) {
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
  vector<int> result(N+1, -1);
  for (int v = 1; v <= N; v++) {
    result[v] = predecessor_edge_index[v];
  }

  for (int i = 1; i <= component_count; i++) {
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
    de.u = e.u;
    de.v = e.v;
    de.w = e.w;
    de.orig_u = e.u;
    de.orig_v = e.v;
    edge_list.push_back(de);
  }

  // 4 running DMST on 1-index to chose incoming edge index for every vertex
  vector<int> chosen_index = dmst(edge_list, N, s);

  vector<int> parent(N+1, -1);
  parent[s] = 0;

  vector<vector<pair<int, long long>>> adj(N+1);
  long long total_cost = 0;

  // 5 convert chosen edges back to original graph and compute total cost
  for (int v = 1; v <= N; v++) {
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

// =============================================================================
// 2. HUFFMAN ==================================================================

// to build tree
struct Huff_Node {
  char ch;
  long long freq;
  Huff_Node* left;
  Huff_Node* right;
  char min_char;
  bool is_leaf;
};

// custom huffman comparator - 1st with freq otherwise with smaller char
bool huff_compare_nodes(const Huff_Node* a, const Huff_Node* b) {
  if (a->freq != b->freq) {
    return a->freq > b->freq;
  }
  return a->min_char > b->min_char;
}

// counting the frequency
map<char, long long> huff_freq(const string& s) {
  map<char, long long> freq;
  for (char ch: s) {
    freq[ch]++;
  }

  return freq;
}

// huffman tree
Huff_Node* huff_build_tree(const map<char, long long>& freq) {
  priority_queue<Huff_Node*, vector<Huff_Node*>, std::function<bool(const Huff_Node*, const Huff_Node*)>> pq(huff_compare_nodes);

  for (auto& key_val: freq) {
    Huff_Node* n = new Huff_Node();
    n->ch = key_val.first;
    n->freq = key_val.second;
    n->left = nullptr;
    n->right = nullptr;
    n->min_char = key_val.first;
    n->is_leaf = true;

    pq.push(n);
  }

  // if only we have single char
  if (pq.size() == 1) {
    return pq.top();
  }

  // merging
  while (pq.size() > 1) {
    Huff_Node* a = pq.top();
    pq.pop();
    Huff_Node* b = pq.top();
    pq.pop();

    // putting lexicographic smaller tree on left
    if (a->min_char > b->min_char) {
      swap(a, b);
    }

    Huff_Node* p = new Huff_Node();
    p->ch = '\0';
    p->freq = a->freq + b->freq;
    p->left = a;
    p->right = b;
    p->min_char = min(a->min_char, b->min_char);
    p->is_leaf = false;

    pq.push(p);
  }

  return pq.empty() ? nullptr : pq.top();
}

// using dfs logic to create coding
void huff_dfs(Huff_Node* node, string& path, array<string, 256>& code_of) {
  if (!node) return;
  if (node->is_leaf) {
    if (path.empty()) {
      code_of[(unsigned char)node->ch] = "0"; // only 1 char
    } else {
      code_of[(unsigned char)node->ch] = path;
    }
    return;
  }

  path.push_back('0');
  huff_dfs(node->left, path, code_of);
  path.pop_back();

  path.push_back('1');
  huff_dfs(node->right, path, code_of);
  path.pop_back();
}

array<string, 256> huff_build_codes(Huff_Node* root) {
  array<string, 256> code_of;
  for (auto& s: code_of) s.clear();
  
  string path;
  huff_dfs(root, path, code_of);
  
  return code_of;
}

// encoding
string huff_encode(const string& s, const array<string, 256>& code_of) {
  string encoded;
  encoded.reserve(s.size());
  
  for (char ch: s) {
    encoded += code_of[(unsigned char)ch];
  }

  return encoded;
}

// decoding
string huff_decode(const string& bits, Huff_Node* root) {
  if (!root) return "";
  if (root->is_leaf) return string(bits.size(), root->ch);

  string decoded;
  Huff_Node* curr = root;

  for (char b: bits) {
    curr = (b == '0') ? curr->left : curr->right;

    if (curr->is_leaf) {
      decoded.push_back(curr->ch);
      curr = root;
    }
  }

  return decoded;
}

// free memory
void huff_free(Huff_Node* node) {
  if (!node) return;
  huff_free(node->left);
  huff_free(node->right);
  delete node;
}

// print huffman output
void huff_print(const map<char, long long>& freq, const array<string, 256>& code_of, const string& encoded, const string& decoded) {
  cout << "Codes:\n";
  for (auto& key_val: freq) {
    char c = key_val.first;
    cout << c << ": " << code_of[(unsigned char)c] << "\n";
  }

  cout << "Encoded: " << encoded << "\n";
  cout << "Decoded: " << decoded << "\n\n";
}

// huffman solution
void huff_sol() {
  int T;
  cin >> T;
  string temp;
  getline(cin, temp);

  for (int t = 0; t < T; t++) {
    string s;
    getline(cin, s);

    map<char, long long> freq = huff_freq(s);

    Huff_Node* root = nullptr;
    if (!freq.empty()) {
      root = huff_build_tree(freq);
    }

    array<string, 256> code_of = huff_build_codes(root);
    string encoded = huff_encode(s, code_of);
    string decoded = huff_decode(encoded, root);

    huff_print(freq, code_of, encoded, decoded);
    huff_free(root);
  }
}


// =============================================================================
// 2. MAX FLOW =================================================================

// struct to store data
struct MaxFlow_Result {
  long long max_flow;
  vector<vector<long long>> residual;
  vector<vector<int>> adj;
  vector<int> parent;
  vector<int> reachable;
};

struct BFS_Result {
  long long bottleneck;
  vector<int> parent;
};

// create augment graph using bfs
BFS_Result mf_bfs(const vector<vector<int>>& adj, const vector<vector<long long>>& capacity, int N, int s, int t) {
  BFS_Result res;
  res.bottleneck = 0;
  res.parent.assign(N+1, -1);
  res.parent[s] = -2;

  queue<pair<int, long long>> q;
  q.push({s, INF});

  while (!q.empty()) {
    int u = q.front().first;
    long long flow = q.front().second;
    q.pop();

    for (auto v: adj[u]) {
      if (res.parent[v] == -1 && capacity[u][v] > 0) {
        res.parent[v] = u;
        long long new_flow = min(flow, capacity[u][v]);

        if (v == t) {
          res.bottleneck = new_flow;
          return res;
        }
        q.push({v, new_flow});
      }
    }
  }
  return res;
}

MaxFlow_Result mf_calculate_max_flow(const vector<vector<int>>& adj, const vector<vector<long long>>& capacity, int N, int s, int t) {
  MaxFlow_Result res;
  res.max_flow = 0;
  res.adj = adj;
  res.residual = capacity;
  res.parent.assign(N+1, -1);
  res.reachable.assign(N+1, 0);

  while (true) {
    BFS_Result step = mf_bfs(res.adj, res.residual, N, s, t);

    // if no augment path available
    if (step.bottleneck == 0) {
      res.parent = step.parent;
      break;
    }

    res.max_flow += step.bottleneck;

    // going back from t to s + update capacities
    int curr = t;

    while (curr != s) {
      int prev = step.parent[curr];
      res.residual[prev][curr] -= step.bottleneck;
      res.residual[curr][prev] += step.bottleneck;
      curr = prev;
    }
  }

  // marking the min-cut side coming from source 's'
  queue<int> q;
  vector<int> visited(N+1, 0);

  q.push(s);
  visited[s] = 1;

  while (!q.empty()) {
    int u = q.front();
    q.pop();

    res.reachable[u] = 1;

    for (auto v: res.adj[u]) {
      if (!visited[v] && res.residual[u][v] > 0) {
        visited[v] = 1;
        q.push(v);
      }
    }
  }

  return res;
}

// Max Flow solution
void mf_solution() {
  int N, m;
  cin >> N >> m;

  vector<vector<long long>> capacity(N+1, vector<long long>(N+1, 0));
  vector<vector<int>> adj(N+1);
  vector<vector<char>> visited (N+1, vector<char>(N+1, 0)); // to avoid duplicates

  for (int i = 0; i < m; i++) {
    int a, b;
    long long c;

    cin >> a >> b >> c;

    capacity[a][b] += c; // merging of parallel edges

    if (!visited[a][b]) {
      adj[a].push_back(b);
      visited[a][b] = 1;
    }

    if (!visited[b][a]) {
      adj[b].push_back(a);
      visited[b][a] = 1;
    }
  }

  int s = 1;
  int t = N;

  MaxFlow_Result res = mf_calculate_max_flow(adj, capacity, N, s, t);

  cout << res.max_flow << "\n";
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

    case 2:
      huff_sol();
    break;

    case 3:
      mf_solution();
    break;
  
    default:
    break;
  }
}