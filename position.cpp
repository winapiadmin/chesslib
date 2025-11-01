#include "position.h"
#include "moves_io.h"
#include <functional>
#include <iomanip>
#include <sstream>
#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR const
#endif
namespace chess {
static _POSSIBLY_CONSTEXPR CastlingRights make_clear_mask(Color c, PieceType pt, Square sq) {
    if (pt == KING) {
        if (c == WHITE && sq == SQ_E1)
            return WHITE_CASTLING;
        if (c == BLACK && sq == SQ_E8)
            return BLACK_CASTLING;
    } else if (pt == ROOK) {
        if (c == WHITE && sq == SQ_H1)
            return WHITE_OO;
        if (c == WHITE && sq == SQ_A1)
            return WHITE_OOO;
        if (c == BLACK && sq == SQ_H8)
            return BLACK_OO;
        if (c == BLACK && sq == SQ_A8)
            return BLACK_OOO;
    }
    return NO_CASTLING;
}

static _POSSIBLY_CONSTEXPR auto make_castle_masks() {
    std::array<std::array<std::array<CastlingRights, SQUARE_NB>, PIECE_TYPE_NB>, COLOR_NB> tbl{};

    for (int c = 0; c < COLOR_NB; ++c)
        for (int pt = 0; pt < PIECE_TYPE_NB; ++pt)
            for (int sq = 0; sq < SQUARE_NB; ++sq)
                tbl[c][pt][sq] = make_clear_mask(Color(c), PieceType(pt), Square(sq));

    return tbl;
}
static _POSSIBLY_CONSTEXPR auto clearCastleMask = make_castle_masks();

template <typename PieceC>
std::ostream &operator<<(std::ostream &os, const _Position<PieceC> &pos) {
    // RAII guard to save/restore stream state
    struct ios_guard {
        std::ostream &strm;
        std::ios::fmtflags flags;
        std::streamsize prec;
        std::ostream::char_type fill;
        ios_guard(std::ostream &s)
            : strm(s), flags(s.flags()), prec(s.precision()), fill(s.fill()) {}
        ~ios_guard() {
            strm.flags(flags);
            strm.precision(prec);
            strm.fill(fill);
        }
    } guard(os);

    constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
    constexpr std::string_view PieceToChar =
        std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;

    os << "\n +---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; --r) {
        for (File f = FILE_A; f <= FILE_H; ++f)
            os << " | " << PieceToChar[static_cast<size_t>(pos.piece_on(make_sq(r, f)))];

        os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
    }
    os << "   a   b   c   d   e   f   g   h\n";

    // Ensure key is printed in hex, but restores after this function
    os << "\nFen: " << pos.fen() << "\nKey: " << std::hex << std::uppercase << std::setfill('0')
       << std::setw(16) << pos.key() << '\n';

    return os;
}
template <typename PieceC, typename T>
template <Color c>
void _Position<PieceC, T>::genEP(Movelist &ep_moves) const {
    const Square king_sq = kingSq(c);
    const Square ep_sq = ep_square();
    if (ep_sq == SQ_NONE)
        return;

    Bitboard candidates = attacks::pawn(~c, ep_sq) & pieces<PAWN, c>();
    if (!candidates)
        return;

    const Square ep_pawn_sq = static_cast<Square>(ep_sq - pawn_push(c));
    const Bitboard ep_mask = (1ULL << ep_pawn_sq) | (1ULL << ep_sq);
    // Bitboard _ir_1 = occ() & ~ep_mask;
    // ASSUME(popcount(candidates) <= 32);
    while (candidates) {
        Square from = static_cast<Square>(pop_lsb(candidates));

        // Remove the EP pawn and this attacker from occupancy
        Bitboard occ_temp = occ();
        occ_temp ^= (1ULL << from) | ep_mask;

        // inline attackers check
        Bitboard atks = 0;
        // atks |= attacks::pawn(c, king_sq) & (pieces<PAWN, ~c>() &~ep_mask);
        // atks |= attacks::knight(king_sq) & pieces<KNIGHT, ~c>();
        atks |= attacks::bishop(king_sq, occ_temp) & (pieces<BISHOP, ~c>() | pieces<QUEEN, ~c>());
        atks |= attacks::rook(king_sq, occ_temp) & (pieces<ROOK, ~c>() | pieces<QUEEN, ~c>());
        // atks |= attacks::king(king_sq) & pieces<KING, ~c>();
        atks &= occ(~c);
        if (!atks) {
            ep_moves.push_back(Move::make<EN_PASSANT>(from, ep_sq));
        }
    }
}
template <typename PieceC, typename T>
template <Color c, bool capturesOnly>
void _Position<PieceC, T>::genPawnSingleMoves(Movelist &moves) const {
    constexpr Direction UP = pawn_push(c);
    // constexpr Bitboard PROMO_RANK = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[1];
    constexpr Bitboard PROMO_NEXT = (c == WHITE) ? attacks::MASK_RANK[7] : attacks::MASK_RANK[0];
    constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
    constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;

    const Bitboard pawns = pieces<PAWN, c>();
    const Bitboard enemy_occ = occ(~c);
    const Bitboard all_occ = occ();

    const Bitboard pinned = pawns & current_state._pin_mask;

    // ---------- 1. Single forward pushes ----------

    if constexpr (!capturesOnly) {
        const Bitboard unpinned = pawns ^ pinned;
        Bitboard one_push = (c == WHITE ? unpinned << 8 : unpinned >> 8) & ~all_occ;
        one_push &= current_state.check_mask;

        Bitboard pinned_push = (c == WHITE ? pinned << 8 : pinned >> 8) & ~all_occ;
        pinned_push &= current_state._rook_pin & current_state.check_mask;
        Bitboard push_targets = one_push | pinned_push;
        Bitboard promo_targets = push_targets & PROMO_NEXT;
        Bitboard non_promo_targets = push_targets & ~promo_targets; // NAND
        while (promo_targets) {
            Square to = Square(pop_lsb(promo_targets));
            Square from = Square(to - UP);

            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        while (non_promo_targets) {
            Square to = Square(pop_lsb(non_promo_targets));
            Square from = Square(to - UP);

            moves.push_back(Move(from, to));
        }
    }
    // ---------- 2. Left captures ----------
    Bitboard left = (pawns & NOT_FILE_A);
    Bitboard left_tgt =
        (c == WHITE ? (left << 7) : (left >> 9)) & enemy_occ & current_state.check_mask;
    Bitboard promo_left_targets = left_tgt & PROMO_NEXT;
    Bitboard non_promo_left_targets = left_tgt & ~promo_left_targets; // NAND

    while (promo_left_targets) {
        Square to = Square(pop_lsb(promo_left_targets));
        Square from = Square(to - ((c == WHITE) ? 7 : -9));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        // skip if pinned but capture not along pin ray
        if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb ||
                                      (move_bb & current_state._rook_pin) == move_bb))) {
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }
    }

    while (non_promo_left_targets) {
        Square to = Square(pop_lsb(non_promo_left_targets));
        Square from = Square(to - ((c == WHITE) ? 7 : -9));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        // skip if pinned but capture not along pin ray
        if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb ||
                                      (move_bb & current_state._rook_pin) == move_bb)))
            moves.push_back(Move(from, to));
    }

    // ---------- 3. Right captures ----------
    Bitboard right = (pawns & NOT_FILE_H);
    Bitboard right_tgt =
        (c == WHITE ? (right << 9) : (right >> 7)) & enemy_occ & current_state.check_mask;
    Bitboard promo_right_targets = right_tgt & PROMO_NEXT;
    Bitboard non_promo_right_targets = right_tgt & ~promo_right_targets; // NAND

    while (promo_right_targets) {
        Square to = Square(pop_lsb(promo_right_targets));
        Square from = Square(to - ((c == WHITE) ? 9 : -7));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb ||
                                      (move_bb & current_state._rook_pin) == move_bb))) {
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }
    }

    while (non_promo_right_targets) {
        Square to = Square(pop_lsb(non_promo_right_targets));
        Square from = Square(to - ((c == WHITE) ? 9 : -7));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb ||
                                      (move_bb & current_state._rook_pin) == move_bb)))
            moves.push_back(Move(from, to));
    }
}

template <typename PieceC, typename T>
template <Color c>
void _Position<PieceC, T>::genPawnDoubleMoves(Movelist &moves) const {
    constexpr Bitboard RANK_2 = (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
    constexpr Direction UP = pawn_push(c);

    Bitboard all_occ = occ();
    Bitboard pawns = pieces<PAWN, c>() & RANK_2;

    // Split pin types
    Bitboard pin_mask = current_state._pin_mask;
    Bitboard pin_file = pin_mask & attacks::MASK_FILE[file_of(kingSq(c))];

    Bitboard unpinned = pawns & ~pin_mask;
    Bitboard file_pinned = pawns & pin_file;

    // First step must be empty
    Bitboard step1_unpinned = attacks::shift<UP>(unpinned) & ~all_occ;
    Bitboard step1_pinned = attacks::shift<UP>(file_pinned) & pin_file & ~all_occ;

    // Second step must also be empty
    Bitboard step2_unpinned = attacks::shift<UP>(step1_unpinned) & ~all_occ;
    Bitboard step2_pinned = attacks::shift<UP>(step1_pinned) & pin_file & ~all_occ;

    Bitboard destinations = (step2_unpinned | step2_pinned) & current_state.check_mask;

    // Source pawns = only from RANK_2 (already masked above)
    Bitboard sources = (c == WHITE) ? (destinations >> 16) : (destinations << 16);

    while (sources) {
        Square from = static_cast<Square>(pop_lsb(sources));
        Square to = from + 2 * UP;
        moves.push_back(Move(from, to));
    }
}
template <typename PieceC, typename T>
template <Color c, bool capturesOnly>
void _Position<PieceC, T>::genKingMoves(Movelist &out) const {
    constexpr Color them = ~c;
    const Square kingSq = this->kingSq(c);
    const Bitboard occAll = occ();
    const Bitboard myOcc = occ(c);

    // Remove king from board when computing enemy attacks
    const Bitboard occWithoutKing = occAll ^ (1ULL << kingSq);
    Bitboard enemyAttacks = 0ULL;

    // Sliding pieces
    Bitboard bLike = pieces<BISHOP, them>() | pieces<QUEEN, them>();
    while (bLike)
        enemyAttacks |= attacks::bishop(static_cast<Square>(pop_lsb(bLike)), occWithoutKing);

    Bitboard rLike = pieces<ROOK, them>() | pieces<QUEEN, them>();
    while (rLike)
        enemyAttacks |= attacks::rook(static_cast<Square>(pop_lsb(rLike)), occWithoutKing);

    // Knights
    enemyAttacks |= attacks::knight(pieces<KNIGHT, them>());

    // Pawns
    enemyAttacks |= attacks::pawn<them>(pieces<PAWN, them>());

    // Enemy king (adjacent control squares)
    enemyAttacks |= attacks::king(this->kingSq(them));

    // Candidate king moves = legal squares not attacked by enemy
    Bitboard moves = attacks::king(kingSq) & ~myOcc & ~enemyAttacks;
    if constexpr (capturesOnly)
        moves &= occ(~c);
    while (moves) {
        const Square to = static_cast<Square>(pop_lsb(moves));
        out.push_back(Move(kingSq, to));
    }
    if constexpr (!capturesOnly) {

        // Castling
        const auto &st = current_state;
        if (st.checkers)
            return;
        if constexpr (c == WHITE) {
            const bool canCastleKingside = (st.castlingRights & WHITE_OO) &&
                                           piece_on(SQ_F1) == PieceC::NO_PIECE &&
                                           piece_on(SQ_G1) == PieceC::NO_PIECE &&
                                           !(enemyAttacks & ((1ULL << SQ_F1) | (1ULL << SQ_G1)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_H1));

            const bool canCastleQueenside =
                (st.castlingRights & WHITE_OOO) && piece_on(SQ_D1) == PieceC::NO_PIECE &&
                piece_on(SQ_C1) == PieceC::NO_PIECE && piece_on(SQ_B1) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_D1) | (1ULL << SQ_C1)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_A1));
        } else {
            const bool canCastleKingside = (st.castlingRights & BLACK_OO) &&
                                           piece_on(SQ_F8) == PieceC::NO_PIECE &&
                                           piece_on(SQ_G8) == PieceC::NO_PIECE &&
                                           !(enemyAttacks & ((1ULL << SQ_F8) | (1ULL << SQ_G8)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_H8));

            const bool canCastleQueenside =
                (st.castlingRights & BLACK_OOO) && piece_on(SQ_D8) == PieceC::NO_PIECE &&
                piece_on(SQ_C8) == PieceC::NO_PIECE && piece_on(SQ_B8) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_D8) | (1ULL << SQ_C8)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_A8));
        }
    }
}
template <typename PieceC, typename T> void _Position<PieceC, T>::doMove(const Move &move) {

    Square from_sq = move.from_sq(), to_sq = move.to_sq();
    Color us = side_to_move(), them = ~us;
    MoveType move_type = move.type_of();
    PieceC moving_piece = piece_on(from_sq);
    PieceC target_piece = piece_on(to_sq);
    PieceType moving_piecetype = piece_of(moving_piece);
    PieceType target_piecetype = piece_of(target_piece);
    Color target_color = color_of(target_piece);
    bool is_capture = isCapture(move);
    history.push_back(current_state);
    current_state.mv = move; // Update the move in the current state
#if defined(_DEBUG) || !defined(NDEBUG)
    if (target_piecetype == KING)
        throw std::invalid_argument("NO WAY");
#endif
    removePiece(moving_piecetype, from_sq, us);
    {
        ASSUME(move_type == NORMAL || move_type == PROMOTION || move_type == EN_PASSANT ||
               move_type == CASTLING);
        switch (move_type) {
        case NORMAL:
            removePiece(target_piecetype, to_sq, target_color);
            placePiece(moving_piecetype, to_sq, us);
            break;
        case PROMOTION:
            removePiece(target_piecetype, to_sq, target_color);
            placePiece(move.promotion_type(), to_sq, us);
            break;
        case EN_PASSANT: {
            Square ep_capture_sq = to_sq + pawn_push(them);
            removePiece<PAWN>(ep_capture_sq, them);
            placePiece<PAWN>(to_sq, us);
            break;
        }
        case CASTLING: {
            removePiece(target_piecetype, to_sq, target_color);
            bool is_king_side = from_sq < to_sq;
            Square rook_dest = relative_square(us, is_king_side ? SQ_F1 : Square::SQ_D1),
                   king_dest = relative_square(us, is_king_side ? SQ_G1 : Square::SQ_C1);
            placePiece<ROOK>(rook_dest, us);
            placePiece<KING>(king_dest, us);
            break;
        }
        default:
            UNREACHABLE();
            return;
        }
    }
    {
        // Detect a two-square pawn push.
        bool isDoublePush = (moving_piecetype == PAWN) && (abs(to_sq - from_sq) == 16);

        // Remove old EP key if it was actually included.
        bool _ir_1 = current_state.enPassant != SQ_NONE && current_state.epIncluded;
        current_state.hash ^= _ir_1 ? zobrist::RandomEP[file_of(current_state.enPassant)] : 0;

        // Default: no EP square and not included.
        current_state.epIncluded = false;

        // If the move creates a potential EP target:
        current_state.enPassant = isDoublePush ? (from_sq + pawn_push(us)) : SQ_NONE;

        // Now side to move is the *opponent*.
        Color stm = ~us;

        // Validate and include EP hash only if side-to-move pawns can capture.
        if (current_state.enPassant != SQ_NONE) {
            File f = file_of(current_state.enPassant);
            Bitboard ep_mask = (1ULL << current_state.enPassant);

            // Shift toward the side-to-move’s capturing rank.
            ep_mask = (stm == WHITE) ? (ep_mask >> 8) : (ep_mask << 8);

            // Keep adjacent files only.
            ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) |
                      ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);

            // Include key if their pawns can attack it.
            current_state.epIncluded = (ep_mask & pieces<PAWN>(stm)) != 0;
            current_state.hash ^= current_state.epIncluded ? zobrist::RandomEP[f] : 0;
        }
    }
    {
        CastlingRights clear_mask = clearCastleMask[us][moving_piecetype][from_sq] |
                                    clearCastleMask[target_color][target_piecetype][to_sq];
        CastlingRights prev = current_state.castlingRights;
        current_state.castlingRights &= ~clear_mask;
        current_state.hash ^=
            zobrist::RandomCastle[prev] ^ zobrist::RandomCastle[current_state.castlingRights];
    }
    current_state.turn = ~current_state.turn;
    // Update halfmoves, fullmoves and stm
    current_state.fullMoveNumber += (current_state.turn == WHITE);
    current_state.halfMoveClock =
        (is_capture || moving_piecetype == PAWN) ? 0 : (current_state.halfMoveClock + 1);
    current_state.hash ^= zobrist::RandomTurn;
    refresh_attacks();
}

template <typename PieceC, typename T> void _Position<PieceC, T>::setFEN(const std::string &str) {
    current_state = HistoryEntry<PieceC>();
    history.size_ = 0;
    std::istringstream ss(str);
    std::string board_fen, active_color, castling, enpassant;
    int halfmove = 0, fullmove = 1;
    ss >> board_fen;
    ss >> active_color;
    ss >> castling;
    ss >> enpassant;
    ss >> halfmove;
    ss >> fullmove;

    // 1. Parse board
    {
        File f = FILE_A;
        Rank r = RANK_8;
#if defined(_DEBUG) || !defined(NDEBUG)
        int file_count = 0;
        int rank_count = 0;
#endif

        for (char c : board_fen) {
            if (c == '/') {
#if defined(_DEBUG) || !defined(NDEBUG)
                assert(file_count == 8 && "Each rank must contain exactly 8 squares");
                file_count = 0;
#endif
                f = FILE_A;
                --r;
#if defined(_DEBUG) || !defined(NDEBUG)
                ++rank_count;
#endif
                continue;
            }

            if (c >= '1' && c <= '8') {
                int empty_squares = c - '0';
#if defined(_DEBUG) || !defined(NDEBUG)
                file_count += empty_squares;
#endif
                f = static_cast<File>(static_cast<uint8_t>(f) + empty_squares);
            } else {
                assert(file_count < 8 && "Too many pieces in one rank");
                assert(is_valid(r, f) && "Invalid file/rank position");
                switch (c) {
                case 'p':
                    placePiece<PAWN>(make_sq(r, f), BLACK);
                    break;
                case 'P':
                    placePiece<PAWN>(make_sq(r, f), WHITE);
                    break;
                case 'n':
                    placePiece<KNIGHT>(make_sq(r, f), BLACK);
                    break;
                case 'N':
                    placePiece<KNIGHT>(make_sq(r, f), WHITE);
                    break;
                case 'b':
                    placePiece<BISHOP>(make_sq(r, f), BLACK);
                    break;
                case 'B':
                    placePiece<BISHOP>(make_sq(r, f), WHITE);
                    break;
                case 'r':
                    placePiece<ROOK>(make_sq(r, f), BLACK);
                    break;
                case 'R':
                    placePiece<ROOK>(make_sq(r, f), WHITE);
                    break;
                case 'q':
                    placePiece<QUEEN>(make_sq(r, f), BLACK);
                    break;
                case 'Q':
                    placePiece<QUEEN>(make_sq(r, f), WHITE);
                    break;
                case 'k':
                    placePiece<KING>(make_sq(r, f), BLACK);
                    break;
                case 'K':
                    placePiece<KING>(make_sq(r, f), WHITE);
                    break;
                default:
                    assert(false && "Invalid FEN character");
                }

#if defined(_DEBUG) || !defined(NDEBUG)
                ++file_count;
#endif
                f = static_cast<File>(static_cast<uint8_t>(f) + 1);
            }
        }

#if defined(_DEBUG) || !defined(NDEBUG)
        // Final assertions after parsing
        assert(file_count == 8 && "Last rank must have 8 squares");
        ++rank_count;
        assert(rank_count == 8 && "FEN must contain exactly 8 ranks");
#endif
    }

    // 2. Turn
    if (active_color == "w") {
        current_state.turn = WHITE;
        current_state.hash ^= zobrist::RandomTurn;
    } else if (active_color == "b") {
        current_state.turn = BLACK;
    } else {
        assert(false && "Well... Not white, not black, what color are you choosing");
    }

    // 3. Castling rights
    current_state.castlingRights = NO_CASTLING;
    for (char c : castling) {
        switch (c) {
        case 'K':
            current_state.castlingRights |= WHITE_OO;
            current_state.hash ^= zobrist::RandomCastle[WHITE_OO];
            break;
        case 'Q':
            current_state.castlingRights |= WHITE_OOO;
            current_state.hash ^= zobrist::RandomCastle[WHITE_OOO];
            break;
        case 'k':
            current_state.castlingRights |= BLACK_OO;
            current_state.hash ^= zobrist::RandomCastle[BLACK_OO];
            break;
        case 'q':
            current_state.castlingRights |= BLACK_OOO;
            current_state.hash ^= zobrist::RandomCastle[BLACK_OOO];
            break;
        // some optional chess960? maybe not.
        case '-':
            break;
        default:
            assert(false && "You want Chess960? I didn't implement it, say goodbye!");
            break;
        }
    }
    if (enpassant != "-" && enpassant.length() == 2 && enpassant[0] >= 'a' && enpassant[0] <= 'h' &&
        enpassant[1] >= '1' && enpassant[1] <= '8') {
        File f = static_cast<File>(enpassant[0] - 'a');
        Rank r = static_cast<Rank>(enpassant[1] - '1');
        Square ep_sq = make_sq(r, f);
        current_state.enPassant = ep_sq;
        Bitboard ep_mask = 1ULL << ep_sq;
        if (sideToMove() == WHITE) {
            ep_mask >>= 8;
        } else
            ep_mask <<= 8;
        ep_mask =
            ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);
        if (ep_mask & pieces<PAWN>(sideToMove())) {
            current_state.hash ^= zobrist::RandomEP[f];
            current_state.epIncluded = true;
        }
    } else {
        assert(enpassant == "-" && "Invalid en passant FEN field");
        current_state.enPassant = SQ_NONE;
    }

    // 5. Halfmove clock
    current_state.halfMoveClock = static_cast<uint8_t>(halfmove);

    // 6. Fullmove number
    current_state.fullMoveNumber = fullmove;
    refresh_attacks();
}

template <typename PieceC, typename T> std::string _Position<PieceC, T>::fen() const {
    constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
    constexpr std::string_view PieceToChar =
        std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;
    std::string fen;

    // 1) Piece placement
    for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
        int emptyCount = 0;
        for (File file = FILE_A; file <= FILE_H; ++file) {
            Square sq = make_sq(rank, file); // Assuming 0..63 indexing

            PieceC piece = piece_on(sq);
            if (piece == PieceC::NO_PIECE) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += PieceToChar[static_cast<size_t>(piece)];
            }
        }
        if (emptyCount > 0)
            fen += std::to_string(emptyCount);
        if (rank > 0)
            fen += '/';
    }

    // 2) Side to move
    fen += ' ';
    fen += (sideToMove() == WHITE) ? 'w' : 'b';

    // 3) Castling availability
    fen += ' ';
    std::string castlingStr;
    if (castlingRights() & WHITE_OO)
        castlingStr += 'K';
    if (castlingRights() & WHITE_OOO)
        castlingStr += 'Q';
    if (castlingRights() & BLACK_OO)
        castlingStr += 'k';
    if (castlingRights() & BLACK_OOO)
        castlingStr += 'q';
    fen += (castlingStr.empty()) ? "-" : castlingStr;

    // 4) En passant target square or '-'
    fen += ' ';
    Square ep = ep_square();
    fen += (ep == SQ_NONE) ? "-" : uci::squareToString(ep);

    // 5) Halfmove clock
    fen += ' ';
    fen += std::to_string(halfmoveClock());

    // 6) Fullmove number
    fen += ' ';
    fen += std::to_string(fullmoveNumber());

    return fen;
}
template <typename PieceC, typename T> void _Position<PieceC, T>::refresh_attacks() {
    Color c = sideToMove();

    Square king_sq = kingSq(c);
    current_state._bishop_pin = pinMask<BISHOP>(c, king_sq);
    current_state._rook_pin = pinMask<ROOK>(c, king_sq);
    current_state._pin_mask = current_state._bishop_pin | current_state._rook_pin;

    Bitboard checkers = attackers(~c, king_sq);
    current_state.checkers = checkers;

    int num_checks = popcount(checkers);

    switch (num_checks) {
    case 0:
        current_state.check_mask = ~0ULL; // no checks, full mask
        break;

    case 1: {
        Square sq = static_cast<Square>(lsb(checkers));
        Bitboard mask = (1ULL << sq) | movegen::between(king_sq, sq);
        current_state.check_mask = mask;
        break;
    }

    default:
        current_state.check_mask = 0ULL; // multiple checks, no blocking mask
        break;
    }
}
template <typename PieceC, typename T> uint64_t _Position<PieceC, T>::zobrist() const {
    uint64_t hash = 0;
    const auto &pl = current_state.pieces_list;
#pragma unroll(64)
    for (int sq = 0; sq < 64; ++sq)
        hash ^= (pl[sq] == PieceC::NO_PIECE) ? 0 : RandomPiece[(int)pl[sq]][sq];

    hash ^= (current_state.turn == WHITE) ? zobrist::RandomTurn : 0;
    hash ^= zobrist::RandomCastle[current_state.castlingRights];
    auto ep_sq = current_state.enPassant;
    if (ep_sq == SQ_NONE)
        return hash;
    if (ep_sq != SQ_NONE) {
        File f = file_of(ep_sq);
        Bitboard ep_mask = (1ULL << ep_sq);

        // Shift to the rank where the opposing pawn sits
        Color stm = sideToMove();
        // Color them = ~stm;
        ep_mask = (stm == WHITE) ? (ep_mask >> 8) : (ep_mask << 8);

        // Pawns on adjacent files only
        ep_mask =
            ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);

        if (ep_mask & pieces<PAWN>(stm))
            hash ^= zobrist::RandomEP[f];
    }
    return hash;
}
template <typename PieceC, typename T>
template <Color c, PieceType pt, bool capturesOnly>
void _Position<PieceC, T>::genSlidingMoves(Movelist &list) const {
    static_assert(pt == BISHOP || pt == ROOK || pt == QUEEN,
                  "Isn't bishop, rooks and queens sliding pieces? What else can?");
    Bitboard sliders = pieces<pt, c>();
    Bitboard occ_all = occ();
    // Square king_sq = current_state.kings[c];
    Bitboard rook_pinners = current_state._rook_pin; // bitboard of enemy rooks/queens pinning
    Bitboard bishop_pinners = current_state._bishop_pin;
    if constexpr (pt == BISHOP)
        sliders &= ~rook_pinners;
    if constexpr (pt == ROOK)
        sliders &= ~bishop_pinners;
    Bitboard filter_list = ~occ(c) & current_state.check_mask;
    while (sliders) {
        Square from = static_cast<Square>(pop_lsb(sliders));
        Bitboard from_bb = 1ULL << from;

        Bitboard rook_hit = rook_pinners & from_bb;
        Bitboard bishop_hit = bishop_pinners & from_bb;
        Bitboard pin_mask = rook_hit ? rook_pinners : bishop_hit ? bishop_pinners : ~0ULL;

        // Bitboard blockers = occ() ^ from_bb; // remove piece temporarily
        auto func = attacks::queen;
        if constexpr (pt == PieceType::BISHOP)
            func = attacks::bishop;
        else if constexpr (pt == PieceType::ROOK)
            func = attacks::rook;
        func = rook_hit ? attacks::rook : bishop_hit ? attacks::bishop : func;
        Bitboard filtered_pin = pin_mask & filter_list;
        Bitboard targets = func(from, occ_all) & filtered_pin;
        if constexpr (capturesOnly)
            targets &= occ(~c);
        while (targets) {
            Square to = static_cast<Square>(pop_lsb(targets));
            list.push_back(Move(from, to));
        }
    }
}

template <typename PieceC, typename T> _Position<PieceC, T>::_Position(std::string fen) {
    setFEN(fen);
}
template void _Position<EnginePiece, void>::genEP<Color::WHITE>(Movelist &) const;
template void _Position<EnginePiece, void>::genEP<Color::BLACK>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genEP<Color::WHITE>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genEP<Color::BLACK>(Movelist &) const;

template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist &) const;
template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist &) const;

template void
_Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE, true>(Movelist &) const;
template void
_Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK, true>(Movelist &) const;

template void
_Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE, false>(Movelist &) const;
template void
_Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genKingMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genKingMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genKingMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genKingMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKingMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKingMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKingMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKingMoves<Color::BLACK, false>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, BISHOP, false>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, BISHOP, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, BISHOP, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, BISHOP, false>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, ROOK, false>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, ROOK, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, ROOK, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, ROOK, false>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, QUEEN, false>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, QUEEN, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, QUEEN, false>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, QUEEN, false>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, BISHOP, true>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, BISHOP, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, BISHOP, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, BISHOP, true>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, ROOK, true>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, ROOK, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, ROOK, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, ROOK, true>(Movelist &) const;

template void
_Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, QUEEN, true>(Movelist &) const;
template void
_Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, QUEEN, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, QUEEN, true>(Movelist &) const;
template void
_Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, QUEEN, true>(Movelist &) const;

template std::ostream &operator<<(std::ostream &, const _Position<EnginePiece> &);
template std::ostream &operator<<(std::ostream &, const _Position<PolyglotPiece> &);

template _Position<EnginePiece, void>::_Position(std::string);
template _Position<PolyglotPiece, void>::_Position(std::string);
template void _Position<EnginePiece, void>::setFEN(const std::string &);
template void _Position<PolyglotPiece, void>::setFEN(const std::string &);
template std::string _Position<EnginePiece, void>::fen() const;
template std::string _Position<PolyglotPiece, void>::fen() const;
template void _Position<EnginePiece, void>::doMove(const Move &move);
template void _Position<PolyglotPiece, void>::doMove(const Move &move);
template void _Position<EnginePiece, void>::refresh_attacks();
template void _Position<PolyglotPiece, void>::refresh_attacks();
template uint64_t _Position<EnginePiece, void>::zobrist() const;
template uint64_t _Position<PolyglotPiece, void>::zobrist() const;
} // namespace chess
