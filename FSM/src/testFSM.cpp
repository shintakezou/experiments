/*
g++-5.2 -Wall -g2 -std=c++11 -Wl,-rpath,$HOME/usr/local/lib FSM.cpp testFSM.cpp -o testFSM
*/

#include "FSM.hpp"

#include <iostream>

using namespace mFSM;


int main()
{
    // #?.info, i.e. .*\.info, handwritten
    FSM m;
    m.connect("S1", "S2", ".");
    m.connect("S2", "S3", "i");
    m.connect("S3", "S4", "n");
    m.connect("S4", "S5", "f");
    m.state("S6", true);
    m.connect("S5", "S6", "o");
    m.connect("S2", "S1", "any");
    m.connect("S3", "S1", "any");
    m.connect("S4", "S1", "any");
    m.connect("S5", "S1", "any");

    // a(ab)^n
    // if you can read italian, see
    // https://okpanico.wordpress.com/2014/03/09/linguaggi-regolari/
    FSM aab;
    aab.strict(true);
    aab.state("S1", true);
    aab.connect("S0", "S1", "a");
    aab.connect("S1", "S2", "a");
    aab.connect("S2", "S1", "b");
    aab.first("S0");

    // recognizes codes 12A45 or 22B48; if you can read italian,
    // https://okpanico.wordpress.com/2014/03/03/macchine-a-stati-finiti-2-2/
    FSM rec;
    rec.strict(true);
    rec.state("S9", true);
    const char* to1[] = {
        "S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8"
    };
    for (auto& s : to1) {
        rec.connect(s, "S1", "1");
        rec.connect(s, "S0", "any");
    }
    rec.connect("S9", "S0", "any");
    const char* to2[] = {
        "S0", "S8", "S7", "S6", "S5"
    };
    for (auto& s : to2) {
        rec.connect(s, "S2", "2");
    }
    rec.connect("S1", "S3", "2");
    rec.connect("S2", "S4", "2");
    rec.connect("S3", "S4", "2");
    rec.connect("S3", "S5", "A");
    rec.connect("S4", "S4", "2");
    rec.connect("S4", "S6", "B");    
    rec.connect("S5", "S7", "4");
    rec.connect("S6", "S8", "4");
    rec.connect("S7", "S9", "5");
    rec.connect("S8", "S9", "8");
    rec.first("S0");


    // test rule ordering
    FSM ro;
    ro.strict(true);
    ro.state("S1", true);
    ro.connect("S0", "S1", "a");
    ro.connect("S0", "S2", "~b");
    ro.connect("S0", "S1", "any");
    ro.first("S0");

    // grow this list to cover more cases on the edges(?)
    struct {
        FSM* f;
        const char* str;
        bool expected;
    } test_list[] = {
        { &ro, "a", true },
        { &ro, "x", false },
        { &ro, "b", true },
        { &m, "pippo.info", true },
        { &m, "pippo.inf", false },
        { &m, "x.inf", false },
        { &m, ".infother", true },
        { &m, ".info", true },
        { &aab, "a", true },
        { &aab, "aab", true },
        { &aab, "aaa", false },
        { &aab, "aabab", true },
        { &aab, "ac", false },
        { &aab, "aac", false },
        { &rec, "12A45", true },
        { &rec, "22B48", true },
        { &rec, "AA", false },
        { &rec, "12B48", false },
        { &rec, "22A45", false },
        { &rec, "12A22B4", false },
        { &rec, "1312A4", false }
    };

    for (auto& s : test_list) {
        bool res = s.f->run(s.str);
        std::cout << s.str << ": " << ((res != s.expected) ?
                                       "FAIL" : "passed") << "\n";
    }
    
    return 0;
}
