#include <iostream> 
#include <bits/stdc++.h>

using namespace std;
typedef pair<char, int> pci;

#ifndef GRAPH
#define GRAPH

/*
    Esta clase se encarga de la contruccion de los autmatas tanto deterministico como el no deterministico, 
    esto lo hace mediante la declaración de 2 grafos, uno para el NFA y el otro para el DFA. 
*/
class Graph {       
private:
    vector<vector<pci> > adj;
    vector<vector<pci> > adjDFA;
    map<int, set<int> > closureDP;
    map<int, bool > closureDPB;
    set<char> abc;
    set<int> accepted;
    int N = 1e6;
    int nfaBegin;
    int nfaEnd; 
    int last_used;
    string polak;

    /*
        Este metodo se encarga de construir algoritmicamente el NFA, 
        esto se logra gracias a que como atributo de la clase tenemos 
        la expresion regular en notación posfija, lo cual nos sirve 
        ya que solo aplicamos los operadores de expresiones regulares 
        de manera parecida a los operadores aritmeticos. 
        Ademas, nos ayudamos de un grafo representado como listas de 
        adyacencia que nos ayuda a conectar los nuevos nodos, generando
        nuevos automatas cuando se trata de elementos del alfabeto, o 
        modificando automatas cuando se trata de operadores. 
        La complejidad temporal de este método es O(n) amortizada, ya que
        solo se recorre la expresión, y unicamente se van realizando 
        operaciones constantes al unir aristas. 
    */
    bool buildNFA(){
        int last_used_node = 0;
        int last_used_at = 0;
        vector<int> bgg(N, -1);
        vector<int> endg(N, -1);

        stack<int> sti;

        for(int i=0; i<polak.size(); i++){
            if(abc.count(polak[i])>0){
                int newBg = last_used_node, newEnd = last_used_node+1;
                int currAt = last_used_at;
                last_used_node += 2;
                last_used_at += 1;

                if(newBg>=N || newEnd >=N || currAt>=N)
                    return false;

                adj[newBg].push_back(pci(polak[i], newEnd));

                bgg[currAt] = newBg;
                endg[currAt] = newEnd;

                sti.push(currAt);


            } else {
                if(polak[i] == '*' || polak[i] == '+'){
                    if(sti.empty())
                        return false;
                    
                    int token = sti.top(); 
                    sti.pop();

                    int newBg = last_used_node, newEnd = last_used_node+1;
                    int currbg=bgg[token], currEnd=endg[token];
                    last_used_node += 2;

                    bgg[token] = newBg;
                    endg[token] = newEnd;

                    adj[newBg].push_back(pci('#', currbg));
                    adj[currEnd].push_back(pci('#', newEnd));
                    adj[currEnd].push_back(pci('#', currbg));

                    if(polak[i] == '*')
                        adj[newBg].push_back(pci('#', newEnd));
                    
                    sti.push(token);

                } else if(polak[i] == '|'){
                    int newBg = last_used_node, newEnd = last_used_node+1;
                    last_used_node+=2;

                    if(sti.empty())
                        return false;
                    int token2 = sti.top(); 
                    sti.pop();

                    if(sti.empty())
                        return false;
                    int token1 = sti.top(); 
                    sti.pop();
                    
                    adj[newBg].push_back(pci('#', bgg[token1]));
                    adj[newBg].push_back(pci('#', bgg[token2]));
                    adj[endg[token1]].push_back(pci('#', newEnd));
                    adj[endg[token2]].push_back(pci('#', newEnd));

                    bgg[token1] = newBg;
                    endg[token1] = newEnd;

                    bgg[token2] = -1;
                    endg[token2] = -1; 

                    sti.push(token1);            

                } else if(polak[i]=='.'){
                    if(sti.empty())
                        return false;
                    int token2 = sti.top(); 
                    sti.pop();

                    if(sti.empty())
                        return false;
                    int token1 = sti.top(); 
                    sti.pop();
                    
                    adj[endg[token1]].push_back(pci('#', bgg[token2]));
                    endg[token1] = endg[token2];

                    bgg[token2] = -1;
                    endg[token2] = -1; 

                    sti.push(token1);  
                }

            }
        }

        if(sti.empty())
            return false;
        int at = sti.top();
        sti.pop();

        nfaBegin = bgg[at];
        nfaEnd = endg[at];

        return true;
    }

    /*
        Este es un metodo axiliar de buildDFA(). 
        Se encarga de verificar los nodos adyacentes a cierto nodo, 
        bajo el costo de algun caracter del alfabeto, y regresa un 
        conjunto conteniendo los nodos adyacentes a los nodos recibidos, 
        bajo la letra del alfabeto recibida. 
        La complejidad de este metodo esta acotada por la cantidad del nodos, 
        y la cantidad de aristas por nodo. 
        En el peor de los casos, si el grafo fuese completo y se nos dieran 
        todos los nodos del grafo la complejidad sería O(V*(V-1)), V son los vertices o nodos.
        No obstante, por la construcción del NFA sabemos que lo maxímo de aristas
        en uno de nuestros nodos es 3, por ende la complejidad se amortiza a O(3*V), 
        es decir O(V)
    */
    set<int> move(set<int> state, char letter){
        set<int> mve;
        for(auto e: state){
            for(auto n: adj[e]){
                if(n.first == letter){
                    mve.insert(n.second);
                }
            }
        }

        return mve;
    }

    /*
        Este es un metodo axiliar de buildDFA(). 
        Se encarga de verificar el alcance de un nodo a todos los demás nodos 
        bajo la gráfica, tal que estos nodos esten conectados con epsilon. 
        Es decir, realiza un bfs desde cada nodo pasado a travez del set que 
        se tiene como parametro. 
        La complejidad de este metodo esta acotada por la cantidad del nodos, 
        y la cantidad de aristas del grafo. 
        La complejidad para este metodo esta realcionada a la cantidad de bfs realizados
        desde, en el peor de los casos esta sería del total de nodos, es decir, O(V*(V+E)), 
        donde V son los vertices o nodos y E las aristas del grafo. 
        Es decir la complejidad de realizar un bfs desde cada nodo del NFA, 
        esto lo podemos afirmar gracias a que mediante el uso de Programación Dinámica, 
        unicamente calculamos una vez el bfs de cada nodo, así nos sea ingresado 2 
        veces o más el mismo nodo. 
    */
    set<int> closure(set<int> state){
        set<int> result;

        for(auto e: state){
            //bfs
            set<int> cre;
            map<int, bool> visited;

            if(closureDPB[e]){
                cre = closureDP[e];
                for(auto node: cre){
                    result.insert(node);
                }
                continue;
            } 

            queue<int> q;
            visited[e]=true;
            q.push(e);


            while(!q.empty()){
                int node = q.front();
                q.pop();

                // process node
                cre.insert(node);

                for(auto e: adj[node]){
                    int nd = e.second;
                    if(!visited[nd] && e.first == '#'){
                        visited[nd]=true;
                        q.push(nd);
                    }
                }
            }

            closureDPB[e]=true;
            closureDP[e]=cre;

            for(auto node: cre){
                result.insert(node);
            }
        }
        
        return result;
    }


    /*
        Este metodo se encarga de construir el DFA, haciendo uso de los metodos auxiliares
        move y closure, asi como del grafo propuesto en el NFA. 
        Esto lo logramos mediante el mapeo de los estados generados previamente y su 
        respectivo nodo, ademas de aplicar el algoritmo visto en clase que hacer uso de 
        una cola, y los metodos move y closure.
        La complejidad temporal se ve afectada por la cantidad de ciclos que puede tener el 
        ciclo while de nuestro metodo, y los sub metodos interiores, podemos notar que el maximo
        de ciclos que puede tener nuestro ciclo while es 2^V, siendo V los vertices del NFA,
        pues al considerar que por cada estado diferente necesitaremos realizar otro ciclo, y un
        estado es generado por algun subconjunto de todos los nodos, entonces sabemos que todos los 
        subconjuntos son 2^n, siendo n el tamaño del conjunto. 
        Ahora bien, respecto a los metodos interiores, podemos afirmar que el metodo closure tiene la 
        peor complejidad ya que hace O(V*(V+E)) operaciones en el peor de los casos, donde V son los 
        vertices o nodos y E las aristas del grafo. 

        al unir estas complejidades obtenemos que la complejidad total en el peor de los casos esta acotada
        a O(2^v * (V * (V+E))). 
    */
    bool buildDFA(){
        map<set<int>, int> mapi;
        map<int, set<int>> ipam;
        accepted.clear();
        closureDP.clear();
        closureDPB.clear();
        set<int> curr;
        queue<int> q;

        curr.insert(nfaBegin);
        curr = closure(curr);

        mapi[curr]=last_used;
        ipam[last_used] = curr;
        q.push(last_used);
        last_used++;

        while(!q.empty()){
            int n = q.front();
            q.pop();
            curr = ipam[n];
            set<int> st;
            for(auto let: abc){
                st = move(curr, let);

                // si mandamos una entrada antes registrada, esta simplemente se calculara usando closureDP, 
                // reduciendo asi la complejidad temporal.
                st = closure(st);

                if(st.empty())
                    continue;

                if(mapi[st]!=0){
                    adjDFA[n].push_back(pci(let, mapi[st]));

                } else {
                    mapi[st]=last_used;
                    ipam[last_used]=st;
                    adjDFA[n].push_back(pci(let, last_used));
                    q.push(last_used);
                    last_used++;
                }

            }
        }

        // registramos todos los estados finales
        for(auto e: ipam){
            if(e.second.count(nfaEnd)>0)
                accepted.insert(e.first);
        }

        return true;
    }

public: 
    // se inicializan las variables a usar dentro del constructor. 
    Graph(set<char> abec, string exp) {
        this->adj = vector<vector<pci>>(N);
        this->adjDFA = vector<vector<pci>>(N);
        this->nfaBegin = -1;
        this->nfaEnd = -1;
        this->last_used = 1;
        this->abc = abec;
        this->polak = exp;
    }

    // se construyen las graficas y si falla algo se le hace saber al usuario
    bool buildGraphs(){
        if(!buildNFA()){
            cout << "NFA building failed" << endl;
            return false;
        }

        if(!buildDFA()){
            cout << "DFA building failed" << endl;
            return false;
        }

        return true;
    }

    // imprime el nfa realizando un bfs sobre el grafo
    // este metodo esta acotado por la complejidad del bfs, 
    // la cual es O(V+E). 
    void bfs(){
        vector<bool> visited(nfaEnd+10, false);
        cout << endl;
        queue<int> q;
        if(nfaBegin==-1){
            nfaBegin = 0;
        } 

        visited[nfaBegin]=true;
        q.push(nfaBegin);


        while(!q.empty()){
            int node = q.front();
            q.pop();

            // process node
            if(nfaEnd != node) cout << node << " => [";

            int j=0;
            for(auto e: adj[node]){
                if(j!=0) printf(", ");
                cout << "(" << e.second << ", '"<< e.first <<"')";
                int nd = e.second;
                if(!visited[nd]){
                    visited[nd]=true;
                    q.push(nd);
                }
                j++;
            }

            if(nfaEnd != node) cout << "]"<< endl;
        }

        cout << "Accepting state: " << nfaEnd << endl;

    }

    // Este metodo se encarga de imprimir el DFA
    // La complejidad del metodo esta acotada por la 
    // cantidad de vertices en el grafo siendo que el 
    // peor de los casos es O(V*(V-1))
    void printDFA(){
        cout << endl;
        for(int i=1; i<last_used; i++){
            cout << char(i+64) << " => [";
            int j=0; 
            for(auto e: adjDFA[i]){
                if(j!=0) printf(", ");
                printf("('%c', '%c')", char(e.second+64), e.first);
                j++;
            }
            cout << "]" << endl;
        }

        cout <<"Accepting states: [";
        int j=0;
        for(auto e: accepted){
            if(j!=0) printf(", ");
            printf("'%c'", char(e+64));
            j++;
        }
        cout << "]" << endl;


    }



};


#endif