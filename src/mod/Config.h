#pragma once
#include <string>
#include <unordered_map>

namespace my_mod {

	struct destroyItem {
		std::string dropitem;
		int count;
	};

	struct Config {
		int version = 1;
		bool instant_destruction = false;
		bool stay_in_the_Dispenser = true;
		bool consume_durable = true;
		std::unordered_map<std::string, std::unordered_map<std::string, destroyItem>> destroy = {
			{"minecraft:iron_pickaxe",{
				{"minecraft:stone",
					{"minecraft:cobblestone",1}},
					{"minecraft:cobblestone",{"",1}}
			}},
			{"minecraft:iron_axe",{
				{"minecraft:oak_log",{"",1}},
				{"minecraft:birch_log",{"",1}},
				{"minecraft:jungle_log",{"",1}},
				{"minecraft:spruce_log",{"",1}},
				{"minecraft:dark_oak_log",{"",1}},
				{"minecraft:acacia_log",{"",1}},
				{"minecraft:cherry_log",{"",1}},
				{"minecraft:crimson_stem",{"",1}},
				{"minecraft:warped_stem",{"",1}}
			}}
		};

	};
}