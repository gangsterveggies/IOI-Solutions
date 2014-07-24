///////////////////////////////////////////////
//
//  _____    ____    _____     ___     ___    __   ___  
// |_   _|  / __ \  |_   _|    |__ \   / _ \  /_ | |__ \ 
//   | |   | |  | |   | |         ) | | | | |  | |    ) |
//   | |   | |  | |   | |        / /  | | | |  | |   / / 
//  _| |_  | |__| |  _| |_      / /_  | |_| |  | |  / /_ 
// |_____|  \____/  |_____|    |____|  \___/   |_| |____|                                                    
//
//
// Year: IOI'2012
// Problem: Ideal City, Day 2
// Solution: 100 Points, O(N)
// Author: Pedro Paredes
//
///////////////////////////////////////////////

#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <map>

using namespace std;

#define MOD 1000000000
#define MAX_N 100050
#define MAX_VL ((1LL)<<31)
#define F first
#define S second

typedef pair<int, int> pii;
typedef pair<int, pii> triple;
typedef long long int lld;

struct node
{
  int weight, index;
  node *parent;
  vector<node *> childs;
} typedef node;

node *root_H, *root_V; // A pointer for the horizontal tree and another for the vertical

int list_H[MAX_N];
int list_V[MAX_N];
int tree_indexes;

vector<int> graph[MAX_N];
int visited[MAX_N];
int weight[MAX_N];
lld sum[MAX_N];

triple pts_x[MAX_N]; // this list will be used to sort by x: triple(X, Y, index)
triple pts_y[MAX_N]; // this list will be used to sort by y: triple(Y, X, index)
map<lld, int> pts; // this map will be used to know the index of a certain point (x, y) hashed to: X + Y * MAX_VL

///////////////////////////////////////////////
//
// Goal: Root the tree-graph into a tree using the node struct
//
///////////////////////////////////////////////
void dfs(node *cur, int vertex)
{
  cur->weight = weight[vertex];
  cur->index = tree_indexes++;
  visited[vertex] = 1;

  int i;
  for (i = 0; i < (int)graph[vertex].size(); i++)
    if (!visited[graph[vertex][i]])
    {
      node *n_node = new node();
      n_node->parent = cur;
      cur->childs.push_back(n_node);
      dfs(n_node, graph[vertex][i]);
    }
}

///////////////////////////////////////////////
//
// Goal: Calculate the sum of weight of all nodes in a subtree rooted in 'cur'
//
///////////////////////////////////////////////
void calc_sum(node *cur)
{
  int sm = cur->weight;
  
  int i;
  for (i = 0; i < (int)cur->childs.size(); i++)
  {
    calc_sum(cur->childs[i]);
    sm += sum[cur->childs[i]->index];
  }

  sum[cur->index] = sm;
}


///////////////////////////////////////////////
//
// Goal: Answer the task
//
///////////////////////////////////////////////
int DistanceSum(int N, int *X, int *Y)
{
  // Fill the 3 point lists as described above
  // We need the index to map points to nodes in each tree
  int i;
  for (i = 0; i < N; i++)
  {
    pts_x[i] = triple(X[i], pii(Y[i], i));
    pts_y[i] = triple(Y[i], pii(X[i], i));
    pts[X[i] + MAX_VL * Y[i]] = i;
  }

  // Sort by X and Y
  sort(pts_x, pts_x + N);
  sort(pts_y, pts_y + N);

  // Construct a graph for the horizontal tree
  for (i = 0; i < N; i++)
  {
    graph[i].clear();
    weight[i] = 0;
  }

  int num_nodes = 1;
  list_H[pts_x[0].S.S] = 0;
  weight[0] = 1;

  for (i = 1; i < N; i++)
  {
    if (pts_x[i].F == pts_x[i - 1].F && pts_x[i].S.F == pts_x[i - 1].S.F + 1)
    {
      weight[list_H[pts_x[i - 1].S.S]]++;
      list_H[pts_x[i].S.S] = list_H[pts_x[i - 1].S.S];
    }
    else
    {
      weight[num_nodes] = 1;
      list_H[pts_x[i].S.S] = num_nodes++;
    }

    if (pts.find(pts_x[i].F - 1 + MAX_VL * pts_x[i].S.F) != pts.end())
    {
      graph[list_H[pts_x[i].S.S]].push_back(list_H[pts[pts_x[i].F - 1 + MAX_VL * pts_x[i].S.F]]);
      graph[list_H[pts[pts_x[i].F - 1 + MAX_VL * pts_x[i].S.F]]].push_back(list_H[pts_x[i].S.S]);
    }
  }

  // Use the graph to construct a rooted tree
  tree_indexes = 0;
  root_H = new node();
  memset(visited, 0, sizeof visited);
  
  dfs(root_H, 0);

  // Calculate the sum of weight per sub-tree
  memset(sum, 0, sizeof sum);
  calc_sum(root_H);

  lld t_sum = sum[0];
  lld ans = 0;

  // Use the method of considering each edge to calculate answer
  for (i = 1; i < tree_indexes; i++)
  {
    ans += sum[i] * (t_sum - sum[i]) % MOD;
    ans %= MOD;
  }

  // Construct a graph for the vertical tree
  for (i = 0; i < N; i++)
  {
    graph[i].clear();
    weight[i] = 0;
  }

  num_nodes = 1;
  list_V[pts_x[0].S.S] = 0;
  weight[0] = 1;

  for (i = 1; i < N; i++)
  {
    if (pts_y[i].F == pts_y[i - 1].F && pts_y[i].S.F == pts_y[i - 1].S.F + 1)
    {
      weight[list_V[pts_y[i - 1].S.S]]++;
      list_V[pts_y[i].S.S] = list_V[pts_y[i - 1].S.S];
    }
    else
    {
      weight[num_nodes] = 1;
      list_V[pts_y[i].S.S] = num_nodes++;
    }

    if (pts.find(pts_y[i].S.F + MAX_VL * (pts_y[i].F - 1)) != pts.end())
    {
      graph[list_V[pts_y[i].S.S]].push_back(list_V[pts[pts_y[i].S.F + MAX_VL * (pts_y[i].F - 1)]]);
      graph[list_V[pts[pts_y[i].S.F + MAX_VL * (pts_y[i].F - 1)]]].push_back(list_V[pts_y[i].S.S]);
    }
  }

  // Use the graph to construct a rooted tree
  tree_indexes = 0;
  root_V = new node();
  memset(visited, 0, sizeof visited);
  
  dfs(root_V, 0);

  // Calculate the sum of weight per sub-tree
  memset(sum, 0, sizeof sum);
  calc_sum(root_V);

  t_sum = sum[0];

  // Use the method of considering each edge to calculate answer
  for (i = 1; i < tree_indexes; i++)
  {
    ans += sum[i] * (t_sum - sum[i]) % MOD;
    ans %= MOD;
  }
  
  return ans;
}
