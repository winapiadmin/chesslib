#include "position.h"
#include <iostream>
#include <iomanip>
// please enable constexpr limit 2B, i only test on latest env
// -fconstexpr-steps=2000000000 or /constexpr:steps2000000000
// no options required for GCC
int main()
{
    {
        chess::Position p("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
        std::cout << p.fen()<<'\n';
        std::cout << "before: "<< std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << p.key()<<'\n';
        p.doMove(chess::Move::make<chess::CASTLING>(chess::SQ_E1, chess::SQ_H1));
        std::cout << "after : "<< std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << p.key()<<'\n';
        chess::Position a(p.fen());
        std::cout << "recalc: " << std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << a.key()<<'\n';
    }
    {
        chess::Position p("8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1");
        std::cout << p.fen()<<'\n';
        std::cout << "before: "<< std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << p.key()<<'\n';
        p.doMove(chess::Move::make<chess::EN_PASSANT>(chess::SQ_F4, chess::SQ_E3));
        std::cout << "after : "<< std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << p.key()<<'\n';
        chess::Position a(p.fen());
        std::cout << "recalc: " << std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << a.key();
    }
    return 0;
}
