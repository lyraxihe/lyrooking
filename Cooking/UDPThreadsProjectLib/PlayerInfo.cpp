#include "pch.h"
#include "PlayerInfo.h"

int PlayerInfo::getMoney()
{
	return money;
}

bool* PlayerInfo::getRecipeInventory() const
{
	bool* copyRecipeInventory = new bool[10];
	for (int i = 0; i < 10; ++i)
	{
		copyRecipeInventory[i] = recipeInventory[i];
	}
	return copyRecipeInventory;

}

int* PlayerInfo::getIngredientsInventory() const
{
	int* copyIngredientsInventory = new int[10];
	for (int i = 0; i < 10; ++i)
	{
		copyIngredientsInventory[i] = ingredientsInventory[i];
	}
	return copyIngredientsInventory;
}

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
