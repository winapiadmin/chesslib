#include "position.h"
#include "position.h"
#include "moves_io.h"
#include "position.h"
#include <sstream>
#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR const
#endif
namespace chess {
template void _Position<EnginePiece, void>::genEP<Color::WHITE>(Movelist &) const;
template void _Position<EnginePiece, void>::genEP<Color::BLACK>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genEP<Color::WHITE>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genEP<Color::BLACK>(Movelist &) const;

template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist &) const;
template void _Position<EnginePiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::WHITE>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnDoubleMoves<Color::BLACK>(Movelist &) const;

template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genPawnSingleMoves<Color::BLACK, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genPawnSingleMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genKingMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genKingMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genKingMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genKingMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKingMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKingMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKingMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKingMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genKnightMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genKnightMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genKnightMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genKnightMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKnightMoves<Color::WHITE, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKnightMoves<Color::BLACK, true>(Movelist &) const;

template void _Position<PolyglotPiece, void>::genKnightMoves<Color::WHITE, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genKnightMoves<Color::BLACK, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, BISHOP, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, BISHOP, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, BISHOP, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, BISHOP, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, ROOK, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, ROOK, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, ROOK, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, ROOK, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, QUEEN, false>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, QUEEN, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, QUEEN, false>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, QUEEN, false>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, BISHOP, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, BISHOP, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, BISHOP, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, BISHOP, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, ROOK, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, ROOK, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, ROOK, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, ROOK, true>(Movelist &) const;

template void _Position<EnginePiece, void>::genSlidingMoves<Color::WHITE, QUEEN, true>(Movelist &) const;
template void _Position<EnginePiece, void>::genSlidingMoves<Color::BLACK, QUEEN, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::WHITE, QUEEN, true>(Movelist &) const;
template void _Position<PolyglotPiece, void>::genSlidingMoves<Color::BLACK, QUEEN, true>(Movelist &) const;

template _Position<EnginePiece, void>::_Position(std::string);
template _Position<PolyglotPiece, void>::_Position(std::string);
template void _Position<EnginePiece, void>::setFEN(const std::string &);
template void _Position<PolyglotPiece, void>::setFEN(const std::string &);
template std::string _Position<EnginePiece, void>::fen() const;
template std::string _Position<PolyglotPiece, void>::fen() const;
template void _Position<EnginePiece, void>::doMove<false>(const Move &move);
template void _Position<PolyglotPiece, void>::doMove<false>(const Move &move);
template void _Position<EnginePiece, void>::doMove<true>(const Move &move);
template void _Position<PolyglotPiece, void>::doMove<true>(const Move &move);
template void _Position<EnginePiece, void>::refresh_attacks();
template void _Position<PolyglotPiece, void>::refresh_attacks();
template uint64_t _Position<EnginePiece, void>::zobrist() const;
template uint64_t _Position<PolyglotPiece, void>::zobrist() const;
template Move _Position<EnginePiece, void>::parse_uci(std::string) const;
template Move _Position<PolyglotPiece, void>::parse_uci(std::string) const;
template Move _Position<EnginePiece, void>::push_uci(std::string);
template Move _Position<PolyglotPiece, void>::push_uci(std::string);
template bool _Position<EnginePiece, void>::is_valid<false>() const;
template bool _Position<PolyglotPiece, void>::is_valid<false>() const;
template bool _Position<EnginePiece, void>::is_valid<true>() const;
template bool _Position<PolyglotPiece, void>::is_valid<true>() const;
namespace _chess {
#if defined(__AVX512BW__)
template<int Offset = 0>
struct alignas(64) SplatTable {
    std::array<uint16_t, 64> data;
    constexpr int clamp64(int x) {
        return (x < 0) ? 0 : (x > 63 ? 63 : x);
    }

    constexpr SplatTable() : data{} {
        for (int i = 0; i < 64; ++i) {
            int from = clamp64(i - Offset);
            data[i] = Move((Square)from, (Square)i).raw();
        }
    }
};

inline constexpr SplatTable<> SPLAT_TABLE{};
template<int Offset>
inline constexpr SplatTable<Offset> SPLAT_PAWN_TABLE{};
// AVX-512 (32 lanes of uint16_t)
inline static Move* write_moves(Move* moveList, uint32_t mask, __m512i vector) {
    // Avoid _mm512_mask_compressstoreu_epi16() as it's 256 uOps on Zen4
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(moveList),
                        _mm512_maskz_compress_epi16(mask, vector));
    return moveList + popcount(mask);
}

inline static Move* splat_moves(Move* moveList, Square from, Bitboard to_bb) {
    const auto* table = reinterpret_cast<const __m512i*>(SPLAT_TABLE.data.data());
    __m512i fromVec = _mm512_set1_epi16(Move(from, SQUARE_ZERO).raw());
    // two 32-lane blocks (0..31, 32..63)
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0),
                           _mm512_or_si512(_mm512_load_si512(table + 0), fromVec));
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 32),
                           _mm512_or_si512(_mm512_load_si512(table + 1), fromVec));

    return moveList;
}

template<int offset>
inline static Move* splat_pawn_moves(Move* moveList, Bitboard to_bb) {
    const auto* table = reinterpret_cast<const __m512i*>(SPLAT_PAWN_TABLE<offset>.data.data());
    moveList =
      write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm512_load_si512(table + 0));
    moveList =
      write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm512_load_si512(table + 1));

    return moveList;
}
// temp AVX2 solution, but apperantly it runs slower and fails the tests
//#elif defined(__AVX2__)
//template <int Offset = 0> struct alignas(64) SplatTable {
//    alignas(32) std::array<uint16_t, 64> data;
//    constexpr int clamp64(int x) { return (x < 0) ? 0 : (x > 63 ? 63 : x); }
//
//    constexpr SplatTable() : data{} {
//        for (int i = 0; i < 64; ++i) {
//            int from = clamp64(i - Offset);
//            data[i] = Move((Square)from, (Square)i).raw();
//        }
//    }
//};
//
//inline constexpr SplatTable<> SPLAT_TABLE{};
//template <int Offset> inline constexpr SplatTable<Offset> SPLAT_PAWN_TABLE{};
//
//constexpr std::array<std::array<uint8_t, 16>, 256> build_shuffle_lut() {
//    std::array<std::array<uint8_t, 16>, 256> lut{};
//
//    for (int m = 0; m < 256; ++m) {
//        int pos = 0;
//        for (int i = 0; i < 8; ++i) {
//            if (m & (1 << i)) {
//                lut[m][pos++] = 2 * i;
//                lut[m][pos++] = 2 * i + 1;
//            }
//        }
//        // fill remaining with 0x80
//        while (pos < 16)
//            lut[m][pos++] = 0x80;
//    }
//
//    return lut;
//}
//
//constexpr auto SHUFFLE_LUT_ARRAY = build_shuffle_lut();
//alignas(16) static __m128i _pshufb_compress_lut[256];
//
//struct ShuffleLutInitializer {
//    ShuffleLutInitializer() {
//        for (int i = 0; i < 256; ++i) {
//            std::memcpy(&_pshufb_compress_lut[i], SHUFFLE_LUT_ARRAY[i].data(), 16);
//        }
//    }
//};
//
//// Guaranteed to run before main()
//static ShuffleLutInitializer _shuffle_lut_init;
//
//// Compress 16×int16_t lanes from v according to mask, store contiguously, return #written
//static inline int compressstore_epi16_avx2(int16_t* dst, __m256i v, uint16_t mask) {
//    __m128i lo = _mm256_castsi256_si128(v);
//    __m128i hi = _mm256_extracti128_si256(v, 1);
//
//    uint8_t mask_lo = mask & 0xFF;
//    uint8_t mask_hi = (mask >> 8) & 0xFF;
//
//    __m128i shuf_lo = _pshufb_compress_lut[mask_lo];
//    __m128i shuf_hi = _pshufb_compress_lut[mask_hi];
//
//    __m128i cmp_lo = _mm_shuffle_epi8(lo, shuf_lo);
//    __m128i cmp_hi = _mm_shuffle_epi8(hi, shuf_hi);
//
//    int count_lo = popcount(mask_lo);
//    int count_hi = popcount(mask_hi);
//
//    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), cmp_lo);
//    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + count_lo), cmp_hi);
//
//    return count_lo + count_hi;
//}
//
//// Same logical behavior as your AVX-512 write_moves()
//inline Move* write_moves(Move* moveList, uint32_t mask, __m256i vector) {
//    int n = compressstore_epi16_avx2(reinterpret_cast<int16_t*>(moveList), vector,
//                                     static_cast<uint16_t>(mask));
//    return moveList + n;
//}
//inline Move *write_moves(Move *moveList, uint32_t mask, __m256i lo_vec, __m256i hi_vec) {
//    uint16_t mask_lo = static_cast<uint16_t>(mask & 0xFFFF);
//    uint16_t mask_hi = static_cast<uint16_t>(mask >> 16);
//
//    moveList = write_moves(moveList, mask_lo, lo_vec);
//    moveList = write_moves(moveList, mask_hi, hi_vec);
//
//    return moveList;
//}
//
//// ----------------- splat_moves AVX2 -----------------
//inline Move *splat_moves(Move *moveList, uint16_t from, uint64_t to_bb) {
//    const uint16_t *base = SPLAT_TABLE.data.data();
//
//    // load 4 blocks: 0..15, 16..31, 32..47, 48..63
//    __m256i t0 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 0));
//    __m256i t1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 16));
//    __m256i t2 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 32));
//    __m256i t3 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 48));
//
//    __m256i fromVec = _mm256_set1_epi16(from);
//
//    // lower 32-bit bitboard
//    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm256_or_si256(t0, fromVec), _mm256_or_si256(t1, fromVec));
//
//    // upper 32-bit bitboard
//    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm256_or_si256(t2, fromVec), _mm256_or_si256(t3, fromVec));
//
//    return moveList;
//}
//
//// ----------------- splat_pawn_moves AVX2 -----------------
//template <int Offset> inline Move *splat_pawn_moves(Move *moveList, uint64_t to_bb) {
//    const uint16_t *base = SPLAT_PAWN_TABLE<Offset>.data.data();
//
//    __m256i t0 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 0));
//    __m256i t1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 16));
//    __m256i t2 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 32));
//    __m256i t3 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(base + 48));
//
//    // lower 32-bit bitboard
//    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), t0, t1);
//
//    // upper 32-bit bitboard
//    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), t2, t3);
//
//    return moveList;
//}

#else
template<Direction offset>
inline static Move* splat_pawn_moves(Move* moveList, Bitboard to_bb) {
    while (to_bb)
    {
        Square to   = (Square)pop_lsb(to_bb);
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

inline static Move *splat_moves(Move *moveList, Square from, Bitboard to_bb) {
    while (to_bb)
        *moveList++ = Move(from, (Square)pop_lsb(to_bb));
    return moveList;
}
#endif
} // namespace _chess
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

template <typename PieceC, typename T> template <Color c> void _Position<PieceC, T>::genEP(Movelist &ep_moves) const {
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
template <typename PieceC, typename T> template <Color c, bool capturesOnly> void _Position<PieceC, T>::genPawnSingleMoves(Movelist &moves) const {
    constexpr Direction UP = pawn_push(c);
    // constexpr Bitboard PROMO_RANK = (c == WHITE) ? attacks::MASK_RANK[6] : attacks::MASK_RANK[1];
    constexpr Bitboard PROMO_NEXT = (c == WHITE) ? attacks::MASK_RANK[7] : attacks::MASK_RANK[0];
    constexpr Bitboard NOT_FILE_A = 0xfefefefefefefefeULL;
    constexpr Bitboard NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;

    const Bitboard pawns = pieces<PAWN, c>();
    const Bitboard enemy_occ = occ(~c);
    const Bitboard all_occ = occ();
    const Bitboard bishopPin = this->_bishop_pin;
    const Bitboard rookPin = this->_rook_pin;
    const Bitboard check_mask = this->_check_mask;
    const Bitboard pinned = pawns & _pin_mask;
    Move* moveList; // no chance

    // ---------- 1. Single forward pushes ----------

    if constexpr (!capturesOnly) {
        const Bitboard unpinned = pawns ^ pinned;
        Bitboard one_push = (c == WHITE ? unpinned << 8 : unpinned >> 8) & ~all_occ;
        one_push &= check_mask;
        Bitboard _pinned = pinned & ~bishopPin;
        Bitboard pinned_push = (c == WHITE ? _pinned << 8 : _pinned >> 8) & ~all_occ;
        pinned_push &= rookPin & check_mask;
        Bitboard push_targets = one_push | pinned_push;
        Bitboard promo_targets = push_targets & PROMO_NEXT;
        Bitboard non_promo_targets = push_targets & ~promo_targets; // NAND
        while (promo_targets) {
            Square to = Square(pop_lsb(promo_targets));
            Square from = Square(to - UP);

            moves.push_back(Move::make<PROMOTION>(from, to, KNIGHT));
            moves.push_back(Move::make<PROMOTION>(from, to, BISHOP));
            moves.push_back(Move::make<PROMOTION>(from, to, ROOK));
            moves.push_back(Move::make<PROMOTION>(from, to, QUEEN));
        }
        // while (non_promo_targets) {
        //     Square to = Square(pop_lsb(non_promo_targets));
        //     Square from = Square(to - UP);
        //     moves.push_back(Move(from, to));
        // }
        moveList =moves.data()+moves.size();
        Move* moveEnd = _chess::splat_pawn_moves<UP>(moveList, non_promo_targets);
        moves.size_+=std::distance(moveList, moveEnd);
        moveList = moveEnd;
    }
    // ---------- 2. Left captures ----------
    Bitboard left = (pawns & NOT_FILE_A);
    Bitboard left_tgt = (c == WHITE ? (left << 7) : (left >> 9)) & enemy_occ & check_mask;
    Bitboard promo_left_targets = left_tgt & PROMO_NEXT;
    Bitboard non_promo_left_targets = left_tgt & ~promo_left_targets; // NAND

    const Bitboard pinnedSet = pinned;
    while (promo_left_targets) {
        Square to = Square(pop_lsb(promo_left_targets));
        Square from = Square(to - ((c == WHITE) ? 7 : -9));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        Bitboard pinMatch = ((move_bb & bishopPin) == move_bb) | ((move_bb & rookPin) == move_bb);
        bool legal = !((pinnedSet >> from) & 1ULL) || pinMatch;
        if (legal) {
            moves.push_back(Move::make<PROMOTION>(from, to, KNIGHT));
            moves.push_back(Move::make<PROMOTION>(from, to, BISHOP));
            moves.push_back(Move::make<PROMOTION>(from, to, ROOK));
            moves.push_back(Move::make<PROMOTION>(from, to, QUEEN));
        }
    }
    {
        // while (non_promo_left_targets) {
        //     Square to = Square(pop_lsb(non_promo_left_targets));
        //     Square from = Square(to - ((c == WHITE) ? 7 : -9));
        //     Bitboard from_bb = 1ULL << from;
        //     Bitboard move_bb = from_bb | (1ULL << to);

        //     // skip if pinned but capture not along pin ray
        //     if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb || (move_bb & current_state._rook_pin) == move_bb)))
        //         moves.push_back(Move(from, to));
        //     }
        // }
        Bitboard legal_sources = 0;
        Bitboard targets = non_promo_left_targets;

        constexpr int shift = (c == WHITE) ? 7 : -9; // pawn left capture delta

        while (targets) {
            int to = pop_lsb(targets);
            int from = to - shift;
            Bitboard to_bb = (1ULL << to);
            Bitboard move_bb = (1ULL << from) | to_bb;

            // Determine legality using bitwise logic instead of branch
            Bitboard pinMatch = ((move_bb & bishopPin) == move_bb) | ((move_bb & rookPin) == move_bb);
            bool legal = !((pinnedSet >> from) & 1ULL) || pinMatch;

            // branchless combine
            legal_sources |= legal ? to_bb : 0;
        }

        // generate pawn moves
        Move *moveList = moves.data() + moves.size();
        constexpr Direction dir = (c == WHITE) ? NORTH_WEST : SOUTH_WEST;
        Move *moveEnd = _chess::splat_pawn_moves<dir>(moveList, legal_sources);
        moves.size_ += moveEnd - moveList;

    }
    // ---------- 3. Right captures ----------
    Bitboard right = (pawns & NOT_FILE_H);
    Bitboard right_tgt = (c == WHITE ? (right << 9) : (right >> 7)) & enemy_occ & check_mask;
    Bitboard promo_right_targets = right_tgt & PROMO_NEXT;
    Bitboard non_promo_right_targets = right_tgt & ~promo_right_targets; // NAND

    while (promo_right_targets) {
        Square to = Square(pop_lsb(promo_right_targets));
        Square from = Square(to - ((c == WHITE) ? 9 : -7));
        Bitboard from_bb = 1ULL << from;
        Bitboard move_bb = from_bb | (1ULL << to);

        Bitboard pinMatch = ((move_bb & bishopPin) == move_bb) | ((move_bb & rookPin) == move_bb);
        bool legal = !((pinnedSet >> from) & 1ULL) || pinMatch;
        if (legal) {
            moves.push_back(Move::make<PROMOTION>(from, to, KNIGHT));
            moves.push_back(Move::make<PROMOTION>(from, to, BISHOP));
            moves.push_back(Move::make<PROMOTION>(from, to, ROOK));
            moves.push_back(Move::make<PROMOTION>(from, to, QUEEN));
        }
    }
    {
        // while (non_promo_right_targets) {
        //     Square to = Square(pop_lsb(non_promo_right_targets));
        //     Square from = Square(to - ((c == WHITE) ? 9 : -7));
        //     Bitboard from_bb = 1ULL << from;
        //     Bitboard move_bb = from_bb | (1ULL << to);

        //     if (!((pinned & from_bb) && !((move_bb & current_state._bishop_pin) == move_bb || (move_bb & current_state._rook_pin) == move_bb)))
        //         moves.push_back(Move(from, to));
        // }
        Bitboard legal_sources=0;
        while (non_promo_right_targets) {
            Square to = Square(pop_lsb(non_promo_right_targets));
            Square from = Square(to - ((c == WHITE) ? 9 : -7));
            Bitboard from_bb = 1ULL << from, to_bb=1ULL<<to;
            Bitboard move_bb = from_bb | to_bb;

            // skip if pinned but capture not along pin ray
            Bitboard pinMatch = ((move_bb & bishopPin) == move_bb) | ((move_bb & rookPin) == move_bb);
            bool legal = !((pinnedSet >> from) & 1ULL) || pinMatch;

            // branchless combine
            legal_sources |= legal?to_bb:0;
        }
        moveList =moves.data()+moves.size();
        constexpr Direction dir=c==WHITE?NORTH_EAST:SOUTH_EAST;
        Move* moveEnd = _chess::splat_pawn_moves<dir>(moveList, legal_sources);
        moves.size_+=std::distance(moveList, moveEnd);
        moveList = moveEnd;
    }
}

template <typename PieceC, typename T> template <Color c, bool capturesOnly> void _Position<PieceC, T>::genKnightMoves(Movelist &list) const {
    Bitboard knights = pieces<KNIGHT, c>() & ~_pin_mask; // yes, unconditionally.
    const Bitboard check_mask = _check_mask;
    while (knights) {
        Square x = static_cast<Square>(pop_lsb(knights));
        Bitboard moves = attacks::knight(x) & ~occ(c);
        moves &= check_mask;
        if constexpr (capturesOnly)
            moves &= occ(~c);
        _chess::splat_moves(list.data()+list.size(), x, moves);
        list.size_ += popcount(moves);
    }
}

template <typename PieceC, typename T> template <Color c> void _Position<PieceC, T>::genPawnDoubleMoves(Movelist &moves) const {
    constexpr Bitboard RANK_2 = (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
    constexpr Direction UP = pawn_push(c);

    Bitboard all_occ = occ();
    Bitboard pawns = pieces<PAWN, c>() & RANK_2;

    // Split pin types
    const Bitboard pin_mask = this->_pin_mask;
    Bitboard pin_file = pin_mask & attacks::MASK_FILE[file_of(kingSq(c))];

    Bitboard unpinned = pawns & ~pin_mask;
    Bitboard file_pinned = pawns & pin_file;

    // First step must be empty
    Bitboard step1_unpinned = attacks::shift<UP>(unpinned) & ~all_occ;
    Bitboard step1_pinned = attacks::shift<UP>(file_pinned) & pin_file & ~all_occ;

    // Second step must also be empty
    Bitboard step2_unpinned = attacks::shift<UP>(step1_unpinned) & ~all_occ;
    Bitboard step2_pinned = attacks::shift<UP>(step1_pinned) & pin_file & ~all_occ;

    Bitboard destinations = (step2_unpinned | step2_pinned) & this->_check_mask;

    //// Source pawns = only from RANK_2 (already masked above)
    //Bitboard sources = (c == WHITE) ? (destinations >> 16) : (destinations << 16);

    //while (sources) {
    //    Square from = static_cast<Square>(pop_lsb(sources));
    //    Square to = from + 2 * UP;
    //    moves.push_back(Move(from, to));
    //}
    _chess::splat_pawn_moves<2 * UP>(moves.data() + moves.size_, destinations);
    moves.size_ += popcount(destinations);
}
template <typename PieceC, typename T> template <Color c, bool capturesOnly> void _Position<PieceC, T>::genKingMoves(Movelist &out) const {
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
    _chess::splat_moves(out.data() + out.size(), kingSq, moves);
    out.size_ += popcount(moves);
    if constexpr (!capturesOnly) {
        // Castling
        const auto &st = current_state;
        if (checkers())
            return;
        if constexpr (c == WHITE) {
            const bool canCastleKingside = (st.castlingRights & WHITE_OO) && piece_on(SQ_F1) == PieceC::NO_PIECE && piece_on(SQ_G1) == PieceC::NO_PIECE && !(enemyAttacks & ((1ULL << SQ_F1) | (1ULL << SQ_G1)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_H1));

            const bool canCastleQueenside = (st.castlingRights & WHITE_OOO) && piece_on(SQ_D1) == PieceC::NO_PIECE && piece_on(SQ_C1) == PieceC::NO_PIECE && piece_on(SQ_B1) == PieceC::NO_PIECE && !(enemyAttacks & ((1ULL << SQ_D1) | (1ULL << SQ_C1)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E1, SQ_A1));
        } else {
            const bool canCastleKingside = (st.castlingRights & BLACK_OO) && piece_on(SQ_F8) == PieceC::NO_PIECE && piece_on(SQ_G8) == PieceC::NO_PIECE && !(enemyAttacks & ((1ULL << SQ_F8) | (1ULL << SQ_G8)));

            if (canCastleKingside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_H8));

            const bool canCastleQueenside = (st.castlingRights & BLACK_OOO) && piece_on(SQ_D8) == PieceC::NO_PIECE && piece_on(SQ_C8) == PieceC::NO_PIECE && piece_on(SQ_B8) == PieceC::NO_PIECE && !(enemyAttacks & ((1ULL << SQ_D8) | (1ULL << SQ_C8)));

            if (canCastleQueenside)
                out.push_back(Move::make<CASTLING>(SQ_E8, SQ_A8));
        }
    }
}
template <typename PieceC, typename T> template <bool Strict> void _Position<PieceC, T>::doMove(const Move &move) {

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
    assert(target_piecetype != KING && "No captures");
    assert(moving_piecetype != NO_PIECE_TYPE && "Expected a piece to move.");
#elif defined(__EXCEPTIONS) && (defined(_DEBUG) || !defined(NDEBUG))

    if (target_piecetype == KING)
        throw std::invalid_argument("No captures to king exists.");
    if (moving_piecetype == NO_PIECE_TYPE)
        throw std::invalid_argument("Expected a piece to move.");
#endif
    removePiece(moving_piecetype, from_sq, us);
    {
        ASSUME(move_type == NORMAL || move_type == PROMOTION || move_type == EN_PASSANT || move_type == CASTLING);
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
            Square rook_dest = relative_square(us, is_king_side ? SQ_F1 : Square::SQ_D1), king_dest = relative_square(us, is_king_side ? SQ_G1 : Square::SQ_C1);
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
        CastlingRights clear_mask = clearCastleMask[us][moving_piecetype][from_sq] | clearCastleMask[target_color][target_piecetype][to_sq];
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
                    throw std::invalid_argument("Each rank must contain exactly 8 squares");
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
                    throw std::invalid_argument("Too many pieces in one rank");
                if (!chess::is_valid(r, f))
                    throw std::invalid_argument("Invalid file/rank position");
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
                    throw std::invalid_argument("Invalid FEN character");
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
            throw std::invalid_argument("Last rank must have 8 squares");
        rank_count++;
        if (rank_count != 8)
            throw std::invalid_argument("FEN must contain exactly 8 ranks");
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
        throw std::invalid_argument("Expected white or black, got something else.");
#endif
    }

    // 3. Castling rights
    current_state.castlingRights = NO_CASTLING;
    for (char c : castling) {
        switch (c) {
        case 'K':
            // possibly legality checks?
            if (piece_on(SQ_E1) != PieceC::WKING || piece_on(SQ_H1) != PieceC::WROOK)
                break;
            current_state.castlingRights |= WHITE_OO;
            current_state.hash ^= zobrist::RandomCastle[WHITE_OO];
            break;
        case 'Q':
            // possibly legality checks?
            if (piece_on(SQ_E1) != PieceC::WKING || piece_on(SQ_A1) != PieceC::WROOK)
                break;
            current_state.castlingRights |= WHITE_OOO;
            current_state.hash ^= zobrist::RandomCastle[WHITE_OOO];
            break;
        case 'k':
            // possibly legality checks?
            if (piece_on(SQ_E8) != PieceC::BKING || piece_on(SQ_H8) != PieceC::BROOK)
                break;
            current_state.castlingRights |= BLACK_OO;
            current_state.hash ^= zobrist::RandomCastle[BLACK_OO];
            break;
        case 'q':
            // possibly legality checks?
            if (piece_on(SQ_E8) != PieceC::BKING || piece_on(SQ_H8) != PieceC::BROOK)
                break;
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
            throw std::invalid_argument("Invalid castling rights, this library doesn't support Chess960");
#endif
            break;
        }
    }

    if (enpassant != "-" && enpassant.length() == 2 && enpassant[0] >= 'a' && enpassant[0] <= 'h' && enpassant[1] >= '1' && enpassant[1] <= '8') {
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
            throw std::invalid_argument("Invalid en passant FEN field");
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
    constexpr std::string_view EnginePieceToChar(" PNBRQK  pnbrqk ");
    constexpr std::string_view PolyglotPieceToChar("PNBRQKpnbrqk ");
    constexpr std::string_view PieceToChar = std::is_same_v<PieceC, EnginePiece> ? EnginePieceToChar : PolyglotPieceToChar;
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
template <typename PieceC, typename T> template <bool Strict> bool _Position<PieceC, T>::is_valid() const {
    if (count<KING, WHITE>()!=1) return false;
    if (count<KING, BLACK>()!=1) return false;
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
    if ((pieces(PAWN) & (attacks::MASK_RANK[RANK_1]|attacks::MASK_RANK[RANK_8]))!=0) return false;
    if constexpr (Strict){
        // pawns not > 8
        {
            int pawns = count<PAWN, WHITE>();
            int queens = count<QUEEN, WHITE>();
            int rooks  = count<ROOK, WHITE>();
            int bishops= count<BISHOP, WHITE>();
            int knights= count<KNIGHT, WHITE>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions = std::max(0, (queens - 1))
                        + std::max(0, (rooks  - 2))
                        + std::max(0, (bishops- 2))
                        + std::max(0, (knights- 2));

            if (pawns + promotions > 8)
                return false;  // impossible
        }
        {
            int pawns = count<PAWN, BLACK>();
            int queens = count<QUEEN, BLACK>();
            int rooks  = count<ROOK, BLACK>();
            int bishops= count<BISHOP, BLACK>();
            int knights= count<KNIGHT, BLACK>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions = std::max(0, (queens - 1))
                        + std::max(0, (rooks  - 2))
                        + std::max(0, (bishops- 2))
                        + std::max(0, (knights- 2));

            if (pawns + promotions > 8)
                return false;  // impossible
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
    if (popcount(checkers())>2) return false;
    return false;
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
template <typename PieceC, typename T> template <Color c, PieceType pt, bool capturesOnly> void _Position<PieceC, T>::genSlidingMoves(Movelist &moves) const {
    static_assert(pt == BISHOP || pt == ROOK || pt == QUEEN, "Sliding pieces only.");
    Bitboard sliders = pieces<pt, c>();
    Bitboard occ_all = occ();
    // Square king_sq = current_state.kings[c];
    Bitboard rook_pinners = _rook_pin; // bitboard of enemy rooks/queens pinning
    Bitboard bishop_pinners = _bishop_pin;
    if constexpr (pt == BISHOP)
        sliders &= ~rook_pinners;
    if constexpr (pt == ROOK)
        sliders &= ~bishop_pinners;
    Bitboard filter_list = ~occ(c) & _check_mask;
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
        _chess::splat_moves(moves.data() + moves.size_, from, targets);
        moves.size_ += popcount(targets);
    }
}

template <typename PieceC, typename T> _Position<PieceC, T>::_Position(std::string fen) { setFEN(fen); }

template <typename PieceC, typename T> Move _Position<PieceC, T>::parse_uci(std::string uci) const { return chess::uci::uciToMove(*this, uci); }

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
} // namespace chess
