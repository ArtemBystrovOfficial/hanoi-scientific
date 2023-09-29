#pragma once

#include "structs.hpp"

#pragma pack (push, 1)
template<hanoi_limit N, hanoi_limit M>
struct FrameImpl {
	static constexpr hanoi_limit HEADER_SIZE = 1; // DEPTH //
	static constexpr hanoi_limit HEADER_DEPTH_OFFSET = 0;
	static constexpr hanoi_limit HEADER_DEPTH_SIZE	= 1;
	static constexpr hanoi_limit COLUMN_POINTERS_SIZE = N;
	static constexpr hanoi_limit COLUMN_POINTERS_OFFSET = HEADER_SIZE;
	static constexpr hanoi_limit DATA_AREA_SIZE = N+M;
	static constexpr hanoi_limit DATA_AREA_OFFSET = HEADER_SIZE + COLUMN_POINTERS_SIZE;
	hanoi_limit getCircleData(hanoi_limit n, hanoi_limit pos) {
		return m_data_[DATA_AREA_OFFSET + m_data_[COLUMN_POINTERS_OFFSET + n] + pos];
	}
	hanoi_limit getColumnSize(hanoi_limit n) {

	}
	hanoi_limit getLastCircle(hanoi_limit n) {

	}
	hanoi_limit getDepth() {
		return m_data_[HEADER_DEPTH_OFFSET];
	}
	FrameImpl() {

	} //this is initial postion 
	FrameImpl(const FrameImpl<N,M>& frame, hanoi_limit n_move_from, hanoi_limit n_move_to) {

	}

	std::array<hanoi_limit, HEADER_SIZE + COLUMN_POINTERS_SIZE + DATA_AREA_SIZE> m_data_;
};
#pragma pack (pop)

template<hanoi_limit N, hanoi_limit M>
class Frame {
private:
public:
	Frame() 
		: m_moves(nullptr)
		, m_is_iterate_all(false)
		, m_impl() {}
	Frame(const Frame& other, hanoi_limit n_from, hanoi_limit n_to) 
		: m_impl(other.m_impl, n_from, n_to)
		, m_moves(nullptr)
		, m_is_iterate_all(false) {}
	void acceptMoves(const frame_moves<N> *moves) {
		m_moves = moves;
	}
	Frame generateNext() {
		m_is_iterate_all = (rand() % 7 == 0);
		return Frame(*this, 0, 0);
	}
	bool isEndIterate() {
		return m_is_iterate_all;
	}
	bool dumpEnd() {
		return (rand() % 3'000'000'000 == 0);
	}
	
private:
	FrameImpl<N,M> m_impl;

	const frame_moves<N>* m_moves;
	bool m_is_iterate_all;
};