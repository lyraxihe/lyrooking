#pragma once
class PlayerInfo
{
private:
	int money = 5000;
	bool recipeInventory[10] = { false, false, false, false, false };
	int ingredientsInventory[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

public:

	int getMoney();
	bool* getRecipeInventory() const;
	int* getIngredientsInventory() const;

	void setMoney(int price); //negativo para restar, positivo para sumar
	void setNewRecipe(int recipeIndex);
	void setIngredients(int ingredientIndex, int amount); //negativo para restar, positivo para sumar
};

