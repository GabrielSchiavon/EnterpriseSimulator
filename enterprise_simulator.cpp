#include <iostream>
#include <random>
#include <math.h>
#include <functional>

#define WATER_COST 50
#define ENERGY_COST 80
#define SALARY_COST 800

#define MAX_FUNC_SMALL 15
#define MAX_FUNC_MEDIUM 30
#define MAX_FUNC_LARGE 60

#define MAX_STOCK_SMALL 150
#define MAX_STOCK_MEDIUM 300
#define MAX_STOCK_LARGE 500

#define BASE_SELL_PORC .1

using namespace std;

enum building {
    small,
    medium,
    large
};

enum investment {
    none = 0,
    low = 1500,
    normal = 3000,
    high = 6000
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

            if (typeOfBuilding == building::small) cost += 2500;
            if (typeOfBuilding == building::medium) cost += 3500;
            if (typeOfBuilding == building::large) cost += 5000;

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
                if (real_rand() <= sellChance) grossProfit += productPrice;

            return grossProfit - calculateCost();
        };

        //Jogadas Possíveis
        //Passivas
        int getStock() { return stock; };
        float getPrice() { return productPrice; };
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
        void setStock(int newStock) {
            if (typeOfBuilding == building::small &&
                    newStock < MAX_STOCK_SMALL) stock = newStock;

            if (typeOfBuilding == building::medium &&
                    newStock < MAX_STOCK_MEDIUM) stock = newStock;

            if (typeOfBuilding == building::large &&
                    newStock < MAX_STOCK_LARGE) stock = newStock;
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

        void addPlayer(Enterprise enterprise) { players.push_back(enterprise); }
        void addPossiblePlay(Node node) { possiblePlays.push_back(node); };
        int verifyPlayerTurn() { return height%players.size(); };
        
        //Criar heuristica
        float calculateHeuristic(Enterprise turnPlayer) {
            return 0.0f;
        };
        
        float calculateEstimateCost(int turnPlayerID) {
            int totalHeuristicEstimate = 0;
            for (int i=0; i < players.size(); i++) 
                if (turnPlayerID == i) totalHeuristicEstimate += calculateHeuristic(players[i]);
                else totalHeuristicEstimate -= calculateHeuristic(players[i]);
            return totalHeuristicEstimate;
        };

};

Node createRawPlay(Node * node) {
    Node newNode(node->getHeight() + 1, node);
    for (auto player : node->getPlayers() ) 
        newNode.addPlayer(player);

    return newNode;
}

void populatePossiblePlays(Node * node) {
    Node rawNode = createRawPlay(node);

    //Investment possibilities - Market Research
    if (node->getMarketResearch != building::none)  {
        rawNode->setMarketResearch(building::none); 
        node->addPossiblePlay(rawNode); 
        rawNode = createRawPlay(node);
    }
    if (node->getMarketResearch != building::low) { 
        rawNode->setMarketResearch(building::low);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node);
    }
    if (node->getMarketResearch != bulding::medium) {
        rawNode->setMarketResearch(building::medium);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }
    if (node->getMarketResearch != bulding::high) {
        rawNode->setMarketResearch(building::high);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }

    //Investment possibilities - Propaganda
    if (node->getPropaganda != building::none)  {
        rawNode->setPropaganda(building::none); 
        node->addPossiblePlay(rawNode); 
        rawNode = createRawPlay(node);
    }
    if (node->getPropaganda != building::low) { 
        rawNode->setPropaganda(building::low);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node);
    }
    if (node->getPropaganda != bulding::medium) {
        rawNode->setPropaganda(building::medium);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }
    if (node->getPropaganda != bulding::high) {
        rawNode->setPropaganda(building::high);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }

    //Investment possibilities - Training
    if (node->getTraining != building::none)  {
        rawNode->setTraining(building::none); 
        node->addPossiblePlay(rawNode); 
        rawNode = createRawPlay(node);
    }
    if (node->getTraining != building::low) { 
        rawNode->setTraining(building::low);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node);
    }
    if (node->getTraining != bulding::medium) {
        rawNode->setTraining(building::medium);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }
    if (node->getTraining != bulding::high) {
        rawNode->setTraining(building::high);
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }

    //Update building
    if (node->getTypeOfBuilding() == bulding::large) {
        rawNode->upgradeBuilding();
        node->addPossiblePlay(rawNode);
        rawNode = createRawPlay(node); 
    }
}

int main() {
    Enterprise CACCOM(building::small, 5, 100, 250, 2000);

    cout << endl << CACCOM.calculatePorcentual(113, 318.153) << endl;
    cout << CACCOM.calculateCost() << endl;
    cout << CACCOM.calculateProfit(113, 318.153) << endl;

    CACCOM.setMarketResearch(investment::normal);
    CACCOM.setPropaganda(investment::high);
    CACCOM.setStock(100);
    //CACCOM.setPrice(800.50);

    cout << endl << CACCOM.calculatePorcentual(113, 318.153) << endl;
    cout << CACCOM.calculateCost() << endl;
    cout << CACCOM.calculateProfit(113, 318.153) << endl;
}
