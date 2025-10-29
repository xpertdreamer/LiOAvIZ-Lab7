#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define NOCRYPT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#endif

#include "../include/adapters/console_adapter.h"
#include "../include/backend/graph_gen.h"

#include <filesystem>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

GraphConsoleAdapter::GraphConsoleAdapter(const std::string& config_path, const std::string& aliases_path): graphs_created(false), graph(nullptr), n(0) {
    // const std::string config_file = ("../../resources/config_files/graph_console.conf");
    // const std::string aliases_file = ("../../resources/config_files/aliases.conf");

    std::string actual_config_path = config_path;
    std::string actual_aliases_path = aliases_path;

    if (actual_config_path.empty()) {
        actual_config_path = find_config_file("graph_console.conf", {
            "./resources/config_files/",
            "../resources/config_files/",
            "../../resources/config_files/",
            "config/",
            "../config/",
            "./",
            get_default_config_path()
        });
    }

    if (actual_aliases_path.empty()) {
        actual_aliases_path = find_config_file("aliases.conf", {
            "./resources/config_files/",
            "../resources/config_files/",
            "../../resources/config_files/",
            "config/",
            "../config/",
            "./",
            get_default_config_path()
        });
    }

    console.load_config(actual_config_path);
    console.load_aliases(actual_aliases_path);

    register_graph_commands();
}

GraphConsoleAdapter::~GraphConsoleAdapter() {
    cleanup();
}

void GraphConsoleAdapter::run() {
    console.run();
}

void GraphConsoleAdapter::cleanup() {
    if (graph != nullptr) {
        delete_graph(*graph, graph->n);
        graph.reset();
    }
    n = 0;
    graphs_created = false;
}


std::string GraphConsoleAdapter::find_config_file(const std::string &filename, const std::vector<std::string> &search_paths) {
    for (const auto& path : search_paths) {
        if (std::string full_path = path + filename; fs::exists(full_path)) {
            return full_path;
        }
    }

    return search_paths[0] + filename;
}

std::string GraphConsoleAdapter::get_default_config_path() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return std::string(path) + "/graph_console/";
    }
    return "./";
#else
    const char* home = getenv("HOME");
    if (home == nullptr) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return std::string(home) + "/.config/graph_console/";
#endif
}

void GraphConsoleAdapter::register_graph_commands() {
    console.register_command("create",
            [this](const std::vector<std::string>& args) { this->cmd_create(args); },
            "Create a new graph system",
            {"vertices", "edge_probability", "loop_probability"},
            "create <n> <edgeProb> <loopProb>"
        );

    console.register_command("print",
        [this](const std::vector<std::string>&) { this->cmd_print(); },
        "Print current graph system"
    );

    console.register_command("clear",
        [this](const std::vector<std::string>&) { this->cmd_clear(); },
        "Clear console screen"
    );

    console.register_command("cleanup",
        [this](const std::vector<std::string>&) { this->cmd_cleanup(); },
        "Cleanup graph system and free memory"
    );

    console.register_command("help",
       [this](const std::vector<std::string>& args) { this->cmd_help(args); },
       "Show help for commands",
       {"command"},
       "help [command]"
   );

    console.register_command("exit",
        [this](const std::vector<std::string>&) { this->cmd_exit(); },
        "Exit the application"
    );

    console.register_command("history",
        [this](const std::vector<std::string>&) { this->cmd_history(); },
            "Show history of commands"
    );

    console.register_command("DFS",
        [this](const std::vector<std::string>& args) { this->cmd_traversal(args); },
        "DFS traversal",
        {"vertex, --representation"},
        "DFS <v> <--representation>"
    );
}

void GraphConsoleAdapter::cmd_create(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Usage: create <n> <edgeProb> <loopProb>" << std::endl;
        return;
    }

    try {
        const int new_n = std::stoi(args[0]);
        const double new_edge_prob = std::stod(args[1]);
        const double new_loop_prob = std::stod(args[2]);

        if (new_n <= 0) {
            std::cout << "Invalid number of vertices." << std::endl;
            return;
        }
        if (new_edge_prob <= 0 || new_edge_prob > 1 || new_loop_prob <= 0 || new_loop_prob > 1) {
            std::cout << "Probabilities must be between 0 and 1" << std::endl;
            return;
        }

        cleanup();

        n = new_n;
        graph = std::make_unique<Graph>(create_graph(n, new_edge_prob, new_loop_prob, 0));
        graphs_created = true;

        std::cout << "Created two graphs with " << n << " vertices" << std::endl;
        std::cout << "  Edge probability: " << new_edge_prob << ", Loop probability: " << new_loop_prob << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error creating graphs: " << e.what() << std::endl;
        std::cout << "Usage: create <vertices> <edge_probability> <loop_probability>" << std::endl;
    }
}

void GraphConsoleAdapter::cmd_print() const {
    if (!graphs_created) {
        std::cout << "No graphs created. Use 'create' command first." << std::endl;
        return;
    }

    std::cout << "=== GRAPH 3 ===" << std::endl;
    print_matrix(graph->adj_matrix, graph->n, graph->n, "Adjacency Matrix 3");
    print_list(graph->adj_list, "Adjacency List 3");
}

void GraphConsoleAdapter::cmd_clear() {
    console.clear_screen();
}

void GraphConsoleAdapter::cmd_cleanup() {
    cleanup();
    std::cout << "Graph system cleaned up" << std::endl;
}

void GraphConsoleAdapter::cmd_exit() {
    cleanup();
    console.stop();
}

void GraphConsoleAdapter::cmd_help(const std::vector<std::string>& args) {
    if (args.empty()) {
        console.print_help();
    } else {
        console.show_command_help(args[0]);
    }
}

void GraphConsoleAdapter::cmd_history() {
    console.show_history();
}

void GraphConsoleAdapter::cmd_traversal(const std::vector<std::string> &args) const {
    if (!graphs_created) {
        std::cout << "No graphs created. Use 'create' command first." << std::endl;
        return;
    }

    if (args.size() < 2 || args.size() > 2) {
        std::cout << "Usage: DFS <v> <representation>" << std::endl;
        return;
    }

    try {
        const auto v = std::stoi(args[0]);
        const auto& rep = args[1];
        if (v > graph->n || v < 0) {
            std::cout << "Invalid number of vertices." << std::endl;
            return;
        }
        if (rep != "--list" && rep != "--matrix") {
            std::cout << "Invalid representation." << std::endl;
            return;
        }
        rep == "--matrix" ? prep(*graph, v) : prep_list(*graph, v);
    } catch (const std::exception& e) {
        std::cout << "Error DFS: " << e.what() << std::endl;
    }
}