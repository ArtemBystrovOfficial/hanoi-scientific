#pragma once

#include "frame.hpp"

#ifdef PARALLEL_MODE
namespace boost::lockfree {
    template<class T, typename ...Options>
    class queue;
} 
#else
#include <queue>
#endif

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
#ifdef PARALLEL_MODE
		Frame<N,M> frame; //rvo
		m_frames.pop(frame);
#else
		Frame<N, M> frame(m_frames.front()); //rvo
		m_frames.pop();
#endif
		return frame;
	}
private:
	RecursiveQueue() {
		push(Frame<N,M>{}); //initital
	}
#ifdef PARALLEL_MODE
	boost::lockfree::queue<Frame<N,M>> m_frames{ 0 };
#else
	std::queue<Frame<N, M>> m_frames;
#endif
};