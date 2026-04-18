#include "stdafx.h"
#include "ArmorStandRenderer.h"
#include "ArmorStandModel.h"
#include "ArmorStandArmorModel.h"
#include "HumanoidModel.h"
#include "ItemInHandLayer.h"
#include "CustomHeadLayer.h"
#include "Textures.h"
#include "HumanoidMobRenderer.h"
#include "../Minecraft.World/ArmorStand.h"
#include "../Minecraft.World/ArmorItem.h"

static const float DEG_TO_RAD = 3.14159265f / 180.0f;

ResourceLocation ArmorStandRenderer::LOC_ARMOR_STAND =
    ResourceLocation(TN_MOB_ARMORSTAND);



ArmorStandRenderer::ArmorStandArmorLayer::ArmorStandArmorLayer(
    LivingEntityRenderer* renderer)
    : HumanoidArmorLayer(renderer)
{
  
    delete armorModel1;
    delete armorModel2;
    armorModel1 = new ArmorStandArmorModel(0.5f);
    armorModel2 = new ArmorStandArmorModel(1.0f);
}

void ArmorStandRenderer::ArmorStandArmorLayer::createArmorModels()
{
    delete armorModel1;
    delete armorModel2;
    armorModel1 = new ArmorStandArmorModel(0.5f);
    armorModel2 = new ArmorStandArmorModel(1.0f);
}



ArmorStandRenderer::ArmorStandRenderer()
    : LivingEntityRenderer(new ArmorStandModel(0.0f), 0.0f)
{
    
    addLayer(new ArmorStandArmorLayer(this));

  
    addLayer(new ItemInHandLayer(this));

    
    ArmorStandModel* m = static_cast<ArmorStandModel*>(getModel());
    addLayer(new CustomHeadLayer(m->head));
}

ArmorStandRenderer::~ArmorStandRenderer()
{
    
}

ResourceLocation* ArmorStandRenderer::getTextureLocation(shared_ptr<Entity> entity)
{
    return &LOC_ARMOR_STAND;
}

bool ArmorStandRenderer::shouldShowName(shared_ptr<LivingEntity> entity)
{
    
    if (!entity) return false;
    return entity->isCustomNameVisible();
}

void ArmorStandRenderer::setupRotations(shared_ptr<LivingEntity> mob,
                                         float bob, float bodyRot, float a)
{
    
    glRotatef(180.0f - bodyRot, 0.0f, 1.0f, 0.0f);
}

void ArmorStandRenderer::render(shared_ptr<Entity> entity,
                                 double x, double y, double z,
                                 float rot, float a)
{
    LivingEntityRenderer::render(entity, x, y, z, rot, a);
}

void ArmorStandRenderer::renderModel(shared_ptr<LivingEntity> mob,
                                      float wp, float ws, float bob,
                                      float headRotMinusBodyRot,
                                      float headRotx, float scale)
{
    shared_ptr<ArmorStand> stand = dynamic_pointer_cast<ArmorStand>(mob);
    if (!stand) return;

    ArmorStandModel* m = static_cast<ArmorStandModel*>(getModel());
    if (!m) return;

   
    Rotations h  = stand->getHeadPose();
    Rotations b  = stand->getBodyPose();
    Rotations la = stand->getLeftArmPose();
    Rotations ra = stand->getRightArmPose();
    Rotations ll = stand->getLeftLegPose();
    Rotations rl = stand->getRightLegPose();

    
    m->setupPose(
        h.getX()  * DEG_TO_RAD,  h.getY()  * DEG_TO_RAD,  h.getZ()  * DEG_TO_RAD,
        b.getX()  * DEG_TO_RAD,  b.getY()  * DEG_TO_RAD,  b.getZ()  * DEG_TO_RAD,
        la.getX() * DEG_TO_RAD,  la.getY() * DEG_TO_RAD,  la.getZ() * DEG_TO_RAD,
        ra.getX() * DEG_TO_RAD,  ra.getY() * DEG_TO_RAD,  ra.getZ() * DEG_TO_RAD,
        ll.getX() * DEG_TO_RAD,  ll.getY() * DEG_TO_RAD,  ll.getZ() * DEG_TO_RAD,
        rl.getX() * DEG_TO_RAD,  rl.getY() * DEG_TO_RAD,  rl.getZ() * DEG_TO_RAD
    );

    
    ArmorStandArmorLayer* al = getArmorLayer();

    if (al)
    {
        auto applyPose = [&](HumanoidModel* am)
        {
            if (!am) return;

            
            am->head->xRot = h.getX() * DEG_TO_RAD;
            am->head->yRot = h.getY() * DEG_TO_RAD;
            am->head->zRot = h.getZ() * DEG_TO_RAD;
            if (am->hair) {
                am->hair->xRot = h.getX() * DEG_TO_RAD;
                am->hair->yRot = h.getY() * DEG_TO_RAD;
                am->hair->zRot = h.getZ() * DEG_TO_RAD;
            }

           
            am->body->xRot = b.getX() * DEG_TO_RAD;
            am->body->yRot = b.getY() * DEG_TO_RAD;
            am->body->zRot = b.getZ() * DEG_TO_RAD;

           
            am->arm1->xRot = la.getX() * DEG_TO_RAD;
            am->arm1->yRot = la.getY() * DEG_TO_RAD;
            am->arm1->zRot = la.getZ() * DEG_TO_RAD;

           
            am->arm0->xRot = ra.getX() * DEG_TO_RAD;
            am->arm0->yRot = ra.getY() * DEG_TO_RAD;
            am->arm0->zRot = ra.getZ() * DEG_TO_RAD;

           
            am->leg0->xRot = rl.getX() * DEG_TO_RAD;
            am->leg0->yRot = rl.getY() * DEG_TO_RAD;
            am->leg0->zRot = rl.getZ() * DEG_TO_RAD;

            
            am->leg1->xRot = ll.getX() * DEG_TO_RAD;
            am->leg1->yRot = ll.getY() * DEG_TO_RAD;
            am->leg1->zRot = ll.getZ() * DEG_TO_RAD;
        };

        applyPose(static_cast<HumanoidModel*>(al->armorModel1));
        applyPose(static_cast<HumanoidModel*>(al->armorModel2));
    }

    LivingEntityRenderer::renderModel(mob, wp, ws, bob,
                                        headRotMinusBodyRot, headRotx, scale);

    for (size_t i = 0; i < renderLayers.size(); ++i) {
        if (renderLayers[i]) {
            renderLayers[i]->render(mob, wp, ws, bob, headRotMinusBodyRot, headRotx, scale, true);
        }
    }
}

int ArmorStandRenderer::prepareArmor(shared_ptr<LivingEntity> mob, int layer, float a)
{
    if (!armorLayer) return -1;

    shared_ptr<ItemInstance> itemInstance = mob->getArmor(3 - layer);
    if (itemInstance != nullptr)
    {
        Item *item = itemInstance->getItem();
        ArmorItem *armorItem = dynamic_cast<ArmorItem *>(item);
        if (armorItem != nullptr)
        {
            bindTexture(HumanoidMobRenderer::getArmorLocation(armorItem, layer));

            HumanoidModel *am = armorLayer->getArmorModel(layer);
            
            am->head->visible = (layer == 0);
            if (am->hair) am->hair->visible = (layer == 0);
            am->body->visible = (layer == 1 || layer == 2);
            am->arm0->visible = (layer == 1);
            am->arm1->visible = (layer == 1);
            am->leg0->visible = (layer == 2 || layer == 3);
            am->leg1->visible = (layer == 2 || layer == 3);

            setArmor(am);
            am->attackTime = model->attackTime;
            am->riding = model->riding;
            am->young = mob->isBaby();

            if (armorItem->getMaterial() == ArmorItem::ArmorMaterial::CLOTH)
            {
                int color = armorItem->getColor(itemInstance);
                float red   = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
                float green = static_cast<float>((color >> 8)  & 0xFF) / 255.0f;
                float blue  = static_cast<float>(color & 0xFF)         / 255.0f;
                glColor3f(red, green, blue);

                if (itemInstance->isEnchanted()) return 0x1f;
                return 0x10;
            }

            glColor3f(1.0f, 1.0f, 1.0f);
            if (itemInstance->isEnchanted()) return 15;

            return 1;
        }
    }
    return -1;
}