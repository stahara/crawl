/**
 * @file
 * @brief Food pool implementation.
**/

#ifndef  FOODPOOL_H
#define  FOODPOOL_H


#include <vector>

using namespace std;


#define TEMPFOOD_DEFAULT_DURATION 100

enum foodpool_select
{
    TEMP_FOOD_PREF,  // Act on temporary food first, then permanent food.
    PERM_FOOD_PREF,  // Act on permanent food first, then temporary food.
    TEMP_FOOD_ONLY,  // Act only on temporary food.
    PERM_FOOD_ONLY,  // Act only on permanent food.
};

class foodpool
{
private:
    struct tfood
    {
        int amount;
        int expires;
    };

    vector<tfood> temp_food;
    int perm_food;

    int del_tempfood(int amount);
    int del_permfood(int amount);

public:
    int perm_food_qty();
    int temp_food_qty();
    int total_food_qty();

    void add_permfood(int amount);
    void add_tempfood(int amount, unsigned duration = TEMPFOOD_DEFAULT_DURATION);
    int del_food(int amount, foodpool_select pool = TEMP_FOOD_PREF);
};


#endif
