#include "mod/MyMod.h"

#include <memory>

#include "ll/api/mod/RegisterHelper.h"

#include <ll/api/service/Bedrock.h>

#include <nlohmann/json.hpp>
#include <ll/api/memory/Hook.h>
#include <mc/world/Container.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/item/Item.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/Spawner.h>
#include <mc/math/Vec3.h>
#include <mc/nbt/CompoundTag.h>

#include <direct.h>

using namespace nlohmann;


json config = R"(
  {
    "version": 1,
    "instant_destruction" : false,
    "stay_in_the_Dispenser" : true,
    "consume_durable" : true,
    "destroy":
		{
			"minecraft:iron_pickaxe" : 
                {
                    "minecraft:stone" : 
                        {
                            "dropitem" : "minecraft:cobblestone",
                            "count" : 1
                        },
                    "minecraft:cobblestone" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        }
                },
            "minecraft:iron_axe" : 
                {
                    "minecraft:oak_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:birch_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:jungle_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:spruce_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:dark_oak_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:acacia_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:cherry_log" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:crimson_stem" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        },
                    "minecraft:warped_stem" : 
                        {
                            "dropitem" : "",
                            "count" : 1
                        }
                }
		}
  }
)"_json;

std::string configpath = "./plugins/DispenserDestroyBlock/";


BlockPos Vec3_toBlockPos(Vec3* v) {
    return BlockPos((int)std::floor((double)v->x), (int)std::floor((double)v->y), (int)std::floor((double)v->z));
}



LL_TYPE_INSTANCE_HOOK(
    ItemdispenseHook,
    ll::memory::HookPriority::Normal,
    Item,
    "?dispense@Item@@UEBA_NAEAVBlockSource@@AEAVContainer@@HAEBVVec3@@E@Z",
    bool,
    BlockSource* a2,
    Container* a3,
    int a4,
    Vec3* a5,
    unsigned char a6
) {
    //origin
    auto itemstack = const_cast<ItemStack*>(&a3->getItem(a4));
    // 发射的物品 名称
    auto itemN = itemstack->getTypeName();
    // 发射器对着的方块 名称
    auto blockN = a2->getBlock(Vec3_toBlockPos(a5)).getTypeName();
    // 如果配置文件中对该物品有行为指定
    if (config["destroy"].contains(itemN)) {
        // 如果配置文件指定，发射器对着的方块是允许该发射物破坏的方块
        if (config["destroy"][itemN].contains(blockN)) {
            bool isdestroy = false;
            if (config["destroy"][itemN][blockN]["dropitem"] == "") {
                isdestroy = a2->getLevel().destroyBlock(*a2, Vec3_toBlockPos(a5), true);
            } else {
                isdestroy = a2->getLevel().destroyBlock(*a2, Vec3_toBlockPos(a5), false);
                
                auto item = ItemStack(
                    std::string(config["destroy"][itemN][blockN]["dropitem"]),
                    config["destroy"][itemN][blockN]["count"]
                );
                ll::service::getLevel()->getSpawner().spawnItem(*a2, item, 0, *a5, a2->getDimensionId());
                //Level::spawnItem(*a5, a2->getDimensionId(), item);
            }

            // 如果破坏成功，并且要求消耗耐久
            if (isdestroy && config["consume_durable"] == true) {
                // auto maxduration = a5->getMaxUseDuration();
                auto damage    = itemstack->getDamageValue();
                auto maxdamage = itemstack->getMaxDamage();
                if (maxdamage == 0) {
                    return true;
                }

                if (damage >= maxdamage) {
                    itemstack->remove(1);
                } else {
                    itemstack->setDamageValue(damage + (short)1);
                    // DispenserDestroyBlockLogger.info("物品特殊值:{0}", (int)(a5->getDamageValue()));
                    // DispenserDestroyBlockLogger.info("最大耐久:{0}", a5->getMaxDamage());
                }
            }

            return true;
        }

        if (config["stay_in_the_Dispenser"]) {
            return true;
        }
    }
    return origin(a2, a3, a4, a5, a6);
}


//Level::destroyBlock



namespace my_mod {

static std::unique_ptr<MyMod> instance;

MyMod& MyMod::getInstance() { return *instance; }

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");

    if (_access(configpath.c_str(), 0) == -1) // 表示配置文件所在的文件夹不存在
    {
        if (_mkdir(configpath.c_str()) == -1) {
            // 文件夹创建失败
            getSelf().getLogger().warn("Directory creation failed, please manually create the plugins/DispenserDestroyBlock directory");
            return false;
        }
    }
    std::ifstream f((configpath + "DispenserDestroyBlock.json").c_str());
    if (f.good()) // 表示配置文件存在
    {
        f >> config;
        f.close();
    } else {
        // 配置文件不存在
        std::ofstream c((configpath + "DispenserDestroyBlock.json").c_str());
        c << config.dump(2);
        c.close();
    }
    
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    ItemdispenseHook::hook();
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    ItemdispenseHook::unhook();
    return true;
}

} // namespace my_mod

LL_REGISTER_MOD(my_mod::MyMod, my_mod::instance);
