#ifndef PARSE_H
#define PARSE_H

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

#include "graph.h"
using namespace std;


namespace IO_opt {
    const int BUF = 65536;
    char buf[BUF + 1];
    char *head = buf, *tail = buf;

    inline char inputchar()
    {
        if(head == tail)
            *(tail = (head = buf) + fread(buf, 1, BUF, stdin)) = 0;
        return *head++;
    }
    template <typename T> inline void read(T &x, T &y) {
        char ch = inputchar(); int f = 1; x = 0;
        while(ch < '0' || ch > '9') {if(ch == '-') f = -1; ch = inputchar();}
        while(ch >= '0' && ch <= '9') x = x*10+ch-'0', ch = inputchar();
        x *= f;

        ch = inputchar(); f = 1; y = 0;
        while(ch < '0' || ch > '9') {if(ch == '-') f = -1; ch = inputchar();}
        while(ch >= '0' && ch <= '9') y = y*10+ch-'0', ch = inputchar();
        y *= f;
    }

    #define I_int int
    inline void write(I_int x) {
        if(x<0) putchar('-'),x=-x;
        if(x>9) write(x/10);
        putchar(x%10+'0');
    }
    #undef I_int
} using namespace IO_opt;

class Parser {
public:
    Parser(std::string filename) : graph_(), input_file_(filename) {
        int u, v;
        while(inputchar() != ']') {
            read(u, v);
            edges_.push_back(std::pair<int, int>(u, v));
        }
        add_vertices_();
        add_edges_();
    }

    const Graph& get_graph() {
        return graph_;
    }
private:
    Graph graph_;
    std::ifstream input_file_;
    std::vector<std::pair<int, int>> edges_;

    void read_file_() {
        std::string edge;
        while (std::getline(input_file_, edge)) {
            parse_edge_(edge);
        }
    }

    void parse_edge_(std::string edge) {
        std::stringstream sstream(edge);

        string s;
        cin >> s;

        int from, to;
        sstream >> from >> to;

        edges_.push_back(std::pair<int, int>(from, to));
    }

    void add_vertices_() {
        for (auto edge : edges_) {
            graph_.add_vertex(edge.first);
            graph_.add_vertex(edge.second);
        }

        graph_.done_with_vertices();
    }

    void add_edges_() {
        for (auto edge : edges_) {
            graph_.add_edge(edge.first, edge.second);
        }
    }
};

#endif
