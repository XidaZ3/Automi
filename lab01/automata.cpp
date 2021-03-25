#include <iostream>
#include "automata.h"

using namespace std;

/**
 * Constructor for Abstract DFA.
 * 
 * @param noStates
 *            Number of states in the DFA.
 */
vector<int> AbstractDFA::getStates() const
{
    return states;
}

void AbstractDFA::setStates(const vector<int> &value)
{
    states = value;
}

vector<int> AbstractDFA::getFinals() const
{
    return finals;
}

void AbstractDFA::setFinals(const vector<int> &value)
{
    finals = value;
}

int AbstractDFA::getInitial() const
{
    return initial;
}

void AbstractDFA::setInitial(int value)
{
    initial = value;
}

int AbstractDFA::getCurrent() const
{
    return current;
}

void AbstractDFA::setCurrent(int value)
{
    current = value;
}

map<tpair, int> AbstractDFA::getTransitions() const
{
    return transitions;
}

void AbstractDFA::setTransitions(const map<tpair, int> &value)
{
    transitions = value;
}

AbstractDFA::AbstractDFA(int noStates) {
    states.resize(noStates);
}

/**
 * Reset the automaton to the initial state.
 */
void AbstractDFA::reset() {
    // TODO: reset automaton to initial state
    current = initial;
}

/**
 * Performs one step of the DFA for a given letter. If there is a transition
 * for the given letter, then the automaton proceeds to the successor state.
 * Otherwise it goes to the sink state. By construction it will stay in the
 * sink for every input letter.
 * 
 * @param letter
 *            The current input.
 */
void AbstractDFA::doStep(char letter) {
    // TODO: do step by going to the next state according to the current
    // state and the read letter.
    current = transitions[{current,letter}];
}

/**
 * Check if the automaton is currently accepting.
 * 
 * @return True, if the automaton is currently in the accepting state.
 */
bool AbstractDFA::isAccepting() {
    // TODO: return if the current state is accepting
    for(auto i : finals)
        if(i==current) return true;
    return false;
}

/**
 * Run the DFA on the input.
 * 
 * @param inputWord
 *            stream that contains the input word
 * @return True, if if the word is accepted by this automaton
 */
bool AbstractDFA::run(const string &inputWord) {
    this->reset();
    for(int i = 0; i < inputWord.length(); i++) {
        doStep(inputWord[i]);
    }
    return isAccepting();
}


/**
 * Construct a new DFA that recognizes exactly the given word. Given a word
 * "foo" the constructed automaton looks like: -> () -f-> () -o-> () -o-> []
 * from every state (including the final one) every other input letter leads
 * to a distinguished sink state in which the automaton then remains
 * 
 * @param word
 *            A String that the automaton should recognize
 */

WordDFA::WordDFA(const string &word) : AbstractDFA(word.length()+2) {
    // TODO: fill in correct number of states
    std::vector<int> states;
    std::vector<int> finals;
    std::map<tpair,int> transitions;

    //trap state
    states.push_back(word.length()+2);

    for(int i=0;i<word.length();i++)
    {
        states.push_back(i);
        transitions.insert({{i,word[i]},i+1});
        //transition anything/anythingelse mappato con l'ASCII del NAK
        transitions.insert({{i,{21}},word.length()+1});

    }

    transitions.insert({{word.length(),{21}},word.length()+1});
    transitions.insert({{word.length()+1,{21}},word.length()+1});
    // TODO: build DFA recognizing the given word

    finals.push_back(word.length());
    setInitial(0);
    setCurrent(0);
    setFinals(finals);
    setStates(states);
    setTransitions(transitions);
}

/**
 * Construct a new DFA that recognizes comments within source code. There
 * are three kinds of comments: single line comment that starts with // and ends
 * with a newline, multiline comments that starts with (* and ends with *), and
 * multiline comments that starts with { and ends with }
 */


CommentDFA::CommentDFA() : AbstractDFA(0) {
    // TODO: fill in correct number of states
    // TODO: build DFA recognizing comments
}

/**
 * Performs one step of the DFA for a given letter. This method works
 * differently than in the superclass AbstractDFA.
 * 
 * @param letter
 *            The current input.
 */
void CommentDFA::doStep(char letter) {
    // TODO: implement accordingly
}	


