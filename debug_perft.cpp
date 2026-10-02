#include <iostream>
#include "position.h"
#include "printers.h"

using namespace chess;

int main() {
    Position pos("1k5b/5b2/8/8/8/8/3N3K/N4B2 w - - 0 0");

    std::cout << "FEN: " << pos.fen() << std::endl;
    std::cout << "Side to move: " << (pos.side_to_move() == WHITE ? "WHITE" : "BLACK") << std::endl;
    std::cout << "White occ: 0x" << std::hex << pos.occ(WHITE) << std::dec << std::endl;
    std::cout << "Black occ: 0x" << std::hex << pos.occ(BLACK) << std::dec << std::endl;
    std::cout << "All occ:   0x" << std::hex << pos.occ() << std::dec << std::endl;
    std::cout << "King sq (white): " << pos.king_sq(WHITE) << std::endl;
    std::cout << "King sq (black): " << pos.king_sq(BLACK) << std::endl;
    std::cout << "Enemy attacks: 0x" << std::hex << pos.enemy_attacks() << std::dec << std::endl;
    std::cout << "Pin mask: 0x" << std::hex << pos.pin_mask() << std::dec << std::endl;
    std::cout << "Checkers: 0x" << std::hex << pos.checkers() << std::dec << std::endl;

    Movelist moves;
    pos.legals(moves);
    std::cout << "\nLegal moves (" << moves.size() << "):" << std::endl;
    for (int i = 0; i < moves.size(); i++) {
        std::cout << "  " << moves[i] << std::endl;
    }

    CountOnlyList counted;
    pos.legals(counted);
    std::cout << "CountOnlyList count: " << counted.size_ << std::endl;

    return 0;
}
