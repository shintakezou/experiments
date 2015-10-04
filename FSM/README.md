Simple Finite State Machine
===========================

`FSM` (in the namespace `mFSM`) is a simple Finite State Machine born as an educational and personal attempt to experiment with string pattern matching. Hence it is not a general `FSM`: “events” are characters read from a string or given to the machine *directly*.

A brief overview (which counts as documentation) follows.


Class State
-----------

This class defines a dumb state. A state has a name and can be “final”; by this I mean that if the machine stops in this state, then the sequence the machine went through so far is **ok** (must be accepted).


Class AwareState
----------------

This class is derived from `State` and supposedly represents a state which is aware of the *rules* that make the machine stay in the state or jump to another. Namely it's a state plus its *connections* with other states, according to *transition rules*.

The class allows to specify which *rule* brings to which *state*. Moreover, it allows to iterate over its connections calling an *action* (a function) that is in charge of checking the rule and acting so that the state of the machine changes accordingly.

A connection is a *struct* holding the pointer to the rule and the **name** of the destination state (in case the rule applies).


Class Rule
----------

This class represents a *rule*. You can ask the instance if the rule is satisfied by a specific character. A rule can be an exact match (when the string describing the rule is a single character) or something more complicated.

Currently a “bloated” *rule* can be:

- keyword `any`: the rule is satisfied by any character;
- character `~` followed by another character: the rule is satisfied if the character **does not** match;

**Note**: when you connect a state to another one through the rule `any`, this rule is checked after all the other rules. The reason is obvious.


Class FSM
---------

This class allows you to build your FSM. It keeps track of the states (recognized by name) and rules (a single rule can be used more than once). Once the machine is well defined, you can *feed* it with a single character input; you can *rewind* it to the initial state; you can check if it stays in an “accepted” state; you can run the machine *over* a whole string.

Before using the machine, you must define it, of course. You can create a new state (deciding if is a *final* one or not), giving it a name you can use when you call the `connect()` method. If a state with a name in the `connect()` method does not exist, it is created, but it won't be final; thus you must create it explicitly (with `state()`) or set the “final” later (…`.get().setFinal(true)`).

The first state created, explicitly or implicitly, will become the initial state; to change it, use `first()` (the state with the name you pass to `first()` must exist; otherwise, the initial state won't be changed).

The third argument to `connect()` is the *rule*. See above.

There are no check to assure the FSM is well-defined: it is up to the user to construct and use it correctly. I doubt it will change in future.

**Note**: when the FSM is not “strict”, any character for which there's no rule, makes the machine stay in the current state. In “strict” mode, an input character for which there's no rule will cause the machine to stop processing input when you use `run()`; if you are feeding the machine by yourself, it is up to you to decide what to do with the code returned by `feed()`. Sayint it differently, when “running” the machine with `run()`, the *strict* flag set to false makes the machine behave like if each state has an implied rule which connect the state with itself through `any`.

**Note**: first and only argument to the *ctor* of the FSM class allows you to enable debugging informations. Likely it will become a compile-time only option.



Compiling test(s)
=================

The line could look like this:

    g++ -Wall -Wextra -g2 -I src/ -std=c++11 src/FSM.cpp src/testFSM.cpp -o testFSM

This is also almost the way you would compile anything using `FSM.cpp`. Be sure your `g++` version supports C++11. `4.7.2` should be enough (though I compile also with 5.2).


