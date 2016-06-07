#include <iostream>
#include <random>
#include <math.h>

#define WATER_COST 50
#define ENERGY_COST 80
#define SALARY_COST 800

#define MAX_FUNC_SMALL 15
#define MAX_FUNC_MEDIUM 30
#define MAX_FUNC_LARGE 60

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
    medium = 3000,
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
                      Stock(initialStock),
                      cash(initialCash);

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

        bool dcmp(float x, float y, float precision) {
            if (x == y) return true;
            if (fabs(x-y) <= precision) return true;
            return false;
        }

        long mapRange(long x, long in_min, long in_max, long out_min, long out_max) {
              return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        float distanceTwoPoints(float equX, float equY, float empX, float empY) {
            return sqrt( pow(equX - empX,2) + pow(equY - empY,2) );            
        }

        float investmentReturn(investment INV) {
            if (INV == none) return 0.0f;
            if (INV == low) return 0.05f;
            if (INV == medium) return 0.1f;
            if (INV == high) return 0.15f;
        }

        float calculatePorcentual(int equX, float equY) {
            float aux = BASE_SELL_PORC;

            aux += investmentReturn(marketResearch);
            aux += investmentReturn(propaganda);
            aux += investmentReturn(training);

            //Adjust by demand and offer
            float distance = distanceTwoPoints(equX, equY, stock, productPrice);
            float value = mapRange(distance, 0, 2000, 45, 0); //verificar limites 0-2000

            aux += value/100;

            return aux;
        }

        float calculateProfit() {
            int grossProfit = 0;
            float sellChance = calculatePorcentual();
            for (int i=0; i<stock; i++) 
                if (real_rand() <= sellChance) grossProfit += productPrice;
            
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

        void setStock() {};
        
}

class Node {
    private:

}
