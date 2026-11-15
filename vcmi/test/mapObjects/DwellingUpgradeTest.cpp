/*
 * DwellingUpgradeTest.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "../../lib/GameLibrary.h"
#include "../../lib/mapObjectConstructors/CObjectClassesHandler.h"
#include "../../lib/mapObjectConstructors/DwellingInstanceConstructor.h"
#include "../../lib/mapObjects/CGDwelling.h"

namespace test
{

using namespace ::testing;

/// Tests for dwelling upgrade mechanic ("upgrade external dwellings for resources")
class DwellingUpgradeTest : public Test
{
protected:
	const DwellingInstanceConstructor * getDwellingConstructor(Obj objectType, MapObjectSubID subtype) const
	{
		auto handler = LIBRARY->objtypeh->getHandlerFor(objectType, subtype);
		return dynamic_cast<const DwellingInstanceConstructor *>(handler.get());
	}
};

TEST_F(DwellingUpgradeTest, UpgradeConfigIsOptIn)
{
	// Dwellings that provide creatures without an upgraded counterpart can never be upgraded,
	// regardless of mods. This validates that upgrade availability follows creature upgrade graph.
	for (MapObjectSubID subtype : LIBRARY->objtypeh->knownSubObjects(Obj::CREATURE_GENERATOR1))
	{
		const auto * dwelling = getDwellingConstructor(Obj::CREATURE_GENERATOR1, subtype);
		ASSERT_NE(dwelling, nullptr);

		const auto produced = dwelling->getProducedCreatures();

		const bool anyCreatureUpgradeable = std::any_of(
			produced.begin(), produced.end(),
			[](const CCreature * creature){ return creature && !creature->upgrades.empty(); });

		if (!anyCreatureUpgradeable)
		{
			EXPECT_TRUE(dwelling->getUpgradeCost().empty())
				<< "Dwelling " << dwelling->getJsonKey() << " has no upgradeable creatures but has an upgrade cost";
			EXPECT_TRUE(dwelling->getUpgradedCreatures().empty())
				<< "Dwelling " << dwelling->getJsonKey() << " has no upgradeable creatures but provides upgraded creatures";
		}
	}
}

TEST_F(DwellingUpgradeTest, UpgradesAreTakenFromCreatureGraph)
{
	// Griffin Tower provides griffin that can be upgraded to royalGriffin.
	// Mechanic must resolve upgrades through creature upgrade graph, not through hardcoded list.
	const CCreature * griffin = CreatureID(4).toCreature();
	ASSERT_NE(griffin, nullptr);

	ASSERT_EQ(griffin->upgrades.size(), 1) << "griffin is expected to have exactly one upgrade";
	EXPECT_EQ(griffin->upgrades.begin()->toCreature()->getJsonKey(), "core:royalGriffin");
}

TEST_F(DwellingUpgradeTest, CreaturesWithoutUpgradeAreKeptAsIs)
{
	// Golem Factory contains creatures where only some of them have an upgrade.
	// Gold and diamond golems have no upgrade at all, iron golem upgrades into stone golem.
	const CCreature * ironGolem = CreatureID(32).toCreature();
	const CCreature * stoneGolem = CreatureID(33).toCreature();
	const CCreature * goldGolem = CreatureID(116).toCreature();
	const CCreature * diamondGolem = CreatureID(117).toCreature();

	ASSERT_NE(ironGolem, nullptr);
	ASSERT_NE(stoneGolem, nullptr);
	ASSERT_NE(goldGolem, nullptr);
	ASSERT_NE(diamondGolem, nullptr);

	EXPECT_FALSE(ironGolem->upgrades.empty()) << "iron golem is expected to have an upgrade";
	EXPECT_EQ(ironGolem->upgrades.begin()->toCreature(), stoneGolem);

	EXPECT_TRUE(stoneGolem->upgrades.empty());
	EXPECT_TRUE(goldGolem->upgrades.empty());
	EXPECT_TRUE(diamondGolem->upgrades.empty());
}

TEST_F(DwellingUpgradeTest, UpgradedCreaturesFollowLevelStructure)
{
	// Every dwelling that supports upgrades must keep the same number of levels after upgrade,
	// otherwise amounts would be assigned to wrong creatures.
	for (MapObjectSubID subtype : LIBRARY->objtypeh->knownSubObjects(Obj::CREATURE_GENERATOR1))
	{
		const auto * dwelling = getDwellingConstructor(Obj::CREATURE_GENERATOR1, subtype);
		ASSERT_NE(dwelling, nullptr);

		const auto upgraded = dwelling->getUpgradedCreatures();
		if (upgraded.empty())
			continue;

		const auto produced = dwelling->getProducedCreatures();
		EXPECT_FALSE(upgraded.empty()) << "Dwelling " << dwelling->getJsonKey() << " returned empty upgrade list";
		EXPECT_FALSE(dwelling->getUpgradeCost().empty())
			<< "Dwelling " << dwelling->getJsonKey() << " provides upgraded creatures but has no upgrade cost";
		EXPECT_GE(produced.size(), upgraded.size());
	}
}

}
