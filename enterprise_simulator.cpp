#include <iostream>
#include <random>

#define WATER_COST 50
#define ENERGY_COST 80

#define MAX_FUNC_SMALL 15
#define MAX_FUNC_MEDIUM 30
#define MAX_FUNC_LARGE 60

using namespace std;

enum building {
    small,
    medium,
    large
};

enum investment {
    none = 0,
    low = 1500,
    medium = 3000,
    high = 6000
};

class Enterprise {
    private:
        investment marketResearch;  //add chance
        investment propaganda;      //add chance
        investment training;        //add chance

        //investment feedStock;
        //investment production;

        building typeOfBuilding;    //Maximum employees
        int numberOfEmployees;
        int stock;

        float cash;

    public:
        Enterprise(building size, int initialEmployees, int initialStock, int initialCash): 
                      marketResearch(investment::none),
                      propaganda(investment::none),
                      training(investment::none),
                      typeOfBuilding(size),
                      numberOfEmployees(initialEmployees),
                      Stock(initialStock),
                      cash(initialCash);

        float calculateCost() {
            int cost = 0;

            if (typeOfBuilding == building::small) cost += 2500;
            if (typeOfBuilding == building::medium) cost += 3500;
            if (typeOfBuilding == building::large) cost += 5000;

            cost += numberOfEmployees*WATER_COST;
            cost += numberOfEmployess*ENERGY_COST;

            cost += marketResearch;
            cost += propaganda;
            cost += training;

            
        };

        float calculateProfit() {
            int grossProfit = 0;
            for (int i=0; i<stock; i++) {
                //gerar numero aleatório e verificar venda
            }
            
            return grossProfit - calculateCost();
        };

        void addEmployee() {
            if (typeOfBuilding == building::small &&
                    numberOfEmployees < MAX_FUNC_SMALL) numberOfEmployees++;

            if (typeOfBuilding == building::medium &&
                    numberOfEmployees < MAX_FUNC_MEDIUM) numberOfEmployees++;

            if (typeOfBuilding == building::large &&
                    numberOfEmployees < MAX_FUNC_LARGE) numberOfEmployees++;
        };

        void buyStock() {
            
        };




        
}

class Node {

}
