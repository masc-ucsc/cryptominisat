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

#include "cms_bosphorus.h"
#include "solver.h"

using namespace CMSat;
using Bosph::Solution;
using Bosph::Bosphorus;

CMSBosph::CMSBosph(Solver* _solver):
    solver(_solver)
{}

template<class T>
CMSBosph::add_cl_ret CMSBosph::add_this_clause(const T& cl, vector<int>& cl_out)
{
    cl_out.clear();

    bool sat = false;
    for(size_t i3 = 0; i3 < cl.size(); i3++) {
        Lit lit = cl[i3];
        assert(solver->varData[lit.var()].removed == Removed::none);
        lbool val = l_Undef;
        if (solver->value(lit) != l_Undef) {
            val = solver->value(lit);
        } else {
            val = solver->lit_inside_assumptions(lit);
        }

        if (val == l_True) {
            //clause is SAT, skip!
            sat = true;
            continue;
        } else if (val == l_False) {
            continue;
        }

        int l = lit.var()+1;
        if (lit.sign()) {
            l *= -1;
        }
        cl_out.push_back(l);
    }

    if (sat) {
        return add_cl_ret::skipped_cl;
    }

    if (cl_out.size() == 0) {
        //it's unsat because of assumptions
        if (solver->conf.verbosity) {
            cout << "c [walksat] UNSAT because of assumptions in clause: " << cl << endl;
        }
        return add_cl_ret::unsat;
    }
    return add_cl_ret::added_cl;
}

lbool CMSBosph::run()
{
    bool ret = init();
    if (!ret) {
        return l_Undef;
    }
    Bosph::Bosphorus::print_stats(anf);

    double myTime = cpuTime();
    if (solver->conf.verbosity) {
        cout << "c [ANF hash] Calculating ANF hash..." << endl;
    }

    auto orig_anf = Bosph::Bosphorus::copy_anf_no_replacer(anf);
    if (solver->conf.verbosity) {
        cout << "c [ANF hash] Done. T: " << (cpuTime() - myTime) << endl;
    }
    Solution solution = bosp.simplify(anf, NULL);
    assert(solution.ret == Bosph::l_Undef);

    /*if (solver->conf.printProcessedANF) {
        Bosphorus::print_anf(anf);
    }*/

    if (solver->conf.verbosity >= 1) {
        Bosphorus::print_stats(anf);
    }
    bosp.add_trivial_learnt_from_anf_to_learnt(anf, orig_anf);
    Bosphorus::delete_anf(orig_anf);

    // remove duplicates from learnt clauses
    bosp.deduplicate();

    cout << "Extra learnt: " << endl;
    vector<Bosph::Clause> cls = bosp.get_learnt(anf);
    for(auto cl: cls) {
        for(auto l: cl.getLits()) {
            cout << l << " ";
        }
        cout << endl;
    }
    exit(-1);

    return l_Undef;
}


bool CMSBosph::init()
{
    anf = bosp.start_cnf_input(solver->nVars());

    //where all clauses' literals are
    vector<Lit> bin_cl;
    vector<int> final_cl;

    solver->check_stats();
    for(size_t i2 = 0; i2 < solver->nVars()*2; i2++) {
        Lit lit = Lit::toLit(i2);
        for(const Watched& w: solver->watches[lit]) {
            if (w.isBin() && !w.red() && lit < w.lit2()) {
                bin_cl.clear();
                bin_cl.push_back(lit);
                bin_cl.push_back(w.lit2());

                if (add_this_clause(bin_cl, final_cl) == add_cl_ret::unsat) {
                    return false;
                }
                bosp.add_clause(anf, final_cl);
            }
        }
    }
    for(ClOffset offs: solver->longIrredCls) {
        const Clause* cl = solver->cl_alloc.ptr(offs);
        assert(!cl->freed());
        assert(!cl->getRemoved());

        if (add_this_clause(*cl, final_cl) == add_cl_ret::unsat) {
            return false;
        }
        bosp.add_clause(anf, final_cl);
    }

    return true;
}
