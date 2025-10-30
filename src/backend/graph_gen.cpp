// Created by IWOFLEUR on 19.10.2025

#include "../../include/backend/graph_gen.h"

#include <chrono>
#include <stack>

Graph create_graph(const int n, const double edgeProb, const double loopProb, const unsigned int seed) {
    Graph graph;
    graph.n = n;

    // Формируем матрицу -> выводим -> все подряд
    // Matrix memory allocating
    graph.adj_matrix = new int*[n];
    for (int i = 0; i < n; i++) {
        graph.adj_matrix[i] = new int[n];
        for (int j = 0; j < n; j++) {
            graph.adj_matrix[i][j] = 0;
        }
    }

    // List initialization
    graph.adj_list.resize(n);

    static unsigned int counter = 0;
    const auto now = std::chrono::high_resolution_clock::now();
    const auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    unsigned int state = seed == 0 ? static_cast<unsigned int>(nanos) + counter++ : seed;

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            state = (state * 1664525 + 1013904223) & 0x7fffffff;
            const int rand_value = static_cast<int>(state) % 100;

            if (i == j) {
                if (rand_value < static_cast<int>(loopProb * 100)) {
                    graph.adj_matrix[i][j] = 1;
                    graph.adj_list[i].push_back(i);
                }
            } else {
                if (rand_value < static_cast<int>(edgeProb * 100)) {
                    graph.adj_matrix[i][j] = graph.adj_matrix[j][i] = 1;
                    graph.adj_list[i].push_back(j);
                    graph.adj_list[j].push_back(i);
                }
            }
        }
    }

    return graph;
}

void print_matrix(int **matrix, const int rows, const int cols, const char *name) {
    if (!matrix || rows <= 0 || cols <= 0) {
        std::cout << "Invalid matrix parameters" << std::endl;
        return;
    }

    std::cout << name << ":" << std::endl;

    // Calculate maximum width needed for numbers
    int max_num_width = 1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (const int num_width = static_cast<int>(std::to_string(matrix[i][j]).length()); num_width > max_num_width) {
                max_num_width = num_width;
            }
        }
    }

    // Calculate width for row indices
    const int row_index_width = static_cast<int>(std::to_string(rows - 1).length());
    max_num_width = std::max(max_num_width, 2);

    // Print column headers with dynamic spacing
    std::cout << std::setw(row_index_width + 2) << " ";
    for (int j = 0; j < cols; j++) {
        std::cout << std::setw(max_num_width + 1) << j;
    }
    std::cout << std::endl;

    // Print separator line
    std::cout << std::setw(row_index_width + 2) << " " << "+";
    for (int j = 0; j < cols; j++) {
        std::cout << std::string(max_num_width + 1, '-');
    }
    std::cout << std::endl;

    // Print matrix rows with borders
    for (int i = 0; i < rows; i++) {
        std::cout << std::setw(row_index_width) << i << " |";
        for (int j = 0; j < cols; j++) {
            std::cout << std::setw(max_num_width + 1) << matrix[i][j];
        }
        std::cout << std::endl;
    }
}

void delete_graph(Graph& graph, const int n) {
    for (int i = 0; i < n; i++) {
        delete[] graph.adj_matrix[i];
    }
    delete[] graph.adj_matrix;
    graph.adj_matrix = nullptr;
    graph.n = 0;
    graph.adj_list.resize(0);
}

void print_list(const std::vector<std::vector<int> > &list, const char* name) {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < list.size(); i++) {
        std::cout << i << ": ";
        for (const int neigh : list[i]) {
            std::cout << neigh << " ";
        }
        std::cout << std::endl;
    }
}

void DFS(const int v, const Graph &graph, bool *visited, const bool is_recursive) {
    if (is_recursive == true) {
        visited[v] = true;

        std::cout << std::setw(3) << v << " ";

        for (int i = 0; i < graph.n; i++) {
            if (graph.adj_matrix[v][i] == 1 && visited[i] == false) DFS(i, graph, visited, is_recursive);
        }
    } else {
        std::stack<int> stack;
        stack.push(v);

        while (!stack.empty()) {
            const int current = stack.top();
            stack.pop();

            if (visited[current] == false) {
                visited[current] = true;
                std::cout << std::setw(3) << current << " ";

                for (int i = graph.n - 1; i >= 0; i--) {
                    if (graph.adj_matrix[current][i] == 1 && visited[i] == false) {
                        stack.push(i);
                    }
                }
            }
        }
    }
}

void prep(const Graph& graph, const int vert, const bool is_recursive) {
    const auto visited = new bool[graph.n];
    for (int i = 0; i < graph.n; i++) {
        visited[i] = false;
    }
    if (is_recursive == true) {
        for (int v = vert; v < graph.n; v++) {
            if (visited[v] == false) {
                DFS(v, graph, visited, is_recursive);
            }
        }
    } else DFS(vert, graph, visited, is_recursive);

    std::cout << std::endl;
    delete[] visited;
}

void DFS_list(const int v, const Graph &graph, bool *visited, const bool is_recursive) {
    if (is_recursive == true) {
        visited[v] = true;
        std::cout << std::setw(3) << v << " ";

        for (const int neighbour : graph.adj_list[v]) {
            if (visited[neighbour] == false) {
                DFS_list(neighbour, graph, visited, is_recursive);
            }
        }
    } else {
        std::stack<int> stack;
        stack.push(v);

        while (!stack.empty()) {
            const int current = stack.top();
            stack.pop();
            if (visited[current] == false) {
                visited[current] = true;
                std::cout << std::setw(3) << current << " ";

                const auto& neighbours = graph.adj_list[current];
                for (int i = static_cast<int>(neighbours.size()) - 1; i >= 0; i--) {
                    if (int neighbour = neighbours[i]; visited[neighbour] == false) stack.push(neighbour);
                }
            }
        }
    }
}

void prep_list(const Graph &graph, const int vert, const bool is_recursive) {
    const auto visited = new bool[graph.n];
    for (int i = 0; i < graph.n; i++) {
        visited[i] = false;
    }
    if (is_recursive == true) {
        for (int v = vert; v < graph.n; v++) {
            if (visited[v] == false) {
                DFS_list(v, graph, visited, is_recursive);
            }
        }
    } else DFS_list(vert, graph, visited, is_recursive);

    std::cout << std::endl;
    delete[] visited;
}
