#include "pch.h"
#include "GameInfo.h"
#include <iostream>

void GameInfo::showRecipeStore()
{
    std::cout << std::endl <<"Recipe Store:\n";
    std::cout << "ID\tName\t\t\tPrice\n";
    std::cout << "----------------------------------------\n";
    for (const auto& recipe : recipeList) {
        std::string name = recipe.name;
        recipe.name; 
        name.resize(24, ' ');
        std::cout << recipe.id << "       " << name << recipe.price << " coins\n";
    }
}

void GameInfo::showIngredientsStore()
{
    std::cout << std::endl << "ingredients Store:\n";
    std::cout << "ID\tName\t\t\tPrice\n";
    std::cout << "----------------------------------------\n";
    for (const auto& ingredient : ingredientsList) {
        std::string name = ingredient.name;
        ingredient.name;
        name.resize(24, ' ');
        std::cout << ingredient.id << "       " << name << ingredient.price << " coins\n";
    }
}

void GameInfo::showRecipeInventory(bool recipeInventory[10])
{
    std::cout << std::endl << "Recipe inventory:\n";
    std::cout << "----------------------------------------\n";
    for (int i = 0; i < 10; ++i)
    {
        if (recipeInventory[i])
            std::cout << recipeList[i].name << "\n";
    }
}

void GameInfo::showIngredientsInventory(int ingredientsInventory[10])
{
    std::cout << std::endl << "Ingredients inventory:\n";
    std::cout << "----------------------------------------\n";
    for (int i = 0; i < 10; ++i)
    {
        std::cout << ingredientsList[i].name << ": " << ingredientsInventory[i] << "unidades" << "\n";
    }
}