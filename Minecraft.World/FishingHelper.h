#pragma once

#include "../Minecraft.World/WeighedTreasure.h"
#include "../Minecraft.World/ItemInstance.h"
#include "net.minecraft.world.item.h"
#include <unordered_map>
#include <memory>

enum CatchType {
  FISH,
  TREASURE,
  JUNK
};

class CatchTypeWeighedItem : public WeighedRandomItem {
	protected:
		CatchType type;
		int quality;
		int weight;

	public:
		CatchTypeWeighedItem(CatchType type, int weight) : WeighedRandomItem(weight)
		{
			this->type = type;
			this->weight = weight;
		}

		CatchType getType()
		{
			return type;
		}

		void modWeight(int mod) {
			this->randomWeight = this->weight + mod;
		}

};

class CatchWeighedItem : public WeighedRandomItem {
	protected:
		int itemId;
		int count;
		int auxValue;

	public:
		CatchWeighedItem(int itemId, int count, int auxValue, int weight) : WeighedRandomItem(weight)
		{
			this->itemId = itemId;
			this->count = count;
			this->auxValue = auxValue;
		}
		int getItemId()
		{
			return this->itemId;
		}
		int getCount()
		{
			return this->count;
		}
		int getAuxValue()
		{
			return this->auxValue;
		}
};

class FishingHelper
{
	private:
		FishingHelper();

		WeighedRandomItemArray catchTypeArray;

		WeighedRandomItemArray fishingFishArray;
		WeighedRandomItemArray fishingJunkArray;
		WeighedRandomItemArray fishingTreasuresArray;

		CatchType getRandCatchType(int level, Random* random);
		CatchWeighedItem* getRandCatch(CatchType catchType, Random* random);
		std::shared_ptr<ItemInstance> handleCatch(CatchWeighedItem* weighedCatch, CatchType catchType, Random* random);
		CatchType getRandCatchType(int fishMod, int junkMod, int treasureMod, Random* random);
	public:
		// Setup singleton
		FishingHelper(const FishingHelper&) = delete;
		FishingHelper& operator=(const FishingHelper&) = delete;
		static FishingHelper* getInstance();
		std::shared_ptr<ItemInstance> getCatch(int fishMod, int junkMod, int treasureMod, Random* random);
};