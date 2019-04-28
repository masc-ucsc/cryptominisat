/******************************************
Copyright (c) 2019, Mate Soos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***********************************************/

#ifndef CMS_BOSPHORUS_H_
#define CMS_BOSPHORUS_H_

#include "bosphorus/bosphorus.hpp"
#include "cryptominisat5/solvertypesmini.h"

using Bosph::Solution;
using Bosph::Bosphorus;

namespace CMSat {

class Solver;

class CMSBosph
{
public:
    CMSBosph(Solver* solver);
    lbool run();

private:
    bool init();

    enum class add_cl_ret {added_cl, skipped_cl, unsat};
    template<class T>
    add_cl_ret add_this_clause(const T& cl, vector<int>& cl2);

    Solver* solver;
    Bosph::Bosphorus bosp;
};

}

#endif //CMS_BOSPHORUS_H_
