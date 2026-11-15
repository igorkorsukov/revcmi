# Dwelling

```json
{
	/// List of creatures in this bank. Each list represents one "level" of bank
	/// Creatures on the same level will have shared growth and available number (similar to towns)
	/// Note that due to GUI limitation it is not recommended to have more than 4 creatures at once
	"creatures" : [ 
		[ "airElemental", "stormElemental" ],
		[ "waterElemental" ]
	],
	
	/// If set to true, this dwelling will not be selected as a replacement for random dwelling on map
	/// Such dwellings have no restrictions on which tiles are visitable or blocked
	/// For dwelling to be usable as a replacement, it must follow some additional restrictions (see below)
	"bannedForRandomDwelling" : true,

	/// List of guards for this dwelling. Can have two possible values:
	/// Boolean true/false - If set to "true", guards will be generated using H3 formula:
	/// 3 week growth of first available creatures
	/// List of objects - custom guards, each entry represent one stack in defender army
	"guards" : true,
	"guards" : [
		{ "amount" : 12, "type" : "earthElemental" }
	],

	/// Image showed on kingdom overview (animation; only frame 0 displayed)
	"kingdomOverviewImage" : "image.def",

	/// Optional upgrade of this dwelling. If defined, owning player can upgrade dwelling
	/// by right-clicking it on adventure map, which will make dwelling produce upgraded creatures
	/// Upgraded creatures are taken from creature upgrade graph, e.g. griffin -> royalGriffin
	/// Creatures that have no upgrade will be kept as is
	"upgrade" : {
		/// Resources required to upgrade this dwelling. Can use any resource, including modded ones
		"cost" : { "mithril" : 5 }
	}
}
```

## Replacement of random dwellings

Existing maps may contain random dwellings that will be replaced with concrete dwellings on map loading.

For dwelling to be a valid replacement for such random dwelling it must be:

- block at most 2x2 tile square
- one tile in bottom row must be visitable, and another - blocked

Visible tiles (`V` in map object template mask) don't have any restrictions and can have any layout

It is possible to make dwellings that don't fulfill this requirements, however such dwellings should only be used for custom maps or random maps. Mod that adds a new faction need to also provide a set of valid dwellings that can be used for replacement of random dwellings.

Examples of valid dwellings:

- minimal - bottom row contains one blocked and one visitable tile, second row fully passable

  ```json
  "mask":[
      "AB"
  ],
  ```

- maximal - bottom row contains one blocked and one visitable tile, both tiles on second row are blocked

  ```json
  "mask":[
      "BB"
      "BA"
  ],
  ```

- extended visual - similar to maximal, but right-most column is fully passable. Note that blocked tiles still fit into 2x2 square

  ```json
  "mask":[
      "BBV"
      "BAV"
  ],
  ```

## Dwelling upgrades

Dwellings can be upgraded by their owner for a configurable amount of resources. This allows mods to implement mechanics such as "upgrade external dwelling for mithril".

To make a dwelling upgradeable, provide an `upgrade` section with a `cost` object:

```json
{
	"griffinTower" : {
		"index" : 25,
		"creatures" : [["griffin"]],
		"upgrade" : {
			"cost" : { "mithril" : 5 }
		}
	}
}
```

Rules of this mechanic:

- Upgraded creatures are resolved from the creature upgrade graph, so `griffin` becomes `royalGriffin`
- If a dwelling provides several creatures on one level, all of them are upgraded
- Creatures without an upgrade are kept as is, so dwellings like Golem Factory can also be upgraded
- Any resource can be used in `cost`, including resources added by mods
- Upgraded creatures share available amount with the base creatures - upgrading does not give extra units
- Upgrading is permanent for the specific dwelling instance on the map and is stored in saved games
- Only the owner of the dwelling can upgrade it, by right-clicking the dwelling on adventure map
- The dwelling must have at least one creature that can actually be upgraded, otherwise `upgrade` has no effect
