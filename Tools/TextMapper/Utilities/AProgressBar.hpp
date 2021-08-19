#pragma once

#include <PGE/String/String.h>
#include <PGE/Exception/Exception.h>

#include "MultiPrint.hpp"

using namespace PGE;

// TODO: Progress bar without numbers.
class ProgressBar {
    public:
        ProgressBar(int max, const String& head, const String& tail, char done, char todo, int width, bool printNum = true) {
            PGE_ASSERT(!printNum || width % 2 == 1, "Width must be odd when using count");
            this->max = max;
            this->head = head;
            this->tail = tail;
            this->done = done;
            this->todo = todo;
            this->width = width;
            this->printNum = printNum;
        }

        void update(int val) const {
            int doneCnt = val * width / max;
            std::cout << '\r';
            std::cout << head;
            if (printNum) {
                int num = String::from(max).length();
                int mid = (num + 2) * 2 + 1;
                int maxSide = (width - mid) / 2;
                int rightDone = std::max(0, doneCnt - maxSide - mid);
                std::cout << Multi{ done, std::min(maxSide, doneCnt) };
                std::cout << Multi{ todo, maxSide - doneCnt };
                std::cout << ' ' << Multi{ '0', num, false } << val;
                std::cout << " / ";
                std::cout << max << ' ';
                std::cout << Multi{ done, rightDone };
                std::cout << Multi{ todo, maxSide - rightDone };
            } else {
                std::cout << Multi{ done, doneCnt };
                std::cout << Multi{ todo, width - doneCnt };
            }
            std::cout << tail;
            std::cout.flush();
        }

        void finish() const {
            update(max);
            std::cout << std::endl;
        }

    private:
        int width;
        char done;
        char todo;
        String head;
        String tail;

        int max;
        int printNum;
};
