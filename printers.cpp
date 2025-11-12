#include "printers.h"
#include "moves_io.h"
#include "position.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <unordered_map>
namespace chess {
template <typename T> using DescriptiveNameNotation = std::unordered_map<T, std::string>;

template <typename PieceC> std::ostream &operator<<(std::ostream &os, const _Position<PieceC, void> &pos) {
    // RAII guard to save/restore stream state
    struct ios_guard {
        std::ostream &strm;
        std::ios::fmtflags flags;
        std::streamsize prec;
        std::ostream::char_type fill;
        ios_guard(std::ostream &s) : strm(s), flags(s.flags()), prec(s.precision()), fill(s.fill()) {}
        ~ios_guard() {
            strm.flags(flags);
            strm.precision(prec);
            strm.fill(fill);
        }
    } guard(os);

    constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
    constexpr std::string_view PieceToChar = std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;

    os << "\n +---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; --r) {
        for (File f = FILE_A; f <= FILE_H; ++f)
            os << " | " << PieceToChar[static_cast<size_t>(pos.piece_on(make_sq(r, f)))];

        os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
    }
    os << "   a   b   c   d   e   f   g   h\n";

    // Ensure key is printed in hex, but restores after this function
    os << "\nFen: " << pos.fen() << "\nKey: " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pos.key() << '\n';

    return os;
}
std::ostream &operator<<(std::ostream &os, const Move &mv) {
    os << uci::moveToUci(mv);
    return os;
}
std::ostream &operator<<(std::ostream &os, const Color &c) {
    DescriptiveNameNotation<Color> colors = {
        {    WHITE,    "WHITE" },
        {    BLACK,    "BLACK" },
        { COLOR_NB, "COLOR_NB" }
    };
    return os << colors[c];
}
std::ostream &operator<<(std::ostream &os, const CastlingRights &cr) {
    DescriptiveNameNotation<CastlingRights> castlingFlags = {
        {                      NO_CASTLING,                      "NO_CASTLING" },
        {                         WHITE_OO,                         "WHITE_OO" },
        {                        WHITE_OOO,                        "WHITE_OOO" },
        {                         BLACK_OO,                         "BLACK_OO" },
        {                        BLACK_OOO,                        "BLACK_OOO" },
        {                   WHITE_CASTLING,                   "WHITE_CASTLING" },
        {                   BLACK_CASTLING,                   "BLACK_CASTLING" },
        {                        KING_SIDE,                        "KING_SIDE" },
        {                       QUEEN_SIDE,                       "QUEEN_SIDE" },
        {                     ANY_CASTLING,                     "ANY_CASTLING" },
        {             WHITE_OOO | BLACK_OO,             "WHITE_OOO | BLACK_OO" },
        {             WHITE_OO | BLACK_OOO,             "WHITE_OO | BLACK_OOO" },
        {  WHITE_OO | WHITE_OOO | BLACK_OO,  "WHITE_OO | WHITE_OOO | BLACK_OO" },
        { WHITE_OO | WHITE_OOO | BLACK_OOO, "WHITE_OO | WHITE_OOO | BLACK_OOO" },
        {  WHITE_OO | BLACK_OO | BLACK_OOO,  "WHITE_OO | BLACK_OO | BLACK_OOO" },
        { WHITE_OOO | BLACK_OO | BLACK_OOO, "WHITE_OOO | BLACK_OO | BLACK_OOO" },
    };

    return os << castlingFlags[cr];
}
static std::string str_toupper(std::string s) {
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   // static_cast<int(*)(int)>(std::toupper)         // wrong
                   // [](int c){ return std::toupper(c); }           // wrong
                   // [](char c){ return std::toupper(c); }          // wrong
                   [](unsigned char c) { return std::toupper(c); } // correct
    );
    return s;
}
std::ostream &operator<<(std::ostream &os, const Square &sq) {
    os << "SQ_" << str_toupper(chess::uci::squareToString(sq));
    return os;
}
std::ostream &operator<<(std::ostream &os, const PolyglotPiece &p) {
    constexpr std::string_view EnginePieceToChar(".PNBRQK'/pnbrqk,");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk.");
    constexpr std::string_view PieceToChar = std::is_same_v<decltype(p), EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;
    return os << PieceToChar[(int)p];
}
std::ostream &operator<<(std::ostream &os, const EnginePiece &p) {
    constexpr std::string_view EnginePieceToChar(".PNBRQK'/pnbrqk,");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk.");
    constexpr std::string_view PieceToChar = std::is_same_v<decltype(p), EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;
    return os << PieceToChar[(int)p];
}
template std::ostream &operator<<(std::ostream &, const _Position<EnginePiece> &);
template std::ostream &operator<<(std::ostream &, const _Position<PolyglotPiece> &);
} // namespace chess