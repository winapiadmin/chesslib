#include "position.h"
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

        Square ep_sq = ep_square();
        if (ep_sq == SQ_NONE)
            return ep_moves;

        constexpr Bitboard NOT_A_FILE = 0xfefefefefefefefeULL;
        constexpr Bitboard NOT_H_FILE = 0x7f7f7f7f7f7f7f7fULL;

        const Bitboard ep_bb           = 1ULL << ep_sq;
        const Bitboard pawns           = pieces<PAWN, c>();
        const Square   king_sq         = square<KING>(c);
        Bitboard       left_attackers  = pawns & attacks::pawn(~c, ep_sq) & NOT_H_FILE;
        Bitboard       right_attackers = pawns & attacks::pawn(~c, ep_sq) & NOT_A_FILE;
        Square         from_left       = SQ_NONE;
        Square         from_right      = SQ_NONE;

        // We got pseudo-legal EP! But unfortunately.... there might be a pin at the capturing pawn (for example, 5k2/5b2/8/2pP4/2K5/8/8/8 w - c6 0 2)

        bool has_left  = left_attackers > 0;
        bool has_right = right_attackers > 0;
        from_left  = static_cast<Square>(lsb(left_attackers));
        from_right =  static_cast<Square>(lsb(right_attackers));
        // Because of how pins work, we just need to check if (bitboard<from_left> |bitboard<ep_square>) & pin mask == 0 to check if it's **not** pinned
        // e.g. 1q5k/8/8/3pP3/8/6K1/8/8 w - d6 0 1
        has_left &= (current_state._pin_mask & ((1ULL << from_left) | (1ULL << ep_sq))) == 0;
        has_right &= (current_state._pin_mask & ((1ULL << from_right) | (1ULL << ep_sq))) == 0;
        // EP capture, but after capturing it reveals a check, so illegal
        {
            const auto queens   = pieces<QUEEN, c>();
            Bitboard   occupied = occ() ^ left_attackers;
            if constexpr (c == WHITE)
                occupied &= ~(ep_bb >> 8);
            else
                occupied &= ~(ep_bb << 8);
            Bitboard enemy_rooks          = pieces<ROOK, ~c>();
            Bitboard enemy_bishops        = pieces<BISHOP, ~c>();
            Bitboard enemy_queens         = pieces<QUEEN, ~c>();
            Bitboard enemy_rook_sliders   = enemy_rooks | enemy_queens;
            Bitboard enemy_bishop_sliders = enemy_bishops | enemy_queens;

            has_left &= !(attacks::rook(king_sq, occupied) & enemy_rook_sliders);
            has_left &= !(attacks::bishop(king_sq, occupied) & enemy_bishop_sliders);

            // Update occupied after first check
            occupied ^= left_attackers ^ right_attackers;

            has_right &= !(attacks::rook(king_sq, occupied) & enemy_rook_sliders);
            has_right &= !(attacks::bishop(king_sq, occupied) & enemy_bishop_sliders);
        }
        ep_moves.size_ = static_cast<size_t>(has_left) + static_cast<size_t>(has_right);
        ep_moves[0] = Move::make<MoveType::EN_PASSANT>(from_left, ep_sq);
        ep_moves[has_left] = Move::make<MoveType::EN_PASSANT>(from_right, ep_sq);
        return ep_moves;
    }

    template<typename PieceC, typename T>
    template<Color c>
    ValueList<Move, 86> Position<PieceC, T>::genPawnSingleMoves() const
    {
        ValueList<Move, 86> moves;

        constexpr Direction push_shift  = (c == WHITE) ? NORTH : SOUTH;
        constexpr Direction left_shift  = (c == WHITE) ? NORTH_WEST : SOUTH_WEST;
        constexpr Direction right_shift = (c == WHITE) ? NORTH_EAST : SOUTH_EAST;

        constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
        constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;
        constexpr Bitboard promotion_bb = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[2];
        Bitboard pawns     = pieces<PAWN, c>();
        Bitboard enemy_occ = occ(~c);
        Bitboard all_occ   = occ();

        // --- FORWARD PUSHES ---
        Bitboard push_pawns = pawns & ~current_state._bishop_pin;  // disallow if diagonally pinned
        Bitboard pushes_nopromo = (c == WHITE) ? (push_pawns << 8) : (push_pawns >> 8);
        pushes_nopromo &= ~all_occ & current_state.check_mask;
        pushes_nopromo &= ~promotion_bb;
        Bitboard pushes     = pushes_nopromo & promotion_bb;
        // now create moves from `pushes` to destination squares
        // for each bit in `pushes`, derive source square: to - 8 or to + 8
        // add Move(from, to) to moves

        // --- LEFT CAPTURES ---
        Bitboard left_cap_pawns = pawns & NOT_FILE_A;
        Bitboard left_targets =
          ((c == WHITE) ? ((left_cap_pawns << 7) & enemy_occ) : ((left_cap_pawns >> 9) & enemy_occ))
          & current_state.check_mask;
        Bitboard left_sources = (c == WHITE) ? (left_targets >> 7) : (left_targets << 9);

        // --- RIGHT CAPTURES ---
        Bitboard right_cap_pawns = pawns & NOT_FILE_H;
        Bitboard right_targets   = ((c == WHITE) ? ((right_cap_pawns << 9) & enemy_occ)
                                                 : ((right_cap_pawns >> 7) & enemy_occ))
                               & current_state.check_mask;
        Bitboard right_sources = (c == WHITE) ? (right_targets >> 9) : (right_targets << 7);

        // --- Pin filtering ---
        Bitboard bishop_pinned_pawns = pawns & current_state._bishop_pin;
        Bitboard rook_pinned_pawns   = pawns & current_state._rook_pin;
        Bitboard not_pinned_pawns = pawns & ~(current_state._bishop_pin | current_state._rook_pin);

        Bitboard left_valid = left_sources & (not_pinned_pawns | bishop_pinned_pawns);
        Bitboard right_valid = right_sources & (not_pinned_pawns | bishop_pinned_pawns);

        Bitboard legal_left_cap_nopromo  = left_valid & ~promotion_bb;
        Bitboard legal_right_cap_nopromo = right_valid & ~promotion_bb;
        Bitboard legal_left_cap  = left_valid & promotion_bb;
        Bitboard legal_right_cap = right_valid & promotion_bb;

        // --- Add moves ---
        while (pushes_nopromo)
        {
            Square to   = static_cast<Square>(lsb(pushes_nopromo));
            Square from = Square(int(to) - (c == WHITE ? NORTH : SOUTH));
            moves.push_back(Move(from, to));
            pushes_nopromo &= pushes_nopromo - 1;
        }

        while (legal_left_cap_nopromo)
        {
            Square from = static_cast<Square>(lsb(legal_left_cap_nopromo));
            Square to   = Square(int(from) + left_shift);
            moves.push_back(Move(from, to));
            legal_left_cap_nopromo &= legal_left_cap_nopromo - 1;
        }

        while (legal_right_cap_nopromo)
        {
            Square from = static_cast<Square>(lsb(legal_right_cap_nopromo));
            Square to   = Square(int(from) + right_shift);
            moves.push_back(Move(from, to));
            legal_right_cap_nopromo &= legal_right_cap_nopromo - 1;
        }
        //Promotions
        while (legal_left_cap)
        {
            Square from = static_cast<Square>(pop_lsb(legal_left_cap));
            Square to   = Square(int(from) + left_shift);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        while (legal_right_cap)
        {
            Square from = static_cast<Square>(pop_lsb(legal_right_cap));
            Square to   = Square(int(from) + right_shift);
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
        }

        while (pushes)
        {
            Square to   = static_cast<Square>(lsb(pushes));
            Square from = Square(int(to) - (c == WHITE ? NORTH : SOUTH));
            moves.push_back(Move::make<PROMOTION, KNIGHT>(from, to));
            moves.push_back(Move::make<PROMOTION, BISHOP>(from, to));
            moves.push_back(Move::make<PROMOTION, ROOK>(from, to));
            moves.push_back(Move::make<PROMOTION, QUEEN>(from, to));
            pushes &= pushes - 1;
        }

        return moves;
    }

    template<typename PieceC, typename T>
    template<Color c>
    ValueList<Move, 8> Position<PieceC, T>::genPawnDoubleMoves() const
    {
        ValueList<Move, 8> moves;

        constexpr Bitboard START_RANK =
          (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
        constexpr Direction PUSH    = (c == WHITE) ? NORTH : SOUTH;
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
    Position<PieceC, T>::Position(std::string fen, bool xfen)
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

        // 4. En passant
        if (enpassant != "-" && enpassant.length() == 2 && enpassant[0] >= 'a'
            && enpassant[0] <= 'h' && enpassant[1] >= '1' && enpassant[1] <= '8')
        {
            File f                  = static_cast<File>(enpassant[0] - 'a');
            Rank r                  = static_cast<Rank>(enpassant[1] - '1');
            current_state.enPassant = make_sq(r, f);
            if (!xfen)
            {
                // We need to check if there's a pawn next to the last-pushed pawn
                constexpr Bitboard EP_Pawn_Lookup[2][8] = {
                  {0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000,
                   0x40000000},
                  {0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000,
                   0xa000000000, 0x4000000000}};
                current_state.hash ^= zobrist::RandomEP[f]
                                    * static_cast<bool>(occ(~sideToMove()) & EP_Pawn_Lookup[sideToMove()][f]);
            }
            else
            {
                current_state.hash ^= zobrist::RandomEP[file_of(current_state.enPassant)];
            }
        }
        else
        {
            assert(
              enpassant == "-"
              && "What are you choosing? You only have two choices: 1. \"-\" 2. a valid chess square");
            current_state.enPassant = SQ_NONE;
        }

        // 5. Halfmove clock
        current_state.halfMoveClock = static_cast<uint8_t>(halfmove);

        // 6. Fullmove number
        current_state.fullMoveNumber = fullmove;
        refresh_pin_mask();
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
    std::string Position<PieceC, T>::fen() const {
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

    template std::ostream& operator<<(std::ostream&, const Position<EnginePiece>&);
    template std::ostream& operator<<(std::ostream&, const Position<PolyglotPiece>&);

    template Position<EnginePiece, void>::Position(std::string, bool);
    template Position<PolyglotPiece, void>::Position(std::string, bool);
    template std::string Position<EnginePiece, void>::fen() const;
    template std::string Position<PolyglotPiece, void>::fen() const;
}
