#include <iostream>
#include <random>
#include <math.h>
#include <functional>
#include <algorithm>
#include <utility>

#define WATER_COST 30
#define ENERGY_COST 50
#define SALARY_COST 500

#define MAX_FUNC_SMALL 15
#define MAX_FUNC_MEDIUM 30
#define MAX_FUNC_LARGE 60

#define MAX_STOCK_SMALL 150
#define MAX_STOCK_MEDIUM 300
#define MAX_STOCK_LARGE 500

#define BASE_SELL_PORC .1

#define ALPHA .75
#define BETA .60
#define LAMBDA 2.10

#define A -250
#define B 1500
#define C 45
#define D 105

using namespace std;

enum building {
    small,
    medium,
    large
};

enum investment {
    none = 0,
    low = 750,
    normal = 1500,
    high = 3000
};

//RANDOM DICE
mt19937 random_generator(4585965885986);
uniform_real_distribution<float> real_distribution(0,1);

auto real_rand = bind(real_distribution, random_generator);

class Enterprise {
    private:
        investment marketResearch;  //add chance
        investment propaganda;      //add chance
        investment training;        //add chance

        //investment feedStock;       //multiply product cost
        //investment production;      //multiply product cost

        building typeOfBuilding;    //Maximum employees
        int numberOfEmployees;
        
        int stock;
        float productPrice;

        float cash;

    public:
        Enterprise(building size, int initialEmployees, int initialStock, 
                int initialPrice, int initialCash): 
                      marketResearch(investment::none),
                      propaganda(investment::none),
                      training(investment::none),
                      typeOfBuilding(size),
                      numberOfEmployees(initialEmployees),
                      productPrice(initialPrice),
                      stock(initialStock),
                      cash(initialCash)
                {};

        float calculateCost() {
            float cost = 0.0f;

            if (typeOfBuilding == building::small) cost += 1000;
            if (typeOfBuilding == building::medium) cost += 2000;
            if (typeOfBuilding == building::large) cost += 3000;

            cost += numberOfEmployees*WATER_COST;
            cost += numberOfEmployees*ENERGY_COST;
            cost += numberOfEmployees*SALARY_COST;

            //Separate
            cost += marketResearch;
            cost += propaganda;
            cost += training;   //Depende do número de funcionários
            
            return cost;
        };

        long mapRange(long x, long in_min, long in_max, long out_min, long out_max) {
              return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        };

        float distanceTwoPoints(float equX, float equY, float empX, float empY) {
            return sqrt( pow(equX - empX,2) + pow(equY - empY,2) );            
        };

        bool verifyInvestment(investment investmentValue) {
            if (investmentValue > cash) return false;
            return true;
        };

        float investmentReturn(investment INV) {
            if (INV == none) return 0.0f;
            if (INV == low) return 0.05f;
            if (INV == normal) return 0.1f;
            if (INV == high) return 0.15f;
        };

        float calculatePorcentual(int equX, float equY) {
            float aux = BASE_SELL_PORC;

            aux += investmentReturn(marketResearch);
            aux += investmentReturn(propaganda);
            aux += investmentReturn(training); //depende do numero de funcionarios

            //Adjust by demand and offer
            float distance = distanceTwoPoints(equX, equY, stock, productPrice);
            float value = mapRange(distance, 0, 500, 45, -15); //verificar limites 0-2000

            aux += value/100;

            return aux;
        };

        float calculateProfit(int equX, float equY) {
            int grossProfit = 0;
            float sellChance = calculatePorcentual(equX, equY);
            for (int i=0; i<stock; i++) 
                if (real_rand() >= sellChance) grossProfit += productPrice;

            return grossProfit - calculateCost();
        };

        float endTurn(int equX, float equY) {
            cash = cash + calculateProfit(equX, equY);
            return cash;
        }

        //Jogadas Possíveis
        //Passivas
        int getStock() { return stock; };
        float getPrice() { return productPrice; };
        int getEmployees() { return numberOfEmployees; };
        float getCash() { return cash; };
        building getTypeOfBuilding() { return typeOfBuilding; };
        investment getMarketResearch() { return marketResearch; };
        investment getPropaganda() { return propaganda; };
        investment getTraining() { return training; };

        //Ativas
        void upgradeBuilding() {
            if (typeOfBuilding == building::small) typeOfBuilding = building::medium;
            if (typeOfBuilding == building::medium) typeOfBuilding = building::large;
        };

        void addEmployee() {
            if (typeOfBuilding == building::small &&
                    numberOfEmployees < MAX_FUNC_SMALL) numberOfEmployees++;

            if (typeOfBuilding == building::medium &&
                    numberOfEmployees < MAX_FUNC_MEDIUM) numberOfEmployees++;

            if (typeOfBuilding == building::large &&
                    numberOfEmployees < MAX_FUNC_LARGE) numberOfEmployees++;
        };

        //Só modifica se tem dinheiro
        void setMarketResearch(investment newInvestment) { marketResearch = newInvestment; };
        void setPropaganda(investment newInvestment) { propaganda = newInvestment; };
        void setTraining(investment newInvestment) { training = newInvestment; };

        //Preço de penalidade ao renovar estoque
        bool setStock(int newStock) {
            if (typeOfBuilding == building::small &&
                    newStock < MAX_STOCK_SMALL) { 
                stock = newStock;
                return true;
            }

            if (typeOfBuilding == building::medium &&
                    newStock < MAX_STOCK_MEDIUM)  {
                stock = newStock;
                return true;
            }

            if (typeOfBuilding == building::large &&
                    newStock < MAX_STOCK_LARGE) { 
                stock = newStock;
                return true;
            }
            return false;
        };

        void setPrice(float newPrice) { productPrice = newPrice; };

};

class Node {
    private:
        int height;
        Node * parent;
        vector<Enterprise> players;
        vector<Node> possiblePlays;

    public:
        Node(int h, Node * node): height(h), parent(node) {};

        int getHeight() { return height; };
        vector<Enterprise> getPlayers() { return players; };
        vector<Node> getPossiblePlays() { return possiblePlays; };

        void addPlayer(Enterprise enterprise) { players.push_back(enterprise); }
        void addPossiblePlay(Node node) { possiblePlays.push_back(node); };
        int verifyPlayerTurn() { return height%players.size(); }; 
        Enterprise getPlayer() { return players[verifyPlayerTurn()]; };

        //Criar heuristica
        float calculateHeuristic(Enterprise turnPlayer) {
            return 100.0f*real_rand();
        };
        
        float calculateEstimateCost() {
            float totalHeuristicEstimate = 0;
            int playerTurn = verifyPlayerTurn();
            for (int i=0; i < players.size(); i++) { 
                if (playerTurn == i) totalHeuristicEstimate += calculateHeuristic(players[i]);
                else totalHeuristicEstimate -= calculateHeuristic(players[i]);
            }
            return totalHeuristicEstimate;
        };

};

pair<int, float> findIntersection() {
    int x = (D - A)/(A - C);
    int y = A*x -B;

    return make_pair(x,y);
}

Node createRawPlay(Node * node) {
    Node newNode(node->getHeight() + 1, node);
    for (auto player : node->getPlayers() ) 
        newNode.addPlayer(player);

    return newNode;
}

void addMarketResearch(investment inv, Node * node, Node rawNode) {
    if (node->getPlayer().getMarketResearch() != inv) {
        rawNode.getPlayer().setMarketResearch(inv);
        node->addPossiblePlay(rawNode);
    }
}

void addPropaganda(investment inv, Node * node, Node rawNode) {
    if (node->getPlayer().getPropaganda() != inv) {
        rawNode.getPlayer().setPropaganda(inv);
        node->addPossiblePlay(rawNode);
    }
}

void addTraining(investment inv, Node * node, Node rawNode) {
    if (node->getPlayer().getTraining() != inv) {
        rawNode.getPlayer().setTraining(inv);
        node->addPossiblePlay(rawNode);
    }
}

bool verifyBuildingUpdate(Enterprise * ent) {
    if (ent->getTypeOfBuilding() == building::large) return false;
    return true;
}

bool verifyMarketResearch(investment inv, Enterprise * ent) {
    if (ent->getMarketResearch() == inv) return false;
    return true;
}

bool verifyPropaganda(investment inv, Enterprise * ent) {
    if (ent->getPropaganda() == inv) return false;
    return true;
}

bool verifyTraining(investment inv, Enterprise * ent) {
    if (ent->getTraining() == inv) return false;
    return true;
}

bool verifyEmployee(Enterprise * ent) {
    int numberOfEmployees = ent->getEmployees();
    if (numberOfEmployees < MAX_FUNC_SMALL || 
        numberOfEmployees < MAX_FUNC_MEDIUM ||
        numberOfEmployees < MAX_FUNC_LARGE) 
        return true; 
    return false;
}

void populatePossiblePlays(Node * node) {
    Node rawNode = createRawPlay(node);

    //Investment possibilities - Market Research
    addMarketResearch(investment::none, node, rawNode);
    addMarketResearch(investment::low, node, rawNode);
    addMarketResearch(investment::normal, node, rawNode);
    addMarketResearch(investment::high, node, rawNode);

    //Investment possibilities - Propaganda
    addPropaganda(investment::none, node, rawNode);
    addPropaganda(investment::low, node, rawNode);   
    addPropaganda(investment::normal, node, rawNode);
    addPropaganda(investment::high, node, rawNode);

    //Investment possibilities - Training
    addTraining(investment::none, node, rawNode);
    addTraining(investment::low, node, rawNode);   
    addTraining(investment::normal, node, rawNode);
    addTraining(investment::high, node, rawNode);

    //Update building
    if (node->getPlayer().getTypeOfBuilding() != building::large) {
        rawNode.getPlayer().upgradeBuilding();
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }

    //Add employees
    int numberOfEmployees = node->getPlayer().getEmployees();
    if (numberOfEmployees < MAX_FUNC_SMALL || 
        numberOfEmployees < MAX_FUNC_MEDIUM ||
        numberOfEmployees < MAX_FUNC_LARGE) {
        rawNode.getPlayer().addEmployee();
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node);
    }
}

float search(int depth, Node * node) {
    float value = 0; 
    if (0 == depth) return node->calculateEstimateCost();
    if (node->getPossiblePlays().empty()) populatePossiblePlays(node);
    for (auto play : node->getPossiblePlays())  
        value += search(depth-1, &play) + node->calculateEstimateCost();

    return value;
}

float prospectAnalysis(float value) {
    if (value >= 0) return pow(value, ALPHA);
    else return -LAMBDA*pow((-value), BETA);
}

int analysis(int depth, Node * root) {
    vector<float> values;

    if (root->getPossiblePlays().empty()) populatePossiblePlays(root);
    for (auto play : root->getPossiblePlays()) {
        values.push_back(
                prospectAnalysis(
                    search(depth-1, &play)));
    }

    return distance(values.begin(), 
            max_element(values.begin(), values.end()) );
}

void initialMenu() {
    cout << "Seja Bem Vindo ao Enterprise Simulator!" << endl << endl;
    cout << "   1 - Player VS Player" << endl;
    cout << "   2 - Player VS IA" << endl;
    cout << "   3 - IA VS IA" << endl;
    cout << "   4 - Multiplayer" << endl;
    cout << "   5 - Sair" << endl << endl;
    cout << "Digite a opção desejada: ";
}

void PVP() {
    int tipo, dinheiro_inicial, func_inicial, tempo_jogo, estoque_inicial;
    int tamanho1, tamanho2;
    float preco1, preco2;
    int jogada;

    pair<int, float> equ = findIntersection();

    cout << "Tipo do produto: " << endl;
    cout << "   1 - Impressora 3D" << endl;
    cout << "   2 - Chocolate" << endl;
    cout << "   3 - Peças automotivas" << endl;
    cout << "Digite o tipo:";
    cin >> tipo;

    cout << "Dinheiro Inicial(0~10000): ";
    cin >> dinheiro_inicial;

    cout << "Número de Funcionários Inicial(1~15): ";
    cin >> func_inicial;

    cout << "Tempo de jogo: ";
    cin >> tempo_jogo;

    cout << "Estoque inicial(10~150): ";
    cin >> estoque_inicial;

    //ler Player 1
    cout << endl << endl << "Configuração player 1" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho1;

    cout << "Digite o preço inicial: ";
    cin >> preco1;

    //ler Player 2
    cout << endl << endl << "Configuração player 2" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho2;

    cout << "Digite o preço inicial: ";
    cin >> preco2;

    building build;
    if (tamanho1 == 1)
        build = building::small;
    if (tamanho1 == 2)
        build = building::medium;
    if (tamanho1 == 3)
        build = building::large;

    Enterprise player1(build, func_inicial, estoque_inicial, preco1, dinheiro_inicial);

    if (tamanho2 == 1)
        build = building::small;
    if (tamanho2 == 2)
        build = building::medium;
    if (tamanho2 == 3)
        build = building::large;

    Enterprise player2(build, func_inicial, estoque_inicial, preco2, dinheiro_inicial);

    for (int i=0; i<tempo_jogo*4; i++) {
        for (int j=0; j<2; j++) {
            if (0==j) {
                cout << endl << "Player1" << endl;

                cout << "Escolha uma jogada: " << endl;
                if(!verifyMarketResearch(investment::none, &player1)) {
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::low, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::normal, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::high, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                }
               
                //
                if(!verifyPropaganda(investment::none, &player1)) {
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::low, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::normal, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::high, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                }
                
                //
                if(!verifyTraining(investment::none, &player1)) {
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::low, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::normal, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::high, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                }

                if(verifyEmployee(&player1))
                    cout << "   12 - Contratar Funcionários" << endl;

                if(verifyBuildingUpdate(&player1))
                    cout << "   13 - Melhorar local de Trabalho" << endl;
                
                cout << endl << "Digite o número da jogada: ";
                cin >> jogada; 

                //processar
                if (0 == jogada) player1.setMarketResearch(investment::none);
                if (1 == jogada) player1.setMarketResearch(investment::low);
                if (2 == jogada) player1.setMarketResearch(investment::normal);
                if (3 == jogada) player1.setMarketResearch(investment::high);

                if (4 == jogada) player1.setPropaganda(investment::none);
                if (5 == jogada) player1.setPropaganda(investment::low);
                if (6 == jogada) player1.setPropaganda(investment::normal);
                if (7 == jogada) player1.setPropaganda(investment::high);
                
                if (8 == jogada) player1.setTraining(investment::none);
                if (9 == jogada) player1.setTraining(investment::low);
                if (10 == jogada) player1.setTraining(investment::normal);
                if (11 == jogada) player1.setTraining(investment::high);

                if (12 == jogada) player1.addEmployee();
                
                if (13 == jogada) player1.upgradeBuilding();

                if (0 != i) {
                    int newprice;
                    cout << endl << "Preço atual: " << player1.getPrice() << endl;
                    cout << "Novo preço: ";
                    cin >> newprice;
                    player1.setPrice(newprice);

                    cout << endl << "Estoque atual: " << player1.getStock() << endl;
                    cout << "Novo estoque: ";
                    cin >> newprice;
                    while (!player1.setStock(newprice)) {
                        cout << "Estoque inválido! Digite novamente." << endl;
                        cout << "Novo Estoque: ";
                        cin >> newprice;
                    }
                }
            } else {
                cout << endl << "Player2" << endl;

                cout << "Escolha uma jogada: " << endl;
                if(!verifyMarketResearch(investment::none, &player2)) {
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::low, &player2)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::normal, &player2)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::high, &player2)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                }
               
                //
                if(!verifyPropaganda(investment::none, &player2)) {
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::low, &player2)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::normal, &player2)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::high, &player2)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                }
                
                //
                if(!verifyTraining(investment::none, &player2)) {
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::low, &player2)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::normal, &player2)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::high, &player2)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                }

                if(verifyEmployee(&player2))
                    cout << "   12 - Contratar Funcionários" << endl;

                if(verifyBuildingUpdate(&player2))
                    cout << "   13 - Melhorar local de Trabalho" << endl;
                
                cout << endl << "Digite o número da jogada: ";
                cin >> jogada; 

                //processar
                if (0 == jogada) player2.setMarketResearch(investment::none);
                if (1 == jogada) player2.setMarketResearch(investment::low);
                if (2 == jogada) player2.setMarketResearch(investment::normal);
                if (3 == jogada) player2.setMarketResearch(investment::high);

                if (4 == jogada) player2.setPropaganda(investment::none);
                if (5 == jogada) player2.setPropaganda(investment::low);
                if (6 == jogada) player2.setPropaganda(investment::normal);
                if (7 == jogada) player2.setPropaganda(investment::high);
                
                if (8 == jogada) player2.setTraining(investment::none);
                if (9 == jogada) player2.setTraining(investment::low);
                if (10 == jogada) player2.setTraining(investment::normal);
                if (11 == jogada) player2.setTraining(investment::high);

                if (12 == jogada) player2.addEmployee();
                
                if (13 == jogada) player2.upgradeBuilding();
           
                if (0 != i) {
                    int newprice;
                    cout << endl << "Preço atual: " << player2.getPrice() << endl;
                    cout << "Novo preço: ";
                    cin >> newprice;
                    player2.setPrice(newprice);

                    cout << endl << "Estoque atual: " << player2.getStock() << endl;
                    cout << "Novo estoque: ";
                    cin >> newprice;
                    while (!player2.setStock(newprice)) {
                        cout << "Estoque inválido! Digite novamente." << endl;
                        cout << "Novo Estoque: ";
                        cin >> newprice;
                    }
                }
            }
        }

        cout << endl << endl << "--------------------------------------------------Fim de Turno|" << endl;
        cout << "Resultados do turno: " << endl;
        cout << "   Player1: " << player1.endTurn(equ.first, equ.second) << endl;
        cout << "   Player2: " << player2.endTurn(equ.first, equ.second) << endl;
        cout << endl << endl;
    }

    if (player1.getCash() < player2.getCash()) cout << "Player 2 Ganhou!" << endl;
    else cout << "Player 1 Ganhou!" << endl;
}

void PVM() {
    int tipo, dinheiro_inicial, func_inicial, tempo_jogo, estoque_inicial;
    int tamanho1, tamanho2, profundidade_pesquisa;
    float preco1, preco2;
    int jogada;

    pair<int, float> equ = findIntersection();

    cout << "Tipo do produto: " << endl;
    cout << "   1 - Impressora 3D" << endl;
    cout << "   2 - Chocolate" << endl;
    cout << "   3 - Peças automotivas" << endl;
    cout << "Digite o tipo:";
    cin >> tipo;

    cout << "Dinheiro Inicial(0~10000): ";
    cin >> dinheiro_inicial;

    cout << "Número de Funcionários Inicial(1~15): ";
    cin >> func_inicial;

    cout << "Tempo de jogo: ";
    cin >> tempo_jogo;

    cout << "Estoque inicial(10~150): ";
    cin >> estoque_inicial;

    //ler Player 1
    cout << endl << endl << "Configuração player 1" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho1;

    cout << "Digite o preço inicial: ";
    cin >> preco1;

    //ler IA
    cout << endl << endl << "Configuração IA" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho2;

    cout << "Digite o preço inicial: ";
    cin >> preco2;

    cout << "Profundidade de Pesquisa: ";
    cin >> profundidade_pesquisa;

    building build;
    if (tamanho1 == 1)
        build = building::small;
    if (tamanho1 == 2)
        build = building::medium;
    if (tamanho1 == 3)
        build = building::large;

    Enterprise player1(build, func_inicial, estoque_inicial, preco1, dinheiro_inicial);

    if (tamanho2 == 1)
        build = building::small;
    if (tamanho2 == 2)
        build = building::medium;
    if (tamanho2 == 3)
        build = building::large;

    Enterprise IA(build, func_inicial, estoque_inicial, preco2, dinheiro_inicial);

    for (int i=0; i<tempo_jogo*4; i++) {
        for (int j=0; j<2; j++) {
            if (0==j) {
                cout << endl << "Player1" << endl;

                cout << "Escolha uma jogada: " << endl;
                if(!verifyMarketResearch(investment::none, &player1)) {
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::low, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::normal, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   3 - Investir em Pesquisa de Mercado: Grande" << endl;
                }

                if(!verifyMarketResearch(investment::high, &player1)) {
                    cout << "   0 - Investir em Pesquisa de Mercado: Nada" << endl;
                    cout << "   1 - Investir em Pesquisa de Mercado: Pequena" << endl;
                    cout << "   2 - Investir em Pesquisa de Mercado: Normal" << endl;
                }
               
                //
                if(!verifyPropaganda(investment::none, &player1)) {
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::low, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::normal, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   7 - Investir em Propaganda: Grande" << endl;
                }

                if(!verifyPropaganda(investment::high, &player1)) {
                    cout << "   4 - Investir em Propaganda: Nada" << endl;
                    cout << "   5 - Investir em Propaganda: Pequena" << endl;
                    cout << "   6 - Investir em Propaganda: Normal" << endl;
                }
                
                //
                if(!verifyTraining(investment::none, &player1)) {
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::low, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::normal, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   11 - Investir em Treinamento: Grande" << endl;
                }

                if(!verifyTraining(investment::high, &player1)) {
                    cout << "   8 - Investir em Treinamento: Nada" << endl;
                    cout << "   9 - Investir em Treinamento: Pequena" << endl;
                    cout << "   10 - Investir em Treinamento: Normal" << endl;
                }

                if(verifyEmployee(&player1))
                    cout << "   12 - Contratar Funcionários" << endl;

                if(verifyBuildingUpdate(&player1))
                    cout << "   13 - Melhorar local de Trabalho" << endl;
                
                cout << endl << "Digite o número da jogada: ";
                cin >> jogada; 

                //processar
                if (0 == jogada) player1.setMarketResearch(investment::none);
                if (1 == jogada) player1.setMarketResearch(investment::low);
                if (2 == jogada) player1.setMarketResearch(investment::normal);
                if (3 == jogada) player1.setMarketResearch(investment::high);

                if (4 == jogada) player1.setPropaganda(investment::none);
                if (5 == jogada) player1.setPropaganda(investment::low);
                if (6 == jogada) player1.setPropaganda(investment::normal);
                if (7 == jogada) player1.setPropaganda(investment::high);
                
                if (8 == jogada) player1.setTraining(investment::none);
                if (9 == jogada) player1.setTraining(investment::low);
                if (10 == jogada) player1.setTraining(investment::normal);
                if (11 == jogada) player1.setTraining(investment::high);

                if (12 == jogada) player1.addEmployee();
                
                if (13 == jogada) player1.upgradeBuilding();

                if (0 != i) {
                    int newprice;
                    cout << endl << "Preço atual: " << player1.getPrice() << endl;
                    cout << "Novo preço: ";
                    cin >> newprice;
                    player1.setPrice(newprice);

                    cout << endl << "Estoque atual: " << player1.getStock() << endl;
                    cout << "Novo estoque: ";
                    cin >> newprice;
                    while (!player1.setStock(newprice)) {
                        cout << "Estoque inválido! Digite novamente." << endl;
                        cout << "Novo Estoque: ";
                        cin >> newprice;
                    }
                }

            } else {
                Node node(0, NULL);
                node.addPlayer(player1);
                node.addPlayer(IA);

                int play = analysis(profundidade_pesquisa, &node);

                vector<Node> plays = node.getPossiblePlays();
                IA = plays[play].getPlayer();

                IA.setPrice(IA.getPrice()*(real_rand() + real_rand()));
                IA.setStock(IA.getStock()*(real_rand() + real_rand()));

                cout << endl << "IA" << endl;
                cout << "Preço IA: " << IA.getPrice() << endl;
                cout << "Estoque IA: " << IA.getStock() << endl;
            }
        }

        cout << endl << endl << "--------------------------------------------------Fim de Turno|" << endl;
        cout << "Resultados do turno: " << endl;
        cout << "   Player1: " << player1.endTurn(equ.first, equ.second) << endl;
        cout << "   IA: " << IA.endTurn(equ.first, equ.second) << endl;
        cout << endl << endl;
    }

    if (player1.getCash() < IA.getCash()) cout << "IA Ganhou!" << endl;
    else cout << "Player 1 Ganhou!" << endl;
   
}

void MVM() {
    int tipo, dinheiro_inicial, func_inicial, tempo_jogo, estoque_inicial;
    int tamanho1, tamanho2, profundidade_pesquisa1, profundidade_pesquisa2;
    float preco1, preco2;
    int jogada;

    pair<int, float> equ = findIntersection();

    cout << "Tipo do produto: " << endl;
    cout << "   1 - Impressora 3D" << endl;
    cout << "   2 - Chocolate" << endl;
    cout << "   3 - Peças automotivas" << endl;
    cout << "Digite o tipo:";
    cin >> tipo;

    cout << "Dinheiro Inicial(0~10000): ";
    cin >> dinheiro_inicial;

    cout << "Número de Funcionários Inicial(1~15): ";
    cin >> func_inicial;

    cout << "Tempo de jogo: ";
    cin >> tempo_jogo;

    cout << "Estoque inicial(10~150): ";
    cin >> estoque_inicial;

    //ler Player 1
    cout << endl << endl << "Configuração IA1" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho1;

    cout << "Digite o preço inicial: ";
    cin >> preco1;

    cout << "Profundidade de Pesquisa: ";
    cin >> profundidade_pesquisa1;

    //ler IA
    cout << endl << endl << "Configuração IA2" << endl << endl;
    cout << "Prédio inicial: " << endl;
    cout << "   1 - Pequeno" << endl;
    cout << "   2 - Médio" << endl;
    cout << "   3 - Grande" << endl;
    cout << "Digite o tamanho: ";
    cin >> tamanho2;

    cout << "Digite o preço inicial: ";
    cin >> preco2;

    cout << "Profundidade de Pesquisa: ";
    cin >> profundidade_pesquisa2;

    building build;
    if (tamanho1 == 1)
        build = building::small;
    if (tamanho1 == 2)
        build = building::medium;
    if (tamanho1 == 3)
        build = building::large;

    Enterprise IA1(build, func_inicial, estoque_inicial, preco1, dinheiro_inicial);

    if (tamanho2 == 1)
        build = building::small;
    if (tamanho2 == 2)
        build = building::medium;
    if (tamanho2 == 3)
        build = building::large;

    Enterprise IA2(build, func_inicial, estoque_inicial, preco2, dinheiro_inicial);

    for (int i=0; i<tempo_jogo*4; i++) {
        for (int j=0; j<2; j++) {
            if (0==j) {
                Node node(0, NULL);
                node.addPlayer(IA2);
                node.addPlayer(IA1);

                int play = analysis(profundidade_pesquisa1, &node);

                vector<Node> plays = node.getPossiblePlays();
                IA1 = plays[play].getPlayer();

                IA1.setPrice(IA1.getPrice()*(real_rand() + real_rand()));
                IA1.setStock(IA1.getStock()*(real_rand() + real_rand()));

                cout << endl << "IA1" << endl;
                cout << "Preço IA1: " << IA1.getPrice() << endl;
                cout << "Estoque IA1: " << IA1.getStock() << endl;
            } else {
                Node node(0, NULL);
                node.addPlayer(IA1);
                node.addPlayer(IA2);

                int play = analysis(profundidade_pesquisa2, &node);

                vector<Node> plays = node.getPossiblePlays();
                IA2 = plays[play].getPlayer();

                IA2.setPrice(IA2.getPrice()*(real_rand() + real_rand()));
                IA2.setStock(IA2.getStock()*(real_rand() + real_rand()));

                cout << endl << "IA2" << endl;
                cout << "Preço IA2: " << IA2.getPrice() << endl;
                cout << "Estoque IA2: " << IA2.getStock() << endl;
            }
        }

        cout << endl << endl << "--------------------------------------------------Fim de Turno|" << endl;
        cout << "Resultados do turno: " << endl;
        cout << "   IA1: " << IA1.endTurn(equ.first, equ.second) << endl;
        cout << "   IA2: " << IA2.endTurn(equ.first, equ.second) << endl;
        cout << endl << endl;
        cin.get();
    }

    if (IA1.getCash() < IA2.getCash()) cout << "IA2 Ganhou!" << endl;
    else cout << "IA1 Ganhou!" << endl;
}

int main() {
    int menu = 0;

    while (menu != 5) {
        initialMenu();
        cin >> menu;

        if (1 == menu) PVP();
        if (2 == menu) PVM();
        if (3 == menu) MVM();
        if (4 == menu) cout << endl << "Opção não implementada" << endl;
    }

    cout << endl << "Obrigado por jogar!" << endl;
}
