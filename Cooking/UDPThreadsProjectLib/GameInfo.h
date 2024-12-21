#pragma once
#include <string>

struct Object
{
    std::string name;
    int price;
    int id;
};

struct Food
{
    std::string foodName;
    int price;
    Object ingredient_1;
    Object ingredient_2;
    int id;
};

class GameInfo
{ 
private:
    Object recipeList[10];
    Object ingredientsList[10];
    Food foodList[10];

public:
    GameInfo()
    {

        // lista de recetas
        recipeList[0].id = 0;
        recipeList[0].name = "fideos";
        recipeList[0].price = 25;

        recipeList[1].id = 1;
        recipeList[1].name = "sushi";
        recipeList[1].price = 45;

        recipeList[2].id = 2;
        recipeList[2].name = "cinnamon Roll";
        recipeList[2].price = 10;

        recipeList[3].id = 3;
        recipeList[3].name = "batido de fresa";
        recipeList[3].price = 15;

        recipeList[4].id = 4;
        recipeList[4].name = "panettone";
        recipeList[4].price = 30;

        recipeList[5].id = 5;
        recipeList[5].name = "poke";
        recipeList[5].price = 35;

        recipeList[6].id = 6;
        recipeList[6].name = "tarta de fresa";
        recipeList[6].price = 20;

        recipeList[7].id = 7;
        recipeList[7].name = "tarta de queso";
        recipeList[7].price = 20;

        recipeList[8].id = 8;
        recipeList[8].name = "pizza";
        recipeList[8].price = 35;

        recipeList[9].id = 9;
        recipeList[9].name = "tortilla";
        recipeList[9].price = 15;


        // lista de ingredientes
        ingredientsList[0].id = 0;
        ingredientsList[0].name = "huevo";
        ingredientsList[0].price = 10;

        ingredientsList[1].id = 1;
        ingredientsList[1].name = "canela";
        ingredientsList[1].price = 5;

        ingredientsList[2].id = 2;
        ingredientsList[2].name = "fresa";
        ingredientsList[2].price = 15;

        ingredientsList[3].id = 3;
        ingredientsList[3].name = "arroz";
        ingredientsList[3].price = 10;

        ingredientsList[4].id = 4;
        ingredientsList[4].name = "salmon";
        ingredientsList[4].price = 25;

        ingredientsList[5].id = 5;
        ingredientsList[5].name = "harina";
        ingredientsList[5].price = 15;

        ingredientsList[6].id = 6;
        ingredientsList[6].name = "queso";
        ingredientsList[6].price = 15;

        ingredientsList[7].id = 7;
        ingredientsList[7].name = "papa";
        ingredientsList[7].price = 20;

        ingredientsList[8].id = 8;
        ingredientsList[8].name = "chips de chocolate";
        ingredientsList[8].price = 15;

        ingredientsList[9].id = 9;
        ingredientsList[9].name = "leche";
        ingredientsList[9].price = 10;

        //lista de comidas
        foodList[0].foodName = recipeList[0].name;     //fideos
        foodList[0].id = recipeList[0].id;     
        foodList[0].price = recipeList[0].price * 2;
        foodList[0].ingredient_1 = ingredientsList[5]; //harina
        foodList[0].ingredient_2 = ingredientsList[0]; //huevo

        foodList[1].foodName = recipeList[1].name;     //sushi
        foodList[1].id = recipeList[1].id;     
        foodList[1].price = recipeList[1].price * 2;
        foodList[1].ingredient_1 = ingredientsList[3]; //arroz
        foodList[1].ingredient_2 = ingredientsList[4]; //salmon

        foodList[2].foodName = recipeList[2].name;     //cinnamon roll
        foodList[2].id = recipeList[2].id;     
        foodList[2].price = recipeList[2].price * 2;
        foodList[2].ingredient_1 = ingredientsList[1]; //canela
        foodList[2].ingredient_2 = ingredientsList[5]; //harina

        foodList[3].foodName = recipeList[3].name;     //batido de fresa
        foodList[3].id = recipeList[3].id;     
        foodList[3].price = recipeList[3].price * 2;
        foodList[3].ingredient_1 = ingredientsList[2]; //fresa
        foodList[3].ingredient_2 = ingredientsList[9]; //leche

        foodList[4].foodName = recipeList[4].name;     //panettone
        foodList[4].id = recipeList[4].id;     
        foodList[4].price = recipeList[4].price * 2;
        foodList[4].ingredient_1 = ingredientsList[5]; //harina
        foodList[4].ingredient_2 = ingredientsList[8]; //chips choco

        foodList[5].foodName = recipeList[5].name;     //poke
        foodList[5].id = recipeList[5].id;    
        foodList[5].price = recipeList[5].price * 2;
        foodList[5].ingredient_1 = ingredientsList[3]; //arroz
        foodList[5].ingredient_2 = ingredientsList[4]; //salmon

        foodList[6].foodName = recipeList[6].name;     //tarta fresa
        foodList[6].id = recipeList[6].id;     
        foodList[6].price = recipeList[6].price * 2;
        foodList[6].ingredient_1 = ingredientsList[2]; //fresa
        foodList[6].ingredient_2 = ingredientsList[5]; //harina

        foodList[7].foodName = recipeList[7].name;     //tarta queso
        foodList[7].id = recipeList[7].id;    
        foodList[7].price = recipeList[7].price * 2;
        foodList[7].ingredient_1 = ingredientsList[0]; //huevo
        foodList[7].ingredient_2 = ingredientsList[6]; //queso

        foodList[8].foodName = recipeList[8].name;     //pizza
        foodList[8].id = recipeList[8].id;    
        foodList[8].price = recipeList[8].price * 2;
        foodList[8].ingredient_1 = ingredientsList[5]; //harina
        foodList[8].ingredient_2 = ingredientsList[6]; //queso

        foodList[9].foodName = recipeList[9].name;     //tortilla
        foodList[9].id = recipeList[9].id;   
        foodList[9].price = recipeList[9].price * 2;
        foodList[9].ingredient_1 = ingredientsList[0]; //huevo
        foodList[9].ingredient_2 = ingredientsList[7]; //papa
    }
    Object* getRecipeList() const;
    Object* getIngredientsList() const;
    Food* getFoodList() const;

    void showRecipeStore();
    void showIngredientsStore();
    void showRecipeInventory(bool recipeInventory[10]);
    void showIngredientsInventory(int ingredientsInventory[10]);
};

