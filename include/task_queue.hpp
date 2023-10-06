#pragma once


#include "frame.hpp"
namespace boost::lockfree {
	template<class T, typename ...Options>
	class queue;
}
#include <queue>

namespace hanoi {

template<hanoi_limit N, hanoi_limit M, bool parallel>
class RecursiveQueue {
public:
	void push(Frame<N, M>&& frame) {
		if constexpr (parallel) {
			m_frames_lockfree.push(frame);
		}
		else {
			m_frames.push(frame);
		}
	}
	Frame<N, M> pop() {
		if constexpr (parallel) {
			Frame<N, M> frame; //rvo
			m_frames_lockfree.pop(frame);
			return frame;
		}
		else {
			Frame<N, M> frame(m_frames.front()); //rvo
			m_frames.pop();
			return frame;
		}
	}
	void clear() {
		if constexpr (!parallel) {
			m_frames = std::queue<Frame<N, M>>{};
		}
	}
	RecursiveQueue() {
		push(Frame<N, M>{}); //initital
	}
private:
	boost::lockfree::queue<Frame<N, M>> m_frames_lockfree{ 0 };
	std::queue<Frame<N, M>> m_frames;
};

}