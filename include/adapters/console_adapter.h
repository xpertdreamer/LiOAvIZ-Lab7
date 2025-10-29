//
// Created by IWOFLEUR on 13.10.2025.
//

#ifndef CONSOLE_ADAPTER_H
#define CONSOLE_ADAPTER_H

#include <memory>

#include "../core/console.h"
#include "backend/graph_gen.h"

class GraphConsoleAdapter {
    public:
    explicit GraphConsoleAdapter(const std::string& config_path = "", const std::string& aliases_path = "");
    ~GraphConsoleAdapter();

    void run();

    private:
    Console console;

    bool graphs_created;
    std::unique_ptr<Graph> graph;
    int n;

    void cleanup();
    void register_graph_commands();
    std::string find_config_file(const std::string& filename, const std::vector<std::string>& search_paths);
    std::string get_default_config_path();

    void cmd_create(const std::vector<std::string>& args);
    void cmd_print() const;
    void cmd_clear();
    void cmd_cleanup();
    void cmd_exit();
    void cmd_help(const std::vector<std::string>& args);
    void cmd_history();
    void cmd_traversal(const std::vector<std::string>& args) const;
    void cmd_traversal_list(const std::vector<std::string>& args) const;
};

#endif //CONSOLE_ADAPTER_H
