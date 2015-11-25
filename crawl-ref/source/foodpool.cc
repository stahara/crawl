/**
 * @file
 * @brief Food pool implementation.
**/

#include <algorithm>

#include "foodpool.h"
#include "player.h"


int foodpool::perm_food_qty()
{
    return perm_food;
}

int foodpool::temp_food_qty()
{
    int qty = 0;

    for (auto t : temp_food)
        qty += t.amount;

    return qty;
}

int foodpool::total_food_qty()
{
    return perm_food_qty() + temp_food_qty();
}

void foodpool::add_permfood(int amount)
{
    perm_food += amount;
}

void foodpool::add_tempfood(int amount, unsigned duration)
{
    tfood new_tf = {amount, you.elapsed_time + duration};
    /* Keep the vector sorted by decreasing expiration date, so that the
     * next 'chunk' to rot or be eaten can simply be popped off the back. */
    temp_food.insert(find_if(temp_food.begin(), temp_food.end(),
                             [&new_tf](tfood tf) { return new_tf.expires > tf.expires; }),
                     new_tf);
}

/* The del_* functions all return how much of 'amount'
 * was not able to be taken from the relevant pool. */
int foodpool::del_tempfood(int amount)
{
    while (amount > 0 && !temp_food.empty())
    {
        auto d = min(amount, temp_food.back().amount);
        temp_food.back().amount -= d;
        amount -= d;
        if (temp_food.back().amount == 0)
            temp_food.pop_back();
    }

    return amount;
}

int foodpool::del_permfood(int amount)
{
    auto d = max(0, min(amount, perm_food));
    perm_food -= d;
    return amount - d;
}

int foodpool::del_food(int amount, foodpool_select pool)
{
    if (pool == PERM_FOOD_ONLY || pool == PERM_FOOD_PREF)
        amount = del_permfood(amount);

    if (pool == PERM_FOOD_ONLY || amount <= 0)
        return amount;

    if (pool == TEMP_FOOD_ONLY || pool == TEMP_FOOD_PREF)
        amount = del_tempfood(amount);

    if (pool == TEMP_FOOD_ONLY || amount <= 0)
        return amount;

    /* One of these (the preferred) will be a no-op. */
    amount = del_permfood(amount);
    amount = del_tempfood(amount);

    return amount;
}
