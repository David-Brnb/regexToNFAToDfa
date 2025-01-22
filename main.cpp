#include <iostream> 
#include <bits/stdc++.h>

#include "StringManagment.cpp"
#include "Graph.cpp"

using namespace std; 

int main(){
    StringManagment manager;

    string abc, arg; 
    cout << "Alphabet: ";
    cin >> abc; 
    if(!manager.mapeo(abc)){
        cout << "Theres an error in the alphabet, it shouldn't contain '(, ), +, *, ., |, #' ";
        return 0;
    }

    cout << "Regex: ";
    cin >> arg;
    arg = manager.polish(arg);
    cout << arg << endl;

    set<char> alph = manager.getAlph();
    Graph gr(alph, arg);

    if(!gr.buildGraphs())
        return 0;
    
    cout << "\nNFA: ";
    gr.bfs();

    cout << "\nDFA: ";
    gr.printDFA();
}