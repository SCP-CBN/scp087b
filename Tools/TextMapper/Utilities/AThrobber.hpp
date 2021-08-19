#pragma once

#include <thread>

#include "MultiPrint.hpp"

template <int SUGGESTED_MS = -1>
class Throbber {
	public:
		Throbber(std::ostream& stream, int ms) : out(stream) {
			this->ms = ms;
		}

		template <typename = typename std::enable_if<SUGGESTED_MS >= 0>::type>
		Throbber(std::ostream& stream) : out(stream) {
			ms = SUGGESTED_MS;
		}

		~Throbber() {
			if (throbbing) {
				stop();
			}
		}

		virtual void start() {
			throbbing = true;
			thread = std::thread([=]() {
				while (throbbing) {
					this->throb();
					std::this_thread::sleep_for(std::chrono::milliseconds(ms));
				}
				this->finalize();
			});
		}

		virtual void stop() {
			throbbing = false;
			thread.join();
		}

	protected:
		std::ostream& out;

	private:
		int ms;
		std::thread thread;
		std::atomic_bool throbbing = false;

		virtual void throb() = 0;
		virtual void finalize() = 0;
};

#define ARRAY_WITH_SIZE(id) sizeof(id)/sizeof(*id), id

template <int size, const String[size] strings, int ms = -1, bool clean = false>
class StringThrobber : public Throbber<ms> {
	public:
		void start() override {
			// Because increment happens before first print.
			i = size - 1;
			first = true;
			Throbber<ms>::start();
		}

	protected:
		int i;
		bool first;

		void throb() override {
			if (!first) {
				Throbber<ms>::out << Multi{ '\b', strings[i].length() };
				if (clean) {
					Throbber<ms>::out << Multi{ ' ', strings[i].length() };
					Throbber<ms>::out << Multi{ '\b', strings[i].length() };
				}
			}
			first = false;

			i = (i + 1) % size;
			Throbber<ms>::out << strings[i];
		}

	private:
		void finalize() override {
			Throbber<ms>::out << Multi{ '\b', strings[i].length() };
			Throbber<ms>::out << Multi{ ' ', strings[i].length() };
		}
};

class ProgressBarThrobber : public Throbber<> {
	public:
		ProgressBarThrobber(std::ostream& stream, int ms, const String& head, const String& tail, char done, char todo, int width, int filled) : Throbber(stream, ms) {
			this->head = head;
			this->tail = tail;
			this->done = done;
			this->todo = todo;
			this->width = width;
			this->filled = filled;
		}

	private:
		String head;
		String tail;
		char done;
		char todo;
		int width;
		int filled;
		int i = -filled;

		void throb() override {
			int to = i + filled;
			int start = std::clamp(i, 0, width);
			int actualFilled = std::max(0, filled - std::max(0, i + filled - width) - std::max(0, -i));

			out << '\r' << head;
			out << Multi{ todo, start };
			out << Multi{ done, actualFilled };
			out << Multi{ todo, width - actualFilled - start };
			out << tail;

			// We could abstract the i incrementation here in order to have a it going back and forth, but eh.
			i++;
			if (i >= width + filled) {
				i = -filled;
			}
		}
		
		void finalize() override {
			out << '\r' << head;
			out << Multi{ done, width };
			out << tail << std::endl;
		}
};

const String BALLOON_THROBBER[] = { '.', 'o', 'O', '@', '*', ' ' };
class BalloonThrobber : public StringThrobber<ARRAY_WITH_SIZE(BALLOON_THROBBER), 1000> {
	private:
		void throb() override {
			StringThrobber::throb();
			// POP!
			if (i == 4) { out << '\a'; }
		}
};

// Windows doesn't like °.
const String BALLOON_THROBBER_2[] = { ".", "o", "O", "o", "." };
using BalloonThrobber2 = StringThrobber<ARRAY_WITH_SIZE(BALLOON_THROBBER_2), 200>;

const String LINE_THROBBER[] = { "-", "\\", "|", "/" };
using LineThrobber = StringThrobber<ARRAY_WITH_SIZE(LINE_THROBBER), 150>;

const String DOT_THROBBER[] = { ".  ", ".. ", "...", "   " };
using DotThrobber = StringThrobber<ARRAY_WITH_SIZE(DOT_THROBBER), 400>;

const String DOT_THROBBER_2[] = { ".  ", ".. ", "...", " ..", "  .", "   " };
using DotThrobber2 = StringThrobber<ARRAY_WITH_SIZE(DOT_THROBBER_2), 200>;

const String STAR_THROBBER[] = { "+x+x+x+", "x+x+x+x" };
using StarThrobber = StringThrobber<ARRAY_WITH_SIZE(STAR_THROBBER), 80>;

// Windows is fine with `, but not ´!
const String FLIP_THROBBER[] = { "_", "_", "_", "-", "`", "`", "'", "'", "-", "_", "_", "_" };
using FlipThrobber = StringThrobber<ARRAY_WITH_SIZE(FLIP_THROBBER), 70>;

const String DQPB_THROBBER[] = { "d", "q", "p", "b" };
using DQPBThrobber = StringThrobber<ARRAY_WITH_SIZE(DQPB_THROBBER), 100>;
