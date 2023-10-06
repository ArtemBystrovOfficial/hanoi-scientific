#pragma once

#include "structs.hpp"
#include <iostream>
#include <array>

namespace hanoi {

#pragma pack (push, 1)
template<hanoi_limit N, hanoi_limit M>
class FrameImpl {
public:
	~FrameImpl() = default;

	static constexpr hanoi_limit	HEADER_DEPTH_OFFSET = 0;
	static constexpr hanoi_limit	HEADER_DEPTH_SIZE = 1;
	static constexpr hanoi_limit	HEADER_MAX_CIRCLE_OFFSET = 1;
	static constexpr hanoi_limit	HEADER_MAX_CIRCLE_SIZE = 1;
	static constexpr hanoi_limit	HEADER_DEPTH_MAX_CIRCLE_OFFSET = 2;
	static constexpr hanoi_limit	HEADER_DEPTH_MAX_CIRCLE_SIZE = 1;
	static constexpr hanoi_limit	LAST_COLUMN_TO_OFFSET = 3;
	static constexpr hanoi_limit	LAST_COLUMN_TO_SIZE = 1;
	static constexpr hanoi_limit	LAST_COLUMN_FROM_OFFSET = 4;
	static constexpr hanoi_limit	LAST_COLUMN_FROM_SIZE = 1;
	static constexpr hanoi_limit HEADER_SIZE = HEADER_DEPTH_SIZE + HEADER_MAX_CIRCLE_SIZE + 
											   HEADER_MAX_CIRCLE_SIZE + LAST_COLUMN_TO_SIZE
											   + LAST_COLUMN_FROM_SIZE; // DEPTH //
	static constexpr hanoi_limit COLUMN_POINTERS_SIZE = N;
	static constexpr hanoi_limit COLUMN_POINTERS_OFFSET = HEADER_SIZE;
	static constexpr hanoi_limit DATA_AREA_SIZE = N+M;
	static constexpr hanoi_limit DATA_AREA_OFFSET = HEADER_SIZE + COLUMN_POINTERS_SIZE;

	hanoi_limit getCircleData(hanoi_limit n, hanoi_limit pos) const {
		return m_data_[DATA_AREA_OFFSET + m_data_[COLUMN_POINTERS_OFFSET + n] + pos + 1];
	}
	hanoi_limit getColumnSize(hanoi_limit n) const {
		return m_data_[DATA_AREA_OFFSET + m_data_[COLUMN_POINTERS_OFFSET + n]];
	}
	hanoi_limit getLastCircle(hanoi_limit n) const {
		return getCircleData(n, getColumnSize(n) - 1);
	}
	hanoi_limit getDepth() const {
		return m_data_[HEADER_DEPTH_OFFSET];
	}
	hanoi_limit getLastMoveTo() const {
		return m_data_[LAST_COLUMN_TO_OFFSET];
	}
	std::pair<hanoi_limit, hanoi_limit>  getMaxCircle() const {
		return { m_data_[HEADER_DEPTH_MAX_CIRCLE_OFFSET], m_data_[HEADER_MAX_CIRCLE_OFFSET] };
	}
	uuid_columns_pack_t<N> getHashColumns() const {
		uuid_columns_pack_t<N> pack; //rvo
		storeColumnsUuids(&pack);
		return pack;
	}
	FrameImpl() { //this is initial postion 
		m_data_[HEADER_DEPTH_OFFSET] = 0;
		m_data_[HEADER_MAX_CIRCLE_OFFSET] = 0;
		m_data_[COLUMN_POINTERS_OFFSET] = 0;
		m_data_[HEADER_DEPTH_MAX_CIRCLE_OFFSET] = 0;
		m_data_[LAST_COLUMN_TO_OFFSET] = HANOI_LIMIT_NULL;
		m_data_[LAST_COLUMN_FROM_OFFSET] = HANOI_LIMIT_NULL;

		for (int j = 1, i = m_data_.size() - (N - 1); i < m_data_.size(); ++i, ++j) {
			m_data_[COLUMN_POINTERS_OFFSET + j] = i - DATA_AREA_OFFSET;
			m_data_[i] = 0;
		}
		hanoi_limit id = M - 1;
		std::generate(m_data_.begin() + DATA_AREA_OFFSET + 1, m_data_.end() - (N - 1),
			[&]() { return id--; });

		m_data_[DATA_AREA_OFFSET] = M;
	} 
	FrameImpl(const FrameImpl<N,M>& frame, hanoi_limit n_move_from, hanoi_limit n_move_to) {
		m_data_[HEADER_DEPTH_OFFSET] = frame.m_data_[HEADER_DEPTH_OFFSET] + 1; // DEPTH
		m_data_[HEADER_DEPTH_MAX_CIRCLE_OFFSET] = frame.m_data_[HEADER_DEPTH_MAX_CIRCLE_OFFSET];
		m_data_[HEADER_MAX_CIRCLE_OFFSET] = frame.m_data_[HEADER_MAX_CIRCLE_OFFSET];
		m_data_[LAST_COLUMN_TO_OFFSET] = n_move_to;
		m_data_[LAST_COLUMN_FROM_OFFSET] = n_move_from;
		if (n_move_from == 0) { // HEADER_MAX_CIRCLE_OFFSET
			if (frame.getLastCircle(n_move_from) > frame.m_data_[HEADER_MAX_CIRCLE_OFFSET]) {
				m_data_[HEADER_MAX_CIRCLE_OFFSET] = frame.getLastCircle(n_move_from);
				m_data_[HEADER_DEPTH_MAX_CIRCLE_OFFSET] = m_data_[HEADER_DEPTH_OFFSET];
			} 
		}

		hanoi_limit i = COLUMN_POINTERS_OFFSET;
		for (;i <= COLUMN_POINTERS_OFFSET + (std::min)(n_move_from, n_move_to); ++i)
			m_data_[i] = frame.m_data_[i];
		
		if (n_move_from > n_move_to) {
			for (;i <= COLUMN_POINTERS_OFFSET + n_move_from; ++i)
				m_data_[i] = frame.m_data_[i] + 1;
		} else {
			for (;i <= COLUMN_POINTERS_OFFSET + n_move_to; ++i)
				m_data_[i] = frame.m_data_[i] - 1;
		}

		for (; i < DATA_AREA_OFFSET; ++i)
			m_data_[i] = frame.m_data_[i];

		auto move_element = frame.getLastCircle(n_move_from);
		auto move_from_offset = DATA_AREA_OFFSET + frame.m_data_[COLUMN_POINTERS_OFFSET + n_move_from] + frame.getColumnSize(n_move_from);
		auto move_to_offset = DATA_AREA_OFFSET + frame.m_data_[COLUMN_POINTERS_OFFSET + n_move_to] + frame.getColumnSize(n_move_to);

		auto begin_other = frame.m_data_.begin() + DATA_AREA_OFFSET;
		auto elem_from_other = frame.m_data_.begin() + move_from_offset;
		auto elem_to_other = frame.m_data_.begin() + move_to_offset;
		auto end_other = frame.m_data_.end();

		auto begin = m_data_.begin() + DATA_AREA_OFFSET;
		auto elem_from = m_data_.begin() + move_from_offset;
		auto elem_to = m_data_.begin() + move_to_offset;

		if (n_move_from > n_move_to) {
			std::copy(begin_other, elem_to_other + 1, begin);
			m_data_[move_to_offset + 1] = move_element;
			std::copy(elem_to_other + 1 , elem_from_other, elem_to + 2);
			std::copy(elem_from_other + 1, end_other, elem_from + 1);
		}
		else {
			std::copy(begin_other, elem_from_other, begin);
			std::copy(elem_from_other + 1, elem_to_other + 1, elem_from );
			m_data_[move_to_offset] = move_element;
			std::copy(elem_to_other + 1, end_other, elem_to + 1);
		}
		m_data_[DATA_AREA_OFFSET + m_data_[COLUMN_POINTERS_OFFSET + n_move_from]]--;
		m_data_[DATA_AREA_OFFSET + m_data_[COLUMN_POINTERS_OFFSET + n_move_to]]++;
	}

	void dumpData() {
		std::cout << "[ RAW ";
		for (auto elem : m_data_)
			std::cout << uint32_t(elem) << " ";
		std::cout << " ] " << std::endl;

		auto [from, to] = getLastMove();

		if (from != HANOI_LIMIT_NULL) {
			std::string padding_left;
			if (from > to) {
				padding_left = (to == 0) ? "" : std::string(to * 2, ' ');
				std::cout << padding_left << "|<" << ((from - to == 1) ? "" : std::string((from - to - 1) * 2, '-')) << "|\n";
			}
			else {
				padding_left = (from == 0) ? "" : std::string(from * 2, ' ');
				std::cout << padding_left << "|" << ((to - from == 1) ? "" : std::string((to - from - 1) * 2, '-')) << ">|\n";
			}
		}
		std::cout << std::string(N * 2 - 1, '-') << std::endl;
		for (int j = M-1; j >= 0; --j) {
			for (int i = 0; i < N; ++i) {
				auto circle = getCircleData(i, j);
				std::cout << (getColumnSize(i) > j ? (circle >= 10 ? char('a' + (circle - 10)) : char('0' + circle)) + std::string(" ") : std::string("  "));
			}
			std::cout << std::endl;
		}
		std::cout << std::string(N*2 - 1, '-') << " | MOV: "<< uint32_t(getDepth()) << " ( "<< uint32_t(getDepth())*2-1 <<" ) " << std::endl;
	}
	
private:

	std::pair<hanoi_limit, hanoi_limit> getLastMove() const {
		return { m_data_[LAST_COLUMN_FROM_OFFSET], m_data_[LAST_COLUMN_TO_OFFSET] };
	}

	void storeColumnsUuids(uuid_columns_pack_t<N> *arr) const {
		for (hanoi_limit i = 0; i < N-1; ++i)
			arr->at(i) = getUuidColumn(i+1);
		std::sort(arr->rbegin(), arr->rend());
	}

	uint64_t getUuidColumn(int n) const {
		uint64_t sum = 0;
		for (hanoi_limit i = 0; i < getColumnSize(n); ++i) {
			sum += std::pow(2,getCircleData(n, i));
		}
		return sum;
	}

	std::array<hanoi_limit, HEADER_SIZE + COLUMN_POINTERS_SIZE + DATA_AREA_SIZE> m_data_;
};
#pragma pack (pop)

template<hanoi_limit N, hanoi_limit M>
class Frame {
private:
public:

//Constructors and operators
	~Frame() = default;

	Frame() 
		: m_impl()
		, m_moves(nullptr) {}
	Frame(const Frame& other, hanoi_limit n_from, hanoi_limit n_to) 
		: m_impl(other.m_impl, n_from, n_to)
		, m_moves(nullptr) {}

//Iterate
	void acceptMoves(frame_moves *moves) {
		m_moves = moves;
		moves->initBegin();
	}
	Frame generateNext() {
		auto [from, to] = m_moves->nextElem();
		Frame frame(*this, from, to); //rvo
		return frame;
	}
	bool isEndIterate() {
		return m_moves->isEnd();
	}
	bool dumpEnd() {
		if (!m_impl.getColumnSize(0)) {
			dumpData();
			return true;
		}
		return false;
	}
	
//Work with data
	hanoi_limit getCircleData(hanoi_limit n, hanoi_limit pos) const {
		return m_impl.getCircleData(n, pos);
	}
	hanoi_limit getColumnSize(hanoi_limit n) const {
		return m_impl.getColumnSize(n);
	}
	hanoi_limit getLastCircle(hanoi_limit n) const {
		return m_impl.getLastCircle(n);
	}
	hanoi_limit getDepth() const {
		return m_impl.getDepth();
	}
	hanoi_limit getLastMoveTo() const {
		return m_impl.getLastMoveTo();
	}
	//DEPTH - MAX_CIRCLE
	std::pair<hanoi_limit, hanoi_limit>  getMaxCircle() const {
		return m_impl.getMaxCircle();
	}
	auto getHashColumns() const {
		return m_impl.getHashColumns();
	}
	void dumpData() {
		m_impl.dumpData();
	}
private:
	FrameImpl<N,M> m_impl;
	frame_moves* m_moves;
};

}