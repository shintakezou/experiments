#ifndef M_FSM_HPP_
#define M_FSM_HPP_

#include <list>
#include <map>
#include <string>
#include <functional>

namespace mFSM
{

// basic State: name and if it's final
class State
{
public:
    State(const std::string& name, bool final=false);
    bool accept() const;
    const std::string& name() const;
    void setFinal(bool final=true);
protected:
    std::string name_;
    bool final_;
};

// a rule that says if the char c satisfies it.
// If it is a single char, then an exact match satifies the rule;
// otherwise: "any" matches any char, and that is all...
class Rule
{
public:
    Rule(const std::string& rule);
    bool satisfiedBy(int c) const;

    bool exactMatch() const;
    bool matchMany() const;
    bool matchAll() const;

    size_t size() const;
    
private:
    std::string rule_;
    size_t setsize_;
};

// a pair Rule-Destination State is a "connection"
// states are identified by name
struct Connection
{
    const Rule* rule;
    const std::string destState;
};

// an aware State it's a state who knows where to go when
// it is fed with a specific input; the input is "recognized"
// by the mean of a Rule
class AwareState : public State
{
public:
    AwareState(const std::string& name, bool final);
    AwareState& onRule(const Rule* rule, const AwareState* dest);
    bool overConnectionsDo(std::function<bool(const Rule*, const std::string&)> action) const;
private:
    std::list<Connection> connections_;
};


// ------------------------------------------
class FSM
{
public:
    FSM(bool debug=false);
    ~FSM();

    bool accepted() const;

    bool feed(int c);
    bool run(const std::string& s);

    AwareState& state(const std::string& n, bool final);
    AwareState& get(const std::string& n);
    bool connect(const std::string& s1, const std::string& s2, const std::string& rule);

    void rewind();
    void first(const std::string& s);
    void strict(bool b);
    
private:
    std::map<std::string,Rule*> rules_;
    std::map<std::string,AwareState*> states_;

    const AwareState* current_;
    const AwareState* first_;
    
    bool strict_;
    
    bool debug_;
};



} // ns: mFSM


#endif
