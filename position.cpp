#include "position.h"
#include "movegen.h"
#include "moves_io.h"
#include "position.h"
#include <sstream>
#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR const
#endif

#if defined(__EXCEPTIONS) && defined(_DEBUG)
#define THROW_IF_EXCEPTIONS_ON(stuff) throw stuff
#else
#define THROW_IF_EXCEPTIONS_ON(stuff) ((void)0)
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
    std::array<std::array<std::array<CastlingRights, SQUARE_NB>, PIECE_TYPE_NB>, COLOR_NB + 1> tbl{};

    for (int c = 0; c <= COLOR_NB; ++c)
        for (int pt = 0; pt < PIECE_TYPE_NB; ++pt)
            for (int sq = 0; sq < SQUARE_NB; ++sq)
                tbl[c][pt][sq] = make_clear_mask(Color(c), PieceType(pt), Square(sq));

    return tbl;
}
static _POSSIBLY_CONSTEXPR auto clearCastleMask = make_castle_masks();

template <typename PieceC, typename T> template <bool Strict> void _Position<PieceC, T>::doMove(const Move &move) {
    assert(move.is_ok() && "doMove called with invalid move");
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
    current_state.incr_sqs[0] = current_state.incr_sqs[1] = current_state.incr_sqs[2] = current_state.incr_sqs[3] = SQ_NONE;
    current_state.incr_pc[0] = current_state.incr_pc[1] = current_state.incr_pc[2] = current_state.incr_pc[3] =
        PieceC::NO_PIECE;
    current_state.mv = move; // Update the move in the current state
#if defined(_DEBUG) || !defined(NDEBUG)
    if (target_piecetype == KING)
        assert(target_piecetype != KING && "No captures");
    if (moving_piecetype == NO_PIECE_TYPE)
        assert(moving_piecetype != NO_PIECE_TYPE && "Expected a piece to move.");
#elif defined(__EXCEPTIONS) && (defined(_DEBUG) || !defined(NDEBUG))

    if (target_piecetype == KING)
        THROW_IF_EXCEPTIONS_ON(std::invalid_argument("No captures to king exists."));
    if (moving_piecetype == NO_PIECE_TYPE)
        THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Expected a piece to move."));
#endif
    removePiece(moving_piecetype, from_sq, us);
    {
        ASSUME(move_type == NORMAL || move_type == PROMOTION || move_type == EN_PASSANT || move_type == CASTLING);
        switch (move_type) {
        case NORMAL:
            removePiece(target_piecetype, to_sq, target_color);
            placePiece(moving_piecetype, to_sq, us);
            current_state.incr_sqs[0] = from_sq;
            current_state.incr_pc[0] = moving_piece;
            current_state.incr_sqs[1] = to_sq;
            current_state.incr_pc[1] = target_piece;
            break;
        case PROMOTION:
            removePiece(target_piecetype, to_sq, target_color);
            placePiece(move.promotion_type(), to_sq, us);
            current_state.incr_sqs[0] = from_sq;
            current_state.incr_pc[0] = moving_piece;
            current_state.incr_sqs[1] = to_sq;
            current_state.incr_pc[1] = target_piece;
            break;
        case EN_PASSANT: {
            Square ep_capture_sq = to_sq + pawn_push(them);
            removePiece<PAWN>(ep_capture_sq, them);
            placePiece<PAWN>(to_sq, us);
            current_state.incr_sqs[0] = from_sq;
            current_state.incr_pc[0] = moving_piece;
            current_state.incr_sqs[1] = ep_capture_sq;
            current_state.incr_pc[1] = make_piece<PieceC>(PAWN, them);
            current_state.incr_sqs[2] = to_sq;
            current_state.incr_pc[2] = PieceC::NO_PIECE;
            break;
        }
        case CASTLING: {
            removePiece(target_piecetype, to_sq, target_color);
            bool is_king_side = from_sq < to_sq;
            Square rook_dest = relative_square(us, is_king_side ? SQ_F1 : Square::SQ_D1),
                   king_dest = relative_square(us, is_king_side ? SQ_G1 : Square::SQ_C1);
            placePiece<ROOK>(rook_dest, us);
            placePiece<KING>(king_dest, us);
            current_state.incr_sqs[0] = from_sq;
            current_state.incr_pc[0] = moving_piece;
            current_state.incr_sqs[1] = to_sq;
            current_state.incr_pc[1] = target_piece;
            current_state.incr_sqs[2] = king_dest;
            current_state.incr_pc[2] = PieceC::NO_PIECE;
            current_state.incr_sqs[3] = rook_dest;
            current_state.incr_pc[3] = PieceC::NO_PIECE;
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
        if (isDoublePush) {
            // Now side to move is the *opponent*.
            Color stm = ~us;
            File f = file_of(current_state.enPassant);
            Bitboard ep_mask = (1ULL << current_state.enPassant);

            // Shift toward the side-to-move's capturing rank.
            ep_mask = (stm == WHITE) ? (ep_mask >> 8) : (ep_mask << 8);

            // Keep adjacent files only.
            ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);

            // Include key if their pawns can attack it.
            current_state.epIncluded = (ep_mask & pieces<PAWN>(stm)) != 0;
            current_state.hash ^= current_state.epIncluded ? zobrist::RandomEP[f] : 0;
        }
    }
    {
        CastlingRights clear_mask =
            clearCastleMask[us][moving_piecetype][from_sq] | clearCastleMask[target_color][target_piecetype][to_sq];
        CastlingRights prev = current_state.castlingRights;
        current_state.castlingRights &= ~clear_mask;
        current_state.hash ^= zobrist::RandomCastle[prev] ^ zobrist::RandomCastle[current_state.castlingRights];
    }
    current_state.turn = ~current_state.turn;
    // Update halfmoves, fullmoves and stm
    current_state.fullMoveNumber += (current_state.turn == WHITE);
    current_state.halfMoveClock = (is_capture || moving_piecetype == PAWN) ? 0 : (current_state.halfMoveClock + 1);
    current_state.hash ^= zobrist::RandomTurn;
    refresh_attacks();
    // DO NOT MIX REPETITIONS
    if constexpr (Strict) {
        current_state.pliesFromNull++;
        // Calculate the repetition info. It is the ply distance from the previous
        // occurrence of the same position, negative in the 3-fold case, or zero
        // if the position was not repeated.
        current_state.repetition = 0;
        int end = std::min(rule50_count(), current_state.pliesFromNull);
        if (end >= 4) {
            auto *stp = &history[history.size_ - 1];
            stp -= 1;
            for (int i = 4; i <= end; i += 2) {
                stp -= 2;
                if (stp->hash == hash()) {
                    current_state.repetition = stp->repetition ? -i : i;
                    break;
                }
            }
        }
    }
}

template <typename PieceC, typename T> void _Position<PieceC, T>::setFEN(const std::string &str) {
    current_state = HistoryEntry<PieceC>();
    history.clear();
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
#if defined(_DEBUG) || !defined(NDEBUG) || defined(__EXCEPTIONS)
        int file_count = 0;
        int rank_count = 0;
#endif

        for (char c : board_fen) {
            if (c == '/') {
#if defined(_DEBUG) || !defined(NDEBUG)
                assert(file_count == 8 && "Each rank must contain exactly 8 squares");
#elif defined(__EXCEPTIONS)
                if (file_count != 8)
                    THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Each rank must contain exactly 8 squares"));
#endif
                f = FILE_A;
                --r;
#if defined(_DEBUG) || !defined(NDEBUG) || defined(__EXCEPTIONS)
                file_count = 0;
                ++rank_count;
#endif
                continue;
            }

            if (c >= '1' && c <= '8') {
                int empty_squares = c - '0';
#if defined(_DEBUG) || !defined(NDEBUG) || defined(__EXCEPTIONS)
                file_count += empty_squares;
#endif
                f = static_cast<File>(static_cast<uint8_t>(f) + empty_squares);
            } else {
#if defined(_DEBUG) || !defined(NDEBUG)
                assert(file_count < 8 && "Too many pieces in one rank");
                assert(chess::is_valid(r, f) && "Invalid file/rank position");
#elif defined(__EXCEPTIONS)
                if (file_count >= 8)
                    THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Too many pieces in one rank"));
                if (!chess::is_valid(r, f))
                    THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Invalid file/rank position"));
#endif
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
#if defined(_DEBUG) || !defined(NDEBUG)
                    assert(false && "Invalid FEN character");
#elif defined(__EXCEPTIONS)
                    THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Invalid FEN character"));
#endif
                    break;
                }

#if defined(_DEBUG) || !defined(NDEBUG) || defined(__EXCEPTIONS)
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
#elif defined(__EXCEPTIONS)
        if (file_count != 8)
            THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Last rank must have 8 squares"));
        rank_count++;
        if (rank_count != 8)
            THROW_IF_EXCEPTIONS_ON(std::invalid_argument("FEN must contain exactly 8 ranks"));
#endif
    }

    // 2. Turn
    if (active_color == "w") {
        current_state.turn = WHITE;
        current_state.hash ^= zobrist::RandomTurn;
    } else if (active_color == "b") {
        current_state.turn = BLACK;
    } else {
#if defined(_DEBUG) || !defined(NDEBUG)
        assert(false && "Expected white or black, got something else.");
#elif defined(__EXCEPTIONS)
        THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Expected white or black, got something else."));
#endif
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
#if defined(_DEBUG) || !defined(NDEBUG)
            assert(false && "Invalid castling rights, this library doesn't support Chess960");
#elif defined(__EXCEPTIONS)
            THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Invalid castling rights, this library doesn't support Chess960"));
#endif
            break;
        }
    }

    if (enpassant != "-" && enpassant.length() == 2 && enpassant[0] >= 'a' && enpassant[0] <= 'h' && enpassant[1] >= '1' &&
        enpassant[1] <= '8') {
        File f = static_cast<File>(enpassant[0] - 'a');
        Rank r = static_cast<Rank>(enpassant[1] - '1');
        Square ep_sq = make_sq(r, f);
        current_state.enPassant = ep_sq;
        Bitboard ep_mask = 1ULL << ep_sq;
        if (sideToMove() == WHITE) {
            ep_mask >>= 8;
        } else
            ep_mask <<= 8;
        ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);
        if (ep_mask & pieces<PAWN>(sideToMove())) {
            current_state.hash ^= zobrist::RandomEP[f];
            current_state.epIncluded = true;
        }
    } else {
#if defined(_DEBUG) || !defined(NDEBUG)
        assert(enpassant == "-" && "Invalid en passant FEN field");
#elif defined(__EXCEPTIONS)
        if (enpassant != "-")
            THROW_IF_EXCEPTIONS_ON(std::invalid_argument("Invalid en passant FEN field"));
#endif
        current_state.enPassant = SQ_NONE;
    }

    // 5. Halfmove clock
    current_state.halfMoveClock = static_cast<uint8_t>(halfmove);

    // 6. Fullmove number
    current_state.fullMoveNumber = fullmove;
    refresh_attacks();
    current_state.repetition = current_state.pliesFromNull = 0;
}

template <typename PieceC, typename T> std::string _Position<PieceC, T>::fen() const {
    std::ostringstream ss;

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
                    ss<< std::to_string(emptyCount);
                    emptyCount = 0;
                }
                ss<< piece;
            }
        }
        if (emptyCount > 0)
            ss<< std::to_string(emptyCount);
        if (rank > 0)
            ss<< '/';
    }

    // 2) Side to move
    ss<< ' ' << (sideToMove() == WHITE ? 'w' : 'b');

    // 3) Castling availability
    ss<< ' ';
    std::string castlingStr;
    if (castlingRights() & WHITE_OO)
        castlingStr += 'K';
    if (castlingRights() & WHITE_OOO)
        castlingStr += 'Q';
    if (castlingRights() & BLACK_OO)
        castlingStr += 'k';
    if (castlingRights() & BLACK_OOO)
        castlingStr += 'q';
    ss<<(castlingStr.empty() ? "-" : castlingStr);

    // 4) En passant target square or '-'
    ss<< ' ';
    Square ep = ep_square();
    ss<< (ep == SQ_NONE ? "-" : uci::squareToString(ep));

    // 5) Halfmove clock
    ss<< ' ' <<(int)halfmoveClock();

    // 6) Fullmove number
    ss<< ' ' <<(int)fullmoveNumber();

    return ss.str();
}
template <typename PieceC, typename T> template <bool Strict> bool _Position<PieceC, T>::is_valid() const {
    if (count<KING, WHITE>() != 1)
        return false;
    if (count<KING, BLACK>() != 1)
        return false;
    Color stm = sideToMove();
    // stm checking
    bool whiteInCheck = isAttacked(kingSq(WHITE), BLACK);
    bool blackInCheck = isAttacked(kingSq(BLACK), WHITE);

    // Both kings cannot be in check simultaneously
    if (whiteInCheck && blackInCheck)
        return false;

    // The side to move cannot have its king currently in check from itself (nonsense)
    if (isAttacked(kingSq(~stm), stm))
        return false;
    if (piece_on(SQ_A1) != PieceC::WROOK && (castlingRights() & WHITE_OOO) != 0)
        return false;
    // pawns not on backrank
    if ((pieces(PAWN) & (attacks::MASK_RANK[RANK_1] | attacks::MASK_RANK[RANK_8])) != 0)
        return false;
    if constexpr (Strict) {
        // pawns not > 8
        {
            int pawns = count<PAWN, WHITE>();
            int queens = count<QUEEN, WHITE>();
            int rooks = count<ROOK, WHITE>();
            int bishops = count<BISHOP, WHITE>();
            int knights = count<KNIGHT, WHITE>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions =
                std::max(0, (queens - 1)) + std::max(0, (rooks - 2)) + std::max(0, (bishops - 2)) + std::max(0, (knights - 2));

            if (pawns + promotions > 8)
                return false; // impossible
        }
        {
            int pawns = count<PAWN, BLACK>();
            int queens = count<QUEEN, BLACK>();
            int rooks = count<ROOK, BLACK>();
            int bishops = count<BISHOP, BLACK>();
            int knights = count<KNIGHT, BLACK>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions =
                std::max(0, (queens - 1)) + std::max(0, (rooks - 2)) + std::max(0, (bishops - 2)) + std::max(0, (knights - 2));

            if (pawns + promotions > 8)
                return false; // impossible
        }
    }
    if (Square ep_sq = ep_square(); ep_sq != SQ_NONE) {
        if ((stm == WHITE && rank_of(ep_sq) != RANK_6) || (stm == BLACK && rank_of(ep_sq) != RANK_3))
            return false;
        Square behind = ep_sq + ((stm == WHITE) ? SOUTH : NORTH);
        if (piece_at(behind) != make_piece<PieceC>(PAWN, ~stm))
            return false;

        if (!(attacks::pawn(~stm, ep_sq) & pieces<PAWN>(stm)))
            return false;
    }
    // Too many checkers
    if (popcount(checkers()) > 2)
        return false;
    return false;
}
template <typename PieceC, typename T> CheckType _Position<PieceC, T>::givesCheck(Move m) const {
    const static auto getSniper = [&](Square ksq, Bitboard oc) {
        const auto us_occ = us(sideToMove());
        const auto bishop = attacks::bishop(ksq, oc) & pieces(PieceType::BISHOP, PieceType::QUEEN) & us_occ;
        const auto rook   = attacks::rook(ksq, oc) & pieces(PieceType::ROOK, PieceType::QUEEN) & us_occ;
        return (bishop | rook);
    };

    assert(color_of(at(m.from())) == sideToMove());

    const Square from   = m.from();
    const Square to     = m.to();
    const Square ksq    = kingSq(~sideToMove());
    const Bitboard toBB = 1ULL<<(to);
    const PieceType pt  = piece_of(at(from));

    Bitboard fromKing = 0ull;

    if (pt == PieceType::PAWN) {
        fromKing = attacks::pawn(~side_to_move(), ksq);
    } else if (pt == PieceType::KNIGHT) {
        fromKing = attacks::knight(ksq);
    } else if (pt == PieceType::BISHOP) {
        fromKing = attacks::bishop(ksq, occ());
    } else if (pt == PieceType::ROOK) {
        fromKing = attacks::rook(ksq, occ());
    } else if (pt == PieceType::QUEEN) {
        fromKing = attacks::queen(ksq, occ());
    }

    if (fromKing & toBB) return CheckType::DIRECT_CHECK;

    // Discovery check
    const Bitboard fromBB = 1ULL<<(from);
    const Bitboard oc     = occ() ^ fromBB;

    Bitboard sniper = getSniper(this, ksq, oc);

    while (sniper) {
        Square sq = (Square)pop_lsb(sniper);
        return (!(movegen::between(ksq, sq) & toBB) || m.typeOf() == Move::CASTLING) ? CheckType::DISCOVERY_CHECK
                                                                                     : CheckType::NO_CHECK;
    }

    switch (m.typeOf()) {
        case Move::NORMAL:
            return CheckType::NO_CHECK;

        case Move::PROMOTION: {
            Bitboard attacks = 0ull;

            switch (m.promotionType()) {
                case KNIGHT:
                    attacks = attacks::knight(to);
                    break;
                case BISHOP:
                    attacks = attacks::bishop(to, oc);
                    break;
                case ROOK:
                    attacks = attacks::rook(to, oc);
                    break;
                case QUEEN:
                    attacks = attacks::queen(to, oc);
                    break;
                default:
                    break;
            }

            return (attacks & pieces(PieceType::KING, ~sideToMove())) ? CheckType::DIRECT_CHECK : CheckType::NO_CHECK;
        }

        case Move::ENPASSANT: {
            Square capSq=make_sq(file_of(to), rank_of(from));
            return (getSniper(this, ksq, (oc ^ (1ULL<<capSq)) | toBB)) ? CheckType::DISCOVERY_CHECK
                                                                                     : CheckType::NO_CHECK;
        }

        case Move::CASTLING: {
            Square rookTo = relative_square(side_to_move(), to > from ? SQ_F1 : SQ_D1);
            return (attacks::rook(ksq, occ()) & (1ULL<<rookTo)) ? CheckType::DISCOVERY_CHECK
                                                                              : CheckType::NO_CHECK;
        }
    }

    assert(false);
    return CheckType::NO_CHECK;  // Prevent a compiler warning
}
template <typename PieceC, typename T> void _Position<PieceC, T>::refresh_attacks() {
    Color c = sideToMove();

    Square king_sq = kingSq(c);
    _bishop_pin = pinMask<BISHOP>(c, king_sq);
    _rook_pin = pinMask<ROOK>(c, king_sq);
    _pin_mask = _bishop_pin | _rook_pin;

    _checkers = attackers(~c, king_sq);

    int num_checks = popcount(_checkers);

    switch (num_checks) {
    case 0:
        _check_mask = ~0ULL; // no checks, full mask
        break;

    case 1: {
        Square sq = static_cast<Square>(lsb(_checkers));
        Bitboard mask = (1ULL << sq) | movegen::between(king_sq, sq);
        _check_mask = mask;
        break;
    }

    default:
        _check_mask = 0ULL; // multiple checks, no blocking mask
        break;
    }
}
template <typename PieceC, typename T> uint64_t _Position<PieceC, T>::zobrist() const {
    uint64_t hash = 0;
#pragma unroll(64)
    for (int sq = 0; sq < 64; ++sq) {
        auto p = piece_on((Square)sq);
        hash ^= (p == PieceC::NO_PIECE) ? 0 : zobrist::RandomPiece[enum_idx<PieceC>()][(int)p][sq];
    }
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
        ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);

        if (ep_mask & pieces<PAWN>(stm))
            hash ^= zobrist::RandomEP[f];
    }
    return hash;
}
template <typename PieceC, typename T> _Position<PieceC, T>::_Position(std::string fen) { setFEN(fen); }

template <typename PieceC, typename T> Move _Position<PieceC, T>::parse_uci(std::string uci) const {
    return chess::uci::uciToMove(*this, uci);
}

template <typename PieceC, typename T> Move _Position<PieceC, T>::push_uci(std::string uci) {
    auto mv = parse_uci(uci);
    doMove(mv);
    return mv;
}
template <typename PieceC, typename T> Square _Position<PieceC, T>::_valid_ep_square() const {
    if (ep_square() == SQ_NONE)
        return SQ_NONE;
    Rank ep_rank = sideToMove() == WHITE ? RANK_3 : RANK_6;
    Bitboard mask = 1ULL << ep_square();
    Bitboard pawn_mask = mask << 8;
    Bitboard org_pawn_mask = mask >> 8;
    // rank 3 or rank 6, depending on color
    if (rank_of(ep_square()) != ep_rank)
        return SQ_NONE;
    // a pawn in 2 ranks behind
    if (!(pieces(PAWN) & occ(~sideToMove()) & pawn_mask))
        return SQ_NONE;
    // ep_sq must be empty
    if (occ() & mask)
        return SQ_NONE;
    // emptied second rank
    if (occ() & org_pawn_mask)
        return SQ_NONE;
    return ep_square();
}
template <typename PieceC, typename T> bool _Position<PieceC, T>::is_insufficient_material(Color c) const {
    const auto count = popcount(occ());

    // only kings, draw
    if (count == 2)
        return true;

    // only bishop + knight, can't mate
    if (count == 3) {
        if (pieces(PieceType::BISHOP, Color::WHITE) || pieces(PieceType::BISHOP, Color::BLACK))
            return true;
        if (pieces(PieceType::KNIGHT, Color::WHITE) || pieces(PieceType::KNIGHT, Color::BLACK))
            return true;
    }

    // same-colored bishops, can't mate
    if (count == 4) {
        // bishops on same color (one per side)
        if (pieces(PieceType::BISHOP, Color::WHITE) && pieces(PieceType::BISHOP, Color::BLACK)) {
            Square w = (Square)lsb(pieces(PieceType::BISHOP, Color::WHITE));
            Square b = (Square)lsb(pieces(PieceType::BISHOP, Color::BLACK));
            if (((9 * (w ^ b)) & 8) == 0)
                return true;
        }

        // one side with two bishops on same color
        auto white_bishops = pieces(PieceType::BISHOP, Color::WHITE);
        auto black_bishops = pieces(PieceType::BISHOP, Color::BLACK);

        if (popcount(white_bishops) == 2) {
            Square b1 = (Square)lsb(white_bishops);
            Square b2 = (Square)msb(white_bishops);
            if (((9 * (b1 ^ b2)) & 8) == 0)
                return true;
        } else if (popcount(black_bishops) == 2) {
            Square b1 = (Square)lsb(black_bishops);
            Square b2 = (Square)msb(black_bishops);
            if (((9 * (b1 ^ b2)) & 8) == 0)
                return true;
        }
    }

    return false;
}
template <typename PieceC, typename T> CastlingRights _Position<PieceC, T>::clean_castling_rights() const {
    constexpr Bitboard cr_WOO = 1ULL << SQ_H1;
    constexpr Bitboard cr_WOOO = 1ULL << SQ_A1;
    constexpr Bitboard cr_BOO = 1ULL << SQ_H8;
    constexpr Bitboard cr_BOOO = 1ULL << SQ_A8;
    if (history.size())
        return castlingRights();
    Bitboard castling = 0;
    // mappings
    castling |= (castlingRights() & WHITE_OO) ? cr_WOO : 0;
    castling |= (castlingRights() & WHITE_OOO) ? cr_WOOO : 0;

    castling |= (castlingRights() & BLACK_OO) ? cr_BOO : 0;
    castling |= (castlingRights() & BLACK_OOO) ? cr_BOOO : 0;
    castling &= pieces(ROOK);
    Bitboard white_castling = castling & attacks::MASK_RANK[RANK_1] & occ(WHITE);
    Bitboard black_castling = castling & attacks::MASK_RANK[RANK_8] & occ(BLACK);
    // rook exists in corresponding square
    white_castling &= (cr_WOO | cr_WOOO);
    black_castling &= (cr_BOO | cr_BOOO);
    // king exists in e1/e8 depending on color
    if (!(occ(WHITE) & pieces(KING) & (1ULL << SQ_E1)))
        white_castling = 0;
    if (!(occ(WHITE) & pieces(KING) & (1ULL << SQ_E8)))
        black_castling = 0;
    castling = white_castling | black_castling;
    // Re-map
    CastlingRights cr = NO_CASTLING;
    cr |= (castling & cr_WOO) ? WHITE_OO : NO_CASTLING;
    cr |= (castling & cr_WOOO) ? WHITE_OOO : NO_CASTLING;
    cr |= (castling & cr_BOO) ? BLACK_OO : NO_CASTLING;
    cr |= (castling & cr_BOOO) ? BLACK_OOO : NO_CASTLING;
    return cr;
}
// clang-format off
#define INSTANTIATE(PieceC) \
template _Position<PieceC, void>::_Position(std::string); \
template void _Position<PieceC, void>::setFEN(const std::string &); \
template std::string _Position<PieceC, void>::fen() const; \
template void _Position<PieceC, void>::doMove<false>(const Move &move); \
template void _Position<PieceC, void>::doMove<true>(const Move &move); \
template void _Position<PieceC, void>::refresh_attacks(); \
template uint64_t _Position<PieceC, void>::zobrist() const; \
template Move _Position<PieceC, void>::parse_uci(std::string) const; \
template Move _Position<PieceC, void>::push_uci(std::string); \
template bool _Position<PieceC, void>::is_valid<false>() const; \
template bool _Position<PieceC, void>::is_valid<true>() const; \
template CheckType _Position<PieceC, void>::givesCheck(Move) const; \
template bool _Position<PieceC, void>::is_insufficient_material() const;
// clang-format off
INSTANTIATE(PolyglotPiece)
INSTANTIATE(EnginePiece)
INSTANTIATE(ContiguousMappingPiece)
#undef INSTANTIATE
} // namespace chess
