#include "position.h"
#include <bitset>
#include <iomanip>
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
    ValueList<Move, 2> Position<PieceC, T>::genEP() const
    {
        ValueList<Move, 2> ep_moves;

        const Square king_sq = square<KING>(c);
        const Square ep_sq = ep_square();
        if (ep_sq == SQ_NONE) return ep_moves; // unavoidable one-time exit

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

        const int count = static_cast<int>(legal1) + static_cast<int>(legal2);
        ep_moves.size_ = count;
        ep_moves[0] = Move::make<EN_PASSANT>(sq1, ep_sq);         // writes legal1 ? move1 : 0
        ep_moves[legal1] = Move::make<EN_PASSANT>(sq2, ep_sq);         // writes legal2 ? move2 : 0

        return ep_moves;
    }
    template<typename PieceC, typename T>
    template<Color c>
    ValueList<Move, 86> Position<PieceC, T>::genPawnSingleMoves() const
    {
        ValueList<Move, 86> moves;

        constexpr Direction push_shift  = pawn_push(c);
        constexpr Direction left_shift  = (c == WHITE) ? NORTH_WEST : SOUTH_WEST;
        constexpr Direction right_shift = (c == WHITE) ? NORTH_EAST : SOUTH_EAST;

        constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
        constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;
        constexpr Bitboard promotion_rank = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[1];

        Bitboard pawns     = pieces<PAWN, c>();
        Bitboard enemy_occ = occ(~c);
        Bitboard all_occ   = occ();

        // Pin masks
        Bitboard bishop_pinned_pawns = pawns & current_state._bishop_pin;
        Bitboard rook_pinned_pawns   = pawns & current_state._rook_pin;
        Bitboard not_pinned_pawns    = pawns & ~(bishop_pinned_pawns | rook_pinned_pawns);

        // ========== FORWARD PUSHES ==========

        // Pawns pinned diagonally can't push
        Bitboard push_pawns = pawns & ~bishop_pinned_pawns;

        Bitboard one_step_push = (c == WHITE) ? (push_pawns << 8) : (push_pawns >> 8);
        one_step_push &= ~all_occ & current_state.check_mask;

        Bitboard pushes_nopromo = one_step_push & ~promotion_rank;
        Bitboard pushes_promo   = one_step_push &  promotion_rank;

        while (pushes_nopromo)
        {
            Square to   = static_cast<Square>(pop_lsb(pushes_nopromo));
            Square from = Square(int(to) - push_shift);
            moves.push_back(Move(from, to));
        }

        while (pushes_promo)
        {
            Square to   = static_cast<Square>(pop_lsb(pushes_promo));
            Square from = Square(int(to) - push_shift);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        // ========== LEFT CAPTURES ==========

        Bitboard left_cap_pawns = pawns & NOT_FILE_A;
        Bitboard left_targets = (c == WHITE)
            ? (left_cap_pawns << 7) & enemy_occ
            : (left_cap_pawns >> 9) & enemy_occ;
        left_targets &= current_state.check_mask;

        Bitboard left_sources = (c == WHITE)
            ? (left_targets >> 7)
            : (left_targets << 9);
        Bitboard left_valid = left_sources & (not_pinned_pawns | bishop_pinned_pawns);

        Bitboard left_cap_nopromo = left_valid & ~promotion_rank;
        Bitboard left_cap_promo   = left_valid &  promotion_rank;

        while (left_cap_nopromo)
        {
            Square from = static_cast<Square>(pop_lsb(left_cap_nopromo));
            Square to   = Square(int(from) + left_shift);
            moves.push_back(Move(from, to));
        }

        while (left_cap_promo)
        {
            Square from = static_cast<Square>(pop_lsb(left_cap_promo));
            Square to   = Square(int(from) + left_shift);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        // ========== RIGHT CAPTURES ==========

        Bitboard right_cap_pawns = pawns & NOT_FILE_H;
        Bitboard right_targets = (c == WHITE)
            ? (right_cap_pawns << 9) & enemy_occ
            : (right_cap_pawns >> 7) & enemy_occ;
        right_targets &= current_state.check_mask;

        Bitboard right_sources = (c == WHITE)
            ? (right_targets >> 9)
            : (right_targets << 7);
        Bitboard right_valid = right_sources & (not_pinned_pawns | bishop_pinned_pawns);

        Bitboard right_cap_nopromo = right_valid & ~promotion_rank;
        Bitboard right_cap_promo   = right_valid &  promotion_rank;

        while (right_cap_nopromo)
        {
            Square from = static_cast<Square>(pop_lsb(right_cap_nopromo));
            Square to   = Square(int(from) + right_shift);
            moves.push_back(Move(from, to));
        }

        while (right_cap_promo)
        {
            Square from = static_cast<Square>(pop_lsb(right_cap_promo));
            Square to   = Square(int(from) + right_shift);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        return moves;
    }

    template<typename PieceC, typename T>
    template<Color c>
    ValueList<Move, 8> Position<PieceC, T>::genPawnDoubleMoves() const
    {
        ValueList<Move, 8> moves;

        constexpr Bitboard START_RANK = attacks::MASK_RANK[relative_rank(c, RANK_2)];
        constexpr Direction PUSH    = pawn_push(c);
        Bitboard            pawns   = pieces<PAWN, c>() & START_RANK;
        Bitboard            all_occ = occ();

        // First step must be free
        Bitboard first_step = (c == WHITE) ? (pawns << 8) : (pawns >> 8);
        first_step &= ~all_occ;

        // Second step must also be free
        Bitboard second_step = (c == WHITE) ? (first_step << 8) : (first_step >> 8);
        second_step &= ~all_occ & current_state.check_mask;
        // Map back to source squares
        Bitboard double_pushers = (c == WHITE) ? (second_step >> 16) : (second_step << 16);

        // Exclude diagonally pinned pawns (illegal double push)
        double_pushers &= ~current_state._bishop_pin;

        // Generate moves
        while (double_pushers)
        {
            Square from = static_cast<Square>(lsb(double_pushers));
            Square to   = from + 2 * PUSH;
            moves.push_back(Move(from, to));
            double_pushers &= double_pushers - 1;
        }

        return moves;
    }
    template<typename PieceC, typename T>
    Position<PieceC, T>::Position(std::string fen)
    {
        current_state = HistoryEntry<PieceC>();
        std::istringstream ss(fen);
        std::string        board_fen, active_color, castling, enpassant;
        int                halfmove = 0, fullmove = 1;

        ss >> board_fen >> active_color >> castling >> enpassant >> halfmove >> fullmove;

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
                        placePiece<BLACK, PAWN>(make_sq(r, f));
                        break;
                    case 'P' :
                        placePiece<WHITE, PAWN>(make_sq(r, f));
                        break;
                    case 'n' :
                        placePiece<BLACK, KNIGHT>(make_sq(r, f));
                        break;
                    case 'N' :
                        placePiece<WHITE, KNIGHT>(make_sq(r, f));
                        break;
                    case 'b' :
                        placePiece<BLACK, BISHOP>(make_sq(r, f));
                        break;
                    case 'B' :
                        placePiece<WHITE, BISHOP>(make_sq(r, f));
                        break;
                    case 'r' :
                        placePiece<BLACK, ROOK>(make_sq(r, f));
                        break;
                    case 'R' :
                        placePiece<WHITE, ROOK>(make_sq(r, f));
                        break;
                    case 'q' :
                        placePiece<BLACK, QUEEN>(make_sq(r, f));
                        break;
                    case 'Q' :
                        placePiece<WHITE, QUEEN>(make_sq(r, f));
                        break;
                    case 'k' :
                        placePiece<BLACK, KING>(make_sq(r, f));
                        break;
                    case 'K' :
                        placePiece<WHITE, KING>(make_sq(r, f));
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

    template<typename PieceC, typename T>
    template<Color c>
    ValueList<Move, 104> Position<PieceC, T>::genKnightMoves() const
    {
        ValueList<Move, 104> list;
        Bitboard knights=pieces<KNIGHT, c>() & ~current_state._pin_mask; // yes, unconditionally.
        while (knights){
            Square x=static_cast<Square>(pop_lsb(knights));
            Bitboard moves=attacks::knight(x) & ~occ(c);
            while (moves){
                Square y=static_cast<Square>(pop_lsb(moves));
                list.push_back(Move(x,y));
            }
        }
        return list;
    }
    template <typename PieceC, typename T>
    template <Color c>
    ValueList<Move, 10> Position<PieceC, T>::genKingMoves() const
    {
        ValueList<Move, 10> a;
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
        int kingside_castle=!(current_state.checkers 
                          || attackers(~c, relative_square(c, SQ_F1)) 
                          || attackers(~c, relative_square(c, SQ_G1)))
                          && (current_state.castlingRights&KING_SIDE);
        int queenside_castle=!(current_state.checkers 
                            || attackers(~c, relative_square(c, SQ_D1))
                            || attackers(~c, relative_square(c, SQ_C1))
                            || attackers(~c, relative_square(c, SQ_B1)))
                            && (current_state.castlingRights&QUEEN_SIDE);
        int index=a.size();
        a.size_+=kingside_castle+queenside_castle;
        a[index]=Move::make<CASTLING>(kingSq, relative_square(c, SQ_H1));
        a[index+kingside_castle]=Move::make<CASTLING>(kingSq, relative_square(c, SQ_A1));
        return a;
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

    template ValueList<Move, 2> Position<EnginePiece, void>::genEP<Color::WHITE>() const;
    template ValueList<Move, 2> Position<EnginePiece, void>::genEP<Color::BLACK>() const;
    template ValueList<Move, 2> Position<PolyglotPiece, void>::genEP<Color::WHITE>() const;
    template ValueList<Move, 2> Position<PolyglotPiece, void>::genEP<Color::BLACK>() const;

    template ValueList<Move, 8> Position<EnginePiece, void>::genPawnDoubleMoves<Color::WHITE>() const;
    template ValueList<Move, 8> Position<EnginePiece, void>::genPawnDoubleMoves<Color::BLACK>() const;
    template ValueList<Move, 8> Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::WHITE>() const;
    template ValueList<Move, 8> Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::BLACK>() const;

    template ValueList<Move, 86> Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE>() const;
    template ValueList<Move, 86> Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK>() const;
    template ValueList<Move, 86> Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE>() const;
    template ValueList<Move, 86> Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK>() const;

    template ValueList<Move, 104> Position<EnginePiece, void>::genKnightMoves<Color::WHITE>() const;
    template ValueList<Move, 104> Position<EnginePiece, void>::genKnightMoves<Color::BLACK>() const;
    template ValueList<Move, 104> Position<PolyglotPiece, void>::genKnightMoves<Color::WHITE>() const;
    template ValueList<Move, 104> Position<PolyglotPiece, void>::genKnightMoves<Color::BLACK>() const;

    template ValueList<Move, 10> Position<EnginePiece, void>::genKingMoves<Color::WHITE>() const;
    template ValueList<Move, 10> Position<EnginePiece, void>::genKingMoves<Color::BLACK>() const;
    template ValueList<Move, 10> Position<PolyglotPiece, void>::genKingMoves<Color::WHITE>() const;
    template ValueList<Move, 10> Position<PolyglotPiece, void>::genKingMoves<Color::BLACK>() const;

    template std::ostream& operator<<(std::ostream&, const Position<EnginePiece>&);
    template std::ostream& operator<<(std::ostream&, const Position<PolyglotPiece>&);

    template Position<EnginePiece, void>::Position(std::string);
    template Position<PolyglotPiece, void>::Position(std::string);
    template std::string Position<EnginePiece, void>::fen() const;
    template std::string Position<PolyglotPiece, void>::fen() const;
}
