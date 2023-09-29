#pragma once

#include "frame.hpp"

namespace boost::lockfree {
    template<class T, typename ...Options>
    class queue;
} 

template<hanoi_limit N, hanoi_limit M>
class RecursiveQueue {
public:
	static RecursiveQueue<N,M>& Instance() {
		static RecursiveQueue<N,M> packet;
		return packet;
	}
	void push(Frame<N,M> && frame) {
		m_frames.push(frame);
	}
	Frame<N,M> pop() {
		Frame<N,M> frame; //rvo
		m_frames.pop(frame);
		return std::move(frame);
	}
private:
	RecursiveQueue() {
		push(Frame<N,M>{}); //initital
	}
	boost::lockfree::queue<Frame<N,M>> m_frames{ 0 };
};