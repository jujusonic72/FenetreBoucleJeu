#pragma once

#include "GObject.h"
#include <string>
#include <vector>
#include "CComponentRegistry.h"

class CEntity;

/*
 * GScene
 * Classe de base concrète pour toutes les scènes du projet.
 *
 * Deux types de scènes coexistent :
 *   - Scènes compilées  : héritent de GScene, override GetSceneName(),
 *                         enregistrées via REGISTER_SCENE → instanciées par CSceneRegistry.
 *   - Scènes génériques : instanciées directement (GScene), nom setté via SetName(),
 *                         créées depuis l'UI du sélecteur de scènes.
 *
 * Sérialisation :
 *   Save() → Scenes/<nom>.json   (entités + composants via réflexion)
 *   Load() → reconstruit depuis le .json via CComponentRegistry
 */
class GScene : public GObject
{
public:
    GScene() = default;
    virtual ~GScene();

    // Nom de la scène — correspond au nom du fichier .json
    virtual const char* GetSceneName() const { return m_name.c_str(); }
    void                SetName(const std::string& name) { m_name = name; }

    // Chemin du fichier JSON
    std::string GetJsonPath() const;

    // Sérialisation
    bool Save() const;
    bool Load(GameAllocator* alloc);

    // Propage Update(dt) à toutes les entités de la scène.
    // Les sous-classes peuvent override et appeler GScene::Update(dt)
    // pour conserver la propagation automatique.
    void Update(float dt) override;

    // Accès aux entités
    const std::vector<CEntity*>& GetEntities() const { return m_entities; }
    std::vector<CEntity*>&       GetEntities()       { return m_entities; }

    // Libère toutes les entités et leurs composants via l'allocateur
    void ClearEntities(GameAllocator* alloc);

    static CClass* StaticClass();
    virtual const CClass* GetClass() const override;

protected:
    std::vector<CEntity*> m_entities;
    std::string           m_name;

private:
    static CClass s_Class;
};