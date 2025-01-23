#include <iostream> 
#include <bits/stdc++.h>

using namespace std;

#ifndef STRINGMANAGMENT
#define STRINGMANAGMENT

/*
    Esta clase se encarga de realizar el procesamiento de entradas, 
    tales como definir el alfabeto, agregar operadores implícitos y 
    cambiar la notación de infija a posfija en una expresión regular.
*/

class StringManagment {
private: 
    set<char> abc; // Conjunto del alfabeto

    /*
        Método privado para refactorizar la entrada de la expresión regular. 
        Agrega operadores definidos para la concatenación implícita.
        Complejidad temporal: O(n), ya que recorre linealmente la cadena.
    */
    string refactoriza(string &str){
        string news="";
        for(int i=0; i<str.size(); i++){
            if(i!=0 && (str[i-1]==')' || str[i-1]=='*' || str[i-1]=='+' || abc.count(str[i-1])>0) && (str[i]=='(' || abc.count(str[i])>0)){
                news+=".";
            }
            news+=str[i];
        }

        return news;
    }

public: 
    // Constructor de la clase
    StringManagment(){};

    /*  
        Realiza el mapeo del alfabeto, verificando que no contenga caracteres reservados.
        Complejidad temporal: O(n), donde n es el tamaño del alfabeto ingresado.
        Retorna true si el mapeo es exitoso, false si contiene caracteres inválidos.
    */
    bool mapeo(string str){
        abc.clear();

        for(auto e: str){
            if(e=='(' || e=='*' || e=='+' || e=='.' || e=='|' || e==')' || e=='#')
                return false;
        }

        for(auto e: str){
            abc.insert(e);
        }

        return true;
    }

    /*
        Convierte una expresión regular de notación infija a notación posfija.
        Primero, refactoriza la expresión para agregar operadores implícitos.
        Utiliza el algoritmo Shunting Yard propuesto por Edsger Dijkstra.
        Complejidad temporal: O(n), donde n es el tamaño de la expresión regular, 
        esto gracias a que las operaciones realizadas durante el procesamiento
        tienen una complejidad de O(1) y se realizan de forma limitada por cada
        carácter.
    */
    string polish(string str){
        str = refactoriza(str);
        map<char, int> mapi;
        mapi['('] = 0;
        mapi['|'] = 1;
        mapi['.'] = 2;
        mapi['+'] = 3;
        mapi['*'] = 3;
        mapi[')'] = -1;

        stack<char> sti;
        queue<char> q; 

        for(int i=0; i<str.size(); i++){
            if(abc.count(str[i])>0){
                q.push(str[i]); // Carácter del alfabeto

            } else if(str[i] == ')'){
                while(!sti.empty() && sti.top() !='('){
                    q.push(sti.top());
                    sti.pop();
                }

                if(!sti.empty())
                    sti.pop(); // Eliminar '('

            } else {
                while(!sti.empty() && mapi[sti.top()]>=mapi[str[i]] && str[i]!='('){
                    if(sti.top()!='(') q.push(sti.top());
                    sti.pop();
                }
                sti.push(str[i]);
            }
        }

        while(!sti.empty()){
            if(sti.top()!='(') q.push(sti.top());
            sti.pop();
        }
        
        string news="";
        while(!q.empty()){
            news+=q.front();
            q.pop();
        }

        return news;
    }

    // Retorna el conjunto de caracteres del alfabeto
    set<char> getAlph(){
        return this->abc;
    }

};




#endif