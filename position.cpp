#include "position.h"
#include <bitset>
#include <iomanip>
#include <sstream>
namespace chess {
    template<typename PieceC>
    std::ostream& operator<<(std::ostream& os, const Position<PieceC>& pos)
    {
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
        os << "   a   b   c   d   e   f   g   h\n"
           << "\nFen: " << pos.fen() << "\nKey: " << std::hex << std::uppercase << std::setfill('0')
           << std::setw(16) << pos.key();
        return os;
    }
    template<typename PieceC, typename T>
    template<Color c>
    void Position<PieceC, T>::genEP(Movelist &ep_moves) const
    {

        const Square king_sq = square<KING>(c);
        const Square ep_sq = ep_square();
        if (ep_sq == SQ_NONE) return; // unavoidable one-time exit

        Bitboard attackers = attacks::pawn(~c, ep_sq) & pieces<PAWN, c>();
        attackers &= ~current_state._pin_mask;
        attackers &= current_state.check_mask;

        const Bitboard occ_base = this->occ() & ~(1ULL << (ep_sq - pawn_push(c)));

        const Square sq1 = static_cast<Square>(msb(attackers));
        const Square sq2 = static_cast<Square>(lsb(attackers));

        // Ensure attacker squares are valid
        const Bitboard bb1 = 1ULL << sq1;
        const Bitboard bb2 = 1ULL << sq2;

        const Bitboard occ1 = occ_base & ~bb1;
        const Bitboard occ2 = occ_base & ~bb2;

        // Compute legality masks as 0xFFFFFFFF if legal, 0 otherwise
        const bool attacker_present = attackers != 0;
        const bool second_valid = attacker_present && (sq1 != sq2);

        const bool legal1 = attacker_present && !this->attackers(~c, king_sq, occ1);
        const bool legal2 = second_valid   && !this->attackers(~c, king_sq, occ2);

        const int count = static_cast<int>(legal1) + static_cast<int>(legal2), i=ep_moves.size_;
        ep_moves.size_ = count;
        ep_moves[i] = Move::make<EN_PASSANT>(sq1, ep_sq);         // writes legal1 ? move1 : 0
        ep_moves[i+legal1] = Move::make<EN_PASSANT>(sq2, ep_sq);         // writes legal2 ? move2 : 0

        return;
    }
    template<typename PieceC, typename T>
    template<Color c>
    void Position<PieceC, T>::genPawnSingleMoves(Movelist& moves) const
    {
        constexpr Direction UP = pawn_push(c);
        constexpr Bitboard PROMO_RANK = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[1];
        constexpr Bitboard PROMO_NEXT_RANK = (c == WHITE) ? attacks::MASK_RANK[7] : attacks::MASK_RANK[0];

        Bitboard pawns = pieces<PAWN, c>();

        Bitboard enemy_occ = occ(~c);
        Bitboard all_occ = occ();

        // Split pawns by pin direction
        Bitboard pawns_lr = pawns & ~current_state._rook_pin;  // pawns that can capture (not pinned along ranks/files)
        Bitboard unpinned_pawns_lr = pawns_lr & ~current_state._bishop_pin;
        Bitboard pinned_pawns_lr = pawns_lr & current_state._bishop_pin;

        Bitboard pawns_hv = pawns & ~current_state._bishop_pin; // pawns that can push (not pinned diagonally)
        Bitboard pinned_pawns_hv = pawns_hv & current_state._rook_pin;
        Bitboard unpinned_pawns_hv = pawns_hv & ~current_state._rook_pin;

        // ========== PAWN PUSHES (single step) ==========

        // Push squares for unpinned pawns
        Bitboard single_push_unpinned = (c == WHITE) ? (unpinned_pawns_hv << 8) : (unpinned_pawns_hv >> 8);
        single_push_unpinned &= ~all_occ;                // empty squares only

        // Push squares for pinned pawns: push only allowed if inside pin mask (rank/file)
        Bitboard single_push_pinned = (c == WHITE) ? (pinned_pawns_hv << 8) : (pinned_pawns_hv >> 8);
        single_push_pinned &= current_state._rook_pin & ~all_occ;

        // Combine pushes and mask by check mask
        Bitboard single_push = (single_push_unpinned | single_push_pinned) & current_state.check_mask;

        // ========== PAWN CAPTURES ==========

        constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
        constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;

        // Left captures
        Bitboard left_cap_pawns = pawns_lr & NOT_FILE_A;
        Bitboard left_targets = (c == WHITE) ? (left_cap_pawns << 7) : (left_cap_pawns >> 9);
        left_targets &= enemy_occ & current_state.check_mask;

        Bitboard left_sources_unpinned = (c == WHITE) ? (left_targets >> 7) : (left_targets << 9);
        Bitboard left_sources_pinned = left_sources_unpinned & current_state._bishop_pin;
        Bitboard left_sources_unpinned_only = left_sources_unpinned & ~current_state._bishop_pin;

        Bitboard left_valid = left_sources_unpinned_only | (left_sources_pinned & current_state._bishop_pin);

        // Right captures
        Bitboard right_cap_pawns = pawns_lr & NOT_FILE_H;
        Bitboard right_targets = (c == WHITE) ? (right_cap_pawns << 9) : (right_cap_pawns >> 7);
        right_targets &= enemy_occ & current_state.check_mask;

        Bitboard right_sources_unpinned = (c == WHITE) ? (right_targets >> 9) : (right_targets << 7);
        Bitboard right_sources_pinned = right_sources_unpinned & current_state._bishop_pin;
        Bitboard right_sources_unpinned_only = right_sources_unpinned & ~current_state._bishop_pin;

        Bitboard right_valid = right_sources_unpinned_only | (right_sources_pinned & current_state._bishop_pin);

        // ========== PROMOTIONS ==========

        // Promotions from pushes
        Bitboard promo_push = single_push & PROMO_RANK;

        // Promotions from captures (left)
        Bitboard promo_left = left_targets & PROMO_RANK;

        // Promotions from captures (right)
        Bitboard promo_right = right_targets & PROMO_RANK;

        // Pushes that are not promotions
        Bitboard normal_push = single_push & ~PROMO_RANK;

        // Left captures that are not promotions
        Bitboard normal_left = left_valid & ~PROMO_RANK;

        // Right captures that are not promotions
        Bitboard normal_right = right_valid & ~PROMO_RANK;

        // Generate normal pushes (non-promotions)
        while (normal_push)
        {
            Square to = static_cast<Square>(pop_lsb(normal_push));
            Square from = static_cast<Square>(to - UP);
            moves.push_back(Move(from, to));
        }

        // Generate normal captures (non-promotions)
        while (normal_left)
        {
            Square from = static_cast<Square>(pop_lsb(normal_left));
            Square to = static_cast<Square>(from + ((c == WHITE) ? 7 : -9));
            moves.push_back(Move(from, to));
        }

        while (normal_right)
        {
            Square from = static_cast<Square>(pop_lsb(normal_right));
            Square to = static_cast<Square>(from + ((c == WHITE) ? 9 : -7));
            moves.push_back(Move(from, to));
        }

        // Generate promotion pushes
        while (promo_push)
        {
            Square to = static_cast<Square>(pop_lsb(promo_push));
            Square from = static_cast<Square>(to - UP);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        // Generate promotion captures (left)
        while (promo_left)
        {
            Square to = static_cast<Square>(pop_lsb(promo_left));
            Square from = static_cast<Square>(to - ((c == WHITE) ? 7 : -9));
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        // Generate promotion captures (right)
        while (promo_right)
        {
            Square to = static_cast<Square>(pop_lsb(promo_right));
            Square from = static_cast<Square>(to - ((c == WHITE) ? 9 : -7));
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }
    }

    template<typename PieceC, typename T>
    template<Color c>
    void Position<PieceC, T>::genPawnDoubleMoves(Movelist& moves) const
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

    template <typename PieceC, typename T>
    template <Color c>
    void Position<PieceC, T>::genKingMoves(Movelist& a) const
    {
		constexpr CastlingRights K_CASTLING = (c == WHITE) ? WHITE_OO : BLACK_OO;
		constexpr CastlingRights Q_CASTLING = (c == WHITE) ? WHITE_OOO : BLACK_OOO;
        Square kingSq=current_state.kings[c];
        Bitboard kingMoves=attacks::king(kingSq) & ~current_state.occ[c];
        while (kingMoves)
        {
            Square sq=static_cast<Square>(pop_lsb(kingMoves));
            Bitboard att=attackers(~c, sq);
            bool add=!att;
            // Add move at current size + add - 1 if add == 1, else no-op
            a.size_ += add;
            a[a.size_ - add] = Move(kingSq, sq);
        }
        constexpr Square f1 = relative_square(c, SQ_F1);
        constexpr Square g1 = relative_square(c, SQ_G1);
        constexpr Square d1 = relative_square(c, SQ_D1);
        constexpr Square c1 = relative_square(c, SQ_C1);
        constexpr Square b1 = relative_square(c, SQ_B1);
        constexpr Square h1 = relative_square(c, SQ_H1);
        constexpr Square a1 = relative_square(c, SQ_A1);

        bool kingside_castle =
            (current_state.castlingRights & K_CASTLING) &&
            piece_on(f1) == PieceC::NO_PIECE &&
            piece_on(g1) == PieceC::NO_PIECE &&
            !current_state.checkers &&
            !attackers(~c, f1) &&
            !attackers(~c, g1);

        bool queenside_castle =
            (current_state.castlingRights & Q_CASTLING) &&
            piece_on(d1) == PieceC::NO_PIECE &&
            piece_on(c1) == PieceC::NO_PIECE &&
            piece_on(b1) == PieceC::NO_PIECE && // optional but commonly required
            !current_state.checkers &&
            !attackers(~c, d1) &&
            !attackers(~c, c1);
        int index=a.size_;
        a.size_+=kingside_castle+queenside_castle;
        a[index]=Move::make<CASTLING>(kingSq, relative_square(c, SQ_H1));
        a[index+kingside_castle]=Move::make<CASTLING>(kingSq, relative_square(c, SQ_A1));
    }
    template<typename PieceC, typename T>
    Position<PieceC, T>::Position(std::string fen)
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
    std::string Position<PieceC, T>::fen() const
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

    template void Position<EnginePiece, void>::genEP<Color::WHITE>(Movelist&) const;
    template void Position<EnginePiece, void>::genEP<Color::BLACK>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genEP<Color::WHITE>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genEP<Color::BLACK>(Movelist&) const;

    template void Position<EnginePiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist&) const;
    template void Position<EnginePiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist&) const;

    template void Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE>(Movelist&) const;
    template void Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK>(Movelist&) const;

    template void Position<EnginePiece, void>::genKnightMoves<Color::WHITE>(Movelist&) const;
    template void Position<EnginePiece, void>::genKnightMoves<Color::BLACK>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genKnightMoves<Color::WHITE>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genKnightMoves<Color::BLACK>(Movelist&) const;

    template void Position<EnginePiece, void>::genKingMoves<Color::WHITE>(Movelist&) const;
    template void Position<EnginePiece, void>::genKingMoves<Color::BLACK>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genKingMoves<Color::WHITE>(Movelist&) const;
    template void Position<PolyglotPiece, void>::genKingMoves<Color::BLACK>(Movelist&) const;

    template std::ostream& operator<<(std::ostream&, const Position<EnginePiece>&);
    template std::ostream& operator<<(std::ostream&, const Position<PolyglotPiece>&);

    template Position<EnginePiece, void>::Position(std::string);
    template Position<PolyglotPiece, void>::Position(std::string);
    template std::string Position<EnginePiece, void>::fen() const;
    template std::string Position<PolyglotPiece, void>::fen() const;
}
