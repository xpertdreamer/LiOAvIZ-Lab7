//
// Created by IWOFLEUR on 29.10.2025.
//

#ifndef GRAPH_GEN_H
#define GRAPH_GEN_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

struct Graph {
    int** adj_matrix;
    std::vector<std::vector<int>> adj_list;
    int n;
};

/**
 * Function for allocating memory for a graph with edge generating probabilities
 * @param n Graph size
 * @param edgeProb Edge generating probability
 * @param loopProb Loop edge generating probability
 * @param seed Seed for random generator
 * @return New Graph
 */
extern Graph create_graph(int n, double edgeProb = 0.4, double loopProb = 0.15, unsigned int seed = 0);

// Function to display the matrix
extern void print_matrix(int **matrix, int rows, int cols, const char *name);

// Free matrix memory
extern void delete_graph(Graph& graph, int n);

// Display adj list
extern void print_list(const std::vector<std::vector<int>> &list, const char *name);

/**
 * Depth-first search algorithm
 * @param v Vertex
 * @param graph Graph
 * @param visited Array of visited vertices
 */
extern void DFS(int v, const Graph& graph, bool* visited);

// Preparation algorithm for DFS
extern void prep(const Graph& graph, int vert);

// Preparation algorithm for DFS (list representation)
extern void prep_list(const Graph& graph, int vert);

/**
 * Depth-first search algorithm for adjacency list
 * @param v Vertex
 * @param graph Graph
 * @param visited Array of visited vertices
 */
extern void DFS_list(int v, const Graph& graph, bool* visited);
#endif //GRAPH_GEN_H
