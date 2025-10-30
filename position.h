#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "types.h"
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include "zobrist.h"
namespace chess {
    template<typename Piece>
    struct HistoryEntry {
        // Bitboards for each piece type (white and black)
        Bitboard pieces[PIECE_TYPE_NB];
        Square   kings[COLOR_NB] = {SQ_NONE};
        Bitboard occ[COLOR_NB], seen[COLOR_NB];
        Piece    pieces_list[SQUARE_NB]
            = { Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
                Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE };
        // Game state information
        Color    turn;            // true if white to move
        CastlingRights  castlingRights;  // Castling rights bitmask
        Square   enPassant = SQ_NONE; // En passant target square
        uint8_t  halfMoveClock;   // Half-move clock for 50/75-move rule
        int      fullMoveNumber;  // Full-move number (starts at 1)
        bool     epIncluded;
        Move     mv;
        uint64_t hash;
        Bitboard _pin_mask;
        Bitboard _rook_pin;
        Bitboard _bishop_pin;
        Bitboard checkers, check_mask;
        // implementation-specific implementations goes here
    };
    template<typename Piece>
    constexpr bool operator==(const HistoryEntry<Piece>& a, const HistoryEntry<Piece>& b) noexcept {
        using std::begin; using std::end;
        return
            std::equal(begin(a.pieces), end(a.pieces), begin(b.pieces)) &&
            std::equal(begin(a.pieces_list), end(a.pieces_list), begin(b.pieces_list)) &&
            std::equal(begin(a.kings), end(a.kings), begin(b.kings)) &&
            std::equal(begin(a.occ), end(a.occ), begin(b.occ)) &&
            a.turn == b.turn &&
            a.castlingRights == b.castlingRights &&
            a.enPassant == b.enPassant &&
            a.halfMoveClock == b.halfMoveClock &&
            a.fullMoveNumber == b.fullMoveNumber &&
            a.mv == b.mv &&
            a.hash == b.hash &&
            a._pin_mask == b._pin_mask &&
            a._rook_pin == b._rook_pin &&
            a._bishop_pin == b._bishop_pin &&
            a.checkers == b.checkers &&
            a.check_mask == b.check_mask;
    }

    enum class MoveGenType : uint8_t {
        ALL,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        CAPTURE
    };
    template<typename PieceC = EnginePiece,
             typename        = std::enable_if_t<std::is_same_v<PieceC, EnginePiece>
                                                || std::is_same_v<PieceC, PolyglotPiece>>>
    class _Position
    {
        private:
        static constexpr const std::array<uint64_t, 64> (*RandomPiece) = std::is_same_v<PieceC, PolyglotPiece>
                                                                 ? zobrist::RandomPiece
                                                                 : zobrist::RandomPiece_EnginePiece;

        HistoryEntry<PieceC> current_state;
        // Move history stack
        HeapAllocatedValueList<HistoryEntry<PieceC>, 8192>
          history;  // ahh, but i hope it fulfils before I manages to find the absolute limit of a game
        // Move generation functions, but INTERNAL. (they're kind of long so i put them into a source file)
        // Pawns (fully extensively tested)
        template<Color c>
        void genEP(Movelist &mv) const;
        template<Color c>
        void genPawnDoubleMoves(Movelist &mv) const;
        template<Color c>
        void genPawnSingleMoves(Movelist &mv) const;
        template<Color c>
        void genKnightMoves(Movelist &list) const
        {
            Bitboard knights = pieces<KNIGHT, c>() & ~current_state._pin_mask; // yes, unconditionally.
            while (knights) {
                Square x = static_cast<Square>(pop_lsb(knights));
                Bitboard moves = attacks::knight(x) & ~occ(c);
                moves &= current_state.check_mask;
                while (moves) {
                    Square y = static_cast<Square>(pop_lsb(moves));
                    list.push_back(Move(x, y));
                }
            }
        }
        template<Color c>
        void genKingMoves(Movelist &mv) const;
        template<Color c, PieceType pt>
        void genSlidingMoves(Movelist& mv) const;
        public:
        // Legal move generation functions
        template<MoveGenType type=MoveGenType::ALL, Color c>
        inline void                legals(Movelist& out) const {
            if constexpr (type==MoveGenType::ALL){
                // Simple cases
                genEP<c>(out);
				genPawnSingleMoves<c>(out);
				genPawnDoubleMoves<c>(out);
				genKnightMoves<c>(out);
				genKingMoves<c>(out);
				genSlidingMoves<c, BISHOP>(out);
				genSlidingMoves<c, ROOK>(out);
				genSlidingMoves<c, QUEEN>(out);
            }
            else if constexpr (type==MoveGenType::PAWN){
				genEP<c>(out);
				genPawnDoubleMoves<c>(out);
				genPawnSingleMoves<c>(out);
            }
            else if constexpr (type==MoveGenType::KNIGHT){
				genKnightMoves<c>(out);
            }
            else if constexpr (type==MoveGenType::KING){
                genKingMoves<c>(out);
            }
            else if constexpr (type==MoveGenType::CAPTURE){
                // Well.... it's SLOWER than everything because this uses branching and another pre-processing variable.
                // Tips in quiescence search: Filter first using legals()
                ValueList<Move, 256> _out;
                legals<MoveGenType::ALL, c>(_out);
                for (Move mv:_out){
                    if (isCapture(mv)) out.push_back(mv);
                }
            }
        }
        // Legal move generation functions
        template<MoveGenType type=MoveGenType::ALL>
        inline void                legals(ValueList<Move, 256>& out) const{
            const Color stm = sideToMove();  // Cache it
            ASSUME(stm == WHITE || stm == BLACK);  // Now clearly no side effects

            switch (stm) {
                case WHITE:
                    legals<type, WHITE>(out);
                    return;
                case BLACK:
                    legals<type, BLACK>(out);
                    return;
                default:
                    UNREACHABLE();
                    return;
            }
        }
        /** \brief Initializes the board as empty.
         *
         * \param nothing
         * \param nothing
         * \return nothing
         *
         */
        inline _Position(HistoryEntry<PieceC> state)
        {
            // compatible!
            current_state = state;
            refresh_attacks();
        }
        _Position(const _Position& other)
            : current_state(other.current_state),
            history(other.history)  // calls HeapAllocatedValueList's copy constructor
        {}

        template<
          typename T,
          std::enable_if_t<(std::is_same_v<T, PolyglotPiece> || std::is_same_v<T, EnginePiece>)
                             && !std::is_same_v<T, PieceC>,
                           int> = 0>
        [[deprecated("Incompatible piece method used, you shouldn't call this")]] inline _Position(
          HistoryEntry<T> state)
        {
            // incompatible!
            current_state                = HistoryEntry<PieceC>();
            current_state.turn           = state.turn;
            current_state.castlingRights = state.castlingRights;
            current_state.enPassant      = state.enPassant;
            current_state.halfMoveClock  = state.halfMoveClock;
            current_state.fullMoveNumber = state.fullMoveNumber;
            current_state.hash           = state.hash;
#pragma unroll
            for (int s = 0; s < 64; ++s)
            {
                current_state.pieces_list[s] = make_piece<PieceC>(
                  type_of(state.pieces_list[s]), color_of(state.pieces_list[s]));
            }
            refresh_attacks();
        }

        inline void doMove(const Move& move) {
            Square from_sq=move.from_sq(), to_sq=move.to_sq();
            Color us=side_to_move(), them=~us;
            MoveType move_type=move.type_of();
            PieceC moving_piece = piece_on(from_sq);
            PieceC target_piece = piece_on(to_sq);
            PieceType moving_piecetype = piece_of(moving_piece);
            PieceType target_piecetype = piece_of(target_piece);
            Color target_color = color_of(target_piece);
            bool is_capture=isCapture(move);
            history.push_back(current_state);
            current_state.mv = move; // Update the move in the current state
            //if (target_piecetype == KING) throw std::invalid_argument("NO WAY");
            removePiece(moving_piecetype, from_sq, us);
            removePiece(target_piecetype, to_sq, target_color);
            {
                ASSUME(move_type==NORMAL||move_type==PROMOTION||move_type==EN_PASSANT||move_type==CASTLING);
                switch(move_type){
                    case NORMAL:
                        placePiece(moving_piecetype, to_sq, us);
                        break;
                    case PROMOTION:
                        placePiece(move.promotion_type(), to_sq, us);
                        break;
                    case EN_PASSANT:
                        {
                            Square ep_capture_sq = to_sq+pawn_push(them);
                            removePiece<PAWN>(ep_capture_sq, them);
                            placePiece<PAWN>(to_sq, us);
                            break;
                        }
                    case CASTLING: 
                        {
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
                if (current_state.enPassant != SQ_NONE && current_state.epIncluded)
                    current_state.hash ^= zobrist::RandomEP[file_of(current_state.enPassant)];

                // Default: no EP square and not included.
                current_state.enPassant = SQ_NONE;
                current_state.epIncluded = false;

                // If the move creates a potential EP target:
                if (isDoublePush)
                    current_state.enPassant = from_sq + pawn_push(us);

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
                    if (ep_mask & pieces<PAWN>(stm)) {
                        current_state.hash ^= zobrist::RandomEP[f];
                        current_state.epIncluded = true;
                    }
                }
            }

            {
                constexpr Square white_king_start = SQ_E1,
                                 black_king_start = SQ_E8,
                                 white_rook_ks_start = SQ_H1,
                                 white_rook_qs_start = SQ_A1,
                                 black_rook_ks_start = SQ_H8,
                                 black_rook_qs_start = SQ_A8;
                CastlingRights clear_white_king = (us == WHITE && moving_piecetype == KING && from_sq == white_king_start)
                                            ? WHITE_CASTLING : NO_CASTLING;

                CastlingRights clear_black_king = (us == BLACK && moving_piecetype == KING && from_sq == black_king_start)
                                            ? BLACK_CASTLING : NO_CASTLING;

                CastlingRights clear_white_rook_ks = (us == WHITE && moving_piecetype == ROOK && from_sq == white_rook_ks_start)
                                                ? WHITE_OO : NO_CASTLING;

                CastlingRights clear_white_rook_qs = (us == WHITE && moving_piecetype == ROOK && from_sq == white_rook_qs_start)
                                                ? WHITE_OOO : NO_CASTLING;

                CastlingRights clear_black_rook_ks = (us == BLACK && moving_piecetype == ROOK && from_sq == black_rook_ks_start)
                                                ? BLACK_OO : NO_CASTLING;

                CastlingRights clear_black_rook_qs = (us == BLACK && moving_piecetype == ROOK && from_sq == black_rook_qs_start)
                                                ? BLACK_OOO : NO_CASTLING;

                CastlingRights clear_white_rook_ks_captured = (target_color == WHITE && target_piecetype == ROOK && to_sq == white_rook_ks_start)
                                                        ? WHITE_OO : NO_CASTLING;

                CastlingRights clear_white_rook_qs_captured = (target_color == WHITE && target_piecetype == ROOK && to_sq == white_rook_qs_start)
                                                        ? WHITE_OOO : NO_CASTLING;

                CastlingRights clear_black_rook_ks_captured = (target_color == BLACK && target_piecetype == ROOK && to_sq == black_rook_ks_start)
                                                        ? BLACK_OO : NO_CASTLING;

                CastlingRights clear_black_rook_qs_captured = (target_color == BLACK && target_piecetype == ROOK && to_sq == black_rook_qs_start)
                                                        ? BLACK_OOO : NO_CASTLING;

                CastlingRights clear_mask =
                    clear_white_king
                    | clear_black_king
                    | clear_white_rook_ks
                    | clear_white_rook_qs
                    | clear_black_rook_ks
                    | clear_black_rook_qs
                    | clear_white_rook_ks_captured
                    | clear_white_rook_qs_captured
                    | clear_black_rook_ks_captured
                    | clear_black_rook_qs_captured;
                CastlingRights prev=current_state.castlingRights;
                current_state.castlingRights &= ~clear_mask;
                current_state.hash ^= zobrist::RandomCastle[prev]^zobrist::RandomCastle[current_state.castlingRights];
            }
            current_state.turn = ~current_state.turn;
            // Update halfmoves, fullmoves and stm
            current_state.fullMoveNumber += (current_state.turn == WHITE);
            current_state.halfMoveClock = (is_capture || moving_piecetype == PAWN) ? 0 : (current_state.halfMoveClock+1);
            current_state.hash ^= zobrist::RandomTurn;
            refresh_attacks();
        }

        template<bool RetAll = false>
        inline auto undoMove()
        {
            current_state = history.pop();

            if constexpr (RetAll)
            {
                return current_state;
            }
        }

        inline void doNullMove()
        {
            current_state.turn = ~current_state.turn;
            current_state.hash ^= zobrist::RandomTurn;
            current_state.fullMoveNumber += (current_state.turn == WHITE);
            history.push_back(current_state);
        }


        inline Bitboard pieces() const { return occ(); }
        template<PieceType pt>
        inline Bitboard pieces(Color c) const
        {
            ASSUME(c == WHITE || c == BLACK);
            if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
                return occ(c);
            return current_state.pieces[pt] & current_state.occ[c];
        }
        template<Color c>
        inline Bitboard pieces(PieceType pt) const
        {
            ASSUME(c == WHITE || c == BLACK);
            if (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
                return occ(c);
            return current_state.pieces[pt] & current_state.occ[c];
        }
        template<PieceType pt, Color c>
        inline Bitboard pieces() const
        {
            ASSUME(c == WHITE || c == BLACK);
            if constexpr (pt == PIECE_TYPE_NB || pt==ALL_PIECES)
                return occ(c);
            return current_state.pieces[pt] & current_state.occ[c];
        }
        inline Bitboard pieces(PieceType pt, Color c) const
        {
            ASSUME(c == WHITE || c == BLACK);
            // still branchless
            switch (pt)
            {
            case PIECE_TYPE_NB :
            case ALL_PIECES :
                return occ(c);
            default :
                return current_state.pieces[pt] & current_state.occ[c];
            }
        }

        template<typename... PTypes,
            typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>>
            [[nodiscard]] inline Bitboard pieces(PTypes... ptypes) const {
            return (pieces(ptypes) | ...);
        }

        template<typename... PTypes,
            typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>>
            [[nodiscard]] inline Bitboard pieces(Color c, PTypes... ptypes) const {
            return (pieces(ptypes, c) | ...);
        }

        /**
         * @brief Returns the origin squares of pieces of a given color attacking a target square
         * @param board
         * @param color Attacker Color
         * @param square Attacked Square
         * @return
         */
        /** Sample code to not make myself (and others) confused:
        ```c++
        if (attackers(BLACK, E4)) {
            std::cout << "E4 is under attack by black!";
        }
        ```
         */
        [[nodiscard]] inline Bitboard attackers(Color color, Square square, Bitboard occupied) const
        {
            auto queens   = pieces<QUEEN>(color);

            // using the fact that if we can attack PieceType from square, they can attack us back
            auto atks = (attacks::pawn(~color, square) & pieces<PAWN>(color));
            atks |= (attacks::knight(square) & pieces<KNIGHT>(color));
            atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(color) | queens));
            atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(color) | queens));
            atks |= (attacks::king(square) & pieces<KING>(color));

            return atks & occupied;
        }
        [[nodiscard]] inline Bitboard attackers(Color color, Square square) const { return attackers(color, square, occ()); }
        // Compile-time piece type and color, runtime square
        template<PieceType pt>
        inline void placePiece(Square s, Color c) {
            if constexpr (pt != NO_PIECE_TYPE) {
                current_state.pieces[pt] |= 1ULL << s;
                current_state.occ[c] |= 1ULL << s;
                current_state.pieces_list[s] = make_piece<PieceC>(pt, c);
                current_state.hash ^= RandomPiece[static_cast<int>(current_state.pieces_list[s])][s];
                if constexpr (pt == KING) current_state.kings[c] = s;
            }
        }

        template<PieceType pt>
        inline void removePiece(Square s, Color c) {
            if constexpr (pt != NO_PIECE_TYPE) {
                current_state.pieces[pt] &= ~(1ULL << s);
                current_state.occ[c] &= ~(1ULL << s);
                current_state.pieces_list[s] = PieceC::NO_PIECE;
                current_state.hash ^= RandomPiece[static_cast<int>(make_piece<PieceC>(pt, c))][s];
                if constexpr (pt == KING) current_state.kings[c] = SQ_NONE;
            }
        }

        inline void placePiece(PieceType pt, Square s, Color c) {
            if (pt != NO_PIECE_TYPE) {
                current_state.pieces[pt] |= 1ULL << s;
                current_state.occ[c] |= 1ULL << s;
                current_state.pieces_list[s] = make_piece<PieceC>(pt, c);
                current_state.hash ^= RandomPiece[static_cast<int>(current_state.pieces_list[s])][s];
                if (pt == KING) current_state.kings[c] = s;
            }
        }

        inline void removePiece(PieceType pt, Square s, Color c) {
            if (pt != NO_PIECE_TYPE) {
                current_state.pieces[pt] &= ~(1ULL << s);
                current_state.occ[c] &= ~(1ULL << s);
                current_state.pieces_list[s] = PieceC::NO_PIECE;
                current_state.hash ^= RandomPiece[static_cast<int>(make_piece<PieceC>(pt, c))][s];
                if (pt == KING) current_state.kings[c] = SQ_NONE;
            }
        }

        inline Bitboard occ(Color c) const { ASSUME(c!=COLOR_NB); return current_state.occ[c]; }
        inline Bitboard occ() const { return current_state.occ[0] | current_state.occ[1]; }
        PieceC          piece_on(Square s) const { 
#ifndef _DEBUG
            return current_state.pieces_list[s];
#else
            auto p = current_state.pieces_list[s];
            auto p2 = piece_of(p);
            if (p2 == ALL_PIECES)return p;
            else {
                Bitboard bb = 1ULL << s;
                Bitboard _ = pieces(p2, color_of(p));
                Bitboard b = _ & bb;
                assert(b);
			}
            return p;
#endif
        }
        inline Color    sideToMove() const { return current_state.turn; }
        inline uint64_t hash() const { return current_state.hash; }
        inline uint64_t key() const { return current_state.hash; }
        inline Color    side_to_move() const { return current_state.turn; }
        inline Square   ep_square() const { return current_state.enPassant; }
        template<PieceType pt>
        inline Square square(Color c) const { return Square(lsb(pieces<pt>(c))); }
        inline Bitboard checkers() const { return current_state.checkers; }
        _Position(std::string fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        inline bool isCapture(Move mv) const { return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE); }

        std::string fen() const;
        inline int halfmoveClock() const { return current_state.halfMoveClock; }
        inline int fullmoveNumber() const { return current_state.fullMoveNumber; }
        inline int rule50_count() const { return current_state.halfMoveClock; }
        CastlingRights castlingRights(Color c) const { return current_state.castlingRights&(c==WHITE?WHITE_CASTLING:BLACK_CASTLING); }
        CastlingRights castlingRights() const { return current_state.castlingRights; }
    private:
        template<PieceType pt>
        [[nodiscard]] inline Bitboard pinMask(Color c, Square sq) const
        {
            static_assert(pt == BISHOP || pt == ROOK, "Only bishop or rook allowed!");
            Bitboard occ_opp = occ(~sideToMove());
            Bitboard occ_us = occ(sideToMove());
            Bitboard opp_sliders;
            opp_sliders = (pieces<pt>(~c) | pieces(QUEEN, ~c)) & occ_opp;

            auto pt_attacks = attacks::slider<pt>(sq, occ_opp) & opp_sliders;

            Bitboard pin = 0ull;

            while (pt_attacks)
            {
                const auto possible_pin = movegen::between(sq, Square(lsb(pt_attacks)));
                if (popcount(possible_pin & occ_us) == 1)
                    pin |= possible_pin;
                pt_attacks &= (pt_attacks - 1);
            }

            return pin;
        }
        inline void refresh_attacks()
        {
            Color    c                = sideToMove();

            current_state._bishop_pin = pinMask<BISHOP>(c, square<KING>(c));
            current_state._rook_pin   = pinMask<ROOK>(c, square<KING>(c));
            current_state._pin_mask   = current_state._bishop_pin | current_state._rook_pin;

            Square   king_sq          = square<KING>(c);
            Bitboard checkers         = attackers(~c, king_sq);
            current_state.checkers    = checkers;

            int num_checks = popcount(checkers);

            switch (num_checks)
            {
            case 0 :
                current_state.check_mask = ~0ULL;  // no checks, full mask
                break;

            case 1 : {
                Square   sq              = static_cast<Square>(lsb(checkers));
                Bitboard mask            = (1ULL << sq) | movegen::between(king_sq, sq);
                current_state.check_mask = mask;
                break;
            }

            default :
                current_state.check_mask = 0ULL;  // multiple checks, no blocking mask
                break;
            }
            const Bitboard occ_all = occ();

            current_state.seen[WHITE] = 0;
            current_state.seen[BLACK] = 0;
            // --- Pawns ---
            Bitboard wp = pieces<PAWN, WHITE>();
            current_state.seen[WHITE] |= attacks::pawn<WHITE>(wp);
            Bitboard bp = pieces<PAWN, BLACK>();
            current_state.seen[BLACK] |= attacks::pawn<BLACK>(bp);

            // --- Knights ---
            Bitboard wn = pieces<KNIGHT, WHITE>();
            current_state.seen[WHITE] |= attacks::knight(wn);
            Bitboard bn = pieces<KNIGHT, BLACK>();
            current_state.seen[BLACK] |= attacks::knight(bn);
            // --- Bishops & Queens (diagonals) ---
            Bitboard wb = pieces<BISHOP, WHITE>() | pieces<QUEEN, WHITE>();
            while (wb) {
                Square sq = static_cast<Square>(pop_lsb(wb));
                current_state.seen[WHITE] |= attacks::bishop(sq, occ_all);
            }
            Bitboard bb = pieces<BISHOP, BLACK>() | pieces<QUEEN, BLACK>();
            while (bb) {
                Square sq = static_cast<Square>(pop_lsb(bb));
                current_state.seen[BLACK] |= attacks::bishop(sq, occ_all);
            }

            // --- Rooks & Queens (orthogonals) ---
            Bitboard wr = pieces<ROOK, WHITE>() | pieces<QUEEN, WHITE>();
            while (wr) {
                Square sq = static_cast<Square>(pop_lsb(wr));
                current_state.seen[WHITE] |= attacks::rook(sq, occ_all);
            }
            Bitboard br = pieces<ROOK, BLACK>() | pieces<QUEEN, BLACK>();
            while (br) {
                Square sq = static_cast<Square>(pop_lsb(br));
                current_state.seen[BLACK] |= attacks::rook(sq, occ_all);
            }

            // --- Kings ---
            Square wk = current_state.kings[WHITE];
            Square bk = current_state.kings[BLACK];
            current_state.seen[WHITE] |= attacks::king(wk);
            current_state.seen[BLACK] |= attacks::king(bk);
        }
    public:
      HistoryEntry<PieceC> state() const { return current_state; }
      uint64_t zobrist() const {
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
              Color them = ~stm;
              ep_mask = (stm == WHITE) ? (ep_mask >> 8) : (ep_mask << 8);

              // Pawns on adjacent files only
              ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) |
                        ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);

              if (ep_mask & pieces<PAWN>(stm))
                  hash ^= zobrist::RandomEP[f];
          }
          return hash;
      }
    };
    template<typename PieceC = EnginePiece>
    std::ostream& operator<<(std::ostream& os, const _Position<PieceC>& pos);
	using Position = _Position<EnginePiece>; // for some fun because I HATE HARDCODING
};
