#include "../Structs.hpp"

#include "Skinchanger.hpp"

#include "../helpers/json.hpp"

#include <fstream>
#include <experimental/filesystem> // hack

#pragma warning (disable: 4715) // condition expression is constant

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

const std::map<size_t, Item_t> k_weapon_info =
{
	{ WEAPON_KNIFE,{ "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ WEAPON_KNIFE_T,{ "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ WEAPON_KNIFE_BAYONET,{ "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ WEAPON_KNIFE_FLIP,{ "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ WEAPON_KNIFE_GUT,{ "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ WEAPON_KNIFE_KARAMBIT,{ "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ WEAPON_KNIFE_M9_BAYONET,{ "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ WEAPON_KNIFE_TACTICAL,{ "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ WEAPON_KNIFE_FALCHION,{ "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ WEAPON_KNIFE_BUTTERFLY,{ "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ WEAPON_KNIFE_PUSH,{ "models/weapons/v_knife_push.mdl", "knife_push" } },
	{ WEAPON_KNIFE_URSUS,{ "models/weapons/v_knife_ursus.mdl", "knife_ursus" } },
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE,{ "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife" } },
	{ WEAPON_KNIFE_STILETTO,{ "models/weapons/v_knife_stiletto.mdl", "knife_stiletto" } },
	{ WEAPON_KNIFE_WIDOWMAKER,{ "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker" } },
	{ WEAPON_KNIFE_CSS,{ "models/weapons/v_knife_css.mdl", "knife_classic" } },
	{ WEAPON_KNIFE_CANIS,{ "models/weapons/v_knife_canis.mdl", "knife_canis" } },
	{ WEAPON_KNIFE_OUTDOOR,{ "models/weapons/v_knife_outdoor.mdl", "knife_outdoor" } },
	{ WEAPON_KNIFE_SKELETON,{ "models/weapons/v_knife_skeleton.mdl", "knife_skeleton" } },
	{ WEAPON_KNIFE_CORD,{ "models/weapons/v_knife_cord.mdl", "knife_cord" } },
	{ GLOVE_STUDDED_BLOODHOUND,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
	{ GLOVE_T_SIDE,{ "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
	{ GLOVE_CT_SIDE,{ "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
	{ GLOVE_SPORTY,{ "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
	{ GLOVE_SLICK,{ "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
	{ GLOVE_LEATHER_WRAP,{ "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
	{ GLOVE_MOTORCYCLE,{ "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
	{ GLOVE_SPECIALIST,{ "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } }
};

const std::vector<WeaponName_t> k_knife_names =
{
	{ 0, "Default" },
	{ WEAPON_KNIFE_BAYONET, "Bayonet" },
	{ WEAPON_KNIFE_FLIP, "Flip Knife" },
	{ WEAPON_KNIFE_GUT, "Gut Knife" },
	{ WEAPON_KNIFE_KARAMBIT, "Karambit" },
	{ WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet" },
	{ WEAPON_KNIFE_TACTICAL, "Huntsman Knife" },
	{ WEAPON_KNIFE_FALCHION, "Falchion Knife" },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife" },
	{ WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife" },
	{ WEAPON_KNIFE_PUSH, "Shadow Daggers" },
	{ WEAPON_KNIFE_URSUS, "Ursus Knife"},
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife" },
	{ WEAPON_KNIFE_STILETTO, "Stiletto Knife" },
	{ WEAPON_KNIFE_WIDOWMAKER, "Talon Knife" },
	{ WEAPON_KNIFE_CSS, "Classic Knife" },
	{ WEAPON_KNIFE_OUTDOOR, "Outdoor Knife" },
	{ WEAPON_KNIFE_SKELETON, "Skeleton Knife" },
	{ WEAPON_KNIFE_CORD, "Cord Knife" },
	{ WEAPON_KNIFE_CANIS, "Canis Knife" },
};

const std::vector<WeaponName_t> k_glove_names =
{
	{ 0, "Default" },
	{ GLOVE_STUDDED_BLOODHOUND, "Bloodhound" },
	{ GLOVE_T_SIDE, "Default (Terrorists)" },
	{ GLOVE_CT_SIDE, "Default (Counter-Terrorists)" },
	{ GLOVE_SPORTY, "Sporty" },
	{ GLOVE_SLICK, "Slick" },
	{ GLOVE_LEATHER_WRAP, "Handwrap" },
	{ GLOVE_MOTORCYCLE, "Motorcycle" },
	{ GLOVE_SPECIALIST, "Specialist" },
	{ GLOVE_HYDRA, "Hydra"}
};

const std::vector<WeaponName_t> k_weapon_names =
{
	{ WEAPON_KNIFE, "Knife" },
	{ GLOVE_T_SIDE, "Glove" },
	{ 7, "AK-47" },
	{ 8, "AUG" },
	{ 9, "AWP" },
	{ 63, "CZ-75" },
	{ 1, "Desert Eagle" },
	{ 2, "Dual Berettas" },
	{ 10, "FAMAS" },
	{ 3, "Five-SeveN" },
	{ 11, "G3SG1" },
	{ 13, "Galil AR" },
	{ 4, "Glock-18" },
	{ 14, "M249" },
	{ 60, "M4A1-S" },
	{ 16, "M4A4" },
	{ 17, "MAC-10" },
	{ 27, "MAG-7" },
	{ 33, "MP7" },
	{ 23, "MP5SD"},
	{ 34, "MP9" },
	{ 28, "Negev" },
	{ 35, "Nova" },
	{ 32, "P2000" },
	{ 36, "P250" },
	{ 19, "P90" },
	{ 26, "PP-Bizon" },
	{ 64, "R8 Revolver" },
	{ 29, "Sawed-Off" },
	{ 38, "SCAR-20" },
	{ 40, "SSG 08" },
	{ 39, "SG-553" },
	{ 30, "Tec-9" },
	{ 24, "UMP-45" },
	{ 61, "USP-S" },
	{ 25, "XM1014" },
};

const std::vector<QualityName_t> k_quality_names =
{
	{ 0, "Default" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int RandomSequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

// Map of animation fixes
// unfortunately can't be constexpr
const static std::unordered_map<std::string, int(*)(int)> animation_fix_map
{
	{ "models/weapons/v_knife_butterfly.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_falchion_advanced.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	} },
	{ "models/weapons/v_knife_push.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	} },
	{ "models/weapons/v_knife_survival_bowie.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	} },
	{ "models/weapons/v_knife_ursus.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_stiletto.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(12, 13);
		}
	} },
	{ "models/weapons/v_knife_widowmaker.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(14, 15);
		}
	} },
	{ "models/weapons/v_knife_cord.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_canis.mdl", [](int sequence) -> int
	{
	switch (sequence)
	{
	case SEQUENCE_DEFAULT_DRAW:
		return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
	case SEQUENCE_DEFAULT_LOOKAT01:
		return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
	default:
		return sequence + 1;
	}
	} },
	{ "models/weapons/v_knife_outdoor.mdl", [](int sequence) -> int
	{
	switch (sequence)
	{
	case SEQUENCE_DEFAULT_DRAW:
		return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
	case SEQUENCE_DEFAULT_LOOKAT01:
		return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
	default:
		return sequence + 1;
	}
	} },
	{ "models/weapons/v_knife_skeleton.mdl", [](int sequence) -> int
	{
	switch (sequence)
	{
	case SEQUENCE_DEFAULT_DRAW:
		return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
	case SEQUENCE_DEFAULT_LOOKAT01:
		return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
	default:
		return sequence + 1;
	}
	} }
		// end
};

void Proxies::nSequence(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	auto local = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());

	if (!local || !local->IsAlive())
		return o_nSequence(pData, pStruct, pOut);

	CRecvProxyData* proxy_data = const_cast<CRecvProxyData*>(pData);
	C_BaseViewModel* view_model = static_cast<C_BaseViewModel*>(pStruct);

	if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid())
	{
		auto owner = view_model->m_hOwner().Get();

		if (owner == local)
		{
			// Get the filename of the current view model.
			auto knife_model = g_MdlInfo->GetModel(view_model->m_nModelIndex());
			auto model_name = g_MdlInfo->GetModelName(knife_model);

			if (animation_fix_map.count(model_name))
				proxy_data->m_Value.m_Int = animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int);
		}
	}

	o_nSequence(proxy_data, pStruct, pOut);
}

bool IsKnife(int i)
{
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

void clearRefCountedVector(CUtlVector<IRefCounted*>& vec)
{
	for (auto& elem : vec)
	{
		if (elem)
		{
			elem->unreference();
			elem = nullptr;
		}
	}
	vec.RemoveAll();
}

void ForceItemUpdate(C_WeaponCSBase* item)
{
	if (!item)
		return;

	C_EconItemView& view = item->m_AttributeManager.m_Item;

	item->m_bCustomMaterialInitialized = (reinterpret_cast<C_BaseAttributableItem*>(item)->m_nFallbackPaintKit() <= 0);

	item->m_CustomMaterials.RemoveAll(); // clear vector, but don't unreference items
	view.m_CustomMaterials.RemoveAll();
	clearRefCountedVector(view.m_VisualsDataProcessors); // prevent memory leak

	item->PostDataUpdate(0);
	item->OnDataChanged(0);
}

static void EraseOverrideIfExistsByIndex(int definition_index)
{
	// We have info about the item not needed to be overridden
	if (k_weapon_info.count(definition_index))
	{
		auto& icon_override_map = Skinchanger::Get().GetIconOverrideMap();

		const auto& original_item = k_weapon_info.at(definition_index);

		// We are overriding its icon when not needed
		if (original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

static void ApplyConfigOnAttributableItem(C_BaseAttributableItem* item, const EconomyItem_t* config, unsigned xuid_low)
{
	if (config->definition_index != GLOVE_T_SIDE && config->definition_override_index != GLOVE_T_SIDE)
	{
		auto world_model_handle = item->m_hWeaponWorldModel();

		if (!world_model_handle.IsValid())
			return;

		auto view_model_weapon = world_model_handle.Get();

		if (!view_model_weapon)
			return;

		view_model_weapon->m_nModelIndex() = item->m_nModelIndex() + 1;
	}

	// Force fallback values to be used.
	item->m_iItemIDHigh() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->m_iAccountID() = xuid_low;

	if (config->entity_quality_index)
		item->m_iEntityQuality() = config->entity_quality_index;

	if (config->custom_name[0])
		strcpy(item->m_szCustomName(), config->custom_name);

	if (config->paint_kit_index)
		item->m_nFallbackPaintKit() = config->paint_kit_index;

	if (config->seed)
		item->m_nFallbackSeed() = config->seed;

	if (config->stat_trak)
		item->m_nFallbackStatTrak() = config->stat_trak;

	item->m_flFallbackWear() = config->wear;

	auto& definition_index = item->m_iItemDefinitionIndex();

	auto& icon_override_map = Skinchanger::Get().GetIconOverrideMap();

	if (config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index // It is not yet overridden
		&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
	{
		unsigned old_definition_index = definition_index;

		definition_index = config->definition_override_index;

		const auto& replacement_item = k_weapon_info.at(config->definition_override_index);

		// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
		item->SetModelIndex(g_MdlInfo->GetModelIndex(replacement_item.model));
		item->GetClientNetworkable()->PreDataUpdate(0);

		// We didn't override 0, but some actual weapon, that we have data for
		if (old_definition_index && k_weapon_info.count(old_definition_index))
		{
			const auto& original_item = k_weapon_info.at(old_definition_index);

			if (original_item.icon && replacement_item.icon)
				icon_override_map[original_item.icon] = replacement_item.icon;
		}
	}
	else
	{
		EraseOverrideIfExistsByIndex(definition_index);
	}
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

void Skinchanger::Work()
{
	auto local_index = g_EngineClient->GetLocalPlayer();
	auto local = C_BasePlayer::GetPlayerByIndex(local_index);
	if (!local)
		return;

	player_info_t player_info;

	if (!g_EngineClient->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		auto wearables = local->m_hMyWearables();

		auto glove_config = this->GetByDefinitionIndex(GLOVE_T_SIDE);

		static auto glove_handle = CBaseHandle(0);

		auto glove = wearables[0].Get();

		if (!glove) // There is no glove
		{
			// Try to get our last created glove
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!local->IsAlive())
		{
			// We are dead but we have a glove, destroy it
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}

		if (glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if (!glove)
			{
				static auto create_wearable_fn = GetWearableCreateFn();

				auto entry = g_EntityList->GetHighestEntityIndex() + 1;
				auto serial = rand() % 0x1000;

				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));

				// He he
				{
					static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
						(Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1"));

					static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

					set_abs_origin_fn(glove, new_pos);
				}

				wearables[0].Init(entry, serial, 16);

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

			ApplyConfigOnAttributableItem(glove, glove_config, player_info.xuid_low);
		}
	}

	// Handle weapon configs
	{
		auto weapons = local->m_hMyWeapons();

		for (size_t i = 0; weapons[i].IsValid(); i++)
		{
			auto weapon = weapons[i].Get();

			if (!weapon)
				continue;

			auto& definition_index = weapon->m_iItemDefinitionIndex();

			// All knives are terrorist knives.
			if (auto active_conf = this->GetByDefinitionIndex(IsKnife(definition_index) ? WEAPON_KNIFE : definition_index))
				ApplyConfigOnAttributableItem(weapon, active_conf, player_info.xuid_low);
			else
				EraseOverrideIfExistsByIndex(definition_index);
		}
	}

	auto view_model_handle = local->m_hViewModel();

	if (!view_model_handle.IsValid())
		return;

	auto view_model = view_model_handle.Get();

	if (!view_model)
		return;

	auto view_model_weapon_handle = view_model->m_hWeapon();

	if (!view_model_weapon_handle.IsValid())
		return;

	auto view_model_weapon = view_model_weapon_handle.Get();

	if (!view_model_weapon)
		return;

	if (k_weapon_info.count(view_model_weapon->m_iItemDefinitionIndex()))
	{
		auto& override_model = k_weapon_info.at(view_model_weapon->m_iItemDefinitionIndex()).model;
		view_model->m_nModelIndex() = g_MdlInfo->GetModelIndex(override_model);
	}

	if (bForceFullUpdate)
	{
		ForceItemUpdates();
		bForceFullUpdate = false;
	}
}

void Skinchanger::ForceItemUpdates()
{
	g_ClientState->ForceFullUpdate();
}

using json = nlohmann::json;

void to_json(json& j, const EconomyItem_t& item)
{
	j = json
	{
		{ "name", item.name },
		{ "enabled", item.enabled },
		{ "definition_index", item.definition_index },
		{ "entity_quality_index", item.entity_quality_index },
		{ "paint_kit_index", item.paint_kit_index },
		{ "definition_override_index", item.definition_override_index },
		{ "seed", item.seed },
		{ "stat_trak", item.stat_trak },
		{ "wear", item.wear },
		{ "custom_name", item.custom_name },
	};
}

void from_json(const json& j, EconomyItem_t& item)
{
	strcpy_s(item.name, j.at("name").get<std::string>().c_str());
	item.enabled = j.at("enabled").get<bool>();
	item.definition_index = j.at("definition_index").get<int>();
	item.entity_quality_index = j.at("entity_quality_index").get<int>();
	item.paint_kit_index = j.at("paint_kit_index").get<int>();
	item.definition_override_index = j.at("definition_override_index").get<int>();
	item.seed = j.at("seed").get<int>();
	item.stat_trak = j.at("stat_trak").get<int>();
	item.wear = j.at("wear").get<float>();
	strcpy_s(item.custom_name, j.at("custom_name").get<std::string>().c_str());
	item.UpdateIds();
}

void Skinchanger::SaveSkins()
{
	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + "skinchanger.json";

	std::ofstream(fPath) << json(m_items);
}

void Skinchanger::LoadSkins()
{
	auto compareFunction = [](const EconomyItem_t& a, const EconomyItem_t& b) { return std::string(a.name) < std::string(b.name); };

	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + "skinchanger.json";

	if (!Config::Get().FileExists(fPath))
		return;

	try
	{
		m_items = json::parse(std::ifstream(fPath)).get<std::vector<EconomyItem_t>>();
		std::sort(m_items.begin(), m_items.end(), compareFunction);
		ForceItemUpdates();
	}
	catch (const std::exception&) {}
}

EconomyItem_t* Skinchanger::GetByDefinitionIndex(int definition_index)
{
	for (auto& x : m_items)
		if (x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}




















































































































// Junk Code By Troll Face & Thaisen's Gen
void AmUeQQICvltNCrQwRZsGcvhDOZMNKyPtrRazkmJl92344664() { double EpvkSYrXNKJEQbBEFRPrDur24106529 = -846848836;    double EpvkSYrXNKJEQbBEFRPrDur39705110 = -693357102;    double EpvkSYrXNKJEQbBEFRPrDur42272927 = -168302802;    double EpvkSYrXNKJEQbBEFRPrDur96819748 = -984417678;    double EpvkSYrXNKJEQbBEFRPrDur6061515 = -240373882;    double EpvkSYrXNKJEQbBEFRPrDur38970310 = -457000967;    double EpvkSYrXNKJEQbBEFRPrDur26325723 = -428088362;    double EpvkSYrXNKJEQbBEFRPrDur31628698 = -697299123;    double EpvkSYrXNKJEQbBEFRPrDur806745 = -979394121;    double EpvkSYrXNKJEQbBEFRPrDur56419053 = -509272226;    double EpvkSYrXNKJEQbBEFRPrDur80482240 = -498910799;    double EpvkSYrXNKJEQbBEFRPrDur56977206 = -238642205;    double EpvkSYrXNKJEQbBEFRPrDur15198167 = -431678546;    double EpvkSYrXNKJEQbBEFRPrDur35601585 = -352366509;    double EpvkSYrXNKJEQbBEFRPrDur30854389 = 68362691;    double EpvkSYrXNKJEQbBEFRPrDur16238343 = -645856272;    double EpvkSYrXNKJEQbBEFRPrDur61699442 = -703039460;    double EpvkSYrXNKJEQbBEFRPrDur31273609 = -287701388;    double EpvkSYrXNKJEQbBEFRPrDur30297968 = -790993537;    double EpvkSYrXNKJEQbBEFRPrDur43546639 = -355639248;    double EpvkSYrXNKJEQbBEFRPrDur16481532 = -904466215;    double EpvkSYrXNKJEQbBEFRPrDur38957982 = -183356104;    double EpvkSYrXNKJEQbBEFRPrDur4928551 = -301033784;    double EpvkSYrXNKJEQbBEFRPrDur18641635 = -985415858;    double EpvkSYrXNKJEQbBEFRPrDur87679243 = -559458214;    double EpvkSYrXNKJEQbBEFRPrDur94126484 = 13729700;    double EpvkSYrXNKJEQbBEFRPrDur29136849 = -563790406;    double EpvkSYrXNKJEQbBEFRPrDur84404713 = -803481938;    double EpvkSYrXNKJEQbBEFRPrDur64156673 = -226689791;    double EpvkSYrXNKJEQbBEFRPrDur88638441 = -305941178;    double EpvkSYrXNKJEQbBEFRPrDur84217349 = -851571211;    double EpvkSYrXNKJEQbBEFRPrDur47349165 = -423596852;    double EpvkSYrXNKJEQbBEFRPrDur33485528 = 82373386;    double EpvkSYrXNKJEQbBEFRPrDur63759458 = -357348615;    double EpvkSYrXNKJEQbBEFRPrDur59712150 = -600239106;    double EpvkSYrXNKJEQbBEFRPrDur62790847 = -43720265;    double EpvkSYrXNKJEQbBEFRPrDur23856195 = -977672851;    double EpvkSYrXNKJEQbBEFRPrDur15592265 = -851411755;    double EpvkSYrXNKJEQbBEFRPrDur12671680 = -629732458;    double EpvkSYrXNKJEQbBEFRPrDur34915251 = -518154166;    double EpvkSYrXNKJEQbBEFRPrDur61185696 = -322053653;    double EpvkSYrXNKJEQbBEFRPrDur85230015 = 66229169;    double EpvkSYrXNKJEQbBEFRPrDur99362999 = -478841320;    double EpvkSYrXNKJEQbBEFRPrDur39366337 = -523868195;    double EpvkSYrXNKJEQbBEFRPrDur46675928 = -680889560;    double EpvkSYrXNKJEQbBEFRPrDur37414511 = -353200628;    double EpvkSYrXNKJEQbBEFRPrDur79315336 = -465847530;    double EpvkSYrXNKJEQbBEFRPrDur51498271 = -476664134;    double EpvkSYrXNKJEQbBEFRPrDur69480116 = -960259538;    double EpvkSYrXNKJEQbBEFRPrDur61439068 = 21755951;    double EpvkSYrXNKJEQbBEFRPrDur95470726 = 14951008;    double EpvkSYrXNKJEQbBEFRPrDur70593896 = -416960721;    double EpvkSYrXNKJEQbBEFRPrDur22900979 = -249864803;    double EpvkSYrXNKJEQbBEFRPrDur84234204 = -905549786;    double EpvkSYrXNKJEQbBEFRPrDur75863132 = -946562407;    double EpvkSYrXNKJEQbBEFRPrDur85148546 = -563492732;    double EpvkSYrXNKJEQbBEFRPrDur34776559 = -292323318;    double EpvkSYrXNKJEQbBEFRPrDur23631293 = -182886944;    double EpvkSYrXNKJEQbBEFRPrDur9140505 = -324959465;    double EpvkSYrXNKJEQbBEFRPrDur11935030 = -154103582;    double EpvkSYrXNKJEQbBEFRPrDur9833462 = -893210562;    double EpvkSYrXNKJEQbBEFRPrDur41921010 = -624606425;    double EpvkSYrXNKJEQbBEFRPrDur67472025 = -370609332;    double EpvkSYrXNKJEQbBEFRPrDur12168303 = -573452944;    double EpvkSYrXNKJEQbBEFRPrDur72201703 = -657701015;    double EpvkSYrXNKJEQbBEFRPrDur33133076 = 24686053;    double EpvkSYrXNKJEQbBEFRPrDur23491678 = -221015591;    double EpvkSYrXNKJEQbBEFRPrDur51438709 = 25670068;    double EpvkSYrXNKJEQbBEFRPrDur75889435 = -752127403;    double EpvkSYrXNKJEQbBEFRPrDur68063541 = -887917044;    double EpvkSYrXNKJEQbBEFRPrDur92382147 = -668183422;    double EpvkSYrXNKJEQbBEFRPrDur46107178 = -851627706;    double EpvkSYrXNKJEQbBEFRPrDur18601929 = -657968930;    double EpvkSYrXNKJEQbBEFRPrDur95382717 = -172839372;    double EpvkSYrXNKJEQbBEFRPrDur82360942 = 66414405;    double EpvkSYrXNKJEQbBEFRPrDur31251517 = -870695384;    double EpvkSYrXNKJEQbBEFRPrDur39594983 = -704514784;    double EpvkSYrXNKJEQbBEFRPrDur65562214 = -777165589;    double EpvkSYrXNKJEQbBEFRPrDur71965706 = -204526299;    double EpvkSYrXNKJEQbBEFRPrDur50264733 = -106257587;    double EpvkSYrXNKJEQbBEFRPrDur14811149 = -520422770;    double EpvkSYrXNKJEQbBEFRPrDur77638577 = 12873728;    double EpvkSYrXNKJEQbBEFRPrDur14924597 = -843222400;    double EpvkSYrXNKJEQbBEFRPrDur2717606 = -148445743;    double EpvkSYrXNKJEQbBEFRPrDur93167715 = -220892186;    double EpvkSYrXNKJEQbBEFRPrDur13623454 = -334610491;    double EpvkSYrXNKJEQbBEFRPrDur24448186 = -73732049;    double EpvkSYrXNKJEQbBEFRPrDur49251324 = -12076829;    double EpvkSYrXNKJEQbBEFRPrDur87896325 = -410786208;    double EpvkSYrXNKJEQbBEFRPrDur74563604 = 63253625;    double EpvkSYrXNKJEQbBEFRPrDur28014288 = -751396948;    double EpvkSYrXNKJEQbBEFRPrDur224903 = -694785907;    double EpvkSYrXNKJEQbBEFRPrDur6451760 = -426452291;    double EpvkSYrXNKJEQbBEFRPrDur736651 = -375628876;    double EpvkSYrXNKJEQbBEFRPrDur25081789 = -624943604;    double EpvkSYrXNKJEQbBEFRPrDur19264687 = -697447229;    double EpvkSYrXNKJEQbBEFRPrDur17757991 = -563161500;    double EpvkSYrXNKJEQbBEFRPrDur87194696 = -905388377;    double EpvkSYrXNKJEQbBEFRPrDur67164633 = -866167180;    double EpvkSYrXNKJEQbBEFRPrDur13542853 = -846848836;     EpvkSYrXNKJEQbBEFRPrDur24106529 = EpvkSYrXNKJEQbBEFRPrDur39705110;     EpvkSYrXNKJEQbBEFRPrDur39705110 = EpvkSYrXNKJEQbBEFRPrDur42272927;     EpvkSYrXNKJEQbBEFRPrDur42272927 = EpvkSYrXNKJEQbBEFRPrDur96819748;     EpvkSYrXNKJEQbBEFRPrDur96819748 = EpvkSYrXNKJEQbBEFRPrDur6061515;     EpvkSYrXNKJEQbBEFRPrDur6061515 = EpvkSYrXNKJEQbBEFRPrDur38970310;     EpvkSYrXNKJEQbBEFRPrDur38970310 = EpvkSYrXNKJEQbBEFRPrDur26325723;     EpvkSYrXNKJEQbBEFRPrDur26325723 = EpvkSYrXNKJEQbBEFRPrDur31628698;     EpvkSYrXNKJEQbBEFRPrDur31628698 = EpvkSYrXNKJEQbBEFRPrDur806745;     EpvkSYrXNKJEQbBEFRPrDur806745 = EpvkSYrXNKJEQbBEFRPrDur56419053;     EpvkSYrXNKJEQbBEFRPrDur56419053 = EpvkSYrXNKJEQbBEFRPrDur80482240;     EpvkSYrXNKJEQbBEFRPrDur80482240 = EpvkSYrXNKJEQbBEFRPrDur56977206;     EpvkSYrXNKJEQbBEFRPrDur56977206 = EpvkSYrXNKJEQbBEFRPrDur15198167;     EpvkSYrXNKJEQbBEFRPrDur15198167 = EpvkSYrXNKJEQbBEFRPrDur35601585;     EpvkSYrXNKJEQbBEFRPrDur35601585 = EpvkSYrXNKJEQbBEFRPrDur30854389;     EpvkSYrXNKJEQbBEFRPrDur30854389 = EpvkSYrXNKJEQbBEFRPrDur16238343;     EpvkSYrXNKJEQbBEFRPrDur16238343 = EpvkSYrXNKJEQbBEFRPrDur61699442;     EpvkSYrXNKJEQbBEFRPrDur61699442 = EpvkSYrXNKJEQbBEFRPrDur31273609;     EpvkSYrXNKJEQbBEFRPrDur31273609 = EpvkSYrXNKJEQbBEFRPrDur30297968;     EpvkSYrXNKJEQbBEFRPrDur30297968 = EpvkSYrXNKJEQbBEFRPrDur43546639;     EpvkSYrXNKJEQbBEFRPrDur43546639 = EpvkSYrXNKJEQbBEFRPrDur16481532;     EpvkSYrXNKJEQbBEFRPrDur16481532 = EpvkSYrXNKJEQbBEFRPrDur38957982;     EpvkSYrXNKJEQbBEFRPrDur38957982 = EpvkSYrXNKJEQbBEFRPrDur4928551;     EpvkSYrXNKJEQbBEFRPrDur4928551 = EpvkSYrXNKJEQbBEFRPrDur18641635;     EpvkSYrXNKJEQbBEFRPrDur18641635 = EpvkSYrXNKJEQbBEFRPrDur87679243;     EpvkSYrXNKJEQbBEFRPrDur87679243 = EpvkSYrXNKJEQbBEFRPrDur94126484;     EpvkSYrXNKJEQbBEFRPrDur94126484 = EpvkSYrXNKJEQbBEFRPrDur29136849;     EpvkSYrXNKJEQbBEFRPrDur29136849 = EpvkSYrXNKJEQbBEFRPrDur84404713;     EpvkSYrXNKJEQbBEFRPrDur84404713 = EpvkSYrXNKJEQbBEFRPrDur64156673;     EpvkSYrXNKJEQbBEFRPrDur64156673 = EpvkSYrXNKJEQbBEFRPrDur88638441;     EpvkSYrXNKJEQbBEFRPrDur88638441 = EpvkSYrXNKJEQbBEFRPrDur84217349;     EpvkSYrXNKJEQbBEFRPrDur84217349 = EpvkSYrXNKJEQbBEFRPrDur47349165;     EpvkSYrXNKJEQbBEFRPrDur47349165 = EpvkSYrXNKJEQbBEFRPrDur33485528;     EpvkSYrXNKJEQbBEFRPrDur33485528 = EpvkSYrXNKJEQbBEFRPrDur63759458;     EpvkSYrXNKJEQbBEFRPrDur63759458 = EpvkSYrXNKJEQbBEFRPrDur59712150;     EpvkSYrXNKJEQbBEFRPrDur59712150 = EpvkSYrXNKJEQbBEFRPrDur62790847;     EpvkSYrXNKJEQbBEFRPrDur62790847 = EpvkSYrXNKJEQbBEFRPrDur23856195;     EpvkSYrXNKJEQbBEFRPrDur23856195 = EpvkSYrXNKJEQbBEFRPrDur15592265;     EpvkSYrXNKJEQbBEFRPrDur15592265 = EpvkSYrXNKJEQbBEFRPrDur12671680;     EpvkSYrXNKJEQbBEFRPrDur12671680 = EpvkSYrXNKJEQbBEFRPrDur34915251;     EpvkSYrXNKJEQbBEFRPrDur34915251 = EpvkSYrXNKJEQbBEFRPrDur61185696;     EpvkSYrXNKJEQbBEFRPrDur61185696 = EpvkSYrXNKJEQbBEFRPrDur85230015;     EpvkSYrXNKJEQbBEFRPrDur85230015 = EpvkSYrXNKJEQbBEFRPrDur99362999;     EpvkSYrXNKJEQbBEFRPrDur99362999 = EpvkSYrXNKJEQbBEFRPrDur39366337;     EpvkSYrXNKJEQbBEFRPrDur39366337 = EpvkSYrXNKJEQbBEFRPrDur46675928;     EpvkSYrXNKJEQbBEFRPrDur46675928 = EpvkSYrXNKJEQbBEFRPrDur37414511;     EpvkSYrXNKJEQbBEFRPrDur37414511 = EpvkSYrXNKJEQbBEFRPrDur79315336;     EpvkSYrXNKJEQbBEFRPrDur79315336 = EpvkSYrXNKJEQbBEFRPrDur51498271;     EpvkSYrXNKJEQbBEFRPrDur51498271 = EpvkSYrXNKJEQbBEFRPrDur69480116;     EpvkSYrXNKJEQbBEFRPrDur69480116 = EpvkSYrXNKJEQbBEFRPrDur61439068;     EpvkSYrXNKJEQbBEFRPrDur61439068 = EpvkSYrXNKJEQbBEFRPrDur95470726;     EpvkSYrXNKJEQbBEFRPrDur95470726 = EpvkSYrXNKJEQbBEFRPrDur70593896;     EpvkSYrXNKJEQbBEFRPrDur70593896 = EpvkSYrXNKJEQbBEFRPrDur22900979;     EpvkSYrXNKJEQbBEFRPrDur22900979 = EpvkSYrXNKJEQbBEFRPrDur84234204;     EpvkSYrXNKJEQbBEFRPrDur84234204 = EpvkSYrXNKJEQbBEFRPrDur75863132;     EpvkSYrXNKJEQbBEFRPrDur75863132 = EpvkSYrXNKJEQbBEFRPrDur85148546;     EpvkSYrXNKJEQbBEFRPrDur85148546 = EpvkSYrXNKJEQbBEFRPrDur34776559;     EpvkSYrXNKJEQbBEFRPrDur34776559 = EpvkSYrXNKJEQbBEFRPrDur23631293;     EpvkSYrXNKJEQbBEFRPrDur23631293 = EpvkSYrXNKJEQbBEFRPrDur9140505;     EpvkSYrXNKJEQbBEFRPrDur9140505 = EpvkSYrXNKJEQbBEFRPrDur11935030;     EpvkSYrXNKJEQbBEFRPrDur11935030 = EpvkSYrXNKJEQbBEFRPrDur9833462;     EpvkSYrXNKJEQbBEFRPrDur9833462 = EpvkSYrXNKJEQbBEFRPrDur41921010;     EpvkSYrXNKJEQbBEFRPrDur41921010 = EpvkSYrXNKJEQbBEFRPrDur67472025;     EpvkSYrXNKJEQbBEFRPrDur67472025 = EpvkSYrXNKJEQbBEFRPrDur12168303;     EpvkSYrXNKJEQbBEFRPrDur12168303 = EpvkSYrXNKJEQbBEFRPrDur72201703;     EpvkSYrXNKJEQbBEFRPrDur72201703 = EpvkSYrXNKJEQbBEFRPrDur33133076;     EpvkSYrXNKJEQbBEFRPrDur33133076 = EpvkSYrXNKJEQbBEFRPrDur23491678;     EpvkSYrXNKJEQbBEFRPrDur23491678 = EpvkSYrXNKJEQbBEFRPrDur51438709;     EpvkSYrXNKJEQbBEFRPrDur51438709 = EpvkSYrXNKJEQbBEFRPrDur75889435;     EpvkSYrXNKJEQbBEFRPrDur75889435 = EpvkSYrXNKJEQbBEFRPrDur68063541;     EpvkSYrXNKJEQbBEFRPrDur68063541 = EpvkSYrXNKJEQbBEFRPrDur92382147;     EpvkSYrXNKJEQbBEFRPrDur92382147 = EpvkSYrXNKJEQbBEFRPrDur46107178;     EpvkSYrXNKJEQbBEFRPrDur46107178 = EpvkSYrXNKJEQbBEFRPrDur18601929;     EpvkSYrXNKJEQbBEFRPrDur18601929 = EpvkSYrXNKJEQbBEFRPrDur95382717;     EpvkSYrXNKJEQbBEFRPrDur95382717 = EpvkSYrXNKJEQbBEFRPrDur82360942;     EpvkSYrXNKJEQbBEFRPrDur82360942 = EpvkSYrXNKJEQbBEFRPrDur31251517;     EpvkSYrXNKJEQbBEFRPrDur31251517 = EpvkSYrXNKJEQbBEFRPrDur39594983;     EpvkSYrXNKJEQbBEFRPrDur39594983 = EpvkSYrXNKJEQbBEFRPrDur65562214;     EpvkSYrXNKJEQbBEFRPrDur65562214 = EpvkSYrXNKJEQbBEFRPrDur71965706;     EpvkSYrXNKJEQbBEFRPrDur71965706 = EpvkSYrXNKJEQbBEFRPrDur50264733;     EpvkSYrXNKJEQbBEFRPrDur50264733 = EpvkSYrXNKJEQbBEFRPrDur14811149;     EpvkSYrXNKJEQbBEFRPrDur14811149 = EpvkSYrXNKJEQbBEFRPrDur77638577;     EpvkSYrXNKJEQbBEFRPrDur77638577 = EpvkSYrXNKJEQbBEFRPrDur14924597;     EpvkSYrXNKJEQbBEFRPrDur14924597 = EpvkSYrXNKJEQbBEFRPrDur2717606;     EpvkSYrXNKJEQbBEFRPrDur2717606 = EpvkSYrXNKJEQbBEFRPrDur93167715;     EpvkSYrXNKJEQbBEFRPrDur93167715 = EpvkSYrXNKJEQbBEFRPrDur13623454;     EpvkSYrXNKJEQbBEFRPrDur13623454 = EpvkSYrXNKJEQbBEFRPrDur24448186;     EpvkSYrXNKJEQbBEFRPrDur24448186 = EpvkSYrXNKJEQbBEFRPrDur49251324;     EpvkSYrXNKJEQbBEFRPrDur49251324 = EpvkSYrXNKJEQbBEFRPrDur87896325;     EpvkSYrXNKJEQbBEFRPrDur87896325 = EpvkSYrXNKJEQbBEFRPrDur74563604;     EpvkSYrXNKJEQbBEFRPrDur74563604 = EpvkSYrXNKJEQbBEFRPrDur28014288;     EpvkSYrXNKJEQbBEFRPrDur28014288 = EpvkSYrXNKJEQbBEFRPrDur224903;     EpvkSYrXNKJEQbBEFRPrDur224903 = EpvkSYrXNKJEQbBEFRPrDur6451760;     EpvkSYrXNKJEQbBEFRPrDur6451760 = EpvkSYrXNKJEQbBEFRPrDur736651;     EpvkSYrXNKJEQbBEFRPrDur736651 = EpvkSYrXNKJEQbBEFRPrDur25081789;     EpvkSYrXNKJEQbBEFRPrDur25081789 = EpvkSYrXNKJEQbBEFRPrDur19264687;     EpvkSYrXNKJEQbBEFRPrDur19264687 = EpvkSYrXNKJEQbBEFRPrDur17757991;     EpvkSYrXNKJEQbBEFRPrDur17757991 = EpvkSYrXNKJEQbBEFRPrDur87194696;     EpvkSYrXNKJEQbBEFRPrDur87194696 = EpvkSYrXNKJEQbBEFRPrDur67164633;     EpvkSYrXNKJEQbBEFRPrDur67164633 = EpvkSYrXNKJEQbBEFRPrDur13542853;     EpvkSYrXNKJEQbBEFRPrDur13542853 = EpvkSYrXNKJEQbBEFRPrDur24106529; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void tnMxngqBUyiquGykCiaWIdNdFXUla13309182() { float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat56685693 = -852672958;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3331525 = -360959581;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42729755 = -380277249;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93134302 = -157269763;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat29433234 = -814509387;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat46892448 = -185915023;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat16167483 = 7528814;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3157936 = -277321080;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85912659 = -410779233;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34002829 = -995082830;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35841456 = 83725178;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99903118 = -927480417;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat19882599 = -986875296;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74827655 = -473379818;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3715930 = -372166942;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38252230 = -615172135;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74484976 = -123141902;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85199850 = -556920564;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18949242 = -196169939;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35006968 = -176208385;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75619602 = -958664865;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42300171 = -100962830;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69436971 = -947977191;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat89552742 = -498931704;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat40058228 = -251525841;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11956635 = -539283274;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35621713 = -122900286;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat65998595 = -462086708;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat96610023 = -574921497;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat43708772 = -60104799;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat54509220 = -940452200;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60471402 = -652027990;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat78439770 = -288735967;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26421416 = -12364715;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71830757 = 9889596;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84783689 = -974630878;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat24772952 = -780572538;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat73912357 = 32269429;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26887621 = -456543024;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95788237 = 46327718;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49865419 = -856890735;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38377039 = -934943459;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26435120 = -52161109;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42135107 = -28866966;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37568886 = -351827595;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34856716 = -512739098;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51555767 = -225569372;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat59335330 = -197421079;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95544080 = -424311826;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat30342061 = -455135331;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74580345 = -780196258;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49905758 = 7040962;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88859087 = -877286506;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38707074 = -34735140;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37689579 = -357147995;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat14385523 = -651710129;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat33894554 = -412982390;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53177012 = -881345546;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53076075 = -905743923;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat17476600 = -175226113;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11270735 = 36985263;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50168888 = -530384478;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat6547912 = -702399583;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42203887 = -250674434;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat79493608 = 45369369;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75370054 = -264246833;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat21463348 = -538744451;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93461183 = -874510581;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat2996899 = -383269414;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18932241 = -397536064;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat13479279 = -834599597;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat572620 = -55411331;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat58312229 = -377541;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat23161004 = -142497657;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85141549 = -319317650;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37242564 = 76278594;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat15865052 = 51198278;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat27301865 = -819110226;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51983857 = -47104109;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat5201513 = -738786743;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60400867 = -213713903;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat76286383 = -925479208;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat70454515 = 62225117;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat66267963 = -19786167;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69128427 = -279908542;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat4603462 = -847493162;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71612315 = -774741484;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat39732696 = -154000827;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88731836 = -655216721;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat57445235 = -338400276;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50889135 = -461648489;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71595939 = -899226993;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat20836282 = -61986649;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat9411022 = -181316911;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84517503 = -990657546;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99696531 = -226506257;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat31829127 = -132543876;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84231232 = -801486675;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62641498 = 25763664;    float bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62198831 = -852672958;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat56685693 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3331525;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3331525 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42729755;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42729755 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93134302;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93134302 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat29433234;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat29433234 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat46892448;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat46892448 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat16167483;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat16167483 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3157936;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3157936 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85912659;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85912659 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34002829;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34002829 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35841456;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35841456 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99903118;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99903118 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat19882599;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat19882599 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74827655;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74827655 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3715930;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat3715930 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38252230;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38252230 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74484976;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74484976 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85199850;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85199850 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18949242;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18949242 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35006968;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35006968 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75619602;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75619602 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42300171;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42300171 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69436971;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69436971 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat89552742;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat89552742 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat40058228;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat40058228 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11956635;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11956635 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35621713;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat35621713 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat65998595;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat65998595 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat96610023;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat96610023 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat43708772;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat43708772 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat54509220;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat54509220 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60471402;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60471402 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat78439770;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat78439770 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26421416;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26421416 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71830757;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71830757 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84783689;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84783689 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat24772952;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat24772952 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat73912357;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat73912357 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26887621;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26887621 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95788237;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95788237 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49865419;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49865419 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38377039;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38377039 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26435120;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat26435120 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42135107;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42135107 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37568886;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37568886 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34856716;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat34856716 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51555767;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51555767 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat59335330;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat59335330 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95544080;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat95544080 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat30342061;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat30342061 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74580345;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat74580345 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49905758;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat49905758 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88859087;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88859087 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38707074;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat38707074 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37689579;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37689579 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat14385523;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat14385523 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat33894554;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat33894554 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53177012;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53177012 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53076075;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat53076075 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat17476600;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat17476600 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11270735;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat11270735 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50168888;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50168888 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat6547912;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat6547912 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42203887;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat42203887 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat79493608;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat79493608 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75370054;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat75370054 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat21463348;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat21463348 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93461183;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat93461183 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat2996899;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat2996899 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18932241;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat18932241 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat13479279;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat13479279 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat572620;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat572620 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat58312229;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat58312229 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat23161004;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat23161004 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85141549;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat85141549 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37242564;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat37242564 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat15865052;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat15865052 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat27301865;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat27301865 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51983857;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat51983857 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat5201513;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat5201513 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60400867;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat60400867 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat76286383;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat76286383 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat70454515;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat70454515 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat66267963;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat66267963 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69128427;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat69128427 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat4603462;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat4603462 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71612315;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71612315 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat39732696;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat39732696 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88731836;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat88731836 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat57445235;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat57445235 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50889135;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat50889135 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71595939;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat71595939 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat20836282;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat20836282 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat9411022;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat9411022 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84517503;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84517503 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99696531;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat99696531 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat31829127;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat31829127 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84231232;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat84231232 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62641498;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62641498 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62198831;     bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat62198831 = bIJpRDRUKfUZOpHYasYJKZPVZKQajrQLEat56685693; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bwGsnRNuEjZFMtCYCXB73640843() { int CIppfeagZUcAlYRn53799418 = -264744323;    int CIppfeagZUcAlYRn92258177 = -190605578;    int CIppfeagZUcAlYRn7113318 = -883713235;    int CIppfeagZUcAlYRn2305364 = -429511607;    int CIppfeagZUcAlYRn5359788 = -651401369;    int CIppfeagZUcAlYRn53397146 = -451913995;    int CIppfeagZUcAlYRn63899173 = -516664743;    int CIppfeagZUcAlYRn81769662 = 82713786;    int CIppfeagZUcAlYRn52241133 = -167479007;    int CIppfeagZUcAlYRn93323406 = -353013307;    int CIppfeagZUcAlYRn73339542 = -591103189;    int CIppfeagZUcAlYRn56552559 = -387676221;    int CIppfeagZUcAlYRn18185300 = 88281985;    int CIppfeagZUcAlYRn14757083 = -963182769;    int CIppfeagZUcAlYRn63555063 = -271984413;    int CIppfeagZUcAlYRn51289363 = -181472041;    int CIppfeagZUcAlYRn87287335 = 64115639;    int CIppfeagZUcAlYRn62366434 = -49783294;    int CIppfeagZUcAlYRn27478455 = 97234815;    int CIppfeagZUcAlYRn4703842 = -182240907;    int CIppfeagZUcAlYRn1340234 = 33944801;    int CIppfeagZUcAlYRn20517101 = -441925145;    int CIppfeagZUcAlYRn298408 = -216246257;    int CIppfeagZUcAlYRn10321014 = -482543468;    int CIppfeagZUcAlYRn12861952 = -346302122;    int CIppfeagZUcAlYRn35662689 = -211925371;    int CIppfeagZUcAlYRn51907586 = -176525739;    int CIppfeagZUcAlYRn42267578 = -559886293;    int CIppfeagZUcAlYRn95073606 = -995128716;    int CIppfeagZUcAlYRn69335562 = -555192623;    int CIppfeagZUcAlYRn39630538 = -864445936;    int CIppfeagZUcAlYRn20494478 = -399309475;    int CIppfeagZUcAlYRn96628424 = -714193559;    int CIppfeagZUcAlYRn59617785 = -579368734;    int CIppfeagZUcAlYRn498949 = -788195376;    int CIppfeagZUcAlYRn98043876 = -670741932;    int CIppfeagZUcAlYRn88219259 = -818429564;    int CIppfeagZUcAlYRn50934725 = -196656485;    int CIppfeagZUcAlYRn46026598 = -245158729;    int CIppfeagZUcAlYRn64262585 = -633359539;    int CIppfeagZUcAlYRn92949201 = -610472801;    int CIppfeagZUcAlYRn87856596 = -335362878;    int CIppfeagZUcAlYRn24407222 = -631430366;    int CIppfeagZUcAlYRn64004436 = -686497770;    int CIppfeagZUcAlYRn12463437 = -669513870;    int CIppfeagZUcAlYRn26765560 = -445398607;    int CIppfeagZUcAlYRn32148827 = 17414525;    int CIppfeagZUcAlYRn24829658 = -360412786;    int CIppfeagZUcAlYRn17373768 = -888391094;    int CIppfeagZUcAlYRn94760888 = -28024683;    int CIppfeagZUcAlYRn86178610 = -179621973;    int CIppfeagZUcAlYRn60279405 = -837450757;    int CIppfeagZUcAlYRn61644395 = -540716664;    int CIppfeagZUcAlYRn87438367 = -604331846;    int CIppfeagZUcAlYRn72300610 = -278142714;    int CIppfeagZUcAlYRn33282317 = -822819178;    int CIppfeagZUcAlYRn91959769 = -974359322;    int CIppfeagZUcAlYRn96792303 = -301169768;    int CIppfeagZUcAlYRn89443412 = 16790515;    int CIppfeagZUcAlYRn69697098 = -339475999;    int CIppfeagZUcAlYRn1489561 = -175388257;    int CIppfeagZUcAlYRn21631596 = -956778450;    int CIppfeagZUcAlYRn86696055 = 77842501;    int CIppfeagZUcAlYRn82905571 = -612286385;    int CIppfeagZUcAlYRn53692868 = -488567372;    int CIppfeagZUcAlYRn52845065 = -91793714;    int CIppfeagZUcAlYRn59924135 = -673482663;    int CIppfeagZUcAlYRn58567515 = -332349281;    int CIppfeagZUcAlYRn14258134 = -74987394;    int CIppfeagZUcAlYRn65511186 = -601242482;    int CIppfeagZUcAlYRn63070104 = -363042477;    int CIppfeagZUcAlYRn36352610 = -739227876;    int CIppfeagZUcAlYRn16339837 = -804624566;    int CIppfeagZUcAlYRn63215869 = -269405647;    int CIppfeagZUcAlYRn11754641 = -571768107;    int CIppfeagZUcAlYRn13483638 = -630692322;    int CIppfeagZUcAlYRn96109879 = -810494780;    int CIppfeagZUcAlYRn36293972 = -529748487;    int CIppfeagZUcAlYRn97857577 = -813029598;    int CIppfeagZUcAlYRn86096391 = -900903516;    int CIppfeagZUcAlYRn3513863 = -129339897;    int CIppfeagZUcAlYRn27077928 = -816112953;    int CIppfeagZUcAlYRn24893810 = -671495200;    int CIppfeagZUcAlYRn312719 = -867104034;    int CIppfeagZUcAlYRn83156951 = -275570651;    int CIppfeagZUcAlYRn79351132 = 73004821;    int CIppfeagZUcAlYRn58850083 = -858592812;    int CIppfeagZUcAlYRn9190058 = -9861713;    int CIppfeagZUcAlYRn87317175 = -201226021;    int CIppfeagZUcAlYRn67216631 = -965376199;    int CIppfeagZUcAlYRn6084107 = -696382610;    int CIppfeagZUcAlYRn91426955 = -417259797;    int CIppfeagZUcAlYRn61491313 = -113447000;    int CIppfeagZUcAlYRn76329499 = -905682731;    int CIppfeagZUcAlYRn62773025 = -357971283;    int CIppfeagZUcAlYRn71317606 = -653694351;    int CIppfeagZUcAlYRn1160541 = -313205379;    int CIppfeagZUcAlYRn41501650 = 80856019;    int CIppfeagZUcAlYRn10311568 = -97930399;    int CIppfeagZUcAlYRn59618372 = -264744323;     CIppfeagZUcAlYRn53799418 = CIppfeagZUcAlYRn92258177;     CIppfeagZUcAlYRn92258177 = CIppfeagZUcAlYRn7113318;     CIppfeagZUcAlYRn7113318 = CIppfeagZUcAlYRn2305364;     CIppfeagZUcAlYRn2305364 = CIppfeagZUcAlYRn5359788;     CIppfeagZUcAlYRn5359788 = CIppfeagZUcAlYRn53397146;     CIppfeagZUcAlYRn53397146 = CIppfeagZUcAlYRn63899173;     CIppfeagZUcAlYRn63899173 = CIppfeagZUcAlYRn81769662;     CIppfeagZUcAlYRn81769662 = CIppfeagZUcAlYRn52241133;     CIppfeagZUcAlYRn52241133 = CIppfeagZUcAlYRn93323406;     CIppfeagZUcAlYRn93323406 = CIppfeagZUcAlYRn73339542;     CIppfeagZUcAlYRn73339542 = CIppfeagZUcAlYRn56552559;     CIppfeagZUcAlYRn56552559 = CIppfeagZUcAlYRn18185300;     CIppfeagZUcAlYRn18185300 = CIppfeagZUcAlYRn14757083;     CIppfeagZUcAlYRn14757083 = CIppfeagZUcAlYRn63555063;     CIppfeagZUcAlYRn63555063 = CIppfeagZUcAlYRn51289363;     CIppfeagZUcAlYRn51289363 = CIppfeagZUcAlYRn87287335;     CIppfeagZUcAlYRn87287335 = CIppfeagZUcAlYRn62366434;     CIppfeagZUcAlYRn62366434 = CIppfeagZUcAlYRn27478455;     CIppfeagZUcAlYRn27478455 = CIppfeagZUcAlYRn4703842;     CIppfeagZUcAlYRn4703842 = CIppfeagZUcAlYRn1340234;     CIppfeagZUcAlYRn1340234 = CIppfeagZUcAlYRn20517101;     CIppfeagZUcAlYRn20517101 = CIppfeagZUcAlYRn298408;     CIppfeagZUcAlYRn298408 = CIppfeagZUcAlYRn10321014;     CIppfeagZUcAlYRn10321014 = CIppfeagZUcAlYRn12861952;     CIppfeagZUcAlYRn12861952 = CIppfeagZUcAlYRn35662689;     CIppfeagZUcAlYRn35662689 = CIppfeagZUcAlYRn51907586;     CIppfeagZUcAlYRn51907586 = CIppfeagZUcAlYRn42267578;     CIppfeagZUcAlYRn42267578 = CIppfeagZUcAlYRn95073606;     CIppfeagZUcAlYRn95073606 = CIppfeagZUcAlYRn69335562;     CIppfeagZUcAlYRn69335562 = CIppfeagZUcAlYRn39630538;     CIppfeagZUcAlYRn39630538 = CIppfeagZUcAlYRn20494478;     CIppfeagZUcAlYRn20494478 = CIppfeagZUcAlYRn96628424;     CIppfeagZUcAlYRn96628424 = CIppfeagZUcAlYRn59617785;     CIppfeagZUcAlYRn59617785 = CIppfeagZUcAlYRn498949;     CIppfeagZUcAlYRn498949 = CIppfeagZUcAlYRn98043876;     CIppfeagZUcAlYRn98043876 = CIppfeagZUcAlYRn88219259;     CIppfeagZUcAlYRn88219259 = CIppfeagZUcAlYRn50934725;     CIppfeagZUcAlYRn50934725 = CIppfeagZUcAlYRn46026598;     CIppfeagZUcAlYRn46026598 = CIppfeagZUcAlYRn64262585;     CIppfeagZUcAlYRn64262585 = CIppfeagZUcAlYRn92949201;     CIppfeagZUcAlYRn92949201 = CIppfeagZUcAlYRn87856596;     CIppfeagZUcAlYRn87856596 = CIppfeagZUcAlYRn24407222;     CIppfeagZUcAlYRn24407222 = CIppfeagZUcAlYRn64004436;     CIppfeagZUcAlYRn64004436 = CIppfeagZUcAlYRn12463437;     CIppfeagZUcAlYRn12463437 = CIppfeagZUcAlYRn26765560;     CIppfeagZUcAlYRn26765560 = CIppfeagZUcAlYRn32148827;     CIppfeagZUcAlYRn32148827 = CIppfeagZUcAlYRn24829658;     CIppfeagZUcAlYRn24829658 = CIppfeagZUcAlYRn17373768;     CIppfeagZUcAlYRn17373768 = CIppfeagZUcAlYRn94760888;     CIppfeagZUcAlYRn94760888 = CIppfeagZUcAlYRn86178610;     CIppfeagZUcAlYRn86178610 = CIppfeagZUcAlYRn60279405;     CIppfeagZUcAlYRn60279405 = CIppfeagZUcAlYRn61644395;     CIppfeagZUcAlYRn61644395 = CIppfeagZUcAlYRn87438367;     CIppfeagZUcAlYRn87438367 = CIppfeagZUcAlYRn72300610;     CIppfeagZUcAlYRn72300610 = CIppfeagZUcAlYRn33282317;     CIppfeagZUcAlYRn33282317 = CIppfeagZUcAlYRn91959769;     CIppfeagZUcAlYRn91959769 = CIppfeagZUcAlYRn96792303;     CIppfeagZUcAlYRn96792303 = CIppfeagZUcAlYRn89443412;     CIppfeagZUcAlYRn89443412 = CIppfeagZUcAlYRn69697098;     CIppfeagZUcAlYRn69697098 = CIppfeagZUcAlYRn1489561;     CIppfeagZUcAlYRn1489561 = CIppfeagZUcAlYRn21631596;     CIppfeagZUcAlYRn21631596 = CIppfeagZUcAlYRn86696055;     CIppfeagZUcAlYRn86696055 = CIppfeagZUcAlYRn82905571;     CIppfeagZUcAlYRn82905571 = CIppfeagZUcAlYRn53692868;     CIppfeagZUcAlYRn53692868 = CIppfeagZUcAlYRn52845065;     CIppfeagZUcAlYRn52845065 = CIppfeagZUcAlYRn59924135;     CIppfeagZUcAlYRn59924135 = CIppfeagZUcAlYRn58567515;     CIppfeagZUcAlYRn58567515 = CIppfeagZUcAlYRn14258134;     CIppfeagZUcAlYRn14258134 = CIppfeagZUcAlYRn65511186;     CIppfeagZUcAlYRn65511186 = CIppfeagZUcAlYRn63070104;     CIppfeagZUcAlYRn63070104 = CIppfeagZUcAlYRn36352610;     CIppfeagZUcAlYRn36352610 = CIppfeagZUcAlYRn16339837;     CIppfeagZUcAlYRn16339837 = CIppfeagZUcAlYRn63215869;     CIppfeagZUcAlYRn63215869 = CIppfeagZUcAlYRn11754641;     CIppfeagZUcAlYRn11754641 = CIppfeagZUcAlYRn13483638;     CIppfeagZUcAlYRn13483638 = CIppfeagZUcAlYRn96109879;     CIppfeagZUcAlYRn96109879 = CIppfeagZUcAlYRn36293972;     CIppfeagZUcAlYRn36293972 = CIppfeagZUcAlYRn97857577;     CIppfeagZUcAlYRn97857577 = CIppfeagZUcAlYRn86096391;     CIppfeagZUcAlYRn86096391 = CIppfeagZUcAlYRn3513863;     CIppfeagZUcAlYRn3513863 = CIppfeagZUcAlYRn27077928;     CIppfeagZUcAlYRn27077928 = CIppfeagZUcAlYRn24893810;     CIppfeagZUcAlYRn24893810 = CIppfeagZUcAlYRn312719;     CIppfeagZUcAlYRn312719 = CIppfeagZUcAlYRn83156951;     CIppfeagZUcAlYRn83156951 = CIppfeagZUcAlYRn79351132;     CIppfeagZUcAlYRn79351132 = CIppfeagZUcAlYRn58850083;     CIppfeagZUcAlYRn58850083 = CIppfeagZUcAlYRn9190058;     CIppfeagZUcAlYRn9190058 = CIppfeagZUcAlYRn87317175;     CIppfeagZUcAlYRn87317175 = CIppfeagZUcAlYRn67216631;     CIppfeagZUcAlYRn67216631 = CIppfeagZUcAlYRn6084107;     CIppfeagZUcAlYRn6084107 = CIppfeagZUcAlYRn91426955;     CIppfeagZUcAlYRn91426955 = CIppfeagZUcAlYRn61491313;     CIppfeagZUcAlYRn61491313 = CIppfeagZUcAlYRn76329499;     CIppfeagZUcAlYRn76329499 = CIppfeagZUcAlYRn62773025;     CIppfeagZUcAlYRn62773025 = CIppfeagZUcAlYRn71317606;     CIppfeagZUcAlYRn71317606 = CIppfeagZUcAlYRn1160541;     CIppfeagZUcAlYRn1160541 = CIppfeagZUcAlYRn41501650;     CIppfeagZUcAlYRn41501650 = CIppfeagZUcAlYRn10311568;     CIppfeagZUcAlYRn10311568 = CIppfeagZUcAlYRn59618372;     CIppfeagZUcAlYRn59618372 = CIppfeagZUcAlYRn53799418; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lzkTWLKtMQTRTKwPRTIFVrLDHIIg30043761() { long mKUlUVESOkZoDJeAVBpVaKOsUsruogM20668616 = -543648108;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM41719306 = -317155329;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM3869968 = -342149532;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM83129448 = -278733770;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM44122741 = -570202315;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM54822245 = -208355736;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM11619817 = -210096771;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM18106700 = -104134225;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM17521027 = 8664081;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM48333251 = -200186145;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM33133234 = -849751150;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM25838389 = -669347802;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM49979594 = -660015920;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM35543527 = -475222638;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM20780317 = -348168933;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM47761328 = -756017643;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM42349383 = -1418124;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM93070925 = -635607913;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM13643549 = -654061727;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM99660015 = -877486246;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM51629281 = -476512382;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM79700535 = -854204336;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM18585013 = -572898178;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM10684456 = -268955827;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM81341936 = -545689200;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM76069218 = -797562587;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM61481811 = -335093562;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM27941403 = -326697026;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM5271136 = -458349945;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM29271854 = -607186447;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM76059201 = -201240041;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM878905 = -305164919;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM40063468 = -561770562;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM44783318 = -492618272;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM5026524 = -861466621;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM51306814 = -486086982;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM61271719 = -875088058;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM41845428 = -699385562;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM49957544 = -552998573;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM78871534 = -66841642;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM23814053 = -17402442;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM45925151 = -964555767;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM52391441 = -110935614;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM25510011 = 30243358;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM48406587 = -445605981;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM27261457 = 79245533;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM67273064 = -591035800;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM90321592 = 15859708;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM35043161 = -750886843;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM25562126 = -303140252;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM72471616 = -457089839;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM14922767 = 99593375;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM16214358 = -640373321;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM67542697 = -536918175;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM78414480 = -245812793;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM40968080 = -689443773;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM23134293 = -744257152;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM93185512 = 26806295;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM1787513 = -733044570;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM68053523 = -772639729;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM93340433 = -873262175;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM83678413 = -883399745;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM12835564 = -645784280;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM88249172 = -384149472;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM72274049 = -998946104;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM32254330 = -444586231;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM85774921 = -7577241;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM5196277 = -67397649;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM30517004 = -613756018;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM69473503 = -862081951;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM86489608 = -880929585;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM503956 = -302032562;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM43113382 = 17390660;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM34772015 = -487220085;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM75845963 = -760083804;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM5704130 = -511956615;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM27309094 = -643268722;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM93075001 = -503141536;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM62277868 = -823349846;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM54080479 = -524934733;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM8796154 = -106526787;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM71160219 = -250953271;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM92898242 = -575810183;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM79709010 = -55209694;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM56800238 = -50096609;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM61136435 = -200833417;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM84664546 = -664791599;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM72520770 = 75147613;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM66368837 = -146805479;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM64058443 = -72022848;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM28172521 = -741829830;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM68086207 = -801894354;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM40057916 = -966340992;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM81904020 = -780358845;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM85531100 = -193579468;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM40135640 = -134002698;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM33089588 = -218771488;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM64142268 = -726786142;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM53235962 = 29189461;    long mKUlUVESOkZoDJeAVBpVaKOsUsruogM16152257 = -543648108;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM20668616 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM41719306;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM41719306 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM3869968;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM3869968 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM83129448;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM83129448 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM44122741;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM44122741 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM54822245;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM54822245 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM11619817;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM11619817 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM18106700;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM18106700 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM17521027;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM17521027 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM48333251;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM48333251 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM33133234;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM33133234 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM25838389;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM25838389 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM49979594;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM49979594 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM35543527;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM35543527 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM20780317;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM20780317 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM47761328;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM47761328 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM42349383;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM42349383 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM93070925;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM93070925 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM13643549;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM13643549 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM99660015;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM99660015 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM51629281;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM51629281 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM79700535;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM79700535 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM18585013;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM18585013 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM10684456;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM10684456 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM81341936;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM81341936 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM76069218;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM76069218 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM61481811;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM61481811 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM27941403;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM27941403 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM5271136;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM5271136 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM29271854;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM29271854 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM76059201;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM76059201 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM878905;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM878905 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM40063468;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM40063468 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM44783318;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM44783318 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM5026524;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM5026524 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM51306814;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM51306814 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM61271719;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM61271719 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM41845428;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM41845428 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM49957544;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM49957544 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM78871534;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM78871534 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM23814053;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM23814053 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM45925151;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM45925151 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM52391441;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM52391441 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM25510011;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM25510011 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM48406587;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM48406587 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM27261457;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM27261457 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM67273064;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM67273064 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM90321592;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM90321592 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM35043161;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM35043161 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM25562126;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM25562126 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM72471616;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM72471616 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM14922767;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM14922767 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM16214358;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM16214358 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM67542697;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM67542697 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM78414480;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM78414480 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM40968080;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM40968080 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM23134293;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM23134293 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM93185512;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM93185512 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM1787513;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM1787513 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM68053523;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM68053523 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM93340433;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM93340433 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM83678413;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM83678413 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM12835564;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM12835564 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM88249172;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM88249172 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM72274049;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM72274049 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM32254330;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM32254330 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM85774921;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM85774921 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM5196277;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM5196277 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM30517004;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM30517004 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM69473503;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM69473503 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM86489608;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM86489608 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM503956;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM503956 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM43113382;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM43113382 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM34772015;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM34772015 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM75845963;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM75845963 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM5704130;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM5704130 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM27309094;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM27309094 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM93075001;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM93075001 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM62277868;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM62277868 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM54080479;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM54080479 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM8796154;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM8796154 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM71160219;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM71160219 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM92898242;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM92898242 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM79709010;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM79709010 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM56800238;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM56800238 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM61136435;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM61136435 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM84664546;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM84664546 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM72520770;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM72520770 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM66368837;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM66368837 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM64058443;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM64058443 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM28172521;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM28172521 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM68086207;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM68086207 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM40057916;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM40057916 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM81904020;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM81904020 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM85531100;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM85531100 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM40135640;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM40135640 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM33089588;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM33089588 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM64142268;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM64142268 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM53235962;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM53235962 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM16152257;     mKUlUVESOkZoDJeAVBpVaKOsUsruogM16152257 = mKUlUVESOkZoDJeAVBpVaKOsUsruogM20668616; }
// Junk Finished
