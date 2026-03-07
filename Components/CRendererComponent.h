#pragma once

#include "CComponent.h"
#include "CTransformComponent.h"

/*
 * EShapeType : type de primitive à dessiner
 */
enum class EShapeType : int
{
    Circle = 0,
    Rect   = 1
};

/*
 * CRendererComponent
 * Se dessine lui-même dans Update() via l'API de CGameSubsystem.
 * Va chercher la position dans le CTransformComponent de son entité parente.
 *
 * Propriétés exposées à la réflexion / éditeur :
 *   ShapeType  : 0 = Circle, 1 = Rect
 *   R, G, B, A : couleur RGBA (0-255)
 *   Radius     : rayon (si Circle)
 *   Width      : largeur (si Rect)
 *   Height     : hauteur (si Rect)
 *   Filled     : rempli ou contour seulement
 *   Thickness  : épaisseur du contour (si !Filled)
 */
class CRendererComponent : public CComponent
{
    GENERATE_REFLECTION
public:
    // Type de forme
    int ShapeType = (int)EShapeType::Circle;

    // Couleur
    float R = 255.0f;
    float G = 255.0f;
    float B = 255.0f;
    float A = 255.0f;

    // Paramètres cercle
    float Radius = 20.0f;

    // Paramètres rect
    float Width  = 40.0f;
    float Height = 40.0f;

    // Rendu
    bool  Filled    = true;
    float Thickness = 1.0f;

    // Appelé chaque frame par CEntity::Update()
    void Update(float dt) override;
};
