#include <fstream>
#include <iostream>
#include <vector>
#include "position.h"

using namespace chess;

void write_tree(std::ofstream& out, Position<EnginePiece, void>& pos, int depth) {
    if (depth == 0) {
        uint8_t count = 0;
        out.write(reinterpret_cast<const char*>(&count), 1);
        return;
    }

    ValueList<Move, 256> moves;
    pos.legals<MoveGenType::PAWN>(moves);

    uint8_t count = static_cast<uint8_t>(moves.size());
    out.write(reinterpret_cast<const char*>(&count), 1);

    for (const auto& m : moves) {
        uint16_t encoded = m.raw();
        out.write(reinterpret_cast<const char*>(&encoded), 2);
    }

    for (const auto& m : moves) {
        pos.doMove(m);
        write_tree(out, pos, depth - 1);
        pos.undoMove();
    }
}

void dump_tree_to_file(const std::string& filename, Position<EnginePiece, void>& pos, int depth) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << filename << "\n";
        return;
    }
    write_tree(out, pos, depth);
    out.close();
}

uint64_t perft(int depth, Position<EnginePiece, void> &p){
    ValueList<Move, 256> out;
    p.template legals<MoveGenType::PAWN>(out);
    if (depth==1) return out.size();
    uint64_t s=0;
    for (auto m:out){
        p.doMove(m);
        s+=perft(depth-1, p);
        p.undoMove();
    }
    return s;
}
uint64_t perft_div(int depth, Position<EnginePiece, void> &p){
    ValueList<Move, 256> out;
    p.template legals<MoveGenType::PAWN>(out);
    if (depth==1) return out.size();
    uint64_t s=0;
    for (auto m:out){
        p.doMove(m);
        std::cout<<m<<' ';
        uint64_t y=perft(depth-1, p);
        std::cout<<y<<'\n';
        s+=y;
        p.undoMove();
    }
    return s;
}
int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    const int depth = 6;
    dump_tree_to_file("my_engine_tree.bin", pos, depth);

    std::cout << "Move tree written to my_engine_tree.bin\n";
    perft_div(6, pos);
    
    return 0;
}
