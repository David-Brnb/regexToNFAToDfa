#include <iostream> 
#include <bits/stdc++.h>

#include "StringManagment.cpp"
#include "Graph.cpp"

using namespace std; 

/*
    Clase principal que une las funcionalidades de las demás clases.
    Se encarga de manejar los inputs y outputs del programa.
*/
int main(){
    StringManagment manager;

    // Definimos variables y procesamos la entrada del alfabeto
    string abc, arg; 
    cout << "Alphabet: ";
    cin >> abc; 
    if(!manager.mapeo(abc)){
        cout << "Theres an error in the alphabet, it shouldn't contain '(, ), +, *, ., |, #' ";
        return 0;
    }

    // Procesamos la entrada de la expresión regular
    cout << "Regex: ";
    cin >> arg;
    arg = manager.polish(arg);

    // Obtenemos el alfabeto para construir los autómatas
    set<char> alph = manager.getAlph();
    Graph gr(alph, arg);

    // Construimos los autómatas, verificando si hay errores
    if(!gr.buildGraphs())
        return 0;
    
    // Imprimimos los autómatas construidos
    cout << "\nNFA: ";
    gr.bfs();

    cout << "\nDFA: ";
    gr.printDFA();
}