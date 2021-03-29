#include <iostream>
#include "automata.h"

using namespace std;


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

/**
 * Constructor for Abstract DFA.
 *
 * @param noStates
 *            Number of states in the DFA.
 */
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
    /*
     * Per effettuare uno step utilizziamo la funzione at() delle std::map che a differenza dell'operator[], non crea
     * la coppia nel momento in cui si vuole accedere alla posizione determinata da una specifica chiave.
     * Nel caso in cui non sia definita una transizione specifica per il simbolo, si ripiega sulla transazione fittizia definita
     * per il simbolo ASCII 21 ( NAK ) nella clausola catch conseguentemente al fallimento del primo tentativo di accesso alla struttura dati.
     * */
    try{
        current = transitions.at({current,letter});
    }catch(out_of_range){
        current = transitions.at({current,char(21)});
    }

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


CommentDFA::CommentDFA() : AbstractDFA(9) {

    /*
     * Per riconoscere i commenti è stato creato un DFA  che fa utilizzo di un trap state (indice 8)
     * La sua implementazione è statica rispetto alle tipologie di commenti di cui è dotato il linguaggio
     * L'implementazione è effettuata tramite la definizione di una mappa che associa, come nella definizione della
     * quintupla che caratterizza un DFA, un coppia {stato, simbolo} -> stato. Per fare ciò viene sfruttata la typedef tpair.
     * Viene definito un vettore di stati finali, in questo caso solo quello di indice 7.
     *
     * Siccome non è stato definito un alfabeto per l'automa, e siccome non sappiamo come si evolveranno le funzionalità di queste
     * classi, abbiamo deciso di implementare una transazione fittizia, corrispondente al carattere ASCII 21 ( NAK ) data l'improbabilità che
     * questo compaia nell'input, per definire lo stato destinazione per le combinazioni di stato e input relativamente a tutti gli altri simboli
     * dell'alfabeto che non sono già trattati.
     * */
    map<tpair,int> transitions;
    vector<int> finals;


    transitions.insert({{0,'/'},1});
    transitions.insert({{0,'('},2});
    transitions.insert({{0,'{'},3});
    transitions.insert({{0,char(21)},8});
    transitions.insert({{1,'/'},4});
    transitions.insert({{1,char(21)},8});
    transitions.insert({{2,'*'},5});
    transitions.insert({{2,char(21)},8});
    transitions.insert({{3,char(21)},3});
    transitions.insert({{3,'}'},7});
    transitions.insert({{4,'\n'},7});
    transitions.insert({{4,char(21)},4});
    transitions.insert({{5,'*'},6});
    transitions.insert({{5,char(21)},5});
    transitions.insert({{6,')'},7});
    transitions.insert({{6,char(21)},5});
    transitions.insert({{7,char(21)},8});
    transitions.insert({{8,char(21)},8});

    /*
     * Come menzionato, l'unico stato finale è il 7, mentre quello iniziale è l'1, viene inoltre resettato l'automa alla fine della costruzione per sicurezza.
     * */
    finals.push_back(7);
    setInitial(0);
    setCurrent(0);
    setFinals(finals);
    setTransitions(transitions);

}

/**
 * Performs one step of the DFA for a given letter. This method works
 * differently than in the superclass AbstractDFA.
 * 
 * @param letter
 *            The current input.
 */
void CommentDFA::doStep(char letter) {
    AbstractDFA::doStep(letter);
}	


