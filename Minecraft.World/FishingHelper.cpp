
#include "../Minecraft.World/WeighedTreasure.h"
#include "../Minecraft.World/Biome.h"
#include "../Minecraft.World/FishingHelper.h"
#include "../Minecraft.World/ItemInstance.h"
#include "../Minecraft.World/EnchantmentHelper.h"
#include "net.minecraft.world.item.h"

#include <memory>

FishingHelper* FishingHelper::getInstance()
{
	static FishingHelper instance;
	return &instance;
}

FishingHelper::FishingHelper() : catchTypeArray(3), fishingFishArray(4), fishingJunkArray(11), fishingTreasuresArray(5)
{
	// Source: https://github.com/WangTingZheng/mcp940/tree/master/src/minecraft/assets/minecraft/loot_tables/gameplay
	catchTypeArray[0] = new CatchTypeWeighedItem(CatchType::FISH, 8); // 850
	catchTypeArray[1] = new CatchTypeWeighedItem(CatchType::JUNK, 100 );
	catchTypeArray[2] = new CatchTypeWeighedItem(CatchType::TREASURE, 5 ); // 50

	fishingTreasuresArray[0] = new CatchWeighedItem(Item::bow_Id, 1, 0, 1);
	fishingTreasuresArray[1] = new CatchWeighedItem(Item::book_Id, 1, 0, 1);
	fishingTreasuresArray[2] = new CatchWeighedItem(Item::fishingRod_Id, 1, 0, 1);
	fishingTreasuresArray[3] = new CatchWeighedItem(Item::nameTag_Id, 1, 0, 1);
	fishingTreasuresArray[4] = new CatchWeighedItem(Item::saddle_Id, 1, 0, 1);
	fishingTreasuresArray[4] = new CatchWeighedItem(Tile::waterLily_Id, 1, 0, 1);

	fishingFishArray[0] = new CatchWeighedItem(Item::fish_raw_Id, 1, 0, 60); // Fish
	fishingFishArray[1] = new CatchWeighedItem(Item::fish_raw_Id, 1, 1, 25); // Salmon
	fishingFishArray[2] = new CatchWeighedItem(Item::fish_raw_Id, 1, 2, 2); // Clownfish
	fishingFishArray[3] = new CatchWeighedItem(Item::fish_raw_Id, 1, 3, 13); // Pufferfish

	fishingJunkArray[0] = new CatchWeighedItem(Item::leather_Id, 1, 0, 10);
	fishingJunkArray[1] = new CatchWeighedItem(Item::bone_Id, 1, 0, 10);
	fishingJunkArray[2] = new CatchWeighedItem(Item::potion_Id, 1, 0, 10); // Water bottle
	fishingJunkArray[3] = new CatchWeighedItem(Item::bowl_Id, 1, 0, 10);
	fishingJunkArray[4] = new CatchWeighedItem(Item::boots_leather_Id, 1, 0, 10);
	fishingJunkArray[5] = new CatchWeighedItem(Item::rotten_flesh_Id, 1, 0, 10);
	fishingJunkArray[6] = new CatchWeighedItem(Tile::tripWireSource_Id, 1, 0, 10);
	fishingJunkArray[7] = new CatchWeighedItem(Item::stick_Id, 1, 0, 5);
	fishingJunkArray[8] = new CatchWeighedItem(Item::string_Id, 1, 0, 5);
	fishingJunkArray[9] = new CatchWeighedItem(Item::fishingRod_Id, 1, 0, 2);
	fishingJunkArray[10] = new CatchWeighedItem(Item::dye_powder_Id, 10, 0, 1); // 10 ink sacs
}

CatchType FishingHelper::getRandCatchType(int fishMod, int junkMod, int treasureMod, Random* random)
{
	CatchTypeWeighedItem* catchTypeWeighedItem = nullptr;
	((CatchTypeWeighedItem*)catchTypeArray[0])->modWeight(fishMod);
	((CatchTypeWeighedItem*)catchTypeArray[1])->modWeight(junkMod);
	((CatchTypeWeighedItem*)catchTypeArray[2])->modWeight(treasureMod);
    // Recalculate the weights based on the luck level of the player
	catchTypeWeighedItem = static_cast<CatchTypeWeighedItem *>(WeighedRandom::getRandomItem(random, catchTypeArray));

	return catchTypeWeighedItem->getType();
}

CatchWeighedItem* FishingHelper::getRandCatch(CatchType catchType, Random* random)
{
	switch (catchType) {
		case CatchType::FISH:
			return static_cast<CatchWeighedItem *>(WeighedRandom::getRandomItem(random, fishingFishArray));
		case CatchType::TREASURE:
			return static_cast<CatchWeighedItem *>(WeighedRandom::getRandomItem(random, fishingTreasuresArray));
		case CatchType::JUNK:
			return static_cast<CatchWeighedItem *>(WeighedRandom::getRandomItem(random, fishingJunkArray));
	}
}

std::shared_ptr<ItemInstance> FishingHelper::handleCatch(CatchWeighedItem* weighedCatch, CatchType catchType, Random* random)
{	
	std::shared_ptr<ItemInstance> itemInstance = std::make_shared<ItemInstance>(
		weighedCatch->getItemId(), weighedCatch->getCount(), weighedCatch->getAuxValue()
	);
	
	if ((itemInstance->id == Item::fishingRod_Id && catchType == CatchType::JUNK) || (itemInstance->id == Item::boots_leather_Id)) {
		itemInstance->setAuxValue((int) (itemInstance->getMaxDamage() * ((double) random->nextInt(901) + 100.0) / 1000.0)); // 10% to 100% damage 
	}
	else if (itemInstance->id == Item::fishingRod_Id && catchType == CatchType::TREASURE) {
		itemInstance->setAuxValue((int)(itemInstance->getMaxDamage() * ((double)random->nextInt(251) / 1000.0))); // 0% to 25% damage
		itemInstance = EnchantmentHelper::enchantItem(random, itemInstance, 30);
	}
	else if (itemInstance->id == Item::bow_Id) {
		itemInstance->setAuxValue((int)(itemInstance->getMaxDamage() * ((double)random->nextInt(251) / 1000.0))); // 0% to 25% damage
		itemInstance = EnchantmentHelper::enchantItem(random, itemInstance, 30);
	}
	else if (itemInstance->id == Item::book_Id) {
		itemInstance = EnchantmentHelper::enchantItem(random, itemInstance, 30);
	}

	return itemInstance;
}

std::shared_ptr<ItemInstance> FishingHelper::getCatch(int fishMod, int junkMod, int treasureMod, Random* random)
{
	CatchType catchType = getRandCatchType(fishMod, junkMod, treasureMod, random);
	CatchWeighedItem* catchWeighedItem = getRandCatch(catchType, random);
	return handleCatch(catchWeighedItem, catchType, random);
}