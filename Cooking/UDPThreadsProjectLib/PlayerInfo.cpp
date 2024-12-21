#include "pch.h"
#include "PlayerInfo.h"

void PlayerInfo::setMoney(int price)
{
	money += price;
}
void PlayerInfo::setNewRecipe(int recipeIndex)
{
	recipeInventory[recipeIndex] = true;
}

void PlayerInfo::setIngredients(int ingredientIndex, int amount)
{
	ingredientsInventory[ingredientIndex] += amount;
}
