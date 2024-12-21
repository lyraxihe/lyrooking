#pragma once
class PlayerInfo
{
public: //hacer getter y setter de los atributos
	bool recipeInventory[10] = { false, false, false, false, false };
	int ingredientsInventory[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int money = 100;

	void setMoney(int price); //negativo para restar, positivo para sumar
	void setNewRecipe(int recipeIndex);
	void setIngredients(int ingredientIndex, int amount); //negativo para restar, positivo para sumar
};

