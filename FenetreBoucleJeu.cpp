
#define SDL_MAIN_USE_CALLBACKS 0  /* use the main() instead of callbacks */

#include <SDL3/SDL.h>
#include "CGameEngine.h"
#include "Example.h"
#include <cstdio>
#include <vector>
#include <iostream>

//Fonction du devis pour prouver que ça fonctionne
void ExampleGameplayFunction(GObject* InMysteriousObjA, GObject* InMysteriousObjB)
{
    if (InMysteriousObjA->GetClass()->IsA(GPlayerState::StaticClass()))
    {
        printf("Object A is of a type GPlayerState or a type that inherits from GPlayerState.\n");
    }
    else
    {
        printf("Object A is NOT a GPlayerState.\n");
    }
}


int main(int argc, char* argv[])
{
    printf("=== Test 1: IsA() avec GPlayerState ===\n");
    GPlayerState playerState;
    GCat cat;
    ExampleGameplayFunction(&playerState, &cat);
    ExampleGameplayFunction(&cat, &playerState);

    printf("\n=== Test 2: Hierarchie GAnimal -> GCat / GDog ===\n");

    // Création d'un vecteur de GAnimal*
    std::vector<GAnimal*> animals;
    GCat cat1, cat2;
    GDog dog1;
    cat1.Name = "Minou";
    cat2.Name = "Felix";
    dog1.Name = "Rex";
    animals.push_back(&cat1);
    animals.push_back(&dog1);
    animals.push_back(&cat2);

    printf("\n--- Filtrage avec Cast<> ---\n");
    for (GAnimal* animal : animals)
    {
        if (GCat* c = Cast<GCat>(animal))
        {
            printf("Chat trouvé: %s (vies: %d)\n", c->Name, c->Lives);
        }
        if (GDog* d = Cast<GDog>(animal))
        {
            printf("Chien trouvé: %s (good boy: %s)\n", d->Name, d->GoodBoy ? "oui" : "non");
        }
    }

    printf("\n=== Test 3: IsA() dans la hierarchie ===\n");
    printf("GCat IsA GCat?    %s\n", cat1.GetClass()->IsA(GCat::StaticClass()) ? "oui" : "non");
    printf("GCat IsA GAnimal? %s\n", cat1.GetClass()->IsA(GAnimal::StaticClass()) ? "oui" : "non");
    printf("GCat IsA GObject? %s\n", cat1.GetClass()->IsA(GObject::StaticClass()) ? "oui" : "non");
    printf("GCat IsA GDog?    %s\n", cat1.GetClass()->IsA(GDog::StaticClass()) ? "oui" : "non");
    printf("GDog IsA GAnimal? %s\n", dog1.GetClass()->IsA(GAnimal::StaticClass()) ? "oui" : "non");

    printf("\n=== Test 4: GetClass()->GetName() ===\n");
    printf("cat1 class: %s\n", cat1.GetClass()->GetName());
    printf("dog1 class: %s\n", dog1.GetClass()->GetName());
    printf("playerState class: %s\n", playerState.GetClass()->GetName());


    std::cout << "1. Before GetInstance..." << std::endl;
    CGameEngine& engine = CGameEngine::GetInstance();
    
    std::cout << "2. After GetInstance, before Init..." << std::endl;
    if (!engine.Init())
    {
        std::cout << "Init failed!" << std::endl;
        return 1;
    }
    
    std::cout << "3. Init OK, entering loop..." << std::endl;
    engine.Loop();
    std::cout << "4. Exiting Game..." << std::endl;
    return 0;
}
