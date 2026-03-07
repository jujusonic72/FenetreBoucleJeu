#include "CRendererComponent.h"
#include "CEntity.h"
#include "CComponent.h"
#include "SubSystems/CGameSubsystem.h"
#include "CComponentRegistry.h"
#include "imgui.h"

IMPLEMENT_REFLECTION(CRendererComponent, CComponent)

REGISTER_COMPONENT(CRendererComponent)

REGISTER_PROPERTY(CRendererComponent, ShapeType, EPropertyType::Int)
REGISTER_PROPERTY(CRendererComponent, R,         EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, G,         EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, B,         EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, A,         EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, Radius,    EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, Width,     EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, Height,    EPropertyType::Float)
REGISTER_PROPERTY(CRendererComponent, Filled,    EPropertyType::Bool)
REGISTER_PROPERTY(CRendererComponent, Thickness, EPropertyType::Float)

void CRendererComponent::Update(float dt)
{
    float posX = 0.0f;
    float posY = 0.0f;

    CEntity* owner = GetOwner();
    if (owner)
    {
        CTransformComponent* transform = owner->GetComponent<CTransformComponent>();
        if (transform)
        {
            posX = transform->PosX;
            posY = transform->PosY;
        }
    }

    ImU32 color = IM_COL32((int)R, (int)G, (int)B, (int)A);

    switch ((EShapeType)ShapeType)
    {
        case EShapeType::Circle:
            if (Filled)
                CGameSubsystem::DrawCircleFilled(posX, posY, Radius, color);
            else
                CGameSubsystem::DrawCircle(posX, posY, Radius, color, 32, Thickness);
            break;

        case EShapeType::Rect:
            if (Filled)
                CGameSubsystem::DrawRectFilled(posX - Width * 0.5f, posY - Height * 0.5f, Width, Height, color);
            else
                CGameSubsystem::DrawRect(posX - Width * 0.5f, posY - Height * 0.5f, Width, Height, color, Thickness);
            break;
    }
}
