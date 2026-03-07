#pragma once

#include "GScene.h"

/*
 * TP2
 * Level script genere automatiquement.
 * Ajoutez REGISTER_SCENE(TP2) dans le .cpp pour
 * que la scene apparaisse dans le selecteur apres recompilation.
 */
class TP2 : public GScene
{
    GENERATE_REFLECTION

public:
    const char* GetSceneName() const override { return "TP2"; }

    void Start()          override;
    void Update(float dt) override;
};
