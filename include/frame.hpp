#pragma once

#include "structs.hpp"
#include <iostream>
#include <array>

#pragma pack (push, 1)
template<hanoi_limit N, hanoi_limit M>
class FrameImpl {
public:
	~FrameImpl() = default;

	static constexpr hanoi_limit HEADER_SIZE = 1; // DEPTH //
	static constexpr hanoi_limit HEADER_DEPTH_OFFSET = 0;
	static constexpr hanoi_limit HEADER_DEPTH_SIZE	= 1;
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
	FrameImpl() { //this is initial postion 
		m_data_[HEADER_DEPTH_OFFSET] = 0;
		m_data_[COLUMN_POINTERS_OFFSET] = 0;

		for (int j = 1, i = m_data_.size() - (N - 1); i < m_data_.size(); ++i, ++j) {
			m_data_[COLUMN_POINTERS_OFFSET + j] = i - DATA_AREA_OFFSET;
			m_data_[i] = 0;
		}

		std::generate(m_data_.begin() + DATA_AREA_OFFSET + 1, m_data_.end() - (N - 1),
			[]() { static hanoi_limit i = M - 1; return i--; });

		m_data_[DATA_AREA_OFFSET] = M;
	} 
	FrameImpl(const FrameImpl<N,M>& frame, hanoi_limit n_move_from, hanoi_limit n_move_to) {

		hanoi_limit i = 0;
		for (;i < COLUMN_POINTERS_OFFSET; ++i)
			m_data_[i] = frame.m_data_[i];

		for (;i <= COLUMN_POINTERS_OFFSET + std::min(n_move_from, n_move_to); ++i)
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
		std::cout << std::string(N * 2 - 1, '-') << std::endl;
		std::cout << "[ RAW ";
		for (auto elem : m_data_)
			std::cout << int(elem) << " ";
		std::cout << " ] " << std::endl;
		std::cout << std::string(N * 2 - 1, '-') << std::endl;
		for (int j = M-1; j >= 0; --j) {
			for (int i = 0; i < N; ++i) 
				std::cout << (getColumnSize(i) > j ? std::to_string(int(getCircleData(i, j))) + " " : "  ");
			std::cout << std::endl;
		}
		std::cout << std::string(N*2 - 1, '-') << std::endl;
	}

private:
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
	void acceptMoves(const frame_moves *moves) {
		m_moves = moves;
		//m_current_it = m_moves->begin();
	}
	Frame generateNext() {
		Frame frame(*this, 0, 0); //rvo
		//m_current_it++;
		return frame;
	}
	bool isEndIterate() {
		return false; //m_current_it == m_moves->end();
	}
	bool dumpEnd() {
		return false;
	}
	
//Work with data
	hanoi_limit getCircleData(hanoi_limit n, hanoi_limit pos) const {
		return m_impl.getCircleData(n, pos);
	}
	hanoi_limit getColumnSize(hanoi_limit n) const {
		return m_impl.getColumnSize(n);
	}
	hanoi_limit getLastCircle(hanoi_limit n) {
		return m_impl.getLastCircle(n);
	}
	hanoi_limit getDepth() const {
		return m_impl.getDepth();
	}
	void dumpData() {
		m_impl.dumpData();
	}
private:
	FrameImpl<N,M> m_impl;
	const frame_moves* m_moves;
};