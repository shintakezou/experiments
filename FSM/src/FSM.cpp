/*
    mFSM
 */

#include "FSM.hpp"

#include <new>
#include <string>
#include <functional>
#include <algorithm>
#include <type_traits>

#include <iostream>

namespace mFSM
{


State::State(const std::string& name, bool final) :
    name_(name), final_(final)
{}

bool State::accept() const
{
    return final_;
}

const std::string& State::name() const
{
    return name_;
}

void State::setFinal(bool final)
{
    final_ = final;
}

// ------------------------------------------

Rule::Rule(const std::string& rule) :
    rule_(rule)
{
    if (rule.length() == 1) {
        setsize_ = 1;
    } else if (rule[0] == '~') {
        setsize_ = 255;
    } else if (rule.compare("any") == 0) {
        setsize_ = 256;
    } else {
        setsize_ = 0;
    }
}

bool Rule::satisfiedBy(int c) const
{
    if (exactMatch()) {
        return rule_[0] == c;
    } else if (matchAll()) {
        return true;
    } else if (rule_[0] == '~') {
        return c != rule_[1];
    }
    // here we should raise an exception; but no exception
    // are used in this code... that's it. Rather, I would
    // set an internal state to "machine is out of service"!
    return false;
}

bool Rule::exactMatch() const
{
    return rule_.length() == 1;
}

bool Rule::matchMany() const
{
    return rule_.length() > 1 &&
        (rule_.compare("any") == 0 || rule_[0] == '~');
}

bool Rule::matchAll() const
{
    return rule_.length() > 1 && rule_.compare("any") == 0;
}

size_t Rule::size() const
{
    return setsize_;
}



// ------------------------------------------

AwareState::AwareState(const std::string& name, bool final) :
    State(name, final)
{}

AwareState& AwareState::onRule(const Rule* rule, const AwareState* dest)
{
    Connection con{rule, dest->name()};
    connections_.push_front(con);
    connections_.sort([](const Connection& a, const Connection& b)->bool {
            return a.rule->size() < b.rule->size();
        });
    return *this;
}

bool AwareState::overConnectionsDo(
    std::function<bool(const Rule*,
                       const std::string&)> action) const
{
    return std::any_of(connections_.begin(),
                       connections_.end(),
                       [&](decltype(connections_.front()) c) {
                           return action(c.rule, c.destState);
                       });
    // the previous piece should work like the following
    /* 
    for (auto& c : connections_) {
        if (action(c.rule, c.destState)) {
            return true;
        }
    }
    return false;
    */
}

// ------------------------------------------

FSM::FSM(bool debug) : current_(nullptr),
                       first_(nullptr),
                       strict_(false),
                       debug_(debug)
{}


FSM::~FSM()
{
    for (auto& p : rules_) {
        delete p.second;
    }
    for (auto& p : states_) {
        delete p.second;
    }
}

bool FSM::accepted() const
{
    if (debug_ && current_) {
        std::cerr << "state: " << current_->name() << "\n";
    }
    return current_ ? current_->accept() : false;
}

void FSM::rewind()
{
    current_ = first_;
}

void FSM::first(const std::string& s)
{
    if (states_.count(s) > 0) {
        first_ = states_[s];
    }
}

void FSM::strict(bool b)
{
    strict_ = b;
}


AwareState& FSM::state(const std::string& n, bool final)
{
    if (states_.count(n) > 0) {
        // the name's already taken;
        // return whatever it was created, ignore final arg.
        return *states_[n];
    }
    AwareState* new_state = new AwareState(n, final); // no nothrow...
    states_[n] = new_state;
    if (!first_) {
        first_ = new_state;
    }
    return *new_state;
}


AwareState& FSM::get(const std::string& n)
{
    if (states_.count(n) > 0) {
        return *states_[n];
    }
    return state(n, false);
}


bool FSM::connect(const std::string& s1,
                  const std::string& s2,
                  const std::string& rule)
{
    AwareState* state1 = nullptr;
    AwareState* state2 = nullptr;
    Rule* orule = nullptr;

    if (states_.count(s1) > 0) {
        state1 = states_[s1];
    } else {
        state1 = &state(s1, false);
        if (!state1) {
            return false;
        }
        states_[s1] = state1;
    }

    if (states_.count(s2) > 0) {
        state2 = states_[s2];
    } else {
        state2 = &state(s2, false);
        if (!state2) {
            return false;
        }
        states_[s2] = state2;
    }

    if (rules_.count(rule) > 0) {
        orule = rules_[rule];
    } else {
        orule = new (std::nothrow) Rule(rule);
        if (!orule) {
            return false;
        }
        rules_[rule] = orule;
    }

    state1->onRule(orule, state2);
    return true;
}

bool FSM::feed(int c)
{
    if (!first_) {
        return false;
    }
    if (!current_) {
        current_ = first_;
    }

    const AwareState* next_current = current_;
    bool matched = current_->overConnectionsDo(
        [c,&next_current,this](const Rule* r, const std::string& s)->bool {
            if (r->satisfiedBy(c)) {
                if (this->states_.count(s) > 0) {
                    next_current = states_[s];
                }
                return true;
            }
            return false;
        });

    if (debug_) {
        std::cerr << current_->name() << "(" << c << ")"
                  << next_current->name() << "\n";
    }

    current_ = next_current;

    return matched;
}


bool FSM::run(const std::string& s)
{
    bool matched;
    rewind();
    for (auto& c : s) {
        matched = feed(c);
        if (strict_ && !matched) {
            break;
        }
    }
    return current_->accept() && (!strict_ || matched);
}



} // ns: mFSM
