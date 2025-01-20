#include <iostream> 
#include <bits/stdc++.h>

#pragma once
#include "StringManagment.cpp"
#include "Graph.cpp"

using namespace std; 

int main(){
    StringManagment manager;

    string abc, arg; 
    cin >> abc; 
    if(!manager.mapeo(abc)){
        cout << "Existe un error en el abecedario, no puedes agregar '(, ), +, *, ., |, #' ";
        return 0;
    }

    cin >> arg;
    arg = manager.polish(arg);
    cout << arg << endl;

    set<char> alph = manager.getAlph();
    Graph gr(alph, arg);

    if(!gr.buildGraphs())
        return 0;
    
    cout << "NFA: ";
    gr.bfs();
}