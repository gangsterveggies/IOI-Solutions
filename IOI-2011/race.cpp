///////////////////////////////////////////////
//
//  _____    ____    _____      ___     ___    __   __ 
// |_   _|  / __ \  |_   _|    |__ \   / _ \  /_ | /_ |
//   | |   | |  | |   | |         ) | | | | |  | |  | |
//   | |   | |  | |   | |        / /  | | | |  | |  | |
//  _| |_  | |__| |  _| |_      / /_  | |_| |  | |  | |
// |_____|  \____/  |_____|    |____|  \___/   |_|  |_|
//
//
// Year: IOI'2011
// Problem: Race, Day 1
// Solution: 100 Points, O(NlogN)
// Author: Pedro Paredes
//
///////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>

using namespace std;

typedef pair<int, int> pii;

#define MAXN 200050
#define MAXK 1000050

#define F first
#define S second

int N, K, global_answer; // Input and result variables
int split_node, current_max; // Variables to calculate centroid
int book_keeping; // Book keeping helper

int H[MAXN][2]; // Input variables
int L[MAXN];

int processed[MAXN]; // Markers to help main recursion
int size[MAXN]; // Size of subtrees in rooted tree
int achievable[MAXK]; // Helper arrays for minimum paths crossing v
int minimum_paths[MAXK];

vector<pii> neighbors[MAXN]; // The actual tree

///////////////////////////////////////////////
//
// Goal: Calculate the size of each subtree
//
///////////////////////////////////////////////
void calc_size(int current, int parent)
{
  size[current] = 0;

  // Recurse on unprocessed nodes and update size
  int i;
  for (i = 0; i < (int)neighbors[current].size(); i++)
    if (!processed[neighbors[current][i].F] && neighbors[current][i].F != parent)
    {
      calc_size(neighbors[current][i].F, current);
      size[current] += 1 + size[neighbors[current][i].F];
    }
}

///////////////////////////////////////////////
//
// Goal: Calculate the centroid
//
///////////////////////////////////////////////
void select_split_node(int current, int parent, int total)
{
  int node_max = (total - size[current] - 1);

  // Recurse on unprocessed nodes updating the maximum subtree on node_max
  int i;
  for (i = 0; i < (int)neighbors[current].size(); i++)
    if (!processed[neighbors[current][i].F] && neighbors[current][i].F != parent)
    {
      select_split_node(neighbors[current][i].F, current, total);
      node_max = max(node_max, 1 + size[neighbors[current][i].F]);
    }

  if (node_max < current_max)
  {
    split_node = current;
    current_max = node_max;
  }
}

///////////////////////////////////////////////
//
// Goal: DFS from the centroid to calculate all paths
//
///////////////////////////////////////////////
void dfs_from_node(int current, int parent, int current_cost, int current_length, int fill)
{
  if (current_cost > K)
    return;

  if (!fill) // If we are calculating the paths
  {
    if (achievable[K - current_cost] == book_keeping)
      if (current_length + minimum_paths[K - current_cost] < global_answer || global_answer == -1)
        global_answer = current_length + minimum_paths[K - current_cost];

    if (current_cost == K)
      if (current_length < global_answer || global_answer == -1)
        global_answer = current_length;
  }
  else // If we are filling the helper array
  {
    if (achievable[current_cost] < book_keeping)
    {
      achievable[current_cost] = book_keeping;
      minimum_paths[current_cost] = current_length;
    }
    else if (current_length < minimum_paths[current_cost])
    {
      achievable[current_cost] = book_keeping;
      minimum_paths[current_cost] = current_length;
    }
  }

  // Recurse on unprocessed nodes
  int i;
  for (i = 0; i < (int)neighbors[current].size(); i++)
    if (!processed[neighbors[current][i].F] && neighbors[current][i].F != parent)
      dfs_from_node(neighbors[current][i].F, current, current_cost + neighbors[current][i].S, current_length + 1, fill);
}

///////////////////////////////////////////////
//
// Goal: Calculate best for subtree
//
///////////////////////////////////////////////
void process(int current)
{
  // Fill the size array
  calc_size(current, -1);

  // Base case
  if (size[current] <= 1)
    return;

  // Calculate the centroid and put it in split_node
  split_node = -1;
  current_max = size[current] + 3;
  select_split_node(current, -1, size[current] + 1);

  // Double dfs to calculate minimums and fill helper array
  book_keeping++;
  int i;
  for (i = 0; i < (int)neighbors[split_node].size(); i++)
    if (!processed[neighbors[split_node][i].F])
    {
      dfs_from_node(neighbors[split_node][i].F, split_node, neighbors[split_node][i].S, 1, 0);
      dfs_from_node(neighbors[split_node][i].F, split_node, neighbors[split_node][i].S, 1, 1);
    }


  int local_split_node = split_node; // Since split_node is global
  processed[split_node] = 1; // Mark as processed to cap recursion

  // Call main method on each subtree from centroid
  for (i = 0; i < (int)neighbors[local_split_node].size(); i++)
    if (!processed[neighbors[local_split_node][i].F])
      process(neighbors[local_split_node][i].F);
}

///////////////////////////////////////////////
//
// Goal: Answer the task
//
///////////////////////////////////////////////
int best_path(int _N, int _K, int H[][2], int L[])
{
  // Reset arrays and variables
  memset(processed, 0, sizeof processed);
  memset(achievable, 0, sizeof achievable);
  memset(minimum_paths, 0, sizeof minimum_paths);
  N = _N;
  K = _K;
  book_keeping = 0;

  // Build tree
  int i;
  for (i = 0; i < N - 1; i++)
  {
    neighbors[H[i][0]].push_back(pii(H[i][1], L[i]));
    neighbors[H[i][1]].push_back(pii(H[i][0], L[i]));
  }

  global_answer = -1;

  // Call main method for whole tree
  process(0);

  return global_answer;
}

///////////////////////////////////////////////
//
// Goal: Read the input
//
///////////////////////////////////////////////
void read_input()
{
  scanf("%d %d", &N, &K);

  int i;
  for (i = 0; i < N - 1; i++)
    scanf("%d %d %d", &H[i][0], &H[i][1], &L[i]);
}

///////////////////////////////////////////////
//
// Goal: Main
//
///////////////////////////////////////////////
int main()
{
  int ans;

  read_input();
  ans = best_path(N, K, H, L);

  printf("%d\n", ans);

  return 0;
}
