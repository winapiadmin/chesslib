#include "position.h"
#include <bitset>
#include <iomanip>
#include <sstream>
#include <functional>
namespace chess {
    template<typename PieceC>
    std::ostream& operator<<(std::ostream& os, const _Position<PieceC>& pos)
    {
        // RAII guard to save/restore stream state
        struct ios_guard {
            std::ostream& strm;
            std::ios::fmtflags flags;
            std::streamsize prec;
            std::ostream::char_type fill;
            ios_guard(std::ostream& s) : strm(s), flags(s.flags()), prec(s.precision()), fill(s.fill()) {}
            ~ios_guard() { strm.flags(flags); strm.precision(prec); strm.fill(fill); }
        } guard(os);

        constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
        constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
        constexpr std::string_view PieceToChar =
            std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;

        os << "\n +---+---+---+---+---+---+---+---+\n";

        for (Rank r = RANK_8; r >= RANK_1; --r)
        {
            for (File f = FILE_A; f <= FILE_H; ++f)
                os << " | " << PieceToChar[static_cast<size_t>(pos.piece_on(make_sq(r, f)))];

            os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
        }
        os << "   a   b   c   d   e   f   g   h\n";

        // Ensure key is printed in hex, but restores after this function
        os << "\nFen: " << pos.fen()
            << "\nKey: " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(16) << pos.key()<<'\n';

        return os;
    }
    template<typename PieceC, typename T>
    template<Color c>
    void _Position<PieceC, T>::genEP(Movelist& ep_moves) const {
        const Square king_sq = square<KING>(c);
        const Square ep_sq = ep_square();
        if (ep_sq == SQ_NONE) return;

        Bitboard candidates = attacks::pawn(~c, ep_sq) & pieces<PAWN, c>();
        if (!candidates) return;

        const Square ep_pawn_sq = static_cast<Square>(ep_sq - pawn_push(c));
        const Bitboard ep_mask = 1ULL << ep_pawn_sq;

        while (candidates) {
            Square from = static_cast<Square>(pop_lsb(candidates));
            Bitboard from_mask = 1ULL << from;

            // Remove the EP pawn and this attacker from occupancy
            Bitboard occ_temp = occ() & ~ep_mask & ~from_mask;

            // inline attackers check
            Bitboard atks = 0;
            atks |= attacks::pawn(c, king_sq) & (pieces<PAWN, ~c>() &~ep_mask);
            atks |= attacks::knight(king_sq) & pieces<KNIGHT, ~c>();
            atks |= attacks::bishop(king_sq, occ_temp) & (pieces<BISHOP, ~c>() | pieces<QUEEN, ~c>());
            atks |= attacks::rook(king_sq, occ_temp) & (pieces<ROOK, ~c>() | pieces<QUEEN, ~c>());
            atks |= attacks::king(king_sq) & pieces<KING, ~c>();
            atks &= occ_temp;
            if (!atks) {
                ep_moves.push_back(Move::make<EN_PASSANT>(from, ep_sq));
            }
        }
    }

    template<typename PieceC, typename T>
    template<Color c>
    void _Position<PieceC, T>::genPawnSingleMoves(Movelist& moves) const
    {
        constexpr Direction UP = pawn_push(c);
        constexpr Bitboard PROMO_RANK = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[1];
        constexpr Bitboard PROMO_NEXT = (c == WHITE) ? attacks::MASK_RANK[7] : attacks::MASK_RANK[0];
        constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
        constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;

        const Bitboard pawns = pieces<PAWN, c>();
        const Bitboard enemy_occ = occ(~c);
        const Bitboard all_occ = occ();

        const Bitboard pinned = pawns & current_state._pin_mask;
        const Bitboard unpinned = pawns ^ pinned;

        // ---------- 1. Single forward pushes ----------
        Bitboard one_push = (c == WHITE ? unpinned << 8 : unpinned >> 8) & ~all_occ;
        one_push &= current_state.check_mask;

        // Pinned pawns: only if pin direction is vertical (same file)
        Bitboard pinned_push = (c == WHITE ? pinned << 8 : pinned >> 8) & ~all_occ;
        pinned_push &= current_state._rook_pin;  // only straight pins allow push
        pinned_push &= current_state.check_mask;

        Bitboard push_targets = one_push | pinned_push;

        while (push_targets)
        {
            Square to = Square(pop_lsb(push_targets));
            Square from = Square(to - UP);

            if ((1ULL << from) & PROMO_RANK)
            {
                moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
                moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
                moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
                moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
            }
            else
                moves.push_back(Move(from, to));
        }

        // ---------- 2. Captures ----------
        auto gen_captures = [&](Bitboard candidates, int shift)
            {
                while (candidates)
                {
                    Square to = Square(pop_lsb(candidates));
                    Square from = Square(to - shift);
                    Bitboard from_bb = 1ULL << from;
                    Bitboard move_bb = from_bb | (1ULL << to);

                    // skip if pinned but capture not along pin ray
                    if ((pinned & from_bb) &&
                        !((move_bb & current_state._bishop_pin) == move_bb ||
                            (move_bb & current_state._rook_pin) == move_bb))
                        continue;

                    if ((1ULL << from) & PROMO_RANK)
                    {
                        moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
                        moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
                        moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
                        moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
                    }
                    else
                        moves.push_back(Move(from, to));
                }
            };

        // left and right captures, masked early for correctness
        Bitboard left = (pawns & NOT_FILE_A);
        Bitboard right = (pawns & NOT_FILE_H);

        Bitboard left_tgt = (c == WHITE ? (left << 7) : (left >> 9)) & enemy_occ & current_state.check_mask;
        Bitboard right_tgt = (c == WHITE ? (right << 9) : (right >> 7)) & enemy_occ & current_state.check_mask;

        gen_captures(left_tgt, (c == WHITE) ? 7 : -9);
        gen_captures(right_tgt, (c == WHITE) ? 9 : -7);
    }

    template<typename PieceC, typename T>
    template<Color c>
    void _Position<PieceC, T>::genPawnDoubleMoves(Movelist& moves) const
    {
        constexpr Bitboard RANK_2 = (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
        constexpr Direction UP = pawn_push(c);

        Bitboard all_occ = occ();
        Bitboard pawns = pieces<PAWN, c>()& RANK_2;

        // Split pin types
        Bitboard pin_mask = current_state._pin_mask;
        Bitboard pin_file = pin_mask & attacks::MASK_FILE[file_of(square<KING>(c))];

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

        while (sources)
        {
            Square from = static_cast<Square>(pop_lsb(sources));
            Square to = from + 2 * UP;
            moves.push_back(Move(from, to));
        }
    }
    template<typename PieceC, typename T>
    template<Color c>
    void _Position<PieceC, T>::genKingMoves(Movelist& out) const {
        constexpr Color them = ~c;
        const Square kingSq = square<KING>(c);
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
        Bitboard n = pieces<KNIGHT, them>();
        while (n)
            enemyAttacks |= attacks::knight(static_cast<Square>(pop_lsb(n)));

        // Pawns
        enemyAttacks |= attacks::pawn<them>(pieces<PAWN, them>());

        // Enemy king (adjacent control squares)
        enemyAttacks |= attacks::king(square<KING>(them));

        // Candidate king moves = legal squares not attacked by enemy
        Bitboard moves = attacks::king(kingSq) & ~myOcc & ~enemyAttacks;

        while (moves) {
            const Square to = static_cast<Square>(pop_lsb(moves));
            out.push_back(Move(kingSq, to));
        }

        // Castling
        const auto& st = current_state;
        if (st.checkers) return;
        if constexpr (c == WHITE) {
            const bool canCastleKingside =
                (st.castlingRights & WHITE_OO) &&
                piece_on(SQ_F1) == PieceC::NO_PIECE &&
                piece_on(SQ_G1) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_F1) | (1ULL << SQ_G1)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_H1));

            const bool canCastleQueenside =
                (st.castlingRights & WHITE_OOO) &&
                piece_on(SQ_D1) == PieceC::NO_PIECE &&
                piece_on(SQ_C1) == PieceC::NO_PIECE &&
                piece_on(SQ_B1) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_D1) | (1ULL << SQ_C1)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_A1));
        }
        else {
            const bool canCastleKingside =
                (st.castlingRights & BLACK_OO) &&
                piece_on(SQ_F8) == PieceC::NO_PIECE &&
                piece_on(SQ_G8) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_F8) | (1ULL << SQ_G8)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_H8));

            const bool canCastleQueenside =
                (st.castlingRights & BLACK_OOO) &&
                piece_on(SQ_D8) == PieceC::NO_PIECE &&
                piece_on(SQ_C8) == PieceC::NO_PIECE &&
                piece_on(SQ_B8) == PieceC::NO_PIECE &&
                !(enemyAttacks & ((1ULL << SQ_D8) | (1ULL << SQ_C8)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_A8));
        }
    }


    template<typename PieceC, typename T>
    _Position<PieceC, T>::_Position(std::string fen)
    {
        current_state = HistoryEntry<PieceC>();
        std::istringstream ss(fen);
        std::string        board_fen, active_color, castling, enpassant;
        int                halfmove = 0, fullmove = 1;
        ss >> board_fen;
        ss >> active_color;
        ss >> castling;
        ss >> enpassant;
        ss >> halfmove;
        ss >> fullmove;

        // 1. Parse board
        {
            File f          = FILE_A;
            Rank r          = RANK_8;
            int  file_count = 0;
            int  rank_count = 0;

            for (char c : board_fen)
            {
                if (c == '/')
                {
                    assert(file_count == 8 && "Each rank must contain exactly 8 squares");
                    file_count = 0;
                    f          = FILE_A;
                    --r;
                    ++rank_count;
                    continue;
                }

                if (c >= '1' && c <= '8')
                {
                    int empty_squares = c - '0';
                    file_count += empty_squares;
                    f = static_cast<File>(static_cast<uint8_t>(f) + empty_squares);
                }
                else
                {
                    assert(file_count < 8 && "Too many pieces in one rank");
                    assert(is_valid(r, f) && "Invalid file/rank position");
                    switch (c)
                    {
                    case 'p' :
                        placePiece<PAWN>(make_sq(r, f), BLACK);
                        break;
                    case 'P' :
                        placePiece<PAWN>(make_sq(r, f), WHITE);
                        break;
                    case 'n' :
                        placePiece<KNIGHT>(make_sq(r, f), BLACK);
                        break;
                    case 'N' :
                        placePiece<KNIGHT>(make_sq(r, f), WHITE);
                        break;
                    case 'b' :
                        placePiece<BISHOP>(make_sq(r, f), BLACK);
                        break;
                    case 'B' :
                        placePiece<BISHOP>(make_sq(r, f), WHITE);
                        break;
                    case 'r' :
                        placePiece<ROOK>(make_sq(r, f), BLACK);
                        break;
                    case 'R' :
                        placePiece<ROOK>(make_sq(r, f), WHITE);
                        break;
                    case 'q' :
                        placePiece<QUEEN>(make_sq(r, f), BLACK);
                        break;
                    case 'Q' :
                        placePiece<QUEEN>(make_sq(r, f), WHITE);
                        break;
                    case 'k' :
                        placePiece<KING>(make_sq(r, f), BLACK);
                        break;
                    case 'K' :
                        placePiece<KING>(make_sq(r, f), WHITE);
                        break;
                    default :
                        assert(false && "Invalid FEN character");
                    }

                    ++file_count;
                    f = static_cast<File>(static_cast<uint8_t>(f) + 1);
                }
            }

            // Final assertions after parsing
            assert(file_count == 8 && "Last rank must have 8 squares");
            ++rank_count;
            assert(rank_count == 8 && "FEN must contain exactly 8 ranks");
        }


        // 2. Turn
        if (active_color == "w")
        {
            current_state.turn = WHITE;
            current_state.hash ^= zobrist::RandomTurn;
        }
        else if (active_color == "b")
        {
            current_state.turn = BLACK;
        }
        else
        {
            assert(false && "Well... Not white, not black, what color are you choosing");
        }

        // 3. Castling rights
        current_state.castlingRights = NO_CASTLING;
        for (char c : castling)
        {
            switch (c)
            {
            case 'K' :
                current_state.castlingRights|=WHITE_OO;
                current_state.hash ^= zobrist::RandomCastle[WHITE_OO];
                break;
            case 'Q' :
                current_state.castlingRights|=WHITE_OOO;
                current_state.hash ^= zobrist::RandomCastle[WHITE_OOO];
                break;
            case 'k' :
                current_state.castlingRights|=BLACK_OO;
                current_state.hash ^= zobrist::RandomCastle[BLACK_OO];
                break;
            case 'q' :
                current_state.castlingRights|=BLACK_OOO;
                current_state.hash ^= zobrist::RandomCastle[BLACK_OOO];
                break;
            // some optional chess960? maybe not.
            case '-' :
                break;
            default :
                assert(false && "You want Chess960? I didn't implement it, say goodbye!");
                break;
            }
        }
        if (enpassant != "-" && enpassant.length() == 2 &&
            enpassant[0] >= 'a' && enpassant[0] <= 'h' &&
            enpassant[1] >= '1' && enpassant[1] <= '8')
        {
            File f = static_cast<File>(enpassant[0] - 'a');
            Rank r = static_cast<Rank>(enpassant[1] - '1');
            Square ep_sq = make_sq(r, f);
            current_state.enPassant = ep_sq;
            Bitboard ep_mask=1ULL<<ep_sq;
            if (sideToMove()==WHITE){
                ep_mask >>= 8;
            }
            else ep_mask <<= 8;
            ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);
            if (ep_mask & pieces<PAWN>(sideToMove())){
                current_state.hash ^= zobrist::RandomEP[f];
            }
        }
        else {
            assert(enpassant == "-" && "Invalid en passant FEN field");
            current_state.enPassant = SQ_NONE;
        }

        // 5. Halfmove clock
        current_state.halfMoveClock = static_cast<uint8_t>(halfmove);

        // 6. Fullmove number
        current_state.fullMoveNumber = fullmove;
        refresh_attacks();
    }

    template <typename PieceC, typename T>
    std::string _Position<PieceC, T>::fen() const
    {
        constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
        constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
        constexpr std::string_view PieceToChar =
          std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;
        std::string fen;

        // 1) Piece placement
        for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
            int emptyCount = 0;
            for (File file = FILE_A; file <= FILE_H; ++file) {
                Square sq = make_sq(rank, file);  // Assuming 0..63 indexing

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
        if (castlingRights() & WHITE_OO)  castlingStr += 'K';
        if (castlingRights() & WHITE_OOO) castlingStr += 'Q';
        if (castlingRights() & BLACK_OO)  castlingStr += 'k';
        if (castlingRights() & BLACK_OOO) castlingStr += 'q';
        fen += (castlingStr.empty()) ? "-" : castlingStr;

        // 4) En passant target square or '-'
        fen += ' ';
        Square ep = ep_square();
        fen += (ep == SQ_NONE) ? "-" : squareToString(ep);

        // 5) Halfmove clock
        fen += ' ';
        fen += std::to_string(halfmoveClock());

        // 6) Fullmove number
        fen += ' ';
        fen += std::to_string(fullmoveNumber());

        return fen;
    }
    template <typename PieceC, typename T>
    template<Color c, PieceType pt>
    void _Position<PieceC, T>::genSlidingMoves(Movelist& list) const
    {
        Bitboard sliders = pieces<pt, c>();
        Bitboard occ_all = occ();
        Square king_sq = current_state.kings[c];
        Bitboard rook_pinners = current_state._rook_pin;   // bitboard of enemy rooks/queens pinning
        Bitboard bishop_pinners = current_state._bishop_pin;
        if constexpr (pt == BISHOP) sliders &= ~rook_pinners;
        if constexpr (pt == ROOK) sliders &= ~bishop_pinners;
        while (sliders)
        {
            Square from = static_cast<Square>(pop_lsb(sliders));
            Bitboard from_bb = 1ULL << from;

            Bitboard pin_mask = ~0ULL;

            Bitboard blockers = occ() ^ from_bb; // remove piece temporarily
            std::function<Bitboard(Square, Bitboard)> func;
            if constexpr (pt == PieceType::BISHOP)
                func = attacks::bishop;
            if constexpr (pt == PieceType::ROOK)
                func = attacks::rook;
            if constexpr (pt == PieceType::QUEEN)
                func = attacks::queen;
            if (rook_pinners & from_bb) {
                pin_mask = rook_pinners;
                func = attacks::rook;
            }
            else if (bishop_pinners & from_bb) {
                pin_mask = bishop_pinners;
                func = attacks::bishop;
            }
            Bitboard targets = func(from, occ_all) & pin_mask & ~occ(c) & current_state.check_mask;
            while (targets)
            {
                Square to = static_cast<Square>(pop_lsb(targets));
                list.push_back(Move(from, to));
            }
        }
    }
    template void _Position<EnginePiece, void>::genEP<Color::WHITE>(Movelist&) const;
    template void _Position<EnginePiece, void>::genEP<Color::BLACK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genEP<Color::WHITE>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genEP<Color::BLACK>(Movelist&) const;

    template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist&) const;
    template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist&) const;

    template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE>(Movelist&) const;
    template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK>(Movelist&) const;

    template void _Position<EnginePiece, void>::genKingMoves<Color::WHITE>(Movelist&) const;
    template void _Position<EnginePiece, void>::genKingMoves<Color::BLACK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genKingMoves<Color::WHITE>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genKingMoves<Color::BLACK>(Movelist&) const;

    template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, BISHOP>(Movelist&) const;
    template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, BISHOP>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, BISHOP>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, BISHOP>(Movelist&) const;

    template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, ROOK>(Movelist&) const;
    template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, ROOK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, ROOK>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, ROOK>(Movelist&) const;

    template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, QUEEN>(Movelist&) const;
    template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, QUEEN>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, QUEEN>(Movelist&) const;
    template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, QUEEN>(Movelist&) const;

    template std::ostream& operator<<(std::ostream&, const _Position<EnginePiece>&);
    template std::ostream& operator<<(std::ostream&, const _Position<PolyglotPiece>&);

    template _Position<EnginePiece, void>::_Position(std::string);
    template _Position<PolyglotPiece, void>::_Position(std::string);
    template std::string _Position<EnginePiece, void>::fen() const;
    template std::string _Position<PolyglotPiece, void>::fen() const;
}
