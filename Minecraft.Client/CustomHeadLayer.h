#pragma once
#include "RenderLayer.h"

class ModelPart;
class LivingEntity;

class CustomHeadLayer : public RenderLayer {
public:
    
    ModelPart* headPart;

    explicit CustomHeadLayer(ModelPart* headPart);
    virtual ~CustomHeadLayer() {}

    virtual int  colorsOnDamage() override;   
    virtual void render(shared_ptr<LivingEntity> mob,
                        float wp, float ws, float bob,
                        float headRot, float headRotX,
                        float scale, bool useCompiled) override;
};