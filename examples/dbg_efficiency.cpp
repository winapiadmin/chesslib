#include "position.h"
int main() {
    std::cout << sizeof(chess::HistoryEntry<chess::PolyglotPiece>) << " " << sizeof(chess::HistoryEntry<chess::EnginePiece>) << '\n';
    std::cout << "8192 entries/board (impl) "<< (8192*sizeof(chess::HistoryEntry<chess::PolyglotPiece>)) << " " << (8192*sizeof(chess::HistoryEntry<chess::EnginePiece>)) << '\n';
    return 0;
}