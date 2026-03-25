#include "movegen.h"
#include "position.h"

namespace chess {

namespace _chess {

#if defined(USE_AVX512ICL)

// clang-format off
const __m512i AllSquares = _mm512_set_epi8(
  63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,
  40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18,
  17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
// clang-format on

template <Direction offset> inline Move *splat_pawn_moves(Move *moveList, Bitboard to_bb) {
    assert(popcount(to_bb) <= 8); // <= 8 pawns per side

    const __m128i toSquares = _mm_cvtepi8_epi16(_mm512_castsi512_si128(_mm512_maskz_compress_epi8(to_bb, AllSquares)));
    const __m128i fromSquares = _mm_subs_epi16(toSquares, _mm_set1_epi16(offset));
    const __m128i moves = _mm_or_si128(_mm_slli_epi16(fromSquares, 6), _mm_slli_epi16(toSquares, 0));

    _mm_storeu_si128(reinterpret_cast<__m128i *>(moveList), moves);
    return moveList + popcount(to_bb);
}

inline Move *splat_moves(Move *moveList, Square from, Bitboard to_bb) {
    assert(popcount(to_bb) <= 32); // Q can attack up to 27 squares

    const __m512i fromVec = _mm512_set1_epi16(Move(from, SQUARE_ZERO).raw());
    const __m512i toSquares = _mm512_cvtepi8_epi16(_mm512_castsi512_si256(_mm512_maskz_compress_epi8(to_bb, AllSquares)));
    const __m512i moves = _mm512_or_si512(fromVec, _mm512_slli_epi16(toSquares, Move::ToSqShift));

    _mm512_storeu_si512(moveList, moves);
    return moveList + popcount(to_bb);
}

#elif defined(__AVX512BW__)
template <int Offset = 0> struct alignas(64) SplatTable {
    std::array<uint16_t, 64> data;
    constexpr int clamp64(int x) { return (x < 0) ? 0 : (x > 63 ? 63 : x); }

    constexpr SplatTable() : data{} {
        for (int i = 0; i < 64; ++i) {
            int from = clamp64(i - Offset);
            data[i] = Move((Square)from, (Square)i).raw();
        }
    }
};

constexpr SplatTable<> SPLAT_TABLE{};
template <int Offset> constexpr SplatTable<Offset> SPLAT_PAWN_TABLE{};
// AVX-512 (32 lanes of uint16_t)
static inline Move *write_moves(Move *moveList, uint32_t mask, __m512i vector) {
    // Avoid _mm512_mask_compressstoreu_epi16() as it's 256 uOps on Zen4
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(moveList), _mm512_maskz_compress_epi16(mask, vector));
    return moveList + popcount(mask);
}

inline Move *splat_moves(Move *moveList, Square from, Bitboard to_bb) {
    const auto *table = reinterpret_cast<const __m512i *>(SPLAT_TABLE.data.data());
    __m512i fromVec = _mm512_set1_epi16(Move(from, SQUARE_ZERO).raw());
    // two 32-lane blocks (0..31, 32..63)
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm512_or_si512(_mm512_load_si512(table + 0), fromVec));
    moveList =
        write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm512_or_si512(_mm512_load_si512(table + 1), fromVec));

    return moveList;
}

template <Direction offset> inline Move *splat_pawn_moves(Move *moveList, Bitboard to_bb) {
    const auto *table = reinterpret_cast<const __m512i *>(SPLAT_PAWN_TABLE<offset>.data.data());
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm512_load_si512(table + 0));
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm512_load_si512(table + 1));

    return moveList;
}
#else
template <Direction offset> inline Move *splat_pawn_moves(Move *moveList, Bitboard to_bb) {
    while (to_bb) {
        auto to = static_cast<Square>(pop_lsb(to_bb));
#if defined(_DEBUG) || !defined(NDEBUG)
        Square from = to - offset;
        assert(from >= 0 && from < 64); // sanity check
        *moveList++ = Move(from, to);
#else
        *moveList++ = Move(to - offset, to);
#endif
    }
    return moveList;
}

inline Move *splat_moves(Move *moveList, Square from, Bitboard to_bb) {
    while (to_bb)
        *moveList++ = Move(from, static_cast<Square>(pop_lsb(to_bb)));
    return moveList;
}
#endif
} // namespace _chess
} // namespace chess
namespace chess {
template <typename T, Color c> void movegen::genEP(const _Position<T, void> &pos, Movelist &mv) {

    const Square king_sq = pos.kingSq(c);
    const Square ep_sq = pos.ep_square();
    if (ep_sq == SQ_NONE)
        return;

    Bitboard candidates = attacks::pawn(~c, ep_sq) & pos.template pieces<PAWN, c>();
    if (!candidates)
        return;

    const Square ep_pawn_sq = ep_sq - pawn_push(c);
    const Bitboard ep_mask = (1ULL << ep_pawn_sq) | (1ULL << ep_sq);
    // ASSUME(popcount(candidates) <= 32);

    while (candidates) {
        Square from = static_cast<Square>(pop_lsb(candidates));

        // Remove the EP pawn and this attacker from occupancy
        Bitboard occ_temp = pos.occ();
        occ_temp ^= (1ULL << from) | ep_mask;

        // attackers check
        Bitboard atks = 0;
        atks |= attacks::bishop(king_sq, occ_temp) & (pos.template pieces<BISHOP, ~c>() | pos.template pieces<QUEEN, ~c>());
        atks |= attacks::rook(king_sq, occ_temp) & (pos.template pieces<ROOK, ~c>() | pos.template pieces<QUEEN, ~c>());
        atks &= pos.occ(~c);
        if (!atks) {
            mv.push_back(Move::make<EN_PASSANT>(from, ep_sq));
        }
    }
}
template <typename T, Color c>
void movegen::genPawnDoubleMoves(const _Position<T, void> &pos, Movelist &moves, Bitboard pin_mask, Bitboard check_mask) {
    constexpr Bitboard RANK_2 = (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
    constexpr Direction UP = pawn_push(c);

    Bitboard all_occ = pos.occ();
    Bitboard pawns = pos.template pieces<PAWN, c>() & RANK_2;

    // Split pin types
    Bitboard pin_file = pin_mask & attacks::MASK_FILE[file_of(pos.kingSq(c))];

    Bitboard unpinned = pawns & ~pin_mask;
    Bitboard file_pinned = pawns & pin_file;

    // First step must be empty
    Bitboard step1_unpinned = attacks::shift<UP>(unpinned) & ~all_occ;
    Bitboard step1_pinned = attacks::shift<UP>(file_pinned) & pin_file & ~all_occ;

    // Second step must also be empty
    Bitboard step2_unpinned = attacks::shift<UP>(step1_unpinned) & ~all_occ;
    Bitboard step2_pinned = attacks::shift<UP>(step1_pinned) & pin_file & ~all_occ;

    Bitboard destinations = (step2_unpinned | step2_pinned) & check_mask;

    _chess::splat_pawn_moves<2 * UP>(moves.data() + moves.size_, destinations);
    moves.size_ += popcount(destinations);
}
template <typename T, Color c, bool capturesOnly>
void movegen::genPawnSingleMoves(
    const _Position<T, void> &pos, Movelist &moves, Bitboard _rook_pin, Bitboard _bishop_pin, Bitboard _check_mask) {
    constexpr auto UP = relative_direction(c, NORTH);
    constexpr auto UP_LEFT = relative_direction(c, NORTH_WEST);
    constexpr auto UP_RIGHT = relative_direction(c, NORTH_EAST);
    constexpr auto RANK_PROMO = attacks::MASK_RANK[relative_rank(c, RANK_8)];

    const auto pawns = pos.template pieces<PAWN, c>();
    const auto occ_opp = pos.occ(~c);
    const auto occ_all = pos.occ();
    // These pawns can maybe take Left or Right
    const Bitboard pawns_lr = pawns & ~_rook_pin;
    const Bitboard unpinned_pawns_lr = pawns_lr & ~_bishop_pin;
    const Bitboard pinned_pawns_lr = pawns_lr & _bishop_pin;

    auto l_pawns = attacks::shift<UP_LEFT>(unpinned_pawns_lr) | (attacks::shift<UP_LEFT>(pinned_pawns_lr) & _bishop_pin);
    auto r_pawns = attacks::shift<UP_RIGHT>(unpinned_pawns_lr) | (attacks::shift<UP_RIGHT>(pinned_pawns_lr) & _bishop_pin);
    // Prune moves that don't capture a piece and are not on the checkmask.
    l_pawns &= occ_opp & _check_mask;
    r_pawns &= occ_opp & _check_mask;

    // These pawns can walk Forward
    const auto pawns_hv = pawns & ~_bishop_pin;

    const auto pawns_pinned_hv = pawns_hv & _rook_pin;
    const auto pawns_unpinned_hv = pawns_hv & ~_rook_pin;

    // Prune moves that are blocked by a piece
    const auto single_push_unpinned = attacks::shift<UP>(pawns_unpinned_hv) & ~occ_all;
    const auto single_push_pinned = attacks::shift<UP>(pawns_pinned_hv) & _rook_pin & ~occ_all;

    // Prune moves that are not on the checkmask.
    Bitboard single_push = (single_push_unpinned | single_push_pinned) & _check_mask;
    {
        Bitboard promo_left = l_pawns & RANK_PROMO;
        Bitboard promo_right = r_pawns & RANK_PROMO;
        Bitboard promo_push = single_push & RANK_PROMO;

        if constexpr (!capturesOnly) {
            while (promo_push) {
                Square to = static_cast<Square>(pop_lsb(promo_push));
                Square from = static_cast<Square>(to - UP);
                moves[moves.size_] = Move::make<PROMOTION>(from, to, KNIGHT);
                moves[moves.size_ + 1] = Move::make<PROMOTION>(from, to, BISHOP);
                moves[moves.size_ + 2] = Move::make<PROMOTION>(from, to, ROOK);
                moves[moves.size_ + 3] = Move::make<PROMOTION>(from, to, QUEEN);
                moves.size_ += 4;
            }
        }

        while (promo_left) {
            Square to = static_cast<Square>(pop_lsb(promo_left));
            Square from = static_cast<Square>(to - UP_LEFT); // correct
            moves[moves.size_] = Move::make<PROMOTION>(from, to, KNIGHT);
            moves[moves.size_ + 1] = Move::make<PROMOTION>(from, to, BISHOP);
            moves[moves.size_ + 2] = Move::make<PROMOTION>(from, to, ROOK);
            moves[moves.size_ + 3] = Move::make<PROMOTION>(from, to, QUEEN);
            moves.size_ += 4;
        }

        while (promo_right) {
            Square to = static_cast<Square>(pop_lsb(promo_right));
            Square from = static_cast<Square>(to - UP_RIGHT); // correct
            moves[moves.size_] = Move::make<PROMOTION>(from, to, KNIGHT);
            moves[moves.size_ + 1] = Move::make<PROMOTION>(from, to, BISHOP);
            moves[moves.size_ + 2] = Move::make<PROMOTION>(from, to, ROOK);
            moves[moves.size_ + 3] = Move::make<PROMOTION>(from, to, QUEEN);
            moves.size_ += 4;
        }
    }

    single_push &= ~RANK_PROMO;
    l_pawns &= ~RANK_PROMO;
    r_pawns &= ~RANK_PROMO;
    if constexpr (!capturesOnly) {
        _chess::splat_pawn_moves<UP>(moves.data() + moves.size_, single_push);
        moves.size_ += popcount(single_push);
    }
    _chess::splat_pawn_moves<UP_LEFT>(moves.data() + moves.size_, l_pawns);
    moves.size_ += popcount(l_pawns);
    _chess::splat_pawn_moves<UP_RIGHT>(moves.data() + moves.size_, r_pawns);
    moves.size_ += popcount(r_pawns);
}
template <typename T, Color c, bool capturesOnly>
void movegen::genKnightMoves(const _Position<T, void> &pos, Movelist &list, Bitboard _pin_mask, Bitboard _check_mask) {
    Bitboard knights = pos.template pieces<KNIGHT, c>() & ~_pin_mask;
    while (knights) {
        Square x = static_cast<Square>(pop_lsb(knights));
        Bitboard moves = attacks::knight(x) & ~pos.occ(c);
        moves &= _check_mask;
        if constexpr (capturesOnly)
            moves &= pos.occ(~c);
        _chess::splat_moves(list.data() + list.size(), x, moves);
        list.size_ += popcount(moves);
    }
}
template <typename T, Color c, bool capturesOnly>
void movegen::genKingMoves(const _Position<T, void> &pos, Movelist &out, Bitboard _pin_mask) {
    constexpr Color them = ~c;
    const Square kingSq = pos.kingSq(c);
    const Bitboard occAll = pos.occ();
    const Bitboard myOcc = pos.occ(c);

    // Remove king from board when computing enemy attacks
    const Bitboard occWithoutKing = occAll ^ 1ULL << kingSq;
    Bitboard enemyAttacks = 0ULL;

    // Sliding pieces
    Bitboard bLike = pos.template pieces<BISHOP, them>() | pos.template pieces<QUEEN, them>();
    while (bLike)
        enemyAttacks |= attacks::bishop(static_cast<Square>(pop_lsb(bLike)), occWithoutKing);

    Bitboard rLike = pos.template pieces<ROOK, them>() | pos.template pieces<QUEEN, them>();
    while (rLike)
        enemyAttacks |= attacks::rook(static_cast<Square>(pop_lsb(rLike)), occWithoutKing);

    // Knights
    enemyAttacks |= attacks::knight(pos.template pieces<KNIGHT, them>());

    // Pawns
    enemyAttacks |= attacks::pawn<them>(pos.template pieces<PAWN, them>());

    // Enemy king (adjacent control squares)
    enemyAttacks |= attacks::king(pos.kingSq(them));

    Bitboard moves = attacks::king(kingSq) & ~myOcc & ~enemyAttacks;
    if constexpr (capturesOnly)
        moves &= pos.occ(~c);
    _chess::splat_moves(out.data() + out.size(), kingSq, moves);
    out.size_ += popcount(moves);
    if constexpr (!capturesOnly) {
        if (pos.checkers())
            return;

        Bitboard occupancy = pos.occ();
        Bitboard enemy_attacks = enemyAttacks;
        constexpr CastlingRights kingRights = KING_SIDE & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING),
                                 queenRights = QUEEN_SIDE & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
        Bitboard OO_EMPTY = pos.getCastlingPath(c, true);
        Bitboard OO_SAFE = between(kingSq, castling_king_square(c, true));
        Bitboard OOO_EMPTY = pos.getCastlingPath(c, false);
        Bitboard OOO_SAFE = between(kingSq, castling_king_square(c, false));
        Square rookKing = pos.getCastlingMetadata(c).rook_start_ks, rookQueen = pos.getCastlingMetadata(c).rook_start_qs;

        if (pos.castlingRights() & kingRights &&
            !(occupancy & OO_EMPTY || enemy_attacks & OO_SAFE || _pin_mask & 1ULL << rookKing)) {
            out.push_back(Move::make<CASTLING>(kingSq, rookKing));
        }
        if (pos.castlingRights() & queenRights &&
            !(occupancy & OOO_EMPTY || enemy_attacks & OOO_SAFE || _pin_mask & 1ULL << rookQueen)) {
            out.push_back(Move::make<CASTLING>(kingSq, rookQueen));
        }
    }
}
template <typename T, Color c, PieceType pt, bool capturesOnly>
void movegen::genSlidingMoves(
    const _Position<T, void> &pos, Movelist &moves, Bitboard _rook_pin, Bitboard _bishop_pin, Bitboard _check_mask) {
    static_assert(pt == BISHOP || pt == ROOK || pt == QUEEN, "Sliding pieces only.");
    Bitboard sliders = pos.template pieces<pt, c>();
    Bitboard occ_all = pos.occ();
    Bitboard rook_pinners = _rook_pin;
    Bitboard bishop_pinners = _bishop_pin;
    if constexpr (pt == BISHOP)
        sliders &= ~rook_pinners;
    if constexpr (pt == ROOK)
        sliders &= ~bishop_pinners;
    Bitboard filter_list = ~pos.occ(c) & _check_mask;
    while (sliders) {
        Square from = static_cast<Square>(pop_lsb(sliders));
        Bitboard from_bb = 1ULL << from;

        Bitboard rook_hit = rook_pinners & from_bb;
        Bitboard bishop_hit = bishop_pinners & from_bb;
        Bitboard pin_mask = rook_hit ? rook_pinners : bishop_hit ? bishop_pinners : ~0ULL;

        auto func = attacks::queen;
        if constexpr (pt == BISHOP)
            func = attacks::bishop;
        else if constexpr (pt == ROOK)
            func = attacks::rook;
        func = rook_hit ? attacks::rook : bishop_hit ? attacks::bishop : func;
        Bitboard filtered_pin = pin_mask & filter_list;
        Bitboard targets = func(from, occ_all) & filtered_pin;
        if constexpr (capturesOnly)
            targets &= pos.occ(~c);
        _chess::splat_moves(moves.data() + moves.size_, from, targets);
        moves.size_ += popcount(targets);
    }
}
#define INSTANTIATE(PieceC)                                                                                                    \
    template void chess::movegen::genEP<PieceC, Color::WHITE>(const _Position<PieceC, void> &, Movelist &);                    \
    template void chess::movegen::genEP<PieceC, Color::BLACK>(const _Position<PieceC, void> &, Movelist &);                    \
    template void chess::movegen::genPawnDoubleMoves<PieceC, Color::WHITE>(const _Position<PieceC, void> &,                    \
                                                                           Movelist &,                                         \
                                                                           Bitboard,                                           \
                                                                           Bitboard);                                          \
    template void chess::movegen::genPawnDoubleMoves<PieceC, Color::BLACK>(const _Position<PieceC, void> &,                    \
                                                                           Movelist &,                                         \
                                                                           Bitboard,                                           \
                                                                           Bitboard);                                          \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::WHITE, true>(const _Position<PieceC, void> &,              \
                                                                                 Movelist &,                                   \
                                                                                 Bitboard,                                     \
                                                                                 Bitboard,                                     \
                                                                                 Bitboard);                                    \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::WHITE, false>(const _Position<PieceC, void> &,             \
                                                                                  Movelist &,                                  \
                                                                                  Bitboard,                                    \
                                                                                  Bitboard,                                    \
                                                                                  Bitboard);                                   \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::BLACK, true>(const _Position<PieceC, void> &,              \
                                                                                 Movelist &,                                   \
                                                                                 Bitboard,                                     \
                                                                                 Bitboard,                                     \
                                                                                 Bitboard);                                    \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::BLACK, false>(const _Position<PieceC, void> &,             \
                                                                                  Movelist &,                                  \
                                                                                  Bitboard,                                    \
                                                                                  Bitboard,                                    \
                                                                                  Bitboard);                                   \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, BISHOP, true>(const _Position<PieceC, void> &,         \
                                                                                      Movelist &,                              \
                                                                                      Bitboard,                                \
                                                                                      Bitboard,                                \
                                                                                      Bitboard);                               \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, ROOK, true>(const _Position<PieceC, void> &,           \
                                                                                    Movelist &,                                \
                                                                                    Bitboard,                                  \
                                                                                    Bitboard,                                  \
                                                                                    Bitboard);                                 \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, QUEEN, true>(const _Position<PieceC, void> &,          \
                                                                                     Movelist &,                               \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard);                                \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, BISHOP, false>(const _Position<PieceC, void> &,        \
                                                                                       Movelist &,                             \
                                                                                       Bitboard,                               \
                                                                                       Bitboard,                               \
                                                                                       Bitboard);                              \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, ROOK, false>(const _Position<PieceC, void> &,          \
                                                                                     Movelist &,                               \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard);                                \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, QUEEN, false>(const _Position<PieceC, void> &,         \
                                                                                      Movelist &,                              \
                                                                                      Bitboard,                                \
                                                                                      Bitboard,                                \
                                                                                      Bitboard);                               \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, BISHOP, true>(const _Position<PieceC, void> &,         \
                                                                                      Movelist &,                              \
                                                                                      Bitboard,                                \
                                                                                      Bitboard,                                \
                                                                                      Bitboard);                               \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, ROOK, true>(const _Position<PieceC, void> &,           \
                                                                                    Movelist &,                                \
                                                                                    Bitboard,                                  \
                                                                                    Bitboard,                                  \
                                                                                    Bitboard);                                 \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, QUEEN, true>(const _Position<PieceC, void> &,          \
                                                                                     Movelist &,                               \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard);                                \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, BISHOP, false>(const _Position<PieceC, void> &,        \
                                                                                       Movelist &,                             \
                                                                                       Bitboard,                               \
                                                                                       Bitboard,                               \
                                                                                       Bitboard);                              \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, ROOK, false>(const _Position<PieceC, void> &,          \
                                                                                     Movelist &,                               \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard,                                 \
                                                                                     Bitboard);                                \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, QUEEN, false>(const _Position<PieceC, void> &,         \
                                                                                      Movelist &,                              \
                                                                                      Bitboard,                                \
                                                                                      Bitboard,                                \
                                                                                      Bitboard);                               \
    template void chess::movegen::genKnightMoves<PieceC, Color::WHITE, true>(const _Position<PieceC, void> &,                  \
                                                                             Movelist &,                                       \
                                                                             Bitboard,                                         \
                                                                             Bitboard);                                        \
    template void chess::movegen::genKnightMoves<PieceC, Color::BLACK, true>(const _Position<PieceC, void> &,                  \
                                                                             Movelist &,                                       \
                                                                             Bitboard,                                         \
                                                                             Bitboard);                                        \
    template void chess::movegen::genKnightMoves<PieceC, Color::WHITE, false>(const _Position<PieceC, void> &,                 \
                                                                              Movelist &,                                      \
                                                                              Bitboard,                                        \
                                                                              Bitboard);                                       \
    template void chess::movegen::genKnightMoves<PieceC, Color::BLACK, false>(const _Position<PieceC, void> &,                 \
                                                                              Movelist &,                                      \
                                                                              Bitboard,                                        \
                                                                              Bitboard);                                       \
    template void chess::movegen::genKingMoves<PieceC, Color::WHITE, true>(const _Position<PieceC, void> &,                    \
                                                                           Movelist &,                                         \
                                                                           Bitboard);                                          \
    template void chess::movegen::genKingMoves<PieceC, Color::BLACK, true>(const _Position<PieceC, void> &,                    \
                                                                           Movelist &,                                         \
                                                                           Bitboard);                                          \
    template void chess::movegen::genKingMoves<PieceC, Color::WHITE, false>(const _Position<PieceC, void> &,                   \
                                                                            Movelist &,                                        \
                                                                            Bitboard);                                         \
    template void chess::movegen::genKingMoves<PieceC, Color::BLACK, false>(const _Position<PieceC, void> &,                   \
                                                                            Movelist &,                                        \
                                                                            Bitboard);
INSTANTIATE(EnginePiece)
INSTANTIATE(PolyglotPiece)
INSTANTIATE(ContiguousMappingPiece)
} // namespace chess
