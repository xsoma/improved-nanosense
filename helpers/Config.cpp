#include "../SDK.hpp"

#include "Config.hpp"
#include "json.hpp"

#include "../Options.hpp"

#include <fstream>
#include <experimental/filesystem> // hack

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

nlohmann::json config;

void Config::CreateConfigFolder(std::string path)
{
	if (!std::experimental::filesystem::create_directory(path)) return;
}

bool Config::FileExists(std::string file)
{
	return std::experimental::filesystem::exists(file);
}

void Config::SaveConfig(const std::string path)
{
	std::ofstream output_file(path);

	if (!output_file.good())
		return;
	Save(XSystemCFG.anti_obs, "anti_obs");
	Save(XSystemCFG.misc_thirdperson_distance, "thirdperson_distance");
	Save(XSystemCFG.lgtpew_rcsx, "lgtpew_rcsx");
	Save(XSystemCFG.lgtpew_rcsy, "lgtpew_rcsy");
	Save(XSystemCFG.angrpwaimtype, "angrpwaimtype");
	Save(XSystemCFG.angrpwaccboost, "angrpwaccboost");
	Save(XSystemCFG.angrpwfacboost, "angrpwfacboost");
	Save(XSystemCFG.angrpwresastype, "angrpwresastype");
	Save(XSystemCFG.angrpwfov, "angrpwfov");
	Save(XSystemCFG.legit_backtrack, "legitbacktrack");
	Save(XSystemCFG.lgtpew_drawfov, "lgtpew_drawfov");
	Save(XSystemCFG.indicators_enabled, "indicators");
	Save(XSystemCFG.esp_enemies_only, "enemyonlyesp");
	Save(XSystemCFG.misc_antikick, "antivotekick");
	Save(XSystemCFG.misc_devinfo, "devinfo");
	Save(XSystemCFG.onlytextnocheckbox, "box");
	Save(XSystemCFG.onlytextboxfill, "boxfill");
	Save(XSystemCFG.esp_player_ammo, "player_ammo");
	Save(XSystemCFG.misc_autoaccept, "misc_autoaccept");
	Save(XSystemCFG.misc_revealAllRanks, "misc_revealAllRanks");
	Save(XSystemCFG.misc_bhop, "misc_bhop");
	Save(XSystemCFG.misc_autostrafe, "misc_autostrafe");
	Save(XSystemCFG.misc_auto_pistol, "misc_auto_pistol");
	Save(XSystemCFG.misc_chatspamer, "misc_chatspamer");
	Save(XSystemCFG.misc_thirdperson, "misc_thirdperson");
	Save(XSystemCFG.misc_thirdperson_bind, "misc_thirdperson_bind");
	Save(XSystemCFG.misc_fakewalk, "misc_fakewalk");
	Save(XSystemCFG.misc_fakewalk_bind, "misc_fakewalk_bind");
	Save(XSystemCFG.misc_fakelag_enabled, "misc_fakelag_enabled");
	Save(XSystemCFG.misc_fakelag_value, "misc_fakelag_value");
	Save(XSystemCFG.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Save(XSystemCFG.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	//Save(XSystemCFG.misc_animated_clantag, "misc_animated_clantag");
	Save(XSystemCFG.misc_spectatorlist, "misc_spectatorlist");
	Save(XSystemCFG.misc_logevents, "misc_logevents");
	Save(XSystemCFG.hvh_resolver_custom, "hvh_resolver_custom");
	Save(XSystemCFG.resolver_eyelby, "resolver_eyelby");
	Save(XSystemCFG.resolver_eyelby_always, "resolver_eyelby_always");
	Save(XSystemCFG.resolver_eyelby_running, "resolver_eyelby_running");
	Save(XSystemCFG.resolver_eyelby_notrunning, "resolver_eyelby");
	Save(XSystemCFG.resolver_eyelby_running_value, "resolver_eyelby_running_value");
	Save(XSystemCFG.removals_flash, "removals_flash");
	Save(XSystemCFG.removals_smoke, "removals_smoke");
	Save(XSystemCFG.removals_smoke_type, "removals_smoke_type");
	Save(XSystemCFG.removals_scope, "removals_scope");
	Save(XSystemCFG.removals_novisualrecoil, "removals_novisualrecoil");
	Save(XSystemCFG.removals_postprocessing, "removals_postprocessing");
	Save(XSystemCFG.removals_crosshair, "removals_crosshair");
	Save(XSystemCFG.force_backtrack, "force_backtrack");
	Save(XSystemCFG.esp_farther, "esp_farther");
	Save(XSystemCFG.esp_fill_amount, "esp_fill_amount");
	SaveArray(XSystemCFG.esp_player_fill_color_t, "esp_player_fill_color_t");
	SaveArray(XSystemCFG.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	SaveArray(XSystemCFG.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	SaveArray(XSystemCFG.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Save(XSystemCFG.esp_player_boundstype, "esp_player_boundstype");
	Save(XSystemCFG.esp_player_boxtype, "esp_player_boxtype");
	SaveArray(XSystemCFG.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	SaveArray(XSystemCFG.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	SaveArray(XSystemCFG.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	SaveArray(XSystemCFG.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Save(XSystemCFG.esp_player_name, "esp_player_name");
	Save(XSystemCFG.esp_player_health, "esp_player_health");
	Save(XSystemCFG.esp_player_weapons, "esp_player_weapons");
	Save(XSystemCFG.esp_player_snaplines, "esp_player_snaplines");
	Save(XSystemCFG.esp_player_chams, "esp_player_chams");
	Save(XSystemCFG.esp_player_chams_type, "esp_player_chams_type");
	SaveArray(XSystemCFG.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	SaveArray(XSystemCFG.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	SaveArray(XSystemCFG.esp_player_chams_color_t, "esp_player_chams_color_t");
	SaveArray(XSystemCFG.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Save(XSystemCFG.esp_player_skelet, "esp_player_skelet");
	Save(XSystemCFG.esp_player_anglelines, "esp_player_anglelines");
	Save(XSystemCFG.esp_dropped_weapons, "esp_dropped_weapons");
	Save(XSystemCFG.esp_planted_c4, "esp_planted_c4");
	Save(XSystemCFG.esp_grenades, "esp_grenades");
	Save(XSystemCFG.esp_grenades_type, "esp_grenades_type");
	Save(XSystemCFG.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Save(XSystemCFG.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Save(XSystemCFG.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Save(XSystemCFG.visuals_manual_aa, "visuals_manual_aa");
	Save(XSystemCFG.visuals_others_player_fov, "visuals_others_player_fov");
	Save(XSystemCFG.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	//Save(XSystemCFG.visuals_others_watermark, "visuals_others_watermark");
	Save(XSystemCFG.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Save(XSystemCFG.visuals_others_hitmarker, "visuals_others_hitmarker");
	Save(XSystemCFG.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	SaveArray(XSystemCFG.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	Save(XSystemCFG.visuals_others_sky, "visuals_others_sky");
	Save(XSystemCFG.glow_enabled, "glow_enabled");
	Save(XSystemCFG.glow_players, "glow_players");
	SaveArray(XSystemCFG.glow_player_color_t, "glow_player_color_t");
	SaveArray(XSystemCFG.glow_player_color_ct, "glow_player_color_ct");
	SaveArray(XSystemCFG.glow_player_color_t_visible, "glow_player_color_t_visible");
	SaveArray(XSystemCFG.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Save(XSystemCFG.glow_players_style, "glow_players_style");
	Save(XSystemCFG.glow_others, "glow_others");
	Save(XSystemCFG.glow_others_style, "glow_others_style");
	Save(XSystemCFG.lgtpew_enabled, "lgtpew_enabled");
	Save(XSystemCFG.lgtpew_hsonly, "lgtpew_hsonly");
	Save(XSystemCFG.lgtpew_rcs, "lgtpew_rcs");
	Save(XSystemCFG.lgtpew_trigger, "lgtpew_trigger");
	Save(XSystemCFG.lgtpew_on_press, "lgtpew_on_press");
	Save(XSystemCFG.trigger_on_press, "trigger_on_press");
	Save(XSystemCFG.rcs_on_press, "rcs_on_press");
	Save(XSystemCFG.lgtpew_aim_keybind, "lgtpew_aim_keybind");
	Save(XSystemCFG.rcs_keybind, "rcs_keybind");
	Save(XSystemCFG.trigger_keybind, "trigger_keybind");
	Save(XSystemCFG.lgtpew_preaim, "lgtpew_preaim");
	Save(XSystemCFG.lgtpew_aftershots, "lgtpew_aftershots");
	Save(XSystemCFG.lgtpew_afteraim, "lgtpew_afteraim");
	Save(XSystemCFG.lgtpew_smooth_factor, "lgtpew_smooth_factor");
	Save(XSystemCFG.lgtpew_fov, "lgtpew_fov");
	Save(XSystemCFG.angrpwenabled, "angrpwenabled");
	Save(XSystemCFG.angrpwaimkey, "angrpwaimkey");
	Save(XSystemCFG.angrpwsilent, "angrpwsilent");
	Save(XSystemCFG.angrpwnorecoil, "angrpwnorecoil");
	Save(XSystemCFG.angrpwautoshoot, "angrpwautoshoot");
	Save(XSystemCFG.angrpwautoscope, "angrpwautoscope");
	Save(XSystemCFG.angrpwautocrouch, "angrpwautocrouch");
	Save(XSystemCFG.angrpwautostop, "angrpwautostop");
	Save(XSystemCFG.angrpwautobaim, "angrpwautobaim");
	Save(XSystemCFG.visuals_draw_xhair, "visuals_draw_xhair");
	Save(XSystemCFG.visuals_xhair_x, "visuals_xhair_x");
	Save(XSystemCFG.visuals_xhair_y, "visuals_xhair_y");
	Save(XSystemCFG.angrpwautocockrevolver, "angrpwautocockrevolver");
	Save(XSystemCFG.angrpwbaim_after_x_shots, "angrpwbaim_after_x_shots");
	Save(XSystemCFG.angrpwlagcompensation, "angrpwlagcompensation");
	Save(XSystemCFG.esp_localplayer_chams_xyz, "esp_localplayer_chams_xyz");
	Save(XSystemCFG.angrpwfixup_entities, "angrpwfixup_entities");
	Save(XSystemCFG.angrpwmindmg, "angrpwmindmg");
	Save(XSystemCFG.angrpwhitchance_amount, "angrpwhitchance_amount");
	Save(XSystemCFG.hvh_antiaim_x, "hvh_antiaim_x");
	Save(XSystemCFG.hvh_antiaim_y, "hvh_antiaim_y");
	Save(XSystemCFG.hvh_show_real_angles, "hvh_show_real_angles");
	Save(XSystemCFG.hvh_resolver, "hvh_resolver");
	Save(XSystemCFG.visuals_manual_aa_opacity, "visuals_manual_aa_opacity");
	Save(XSystemCFG.hvh_resolver_disabled_draw, "hvh_resolver_disabled_draw");
	Save(XSystemCFG.hvh_resolver_override, "hvh_resolver_override");
	Save(XSystemCFG.hvh_resolver_override_key, "hvh_resolver_override_key");
	Save(XSystemCFG.hvh_resolver_disable_key, "hvh_resolver_disable_key");
	Save(XSystemCFG.skinchanger_enabled, "skinchanger_enabled");
	Save(XSystemCFG.visuals_asuswalls_world, "visuals_asuswalls_world");
	Save(XSystemCFG.visuals_asuswalls_staticprops, "visuals_asuswalls_staticprops");
	Save(XSystemCFG.hvh_antiaim_y_desync, "hvh_antiaim_y_desync");
	Save(XSystemCFG.menu_open_alternative, "menu_open_alternative");
	Save(XSystemCFG.esp_localplayer_chams, "esp_localplayer_chams");
	Save(XSystemCFG.esp_localplayer_chams_type, "esp_localplayer_chams_type");
	SaveArray(XSystemCFG.esp_localplayer_chams_color_invisible, "esp_localplayer_chams_color_invisible");
	SaveArray(XSystemCFG.esp_localplayer_chams_color_visible, "esp_localplayer_chams_color_visible");
	Save(XSystemCFG.esp_localplayer_fakechams_type, "esp_localplayer_fakechams_type");
	SaveArray(XSystemCFG.esp_localplayer_fakechams_color_invisible, "esp_localplayer_fakechams_color_invisible");
	SaveArray(XSystemCFG.esp_localplayer_fakechams_color_visible, "esp_localplayer_fakechams_color_visible");
	Save(XSystemCFG.angrpwhitchance_amount, "angrpwhitchance_amount_auto");
	Save(XSystemCFG.dropped_weapons_color[0], "dropped_weapons_color_red");
	Save(XSystemCFG.dropped_weapons_color[1], "dropped_weapons_color_green");
	Save(XSystemCFG.dropped_weapons_color[2], "dropped_weapons_color_blue");
	Save(XSystemCFG.fake_chams, "fake_chams");
	Save(XSystemCFG.angrpwmindmg_amount, "angrpwmindmg_amount");
	Save(XSystemCFG.hvh_antiaim_y_custom_left, "hvh_antiaim_y_custom_left");
	Save(XSystemCFG.visuals_nightmode, "visuals_nightmode");
	SaveArray(XSystemCFG.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	Save(XSystemCFG.visuals_nightmode_type, "visuals_nightmode_type");
	Save(XSystemCFG.cl_phys_timescale, "cl_phys_timescale");
	Save(XSystemCFG.cl_phys_timescale_value, "cl_phys_timescale_value");
	Save(XSystemCFG.hvh_aa_left_bind, "hvh_aa_left_bind");
	Save(XSystemCFG.hvh_aa_right_bind, "hvh_aa_right_bind");
	Save(XSystemCFG.hvh_aa_back_bind, "hvh_aa_back_bind");
	Save(XSystemCFG.removals_zoom, "removals_zoom");
	Save(XSystemCFG.visuals_asuswalls, "visuals_asuswalls");
	Save(XSystemCFG.visuals_asuswalls_value, "visuals_asuswalls_value");
	Save(XSystemCFG.hvh_antiaim_y_custom_right, "hvh_antiaim_y_custom_right");
	Save(XSystemCFG.hvh_antiaim_y_custom_back, "hvh_antiaim_y_custom_back");
	Save(XSystemCFG.mp_radar_showall, "mp_radar_showall");
	Save(XSystemCFG.hvh_antiaim_y_desync_start_right, "hvh_antiaim_y_desync_start_right");
	Save(XSystemCFG.hvh_antiaim_y_desync_start_left, "hvh_antiaim_y_desync_start_left");
	Save(XSystemCFG.hvh_antiaim_y_desync_start_back, "hvh_antiaim_y_desync_start_back");
	Save(XSystemCFG.lgtpew_on_key, "lgtpew_on_key");
	Save(XSystemCFG.xhair_color[0], "xhair_color_red");
	Save(XSystemCFG.xhair_color[1], "xhair_color_green");
	Save(XSystemCFG.xhair_color[2], "xhair_color_blue");
	Save(XSystemCFG.angrpwpointscale_amount, "angrpwpointscale_amount");
	SaveArray(XSystemCFG.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	SaveArray(XSystemCFG.visuals_others_skybox_color, "visuals_others_skybox_color");
	Save(XSystemCFG.fog_override, "fog_override");
	Save(XSystemCFG.change_viewmodel_offset, "change_viewmodel_offset");
	Save(XSystemCFG.viewmodel_offset_x, "viewmodel_offset_x");
	Save(XSystemCFG.viewmodel_offset_y, "viewmodel_offset_y");
	Save(XSystemCFG.viewmodel_offset_z, "viewmodel_offset_z");
	Save(XSystemCFG.iangrpwhitbox, "iangrpwhitbox");
	Save(XSystemCFG.misc_fakewalk_speed, "misc_fakewalk_speed");
	Save(XSystemCFG.bangrpwmultipoint, "bangrpwmultipoint");
	Save(XSystemCFG.hvh_antiaim_y_move, "hvh_antiaim_y_move");
	Save(XSystemCFG.hvh_antiaim_y_custom_realmove_left, "hvh_antiaim_y_custom_realmove_left");
	Save(XSystemCFG.hvh_antiaim_y_custom_realmove_right, "hvh_antiaim_y_custom_realmove_right");
	Save(XSystemCFG.hvh_antiaim_y_custom_realmove_back, "hvh_antiaim_y_custom_realmove_back");
	Save(XSystemCFG.hvh_antiaim_y_move_trigger_speed, "hvh_antiaim_y_move_trigger_speed");
	Save(XSystemCFG.bangrpwprioritize, "bangrpwprioritize");
	Save(XSystemCFG.cl_crosshair_recoil, "cl_crosshair_recoil");
	SaveArray(XSystemCFG.angrpwmultiHitboxesAll, "angrpwmultiHitboxesAll");
	Save(XSystemCFG.misc_infinite_duck, "misc_infinite_duck");
	Save(XSystemCFG.esp_localplayer_viewmodel_chams, "esp_localplayer_viewmodel_chams");
	Save(XSystemCFG.esp_localplayer_viewmodel_materials, "esp_localplayer_viewmodel_materials");

	output_file << std::setw(4) << config << std::endl;
	output_file.close();
}

void Config::LoadConfig(const std::string path)
{
	std::ifstream input_file(path);

	if (!input_file.good())
		return;

	try
	{
		config << input_file;
	}
	catch (...)
	{
		input_file.close();
		return;
	}
	Load(XSystemCFG.anti_obs, "anti_obs");
	Load(XSystemCFG.misc_thirdperson_distance, "thirdperson_distance");
	Load(XSystemCFG.lgtpew_rcsx, "lgtpew_rcsx");
	Load(XSystemCFG.lgtpew_rcsy, "lgtpew_rcsy");
	Load(XSystemCFG.angrpwaccboost, "angrpwaccboost");
	Load(XSystemCFG.angrpwaimtype, "angrpwaimtype");
	Load(XSystemCFG.angrpwfacboost, "angrpwfacboost");
	Load(XSystemCFG.angrpwresastype, "angrpwresastype");
	Load(XSystemCFG.angrpwfov, "angrpwfov");
	Load(XSystemCFG.legit_backtrack, "legitbacktrack");
	Load(XSystemCFG.lgtpew_drawfov, "lgtpew_drawfov");
	Load(XSystemCFG.indicators_enabled, "indicators");
	Load(XSystemCFG.esp_enemies_only, "enemyonlyesp");
	Load(XSystemCFG.misc_antikick, "antivotekick");
	Load(XSystemCFG.misc_devinfo, "devinfo");
	Load(XSystemCFG.onlytextnocheckbox, "box");
	Load(XSystemCFG.onlytextboxfill, "boxfill");
	Load(XSystemCFG.esp_player_ammo, "player_ammo");
	Load(XSystemCFG.misc_autoaccept, "misc_autoaccept");
	Load(XSystemCFG.misc_revealAllRanks, "misc_revealAllRanks");
	Load(XSystemCFG.misc_bhop, "misc_bhop");
	Load(XSystemCFG.misc_autostrafe, "misc_autostrafe");
	Load(XSystemCFG.misc_auto_pistol, "misc_auto_pistol");
	Load(XSystemCFG.misc_chatspamer, "misc_chatspamer");
	Load(XSystemCFG.misc_thirdperson, "misc_thirdperson");
	Load(XSystemCFG.misc_thirdperson_bind, "misc_thirdperson_bind");
	Load(XSystemCFG.misc_fakewalk, "misc_fakewalk");
	Load(XSystemCFG.misc_fakewalk_bind, "misc_fakewalk_bind");
	Load(XSystemCFG.visuals_manual_aa_opacity, "visuals_manual_aa_opacity");
	Load(XSystemCFG.hvh_aa_left_bind, "hvh_aa_left_bind");
	Load(XSystemCFG.hvh_aa_right_bind, "hvh_aa_right_bind");
	Load(XSystemCFG.hvh_aa_back_bind, "hvh_aa_back_bind");
	Load(XSystemCFG.misc_fakelag_enabled, "misc_fakelag_enabled");
	Load(XSystemCFG.misc_fakelag_value, "misc_fakelag_value");
	Load(XSystemCFG.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Load(XSystemCFG.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	//Load(XSystemCFG.misc_animated_clantag, "misc_animated_clantag");
	Load(XSystemCFG.misc_spectatorlist, "misc_spectatorlist");
	Load(XSystemCFG.misc_logevents, "misc_logevents");
	Load(XSystemCFG.removals_flash, "removals_flash");
	Load(XSystemCFG.removals_smoke, "removals_smoke");
	Load(XSystemCFG.removals_smoke_type, "removals_smoke_type");
	Load(XSystemCFG.removals_scope, "removals_scope");
	Load(XSystemCFG.removals_novisualrecoil, "removals_novisualrecoil");
	Load(XSystemCFG.removals_postprocessing, "removals_postprocessing");
	Load(XSystemCFG.lgtpew_on_press, "lgtpew_on_press");
	Load(XSystemCFG.lgtpew_aim_keybind, "lgtpew_aim_keybind");
	Load(XSystemCFG.trigger_on_press, "trigger_on_press");
	Load(XSystemCFG.trigger_keybind, "trigger_keybind");
	Load(XSystemCFG.rcs_on_press, "rcs_on_press");
	Load(XSystemCFG.rcs_keybind, "rcs_keybind");
	Load(XSystemCFG.removals_crosshair, "removals_crosshair");
	Load(XSystemCFG.esp_farther, "esp_farther");
	Load(XSystemCFG.cl_phys_timescale, "cl_phys_timescale");
	Load(XSystemCFG.cl_phys_timescale_value, "cl_phys_timescale_value");
	Load(XSystemCFG.esp_fill_amount, "esp_fill_amount");
	LoadArray(XSystemCFG.esp_player_fill_color_t, "esp_player_fill_color_t");
	LoadArray(XSystemCFG.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	LoadArray(XSystemCFG.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	LoadArray(XSystemCFG.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Load(XSystemCFG.esp_player_boundstype, "esp_player_boundstype");
	Load(XSystemCFG.esp_player_boxtype, "esp_player_boxtype");
	LoadArray(XSystemCFG.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	LoadArray(XSystemCFG.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	LoadArray(XSystemCFG.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	Load(XSystemCFG.hvh_resolver_custom, "hvh_resolver_custom");
	Load(XSystemCFG.resolver_eyelby, "resolver_eyelby");
	Load(XSystemCFG.resolver_eyelby_always, "resolver_eyelby_always");
	Load(XSystemCFG.resolver_eyelby_running, "resolver_eyelby_running");
	Load(XSystemCFG.resolver_eyelby_notrunning, "resolver_eyelby");
	Load(XSystemCFG.resolver_eyelby_running_value, "resolver_eyelby_running_value");
	LoadArray(XSystemCFG.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Load(XSystemCFG.esp_player_name, "esp_player_name");
	Load(XSystemCFG.esp_player_health, "esp_player_health");
	Load(XSystemCFG.esp_player_weapons, "esp_player_weapons");
	Load(XSystemCFG.esp_player_snaplines, "esp_player_snaplines");
	Load(XSystemCFG.esp_player_chams, "esp_player_chams");
	Load(XSystemCFG.esp_player_chams_type, "esp_player_chams_type");
	LoadArray(XSystemCFG.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	LoadArray(XSystemCFG.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	LoadArray(XSystemCFG.esp_player_chams_color_t, "esp_player_chams_color_t");
	LoadArray(XSystemCFG.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Load(XSystemCFG.esp_localplayer_chams, "esp_localplayer_chams");
	Load(XSystemCFG.esp_localplayer_chams_type, "esp_localplayer_chams_type");
	LoadArray(XSystemCFG.esp_localplayer_chams_color_invisible, "esp_localplayer_chams_color_invisible");
	LoadArray(XSystemCFG.esp_localplayer_chams_color_visible, "esp_localplayer_chams_color_visible");
	Load(XSystemCFG.esp_localplayer_fakechams_type, "esp_localplayer_fakechams_type");
	LoadArray(XSystemCFG.esp_localplayer_fakechams_color_invisible, "esp_localplayer_fakechams_color_invisible");
	LoadArray(XSystemCFG.esp_localplayer_fakechams_color_visible, "esp_localplayer_fakechams_color_visible");
	Load(XSystemCFG.esp_player_skelet, "esp_player_skelet");
	Load(XSystemCFG.esp_player_anglelines, "esp_player_anglelines");
	Load(XSystemCFG.esp_dropped_weapons, "esp_dropped_weapons");
	Load(XSystemCFG.dropped_weapons_color[0], "dropped_weapons_color_red");
	Load(XSystemCFG.dropped_weapons_color[1], "dropped_weapons_color_green");
	Load(XSystemCFG.dropped_weapons_color[2], "dropped_weapons_color_blue");
	Load(XSystemCFG.xhair_color[0], "xhair_color_red");
	Load(XSystemCFG.xhair_color[1], "xhair_color_green");
	Load(XSystemCFG.xhair_color[2], "xhair_color_blue");
	Load(XSystemCFG.change_viewmodel_offset, "change_viewmodel_offset");
	Load(XSystemCFG.viewmodel_offset_x, "viewmodel_offset_x");
	Load(XSystemCFG.viewmodel_offset_y, "viewmodel_offset_y");
	Load(XSystemCFG.viewmodel_offset_z, "viewmodel_offset_z");
	Load(XSystemCFG.fog_override, "fog_override");
	Load(XSystemCFG.visuals_asuswalls_world, "visuals_asuswalls_world");
	Load(XSystemCFG.visuals_asuswalls_staticprops, "visuals_asuswalls_staticprops");
	Load(XSystemCFG.esp_planted_c4, "esp_planted_c4");
	Load(XSystemCFG.esp_grenades, "esp_grenades");
	Load(XSystemCFG.esp_grenades_type, "esp_grenades_type");
	Load(XSystemCFG.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Load(XSystemCFG.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Load(XSystemCFG.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Load(XSystemCFG.visuals_manual_aa, "visuals_manual_aa");
	Load(XSystemCFG.visuals_others_player_fov, "visuals_others_player_fov");
	Load(XSystemCFG.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	//	Load(XSystemCFG.visuals_others_watermark, "visuals_others_watermark");
	Load(XSystemCFG.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Load(XSystemCFG.visuals_draw_xhair, "visuals_draw_xhair");
	Load(XSystemCFG.visuals_xhair_x, "visuals_xhair_x");
	Load(XSystemCFG.visuals_xhair_y, "visuals_xhair_y");
	Load(XSystemCFG.visuals_others_hitmarker, "visuals_others_hitmarker");
	Load(XSystemCFG.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	LoadArray(XSystemCFG.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	LoadArray(XSystemCFG.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	LoadArray(XSystemCFG.visuals_others_skybox_color, "visuals_others_skybox_color");
	Load(XSystemCFG.visuals_others_sky, "visuals_others_sky");
	Load(XSystemCFG.removals_zoom, "removals_zoom");
	Load(XSystemCFG.glow_enabled, "glow_enabled");
	Load(XSystemCFG.glow_players, "glow_players");
	LoadArray(XSystemCFG.glow_player_color_t, "glow_player_color_t");
	LoadArray(XSystemCFG.glow_player_color_ct, "glow_player_color_ct");
	LoadArray(XSystemCFG.glow_player_color_t_visible, "glow_player_color_t_visible");
	LoadArray(XSystemCFG.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Load(XSystemCFG.glow_players_style, "glow_players_style");
	Load(XSystemCFG.glow_others, "glow_others");
	Load(XSystemCFG.glow_others_style, "glow_others_style");
	Load(XSystemCFG.lgtpew_enabled, "lgtpew_enabled");
	Load(XSystemCFG.lgtpew_hsonly, "lgtpew_hsonly");
	Load(XSystemCFG.lgtpew_rcs, "lgtpew_rcs");
	Load(XSystemCFG.lgtpew_trigger, "lgtpew_trigger");
	Load(XSystemCFG.lgtpew_trigger_with_aimkey, "lgtpew_trigger_with_aimkey");
	Load(XSystemCFG.lgtpew_preaim, "lgtpew_preaim");
	Load(XSystemCFG.lgtpew_aftershots, "lgtpew_aftershots");
	Load(XSystemCFG.lgtpew_afteraim, "lgtpew_afteraim");
	Load(XSystemCFG.lgtpew_smooth_factor, "lgtpew_smooth_factor");
	Load(XSystemCFG.lgtpew_fov, "lgtpew_fov");
	Load(XSystemCFG.angrpwenabled, "angrpwenabled");
	Load(XSystemCFG.angrpwaimkey, "angrpwaimkey");
	Load(XSystemCFG.angrpwsilent, "angrpwsilent");
	Load(XSystemCFG.misc_infinite_duck, "misc_infinite_duck");
	Load(XSystemCFG.angrpwnorecoil, "angrpwnorecoil");
	Load(XSystemCFG.angrpwautoshoot, "angrpwautoshoot");
	Load(XSystemCFG.force_backtrack, "force_backtrack");
	Load(XSystemCFG.angrpwautoscope, "angrpwautoscope");
	Load(XSystemCFG.angrpwautocrouch, "angrpwautocrouch");
	Load(XSystemCFG.angrpwautostop, "angrpwautostop");
	Load(XSystemCFG.angrpwautobaim, "angrpwautobaim");
	Load(XSystemCFG.esp_localplayer_chams_xyz, "esp_localplayer_chams_xyz");
	Load(XSystemCFG.angrpwautocockrevolver, "angrpwautocockrevolver");
	Load(XSystemCFG.visuals_nightmode, "visuals_nightmode");
	LoadArray(XSystemCFG.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	Load(XSystemCFG.visuals_nightmode_type, "visuals_nightmode_type");
	Load(XSystemCFG.visuals_asuswalls, "visuals_asuswalls");
	Load(XSystemCFG.visuals_asuswalls_value, "visuals_asuswalls_value");
	Load(XSystemCFG.angrpwbaim_after_x_shots, "angrpwbaim_after_x_shots");
	Load(XSystemCFG.angrpwlagcompensation, "angrpwlagcompensation");
	Load(XSystemCFG.hvh_antiaim_y_custom_left, "hvh_antiaim_y_custom_left");
	Load(XSystemCFG.hvh_antiaim_y_custom_right, "hvh_antiaim_y_custom_right");
	Load(XSystemCFG.hvh_antiaim_y_custom_back, "hvh_antiaim_y_custom_back");
	Load(XSystemCFG.mp_radar_showall, "mp_radar_showall");
	Load(XSystemCFG.cl_crosshair_recoil, "cl_crosshair_recoil");
	Load(XSystemCFG.angrpwfixup_entities, "angrpwfixup_entities");
	Load(XSystemCFG.angrpwmindmg, "angrpwmindmg");
	Load(XSystemCFG.angrpwhitchance_amount, "angrpwhitchance_amount");
	Load(XSystemCFG.fake_chams, "fake_chams");
	Load(XSystemCFG.hvh_antiaim_x, "hvh_antiaim_x");
	Load(XSystemCFG.hvh_antiaim_y, "hvh_antiaim_y");
	Load(XSystemCFG.hvh_antiaim_y_move, "hvh_antiaim_y_move");
	Load(XSystemCFG.hvh_antiaim_y_custom_realmove_left, "hvh_antiaim_y_custom_realmove_left");
	Load(XSystemCFG.hvh_antiaim_y_custom_realmove_right, "hvh_antiaim_y_custom_realmove_right");
	Load(XSystemCFG.hvh_antiaim_y_custom_realmove_back, "hvh_antiaim_y_custom_realmove_back");
	Load(XSystemCFG.hvh_antiaim_y_move_trigger_speed, "hvh_antiaim_y_move_trigger_speed");
	Load(XSystemCFG.hvh_show_real_angles, "hvh_show_real_angles");
	Load(XSystemCFG.hvh_resolver, "hvh_resolver");
	Load(XSystemCFG.hvh_resolver_disabled_draw, "hvh_resolver_disabled_draw");
	Load(XSystemCFG.hvh_resolver_override, "hvh_resolver_override");
	Load(XSystemCFG.hvh_resolver_override_key, "hvh_resolver_override_key");
	Load(XSystemCFG.hvh_resolver_disable_key, "hvh_resolver_disable_key");
	Load(XSystemCFG.hvh_antiaim_y_desync_start_back, "hvh_antiaim_y_desync_start_back");
	Load(XSystemCFG.hvh_antiaim_y_desync_start_right, "hvh_antiaim_y_desync_start_right");
	Load(XSystemCFG.hvh_antiaim_y_desync_start_left, "hvh_antiaim_y_desync_start_left");
	Load(XSystemCFG.skinchanger_enabled, "skinchanger_enabled");
	Load(XSystemCFG.angrpwhitchance_amount, "angrpwhitchance_amount");
	Load(XSystemCFG.angrpwmindmg_amount, "angrpwmindmg_amount");
	Load(XSystemCFG.angrpwpointscale_amount, "angrpwpointscale_amount");
	Load(XSystemCFG.iangrpwhitbox, "iangrpwhitbox");
	Load(XSystemCFG.misc_fakewalk_speed, "misc_fakewalk_speed");
	Load(XSystemCFG.bangrpwmultipoint, "bangrpwmultipoint");
	Load(XSystemCFG.bangrpwprioritize, "bangrpwprioritize");
	LoadArray(XSystemCFG.angrpwmultiHitboxesAll, "angrpwmultiHitboxesAll");
	Load(XSystemCFG.lgtpew_on_key, "lgtpew_on_key");
	Load(XSystemCFG.menu_open_alternative, "menu_open_alternative");
	Load(XSystemCFG.hvh_antiaim_y_desync, "hvh_antiaim_y_desync");
	Load(XSystemCFG.lgtpew_on_press, "lgtpew_on_press");
	Load(XSystemCFG.trigger_on_press, "trigger_on_press");
	Load(XSystemCFG.rcs_on_press, "rcs_on_press");
	Load(XSystemCFG.lgtpew_aim_keybind, "lgtpew_aim_keybind");
	Load(XSystemCFG.rcs_keybind, "rcs_keybind");
	Load(XSystemCFG.trigger_keybind, "trigger_keybind");
	Load(XSystemCFG.esp_localplayer_viewmodel_chams, "esp_localplayer_viewmodel_chams");
	Load(XSystemCFG.esp_localplayer_viewmodel_materials, "esp_localplayer_viewmodel_materials");

	input_file.close();
}

std::vector<std::string> Config::GetAllConfigs()
{
	namespace fs = std::experimental::filesystem;

	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\";

	std::vector<ConfigFile> config_files = GetAllConfigsInFolder(fPath, ".cfg");
	std::vector<std::string> config_file_names;

	for (auto config = config_files.begin(); config != config_files.end(); config++)
		config_file_names.emplace_back(config->GetName());

	std::sort(config_file_names.begin(), config_file_names.end());

	return config_file_names;
}


std::vector<std::string> Config::GetAllLuas()
{
	namespace xd = std::experimental::filesystem;

	std::string luaPath = std::string(Global::my_documents_folder) + "\\nanosense\\lua\\";

	std::vector<LuaFiles> lua_files = GetAllLuasInFolder(luaPath, ".lua");
	std::vector<std::string> lua_file_names;

	for (auto luas = lua_files.begin(); luas != lua_files.end(); luas++)
		lua_file_names.emplace_back(luas->GetName());

	std::sort(lua_file_names.begin(), lua_file_names.end());

	return lua_file_names;
}

std::vector<LuaFiles> Config::GetAllLuasInFolder(const std::string path, const std::string ext)
{
	namespace xd = std::experimental::filesystem;

	std::vector<LuaFiles> lua_files;

	if (xd::exists(path) && xd::is_directory(path))
	{
		for (auto it = xd::recursive_directory_iterator(path); it != xd::recursive_directory_iterator(); it++)
		{
			if (xd::is_regular_file(*it) && it->path().extension() == ext)
			{
				std::string luaPath = path + it->path().filename().string();

				std::string tmp_f_name = it->path().filename().string();
				size_t pos = tmp_f_name.find(".");
				std::string luaName = (std::string::npos == pos) ? tmp_f_name : tmp_f_name.substr(0, pos);

				LuaFiles new_lua(luaName, luaPath);
				lua_files.emplace_back(new_lua);
			}
		}
	}
	return lua_files;
}

std::vector<ConfigFile> Config::GetAllConfigsInFolder(const std::string path, const std::string ext)
{
	namespace fs = std::experimental::filesystem;

	std::vector<ConfigFile> config_files;

	if (fs::exists(path) && fs::is_directory(path))
	{
		for (auto it = fs::recursive_directory_iterator(path); it != fs::recursive_directory_iterator(); it++)
		{
			if (fs::is_regular_file(*it) && it->path().extension() == ext)
			{
				std::string fPath = path + it->path().filename().string();

				std::string tmp_f_name = it->path().filename().string();
				size_t pos = tmp_f_name.find(".");
				std::string fName = (std::string::npos == pos) ? tmp_f_name : tmp_f_name.substr(0, pos);

				ConfigFile new_config(fName, fPath);
				config_files.emplace_back(new_config);
			}
		}
	}
	return config_files;
}

template<typename T>
void Config::Load(T& value, std::string str)
{
	if (config[str].empty())
		return;

	value = config[str].get<T>();
}

void Config::LoadArray(float_t value[4], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty())
		return;

	value[0] = config[str]["0"].get<float_t>();
	value[1] = config[str]["1"].get<float_t>();
	value[2] = config[str]["2"].get<float_t>();
	value[3] = config[str]["3"].get<float_t>();
}

void Config::LoadArray(bool value[14], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty()
		|| config[str]["4"].empty() || config[str]["5"].empty() || config[str]["6"].empty() || config[str]["7"].empty()
		|| config[str]["8"].empty() || config[str]["9"].empty() || config[str]["10"].empty() || config[str]["11"].empty()
		|| config[str]["12"].empty() || config[str]["13"].empty())
		return;

	value[0] = config[str]["0"].get<bool>();
	value[1] = config[str]["1"].get<bool>();
	value[2] = config[str]["2"].get<bool>();
	value[3] = config[str]["3"].get<bool>();
	value[4] = config[str]["4"].get<bool>();
	value[5] = config[str]["5"].get<bool>();
	value[6] = config[str]["6"].get<bool>();
	value[7] = config[str]["7"].get<bool>();
	value[8] = config[str]["8"].get<bool>();
	value[9] = config[str]["9"].get<bool>();
	value[10] = config[str]["10"].get<bool>();
	value[11] = config[str]["11"].get<bool>();
	value[12] = config[str]["12"].get<bool>();
	value[13] = config[str]["13"].get<bool>();
}

template<typename T>
void Config::Save(T& value, std::string str)
{
	config[str] = value;
}

void Config::SaveArray(float_t value[4], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
}

void Config::SaveArray(bool value[14], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
	config[str]["4"] = value[4];
	config[str]["5"] = value[5];
	config[str]["6"] = value[6];
	config[str]["7"] = value[7];
	config[str]["8"] = value[8];
	config[str]["9"] = value[9];
	config[str]["10"] = value[10];
	config[str]["11"] = value[11];
	config[str]["12"] = value[12];
	config[str]["13"] = value[13];
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void OLBLNIfAdSdFaeZwRsPglPfTejSzWWvDTqawcRQe54161996() {     int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86610562 = -95128344;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45276852 = -280347568;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13506264 = -861563256;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT16262476 = -778917585;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT65594633 = -267796292;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT78623824 = -110097491;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT39368253 = -697469692;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT93679135 = -495714786;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT34669521 = -888948836;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57158956 = -422925521;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89654313 = -835136785;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83918551 = -853931579;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89671472 = -971712213;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT91407516 = -549478154;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98397684 = -613488401;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT60275650 = -564939056;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT10295792 = -173031242;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT81691965 = -130433471;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT75869517 = -980194207;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT31322474 = -160375898;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71183325 = -462430117;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT43696449 = -395841978;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT4280675 = -669612785;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89910938 = -518914985;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71397859 = -837558384;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57122083 = -925323654;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98886094 = -311248002;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85186069 = -729405340;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT1463075 = -674970832;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13895860 = -566184810;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28620856 = -97258115;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT74734191 = -637766594;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98689836 = -123389239;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71307471 = -262377393;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45293314 = -761882722;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT12313222 = -912965975;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT38541108 = -696382669;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT96273294 = -352047157;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT32782501 = -487393560;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13029571 = -581198025;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73033456 = -728235309;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40665972 = -777733751;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT15769901 = -777693321;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76563443 = -605457676;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68887230 = -38692010;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT67489775 = -361580948;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83786950 = -863884176;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58699360 = -885026385;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT6182865 = -19102152;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88298515 = 31149308;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94922724 = -519604757;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76412219 = -236617520;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8684058 = 25152833;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68384578 = -211345327;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT82978670 = -943931700;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT42914114 = -699286366;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40996178 = -610734783;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT23595326 = -242648272;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT44864617 = -941359201;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8472551 = -342472639;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79737729 = -798849490;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT54182183 = -968064353;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT92216061 = -820743954;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT20773662 = -222764027;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28538100 = -225667406;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14920123 = -97370191;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85228715 = -630542341;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18364002 = -609334820;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT46114202 = -787595432;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86084462 = -700522426;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21734542 = -868556388;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14022497 = -820984086;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48909464 = -643039912;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT62839947 = -298996182;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58289017 = -432140590;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30517354 = -684696367;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27926549 = -618148658;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27717231 = 35844890;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21023709 = -380222976;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT3908085 = -375977436;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73335132 = 38560521;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40186734 = -426221617;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79003205 = -610303188;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13164559 = -606120140;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18973135 = 53419946;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT52208636 = -860640596;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT66050133 = -562919427;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30305259 = -912043912;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88328800 = -318445694;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT2379201 = 37403644;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71317043 = -202231193;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14945783 = -353734397;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT51408678 = -410687956;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT24309950 = -44920921;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT33291841 = -782348536;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18851273 = -760170956;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48449910 = -956989797;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94996238 = -454929294;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48025343 = -279790270;    int PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT53967108 = -95128344;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86610562 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45276852;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45276852 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13506264;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13506264 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT16262476;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT16262476 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT65594633;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT65594633 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT78623824;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT78623824 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT39368253;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT39368253 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT93679135;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT93679135 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT34669521;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT34669521 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57158956;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57158956 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89654313;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89654313 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83918551;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83918551 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89671472;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89671472 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT91407516;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT91407516 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98397684;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98397684 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT60275650;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT60275650 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT10295792;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT10295792 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT81691965;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT81691965 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT75869517;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT75869517 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT31322474;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT31322474 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71183325;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71183325 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT43696449;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT43696449 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT4280675;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT4280675 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89910938;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT89910938 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71397859;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71397859 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57122083;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT57122083 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98886094;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98886094 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85186069;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85186069 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT1463075;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT1463075 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13895860;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13895860 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28620856;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28620856 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT74734191;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT74734191 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98689836;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT98689836 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71307471;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71307471 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45293314;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT45293314 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT12313222;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT12313222 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT38541108;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT38541108 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT96273294;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT96273294 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT32782501;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT32782501 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13029571;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13029571 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73033456;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73033456 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40665972;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40665972 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT15769901;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT15769901 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76563443;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76563443 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68887230;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68887230 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT67489775;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT67489775 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83786950;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT83786950 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58699360;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58699360 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT6182865;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT6182865 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88298515;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88298515 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94922724;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94922724 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76412219;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT76412219 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8684058;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8684058 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68384578;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT68384578 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT82978670;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT82978670 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT42914114;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT42914114 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40996178;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40996178 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT23595326;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT23595326 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT44864617;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT44864617 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8472551;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT8472551 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79737729;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79737729 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT54182183;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT54182183 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT92216061;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT92216061 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT20773662;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT20773662 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28538100;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT28538100 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14920123;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14920123 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85228715;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT85228715 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18364002;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18364002 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT46114202;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT46114202 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86084462;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86084462 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21734542;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21734542 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14022497;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14022497 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48909464;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48909464 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT62839947;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT62839947 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58289017;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT58289017 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30517354;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30517354 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27926549;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27926549 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27717231;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT27717231 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21023709;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT21023709 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT3908085;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT3908085 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73335132;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT73335132 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40186734;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT40186734 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79003205;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT79003205 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13164559;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT13164559 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18973135;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18973135 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT52208636;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT52208636 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT66050133;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT66050133 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30305259;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT30305259 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88328800;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT88328800 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT2379201;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT2379201 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71317043;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT71317043 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14945783;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT14945783 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT51408678;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT51408678 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT24309950;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT24309950 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT33291841;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT33291841 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18851273;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT18851273 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48449910;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48449910 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94996238;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT94996238 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48025343;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT48025343 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT53967108;     PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT53967108 = PInRzJMAdfUZrfvzghheqfeGXZmjEcUzckQcbNUUXSWrCWMouaQukmojehuZXcwALVwGOT86610562;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aNmMIQFkFwUlFqtPQiwHjEqjSnDStaEnZvRvuDQNTnieMpPoRBh47321793() {     float JveoVXDtVNmqeshLKYxUMRjnwWMX65707502 = -470360049;    float JveoVXDtVNmqeshLKYxUMRjnwWMX20056856 = -968692005;    float JveoVXDtVNmqeshLKYxUMRjnwWMX43660493 = -578794009;    float JveoVXDtVNmqeshLKYxUMRjnwWMX36315984 = -830177838;    float JveoVXDtVNmqeshLKYxUMRjnwWMX50988667 = -596252303;    float JveoVXDtVNmqeshLKYxUMRjnwWMX97688759 = -94964514;    float JveoVXDtVNmqeshLKYxUMRjnwWMX76614164 = -373368141;    float JveoVXDtVNmqeshLKYxUMRjnwWMX98750005 = -960487940;    float JveoVXDtVNmqeshLKYxUMRjnwWMX11974570 = 37482851;    float JveoVXDtVNmqeshLKYxUMRjnwWMX91267633 = -564856185;    float JveoVXDtVNmqeshLKYxUMRjnwWMX89989190 = -808131991;    float JveoVXDtVNmqeshLKYxUMRjnwWMX31142143 = -659893925;    float JveoVXDtVNmqeshLKYxUMRjnwWMX25736917 = -421450770;    float JveoVXDtVNmqeshLKYxUMRjnwWMX32325534 = -371148209;    float JveoVXDtVNmqeshLKYxUMRjnwWMX92279900 = -933310105;    float JveoVXDtVNmqeshLKYxUMRjnwWMX14322950 = -318279396;    float JveoVXDtVNmqeshLKYxUMRjnwWMX8882606 = -191269801;    float JveoVXDtVNmqeshLKYxUMRjnwWMX29503296 = -444374997;    float JveoVXDtVNmqeshLKYxUMRjnwWMX6122565 = -961011208;    float JveoVXDtVNmqeshLKYxUMRjnwWMX59452740 = 18548473;    float JveoVXDtVNmqeshLKYxUMRjnwWMX78248183 = -394328585;    float JveoVXDtVNmqeshLKYxUMRjnwWMX54218196 = -33972484;    float JveoVXDtVNmqeshLKYxUMRjnwWMX30627260 = -978257522;    float JveoVXDtVNmqeshLKYxUMRjnwWMX1909236 = -630857843;    float JveoVXDtVNmqeshLKYxUMRjnwWMX55719834 = -10031380;    float JveoVXDtVNmqeshLKYxUMRjnwWMX63546039 = -76477445;    float JveoVXDtVNmqeshLKYxUMRjnwWMX75601458 = -571939944;    float JveoVXDtVNmqeshLKYxUMRjnwWMX32477671 = -137040968;    float JveoVXDtVNmqeshLKYxUMRjnwWMX56603561 = -129027785;    float JveoVXDtVNmqeshLKYxUMRjnwWMX87153295 = 71448257;    float JveoVXDtVNmqeshLKYxUMRjnwWMX82947297 = -747787331;    float JveoVXDtVNmqeshLKYxUMRjnwWMX35063718 = -354337431;    float JveoVXDtVNmqeshLKYxUMRjnwWMX46999200 = 41862802;    float JveoVXDtVNmqeshLKYxUMRjnwWMX46416969 = -655392194;    float JveoVXDtVNmqeshLKYxUMRjnwWMX55128231 = -171934102;    float JveoVXDtVNmqeshLKYxUMRjnwWMX45856175 = -136128987;    float JveoVXDtVNmqeshLKYxUMRjnwWMX86058805 = -759966280;    float JveoVXDtVNmqeshLKYxUMRjnwWMX25282102 = -393050933;    float JveoVXDtVNmqeshLKYxUMRjnwWMX19247551 = -395761814;    float JveoVXDtVNmqeshLKYxUMRjnwWMX74515287 = -570990260;    float JveoVXDtVNmqeshLKYxUMRjnwWMX3092424 = -353656637;    float JveoVXDtVNmqeshLKYxUMRjnwWMX48733101 = 58996720;    float JveoVXDtVNmqeshLKYxUMRjnwWMX60171437 = -977937991;    float JveoVXDtVNmqeshLKYxUMRjnwWMX72116397 = -584366868;    float JveoVXDtVNmqeshLKYxUMRjnwWMX12753363 = -711839832;    float JveoVXDtVNmqeshLKYxUMRjnwWMX32292030 = -719413711;    float JveoVXDtVNmqeshLKYxUMRjnwWMX82166129 = 8833801;    float JveoVXDtVNmqeshLKYxUMRjnwWMX15488757 = -37306311;    float JveoVXDtVNmqeshLKYxUMRjnwWMX61862104 = -716835739;    float JveoVXDtVNmqeshLKYxUMRjnwWMX64968480 = -5012764;    float JveoVXDtVNmqeshLKYxUMRjnwWMX65876456 = -60214973;    float JveoVXDtVNmqeshLKYxUMRjnwWMX67222277 = -383171776;    float JveoVXDtVNmqeshLKYxUMRjnwWMX35199284 = -650137009;    float JveoVXDtVNmqeshLKYxUMRjnwWMX50676940 = -216791798;    float JveoVXDtVNmqeshLKYxUMRjnwWMX69073610 = -969564621;    float JveoVXDtVNmqeshLKYxUMRjnwWMX11489306 = -336387565;    float JveoVXDtVNmqeshLKYxUMRjnwWMX89429596 = -990434484;    float JveoVXDtVNmqeshLKYxUMRjnwWMX41751257 = -947936167;    float JveoVXDtVNmqeshLKYxUMRjnwWMX80596149 = -720146458;    float JveoVXDtVNmqeshLKYxUMRjnwWMX87442627 = -419774858;    float JveoVXDtVNmqeshLKYxUMRjnwWMX22087302 = -523024571;    float JveoVXDtVNmqeshLKYxUMRjnwWMX44136494 = -136327173;    float JveoVXDtVNmqeshLKYxUMRjnwWMX42146444 = -731460155;    float JveoVXDtVNmqeshLKYxUMRjnwWMX24821275 = 66034593;    float JveoVXDtVNmqeshLKYxUMRjnwWMX8320336 = -817068854;    float JveoVXDtVNmqeshLKYxUMRjnwWMX54925472 = -353794561;    float JveoVXDtVNmqeshLKYxUMRjnwWMX84142943 = -601756728;    float JveoVXDtVNmqeshLKYxUMRjnwWMX79319948 = -766058576;    float JveoVXDtVNmqeshLKYxUMRjnwWMX77197303 = -99214107;    float JveoVXDtVNmqeshLKYxUMRjnwWMX46423726 = -697181119;    float JveoVXDtVNmqeshLKYxUMRjnwWMX28264145 = -558313116;    float JveoVXDtVNmqeshLKYxUMRjnwWMX83600503 = -798218869;    float JveoVXDtVNmqeshLKYxUMRjnwWMX10255745 = 51386816;    float JveoVXDtVNmqeshLKYxUMRjnwWMX31607278 = -290020949;    float JveoVXDtVNmqeshLKYxUMRjnwWMX56360317 = -627794890;    float JveoVXDtVNmqeshLKYxUMRjnwWMX29515082 = -353325306;    float JveoVXDtVNmqeshLKYxUMRjnwWMX94046759 = -56034494;    float JveoVXDtVNmqeshLKYxUMRjnwWMX58510862 = -293890654;    float JveoVXDtVNmqeshLKYxUMRjnwWMX89155872 = -919018012;    float JveoVXDtVNmqeshLKYxUMRjnwWMX23427805 = -290617670;    float JveoVXDtVNmqeshLKYxUMRjnwWMX81379910 = 14688753;    float JveoVXDtVNmqeshLKYxUMRjnwWMX60112701 = -434633633;    float JveoVXDtVNmqeshLKYxUMRjnwWMX70615566 = -420205230;    float JveoVXDtVNmqeshLKYxUMRjnwWMX91635080 = -24015022;    float JveoVXDtVNmqeshLKYxUMRjnwWMX21276839 = -868336770;    float JveoVXDtVNmqeshLKYxUMRjnwWMX15725021 = -264615556;    float JveoVXDtVNmqeshLKYxUMRjnwWMX99864434 = -704200423;    float JveoVXDtVNmqeshLKYxUMRjnwWMX96322260 = -741345401;    float JveoVXDtVNmqeshLKYxUMRjnwWMX77343359 = -685827573;    float JveoVXDtVNmqeshLKYxUMRjnwWMX43638925 = -835546538;    float JveoVXDtVNmqeshLKYxUMRjnwWMX56426579 = -145694504;    float JveoVXDtVNmqeshLKYxUMRjnwWMX44307548 = -812030114;    float JveoVXDtVNmqeshLKYxUMRjnwWMX44685953 = -672904475;    float JveoVXDtVNmqeshLKYxUMRjnwWMX31804924 = -975986956;    float JveoVXDtVNmqeshLKYxUMRjnwWMX52427986 = 52034311;    float JveoVXDtVNmqeshLKYxUMRjnwWMX58955930 = -117329464;    float JveoVXDtVNmqeshLKYxUMRjnwWMX6586658 = -209543125;    float JveoVXDtVNmqeshLKYxUMRjnwWMX35350162 = -943972584;    float JveoVXDtVNmqeshLKYxUMRjnwWMX63796061 = -767298015;    float JveoVXDtVNmqeshLKYxUMRjnwWMX57827890 = -470360049;     JveoVXDtVNmqeshLKYxUMRjnwWMX65707502 = JveoVXDtVNmqeshLKYxUMRjnwWMX20056856;     JveoVXDtVNmqeshLKYxUMRjnwWMX20056856 = JveoVXDtVNmqeshLKYxUMRjnwWMX43660493;     JveoVXDtVNmqeshLKYxUMRjnwWMX43660493 = JveoVXDtVNmqeshLKYxUMRjnwWMX36315984;     JveoVXDtVNmqeshLKYxUMRjnwWMX36315984 = JveoVXDtVNmqeshLKYxUMRjnwWMX50988667;     JveoVXDtVNmqeshLKYxUMRjnwWMX50988667 = JveoVXDtVNmqeshLKYxUMRjnwWMX97688759;     JveoVXDtVNmqeshLKYxUMRjnwWMX97688759 = JveoVXDtVNmqeshLKYxUMRjnwWMX76614164;     JveoVXDtVNmqeshLKYxUMRjnwWMX76614164 = JveoVXDtVNmqeshLKYxUMRjnwWMX98750005;     JveoVXDtVNmqeshLKYxUMRjnwWMX98750005 = JveoVXDtVNmqeshLKYxUMRjnwWMX11974570;     JveoVXDtVNmqeshLKYxUMRjnwWMX11974570 = JveoVXDtVNmqeshLKYxUMRjnwWMX91267633;     JveoVXDtVNmqeshLKYxUMRjnwWMX91267633 = JveoVXDtVNmqeshLKYxUMRjnwWMX89989190;     JveoVXDtVNmqeshLKYxUMRjnwWMX89989190 = JveoVXDtVNmqeshLKYxUMRjnwWMX31142143;     JveoVXDtVNmqeshLKYxUMRjnwWMX31142143 = JveoVXDtVNmqeshLKYxUMRjnwWMX25736917;     JveoVXDtVNmqeshLKYxUMRjnwWMX25736917 = JveoVXDtVNmqeshLKYxUMRjnwWMX32325534;     JveoVXDtVNmqeshLKYxUMRjnwWMX32325534 = JveoVXDtVNmqeshLKYxUMRjnwWMX92279900;     JveoVXDtVNmqeshLKYxUMRjnwWMX92279900 = JveoVXDtVNmqeshLKYxUMRjnwWMX14322950;     JveoVXDtVNmqeshLKYxUMRjnwWMX14322950 = JveoVXDtVNmqeshLKYxUMRjnwWMX8882606;     JveoVXDtVNmqeshLKYxUMRjnwWMX8882606 = JveoVXDtVNmqeshLKYxUMRjnwWMX29503296;     JveoVXDtVNmqeshLKYxUMRjnwWMX29503296 = JveoVXDtVNmqeshLKYxUMRjnwWMX6122565;     JveoVXDtVNmqeshLKYxUMRjnwWMX6122565 = JveoVXDtVNmqeshLKYxUMRjnwWMX59452740;     JveoVXDtVNmqeshLKYxUMRjnwWMX59452740 = JveoVXDtVNmqeshLKYxUMRjnwWMX78248183;     JveoVXDtVNmqeshLKYxUMRjnwWMX78248183 = JveoVXDtVNmqeshLKYxUMRjnwWMX54218196;     JveoVXDtVNmqeshLKYxUMRjnwWMX54218196 = JveoVXDtVNmqeshLKYxUMRjnwWMX30627260;     JveoVXDtVNmqeshLKYxUMRjnwWMX30627260 = JveoVXDtVNmqeshLKYxUMRjnwWMX1909236;     JveoVXDtVNmqeshLKYxUMRjnwWMX1909236 = JveoVXDtVNmqeshLKYxUMRjnwWMX55719834;     JveoVXDtVNmqeshLKYxUMRjnwWMX55719834 = JveoVXDtVNmqeshLKYxUMRjnwWMX63546039;     JveoVXDtVNmqeshLKYxUMRjnwWMX63546039 = JveoVXDtVNmqeshLKYxUMRjnwWMX75601458;     JveoVXDtVNmqeshLKYxUMRjnwWMX75601458 = JveoVXDtVNmqeshLKYxUMRjnwWMX32477671;     JveoVXDtVNmqeshLKYxUMRjnwWMX32477671 = JveoVXDtVNmqeshLKYxUMRjnwWMX56603561;     JveoVXDtVNmqeshLKYxUMRjnwWMX56603561 = JveoVXDtVNmqeshLKYxUMRjnwWMX87153295;     JveoVXDtVNmqeshLKYxUMRjnwWMX87153295 = JveoVXDtVNmqeshLKYxUMRjnwWMX82947297;     JveoVXDtVNmqeshLKYxUMRjnwWMX82947297 = JveoVXDtVNmqeshLKYxUMRjnwWMX35063718;     JveoVXDtVNmqeshLKYxUMRjnwWMX35063718 = JveoVXDtVNmqeshLKYxUMRjnwWMX46999200;     JveoVXDtVNmqeshLKYxUMRjnwWMX46999200 = JveoVXDtVNmqeshLKYxUMRjnwWMX46416969;     JveoVXDtVNmqeshLKYxUMRjnwWMX46416969 = JveoVXDtVNmqeshLKYxUMRjnwWMX55128231;     JveoVXDtVNmqeshLKYxUMRjnwWMX55128231 = JveoVXDtVNmqeshLKYxUMRjnwWMX45856175;     JveoVXDtVNmqeshLKYxUMRjnwWMX45856175 = JveoVXDtVNmqeshLKYxUMRjnwWMX86058805;     JveoVXDtVNmqeshLKYxUMRjnwWMX86058805 = JveoVXDtVNmqeshLKYxUMRjnwWMX25282102;     JveoVXDtVNmqeshLKYxUMRjnwWMX25282102 = JveoVXDtVNmqeshLKYxUMRjnwWMX19247551;     JveoVXDtVNmqeshLKYxUMRjnwWMX19247551 = JveoVXDtVNmqeshLKYxUMRjnwWMX74515287;     JveoVXDtVNmqeshLKYxUMRjnwWMX74515287 = JveoVXDtVNmqeshLKYxUMRjnwWMX3092424;     JveoVXDtVNmqeshLKYxUMRjnwWMX3092424 = JveoVXDtVNmqeshLKYxUMRjnwWMX48733101;     JveoVXDtVNmqeshLKYxUMRjnwWMX48733101 = JveoVXDtVNmqeshLKYxUMRjnwWMX60171437;     JveoVXDtVNmqeshLKYxUMRjnwWMX60171437 = JveoVXDtVNmqeshLKYxUMRjnwWMX72116397;     JveoVXDtVNmqeshLKYxUMRjnwWMX72116397 = JveoVXDtVNmqeshLKYxUMRjnwWMX12753363;     JveoVXDtVNmqeshLKYxUMRjnwWMX12753363 = JveoVXDtVNmqeshLKYxUMRjnwWMX32292030;     JveoVXDtVNmqeshLKYxUMRjnwWMX32292030 = JveoVXDtVNmqeshLKYxUMRjnwWMX82166129;     JveoVXDtVNmqeshLKYxUMRjnwWMX82166129 = JveoVXDtVNmqeshLKYxUMRjnwWMX15488757;     JveoVXDtVNmqeshLKYxUMRjnwWMX15488757 = JveoVXDtVNmqeshLKYxUMRjnwWMX61862104;     JveoVXDtVNmqeshLKYxUMRjnwWMX61862104 = JveoVXDtVNmqeshLKYxUMRjnwWMX64968480;     JveoVXDtVNmqeshLKYxUMRjnwWMX64968480 = JveoVXDtVNmqeshLKYxUMRjnwWMX65876456;     JveoVXDtVNmqeshLKYxUMRjnwWMX65876456 = JveoVXDtVNmqeshLKYxUMRjnwWMX67222277;     JveoVXDtVNmqeshLKYxUMRjnwWMX67222277 = JveoVXDtVNmqeshLKYxUMRjnwWMX35199284;     JveoVXDtVNmqeshLKYxUMRjnwWMX35199284 = JveoVXDtVNmqeshLKYxUMRjnwWMX50676940;     JveoVXDtVNmqeshLKYxUMRjnwWMX50676940 = JveoVXDtVNmqeshLKYxUMRjnwWMX69073610;     JveoVXDtVNmqeshLKYxUMRjnwWMX69073610 = JveoVXDtVNmqeshLKYxUMRjnwWMX11489306;     JveoVXDtVNmqeshLKYxUMRjnwWMX11489306 = JveoVXDtVNmqeshLKYxUMRjnwWMX89429596;     JveoVXDtVNmqeshLKYxUMRjnwWMX89429596 = JveoVXDtVNmqeshLKYxUMRjnwWMX41751257;     JveoVXDtVNmqeshLKYxUMRjnwWMX41751257 = JveoVXDtVNmqeshLKYxUMRjnwWMX80596149;     JveoVXDtVNmqeshLKYxUMRjnwWMX80596149 = JveoVXDtVNmqeshLKYxUMRjnwWMX87442627;     JveoVXDtVNmqeshLKYxUMRjnwWMX87442627 = JveoVXDtVNmqeshLKYxUMRjnwWMX22087302;     JveoVXDtVNmqeshLKYxUMRjnwWMX22087302 = JveoVXDtVNmqeshLKYxUMRjnwWMX44136494;     JveoVXDtVNmqeshLKYxUMRjnwWMX44136494 = JveoVXDtVNmqeshLKYxUMRjnwWMX42146444;     JveoVXDtVNmqeshLKYxUMRjnwWMX42146444 = JveoVXDtVNmqeshLKYxUMRjnwWMX24821275;     JveoVXDtVNmqeshLKYxUMRjnwWMX24821275 = JveoVXDtVNmqeshLKYxUMRjnwWMX8320336;     JveoVXDtVNmqeshLKYxUMRjnwWMX8320336 = JveoVXDtVNmqeshLKYxUMRjnwWMX54925472;     JveoVXDtVNmqeshLKYxUMRjnwWMX54925472 = JveoVXDtVNmqeshLKYxUMRjnwWMX84142943;     JveoVXDtVNmqeshLKYxUMRjnwWMX84142943 = JveoVXDtVNmqeshLKYxUMRjnwWMX79319948;     JveoVXDtVNmqeshLKYxUMRjnwWMX79319948 = JveoVXDtVNmqeshLKYxUMRjnwWMX77197303;     JveoVXDtVNmqeshLKYxUMRjnwWMX77197303 = JveoVXDtVNmqeshLKYxUMRjnwWMX46423726;     JveoVXDtVNmqeshLKYxUMRjnwWMX46423726 = JveoVXDtVNmqeshLKYxUMRjnwWMX28264145;     JveoVXDtVNmqeshLKYxUMRjnwWMX28264145 = JveoVXDtVNmqeshLKYxUMRjnwWMX83600503;     JveoVXDtVNmqeshLKYxUMRjnwWMX83600503 = JveoVXDtVNmqeshLKYxUMRjnwWMX10255745;     JveoVXDtVNmqeshLKYxUMRjnwWMX10255745 = JveoVXDtVNmqeshLKYxUMRjnwWMX31607278;     JveoVXDtVNmqeshLKYxUMRjnwWMX31607278 = JveoVXDtVNmqeshLKYxUMRjnwWMX56360317;     JveoVXDtVNmqeshLKYxUMRjnwWMX56360317 = JveoVXDtVNmqeshLKYxUMRjnwWMX29515082;     JveoVXDtVNmqeshLKYxUMRjnwWMX29515082 = JveoVXDtVNmqeshLKYxUMRjnwWMX94046759;     JveoVXDtVNmqeshLKYxUMRjnwWMX94046759 = JveoVXDtVNmqeshLKYxUMRjnwWMX58510862;     JveoVXDtVNmqeshLKYxUMRjnwWMX58510862 = JveoVXDtVNmqeshLKYxUMRjnwWMX89155872;     JveoVXDtVNmqeshLKYxUMRjnwWMX89155872 = JveoVXDtVNmqeshLKYxUMRjnwWMX23427805;     JveoVXDtVNmqeshLKYxUMRjnwWMX23427805 = JveoVXDtVNmqeshLKYxUMRjnwWMX81379910;     JveoVXDtVNmqeshLKYxUMRjnwWMX81379910 = JveoVXDtVNmqeshLKYxUMRjnwWMX60112701;     JveoVXDtVNmqeshLKYxUMRjnwWMX60112701 = JveoVXDtVNmqeshLKYxUMRjnwWMX70615566;     JveoVXDtVNmqeshLKYxUMRjnwWMX70615566 = JveoVXDtVNmqeshLKYxUMRjnwWMX91635080;     JveoVXDtVNmqeshLKYxUMRjnwWMX91635080 = JveoVXDtVNmqeshLKYxUMRjnwWMX21276839;     JveoVXDtVNmqeshLKYxUMRjnwWMX21276839 = JveoVXDtVNmqeshLKYxUMRjnwWMX15725021;     JveoVXDtVNmqeshLKYxUMRjnwWMX15725021 = JveoVXDtVNmqeshLKYxUMRjnwWMX99864434;     JveoVXDtVNmqeshLKYxUMRjnwWMX99864434 = JveoVXDtVNmqeshLKYxUMRjnwWMX96322260;     JveoVXDtVNmqeshLKYxUMRjnwWMX96322260 = JveoVXDtVNmqeshLKYxUMRjnwWMX77343359;     JveoVXDtVNmqeshLKYxUMRjnwWMX77343359 = JveoVXDtVNmqeshLKYxUMRjnwWMX43638925;     JveoVXDtVNmqeshLKYxUMRjnwWMX43638925 = JveoVXDtVNmqeshLKYxUMRjnwWMX56426579;     JveoVXDtVNmqeshLKYxUMRjnwWMX56426579 = JveoVXDtVNmqeshLKYxUMRjnwWMX44307548;     JveoVXDtVNmqeshLKYxUMRjnwWMX44307548 = JveoVXDtVNmqeshLKYxUMRjnwWMX44685953;     JveoVXDtVNmqeshLKYxUMRjnwWMX44685953 = JveoVXDtVNmqeshLKYxUMRjnwWMX31804924;     JveoVXDtVNmqeshLKYxUMRjnwWMX31804924 = JveoVXDtVNmqeshLKYxUMRjnwWMX52427986;     JveoVXDtVNmqeshLKYxUMRjnwWMX52427986 = JveoVXDtVNmqeshLKYxUMRjnwWMX58955930;     JveoVXDtVNmqeshLKYxUMRjnwWMX58955930 = JveoVXDtVNmqeshLKYxUMRjnwWMX6586658;     JveoVXDtVNmqeshLKYxUMRjnwWMX6586658 = JveoVXDtVNmqeshLKYxUMRjnwWMX35350162;     JveoVXDtVNmqeshLKYxUMRjnwWMX35350162 = JveoVXDtVNmqeshLKYxUMRjnwWMX63796061;     JveoVXDtVNmqeshLKYxUMRjnwWMX63796061 = JveoVXDtVNmqeshLKYxUMRjnwWMX57827890;     JveoVXDtVNmqeshLKYxUMRjnwWMX57827890 = JveoVXDtVNmqeshLKYxUMRjnwWMX65707502;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void amMwPllmZeoHmgpvIJwGvCNKe13118974() {     long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26501971 = -11919849;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38536518 = -554060344;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV48720991 = -492165679;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV17976097 = -669915237;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV81250270 = -193546962;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV51284248 = -273883385;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6080389 = -823905124;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70501428 = -220949874;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91306529 = 89423275;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83243987 = -272929003;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80008366 = -388596725;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56225191 = -605580152;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98481943 = -366572637;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV13330111 = -408445915;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22052513 = -704174573;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98520097 = -455407494;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42993108 = -929084194;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37734083 = -412920242;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99991377 = -944057864;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33349960 = -42887789;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV34627567 = -976264069;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV86486272 = -657259549;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV23987232 = -596730232;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV82351530 = -184645060;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84976615 = -268216942;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11611366 = -251891547;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV10830695 = -524707009;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83327841 = -846993024;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45262351 = -801314029;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV64416261 = -973872635;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76622431 = -830604442;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV18736693 = -898017239;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40295881 = -559777826;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70156231 = 61394969;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV39611316 = -16639943;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV85038033 = -975737132;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98439849 = -507437740;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66392068 = -50722552;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96731631 = 84097052;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11191285 = -386265800;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76863646 = -960369973;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22233524 = -28797652;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV2052691 = -56941317;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45294295 = -718757929;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73566376 = -515851476;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV5754779 = -619043632;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV68865681 = -180653086;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19885551 = -124235634;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV67022185 = -997805436;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73394161 = -766477462;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42611642 = -475693718;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV57011077 = -520140843;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59622831 = -229155493;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV55524794 = -431712083;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78491406 = -888494201;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40015698 = -354660301;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV14549287 = -957330113;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66369460 = -207520620;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32999482 = -301698296;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV69638905 = -941655416;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40453554 = -749176376;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22752547 = -976912101;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV25239077 = -419635845;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26890269 = 63295910;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6621556 = -442324561;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV61271673 = -490579486;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV15929310 = 54197673;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV28325713 = -327967607;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73718794 = -291805973;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37014480 = -728437442;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80249 = -947969754;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76601040 = -778361643;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV41002452 = -397017294;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV88800092 = -457792065;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56486314 = -82517816;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12394043 = -847466418;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84433582 = -500318233;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78692936 = -877972303;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV8785155 = -668793585;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV79221837 = -649173311;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42745684 = 28761539;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV90945143 = -300471375;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV16305656 = -849187589;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV71868190 = 65163432;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV21804620 = -398178918;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19611355 = -210463600;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59113861 = -568861747;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84771087 = -28065744;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91664824 = -50110830;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99595618 = -661979643;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70488747 = 81592980;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32070389 = -199917121;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33392587 = -749024257;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV27092727 = 25752467;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70737731 = -637089424;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV54111099 = -983457873;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96994446 = -609161807;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV75162422 = -20237227;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38672739 = -176433369;    long ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12294703 = -11919849;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26501971 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38536518;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38536518 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV48720991;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV48720991 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV17976097;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV17976097 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV81250270;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV81250270 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV51284248;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV51284248 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6080389;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6080389 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70501428;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70501428 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91306529;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91306529 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83243987;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83243987 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80008366;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80008366 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56225191;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56225191 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98481943;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98481943 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV13330111;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV13330111 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22052513;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22052513 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98520097;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98520097 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42993108;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42993108 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37734083;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37734083 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99991377;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99991377 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33349960;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33349960 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV34627567;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV34627567 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV86486272;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV86486272 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV23987232;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV23987232 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV82351530;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV82351530 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84976615;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84976615 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11611366;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11611366 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV10830695;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV10830695 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83327841;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV83327841 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45262351;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45262351 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV64416261;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV64416261 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76622431;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76622431 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV18736693;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV18736693 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40295881;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40295881 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70156231;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70156231 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV39611316;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV39611316 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV85038033;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV85038033 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98439849;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV98439849 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66392068;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66392068 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96731631;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96731631 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11191285;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV11191285 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76863646;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76863646 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22233524;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22233524 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV2052691;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV2052691 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45294295;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV45294295 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73566376;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73566376 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV5754779;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV5754779 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV68865681;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV68865681 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19885551;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19885551 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV67022185;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV67022185 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73394161;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73394161 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42611642;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42611642 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV57011077;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV57011077 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59622831;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59622831 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV55524794;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV55524794 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78491406;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78491406 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40015698;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40015698 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV14549287;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV14549287 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66369460;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV66369460 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32999482;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32999482 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV69638905;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV69638905 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40453554;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV40453554 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22752547;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV22752547 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV25239077;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV25239077 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26890269;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26890269 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6621556;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV6621556 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV61271673;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV61271673 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV15929310;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV15929310 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV28325713;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV28325713 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73718794;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV73718794 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37014480;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV37014480 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80249;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV80249 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76601040;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV76601040 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV41002452;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV41002452 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV88800092;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV88800092 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56486314;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV56486314 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12394043;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12394043 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84433582;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84433582 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78692936;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV78692936 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV8785155;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV8785155 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV79221837;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV79221837 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42745684;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV42745684 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV90945143;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV90945143 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV16305656;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV16305656 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV71868190;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV71868190 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV21804620;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV21804620 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19611355;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV19611355 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59113861;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV59113861 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84771087;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV84771087 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91664824;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV91664824 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99595618;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV99595618 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70488747;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70488747 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32070389;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV32070389 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33392587;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV33392587 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV27092727;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV27092727 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70737731;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV70737731 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV54111099;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV54111099 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96994446;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV96994446 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV75162422;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV75162422 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38672739;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV38672739 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12294703;     ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV12294703 = ISiHaTxsNETkuJZatvxCVqxPEFNNcZYYBSHTCfyLrOYcV26501971;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void cvkBBvUvApHwOWkqxNIZIaCpIyDtKdWMSKnZB4440807() {     double EaMfWeTgpSpgsktRApBtv33810979 = -409149302;    double EaMfWeTgpSpgsktRApBtv37455928 = -81468204;    double EaMfWeTgpSpgsktRApBtv34135497 = -439357961;    double EaMfWeTgpSpgsktRApBtv15915094 = -748331220;    double EaMfWeTgpSpgsktRApBtv77954238 = -879339788;    double EaMfWeTgpSpgsktRApBtv33425251 = -558471629;    double EaMfWeTgpSpgsktRApBtv67224431 = -688225966;    double EaMfWeTgpSpgsktRApBtv10401509 = -218251663;    double EaMfWeTgpSpgsktRApBtv4207706 = 92357463;    double EaMfWeTgpSpgsktRApBtv30065000 = -5525316;    double EaMfWeTgpSpgsktRApBtv25163506 = -504428679;    double EaMfWeTgpSpgsktRApBtv92751756 = -636129695;    double EaMfWeTgpSpgsktRApBtv18534439 = -867065549;    double EaMfWeTgpSpgsktRApBtv72948745 = -318976893;    double EaMfWeTgpSpgsktRApBtv95431856 = -335687538;    double EaMfWeTgpSpgsktRApBtv2032932 = 7042106;    double EaMfWeTgpSpgsktRApBtv90831270 = 25158010;    double EaMfWeTgpSpgsktRApBtv1350226 = -74723647;    double EaMfWeTgpSpgsktRApBtv95676098 = -711736254;    double EaMfWeTgpSpgsktRApBtv79953998 = -410842769;    double EaMfWeTgpSpgsktRApBtv78768450 = -191132559;    double EaMfWeTgpSpgsktRApBtv15082041 = -706066335;    double EaMfWeTgpSpgsktRApBtv97624567 = -656383192;    double EaMfWeTgpSpgsktRApBtv25706069 = -277319551;    double EaMfWeTgpSpgsktRApBtv10397732 = -770154799;    double EaMfWeTgpSpgsktRApBtv6557537 = -79549430;    double EaMfWeTgpSpgsktRApBtv1401221 = -229455992;    double EaMfWeTgpSpgsktRApBtv17577493 = -510459432;    double EaMfWeTgpSpgsktRApBtv62947500 = -352460678;    double EaMfWeTgpSpgsktRApBtv47911267 = -365112526;    double EaMfWeTgpSpgsktRApBtv71038476 = -568616398;    double EaMfWeTgpSpgsktRApBtv41978886 = -25747627;    double EaMfWeTgpSpgsktRApBtv79673895 = -176029168;    double EaMfWeTgpSpgsktRApBtv6484452 = -448157434;    double EaMfWeTgpSpgsktRApBtv62989731 = -11182827;    double EaMfWeTgpSpgsktRApBtv57184099 = -697480549;    double EaMfWeTgpSpgsktRApBtv13392518 = -15419813;    double EaMfWeTgpSpgsktRApBtv13149589 = -630710470;    double EaMfWeTgpSpgsktRApBtv64121619 = -423942479;    double EaMfWeTgpSpgsktRApBtv34416459 = -10531301;    double EaMfWeTgpSpgsktRApBtv85941947 = -151639506;    double EaMfWeTgpSpgsktRApBtv43502883 = -575584966;    double EaMfWeTgpSpgsktRApBtv41405041 = -42434651;    double EaMfWeTgpSpgsktRApBtv90872325 = -5541634;    double EaMfWeTgpSpgsktRApBtv29361591 = 91297392;    double EaMfWeTgpSpgsktRApBtv66791561 = -72990180;    double EaMfWeTgpSpgsktRApBtv95552877 = 21659533;    double EaMfWeTgpSpgsktRApBtv60926473 = -497068617;    double EaMfWeTgpSpgsktRApBtv61126737 = -644338363;    double EaMfWeTgpSpgsktRApBtv99609524 = -880725393;    double EaMfWeTgpSpgsktRApBtv17820625 = -525912917;    double EaMfWeTgpSpgsktRApBtv65571700 = -868738723;    double EaMfWeTgpSpgsktRApBtv32327671 = -352069119;    double EaMfWeTgpSpgsktRApBtv93317275 = -138853409;    double EaMfWeTgpSpgsktRApBtv25672356 = -200920754;    double EaMfWeTgpSpgsktRApBtv18728939 = -703082967;    double EaMfWeTgpSpgsktRApBtv39831360 = -425085012;    double EaMfWeTgpSpgsktRApBtv8429428 = -62038411;    double EaMfWeTgpSpgsktRApBtv5517362 = -978176421;    double EaMfWeTgpSpgsktRApBtv71396701 = -699790358;    double EaMfWeTgpSpgsktRApBtv32024030 = -229015637;    double EaMfWeTgpSpgsktRApBtv49646938 = -77766534;    double EaMfWeTgpSpgsktRApBtv47454008 = -865790985;    double EaMfWeTgpSpgsktRApBtv56296438 = -542530011;    double EaMfWeTgpSpgsktRApBtv59026524 = -436908918;    double EaMfWeTgpSpgsktRApBtv83184619 = -378681052;    double EaMfWeTgpSpgsktRApBtv13077862 = -360100527;    double EaMfWeTgpSpgsktRApBtv12049988 = -318908116;    double EaMfWeTgpSpgsktRApBtv9959015 = -207794066;    double EaMfWeTgpSpgsktRApBtv38247757 = -638206990;    double EaMfWeTgpSpgsktRApBtv88640414 = -977538082;    double EaMfWeTgpSpgsktRApBtv77681681 = -344131520;    double EaMfWeTgpSpgsktRApBtv37228606 = -650781168;    double EaMfWeTgpSpgsktRApBtv61259640 = -601204953;    double EaMfWeTgpSpgsktRApBtv94012051 = -159203264;    double EaMfWeTgpSpgsktRApBtv35265567 = -615547593;    double EaMfWeTgpSpgsktRApBtv73677000 = -563631685;    double EaMfWeTgpSpgsktRApBtv6752242 = -550841559;    double EaMfWeTgpSpgsktRApBtv96344477 = -268616944;    double EaMfWeTgpSpgsktRApBtv43606170 = -597164620;    double EaMfWeTgpSpgsktRApBtv11004659 = -1208964;    double EaMfWeTgpSpgsktRApBtv40474748 = -732387375;    double EaMfWeTgpSpgsktRApBtv56450755 = -866121070;    double EaMfWeTgpSpgsktRApBtv63337976 = -471735285;    double EaMfWeTgpSpgsktRApBtv30090643 = -839199609;    double EaMfWeTgpSpgsktRApBtv5466776 = -699877675;    double EaMfWeTgpSpgsktRApBtv9651216 = -673678509;    double EaMfWeTgpSpgsktRApBtv86356619 = 62824240;    double EaMfWeTgpSpgsktRApBtv80812095 = -147236680;    double EaMfWeTgpSpgsktRApBtv44260792 = -308099861;    double EaMfWeTgpSpgsktRApBtv17352739 = -172395537;    double EaMfWeTgpSpgsktRApBtv4963090 = -953381402;    double EaMfWeTgpSpgsktRApBtv7632227 = -652534050;    double EaMfWeTgpSpgsktRApBtv92724918 = -724152122;    double EaMfWeTgpSpgsktRApBtv2392429 = -781515665;    double EaMfWeTgpSpgsktRApBtv36295009 = 26127028;    double EaMfWeTgpSpgsktRApBtv96048875 = -709793981;    double EaMfWeTgpSpgsktRApBtv85108602 = -499904640;    double EaMfWeTgpSpgsktRApBtv31845802 = -568632716;    double EaMfWeTgpSpgsktRApBtv46176972 = -409149302;     EaMfWeTgpSpgsktRApBtv33810979 = EaMfWeTgpSpgsktRApBtv37455928;     EaMfWeTgpSpgsktRApBtv37455928 = EaMfWeTgpSpgsktRApBtv34135497;     EaMfWeTgpSpgsktRApBtv34135497 = EaMfWeTgpSpgsktRApBtv15915094;     EaMfWeTgpSpgsktRApBtv15915094 = EaMfWeTgpSpgsktRApBtv77954238;     EaMfWeTgpSpgsktRApBtv77954238 = EaMfWeTgpSpgsktRApBtv33425251;     EaMfWeTgpSpgsktRApBtv33425251 = EaMfWeTgpSpgsktRApBtv67224431;     EaMfWeTgpSpgsktRApBtv67224431 = EaMfWeTgpSpgsktRApBtv10401509;     EaMfWeTgpSpgsktRApBtv10401509 = EaMfWeTgpSpgsktRApBtv4207706;     EaMfWeTgpSpgsktRApBtv4207706 = EaMfWeTgpSpgsktRApBtv30065000;     EaMfWeTgpSpgsktRApBtv30065000 = EaMfWeTgpSpgsktRApBtv25163506;     EaMfWeTgpSpgsktRApBtv25163506 = EaMfWeTgpSpgsktRApBtv92751756;     EaMfWeTgpSpgsktRApBtv92751756 = EaMfWeTgpSpgsktRApBtv18534439;     EaMfWeTgpSpgsktRApBtv18534439 = EaMfWeTgpSpgsktRApBtv72948745;     EaMfWeTgpSpgsktRApBtv72948745 = EaMfWeTgpSpgsktRApBtv95431856;     EaMfWeTgpSpgsktRApBtv95431856 = EaMfWeTgpSpgsktRApBtv2032932;     EaMfWeTgpSpgsktRApBtv2032932 = EaMfWeTgpSpgsktRApBtv90831270;     EaMfWeTgpSpgsktRApBtv90831270 = EaMfWeTgpSpgsktRApBtv1350226;     EaMfWeTgpSpgsktRApBtv1350226 = EaMfWeTgpSpgsktRApBtv95676098;     EaMfWeTgpSpgsktRApBtv95676098 = EaMfWeTgpSpgsktRApBtv79953998;     EaMfWeTgpSpgsktRApBtv79953998 = EaMfWeTgpSpgsktRApBtv78768450;     EaMfWeTgpSpgsktRApBtv78768450 = EaMfWeTgpSpgsktRApBtv15082041;     EaMfWeTgpSpgsktRApBtv15082041 = EaMfWeTgpSpgsktRApBtv97624567;     EaMfWeTgpSpgsktRApBtv97624567 = EaMfWeTgpSpgsktRApBtv25706069;     EaMfWeTgpSpgsktRApBtv25706069 = EaMfWeTgpSpgsktRApBtv10397732;     EaMfWeTgpSpgsktRApBtv10397732 = EaMfWeTgpSpgsktRApBtv6557537;     EaMfWeTgpSpgsktRApBtv6557537 = EaMfWeTgpSpgsktRApBtv1401221;     EaMfWeTgpSpgsktRApBtv1401221 = EaMfWeTgpSpgsktRApBtv17577493;     EaMfWeTgpSpgsktRApBtv17577493 = EaMfWeTgpSpgsktRApBtv62947500;     EaMfWeTgpSpgsktRApBtv62947500 = EaMfWeTgpSpgsktRApBtv47911267;     EaMfWeTgpSpgsktRApBtv47911267 = EaMfWeTgpSpgsktRApBtv71038476;     EaMfWeTgpSpgsktRApBtv71038476 = EaMfWeTgpSpgsktRApBtv41978886;     EaMfWeTgpSpgsktRApBtv41978886 = EaMfWeTgpSpgsktRApBtv79673895;     EaMfWeTgpSpgsktRApBtv79673895 = EaMfWeTgpSpgsktRApBtv6484452;     EaMfWeTgpSpgsktRApBtv6484452 = EaMfWeTgpSpgsktRApBtv62989731;     EaMfWeTgpSpgsktRApBtv62989731 = EaMfWeTgpSpgsktRApBtv57184099;     EaMfWeTgpSpgsktRApBtv57184099 = EaMfWeTgpSpgsktRApBtv13392518;     EaMfWeTgpSpgsktRApBtv13392518 = EaMfWeTgpSpgsktRApBtv13149589;     EaMfWeTgpSpgsktRApBtv13149589 = EaMfWeTgpSpgsktRApBtv64121619;     EaMfWeTgpSpgsktRApBtv64121619 = EaMfWeTgpSpgsktRApBtv34416459;     EaMfWeTgpSpgsktRApBtv34416459 = EaMfWeTgpSpgsktRApBtv85941947;     EaMfWeTgpSpgsktRApBtv85941947 = EaMfWeTgpSpgsktRApBtv43502883;     EaMfWeTgpSpgsktRApBtv43502883 = EaMfWeTgpSpgsktRApBtv41405041;     EaMfWeTgpSpgsktRApBtv41405041 = EaMfWeTgpSpgsktRApBtv90872325;     EaMfWeTgpSpgsktRApBtv90872325 = EaMfWeTgpSpgsktRApBtv29361591;     EaMfWeTgpSpgsktRApBtv29361591 = EaMfWeTgpSpgsktRApBtv66791561;     EaMfWeTgpSpgsktRApBtv66791561 = EaMfWeTgpSpgsktRApBtv95552877;     EaMfWeTgpSpgsktRApBtv95552877 = EaMfWeTgpSpgsktRApBtv60926473;     EaMfWeTgpSpgsktRApBtv60926473 = EaMfWeTgpSpgsktRApBtv61126737;     EaMfWeTgpSpgsktRApBtv61126737 = EaMfWeTgpSpgsktRApBtv99609524;     EaMfWeTgpSpgsktRApBtv99609524 = EaMfWeTgpSpgsktRApBtv17820625;     EaMfWeTgpSpgsktRApBtv17820625 = EaMfWeTgpSpgsktRApBtv65571700;     EaMfWeTgpSpgsktRApBtv65571700 = EaMfWeTgpSpgsktRApBtv32327671;     EaMfWeTgpSpgsktRApBtv32327671 = EaMfWeTgpSpgsktRApBtv93317275;     EaMfWeTgpSpgsktRApBtv93317275 = EaMfWeTgpSpgsktRApBtv25672356;     EaMfWeTgpSpgsktRApBtv25672356 = EaMfWeTgpSpgsktRApBtv18728939;     EaMfWeTgpSpgsktRApBtv18728939 = EaMfWeTgpSpgsktRApBtv39831360;     EaMfWeTgpSpgsktRApBtv39831360 = EaMfWeTgpSpgsktRApBtv8429428;     EaMfWeTgpSpgsktRApBtv8429428 = EaMfWeTgpSpgsktRApBtv5517362;     EaMfWeTgpSpgsktRApBtv5517362 = EaMfWeTgpSpgsktRApBtv71396701;     EaMfWeTgpSpgsktRApBtv71396701 = EaMfWeTgpSpgsktRApBtv32024030;     EaMfWeTgpSpgsktRApBtv32024030 = EaMfWeTgpSpgsktRApBtv49646938;     EaMfWeTgpSpgsktRApBtv49646938 = EaMfWeTgpSpgsktRApBtv47454008;     EaMfWeTgpSpgsktRApBtv47454008 = EaMfWeTgpSpgsktRApBtv56296438;     EaMfWeTgpSpgsktRApBtv56296438 = EaMfWeTgpSpgsktRApBtv59026524;     EaMfWeTgpSpgsktRApBtv59026524 = EaMfWeTgpSpgsktRApBtv83184619;     EaMfWeTgpSpgsktRApBtv83184619 = EaMfWeTgpSpgsktRApBtv13077862;     EaMfWeTgpSpgsktRApBtv13077862 = EaMfWeTgpSpgsktRApBtv12049988;     EaMfWeTgpSpgsktRApBtv12049988 = EaMfWeTgpSpgsktRApBtv9959015;     EaMfWeTgpSpgsktRApBtv9959015 = EaMfWeTgpSpgsktRApBtv38247757;     EaMfWeTgpSpgsktRApBtv38247757 = EaMfWeTgpSpgsktRApBtv88640414;     EaMfWeTgpSpgsktRApBtv88640414 = EaMfWeTgpSpgsktRApBtv77681681;     EaMfWeTgpSpgsktRApBtv77681681 = EaMfWeTgpSpgsktRApBtv37228606;     EaMfWeTgpSpgsktRApBtv37228606 = EaMfWeTgpSpgsktRApBtv61259640;     EaMfWeTgpSpgsktRApBtv61259640 = EaMfWeTgpSpgsktRApBtv94012051;     EaMfWeTgpSpgsktRApBtv94012051 = EaMfWeTgpSpgsktRApBtv35265567;     EaMfWeTgpSpgsktRApBtv35265567 = EaMfWeTgpSpgsktRApBtv73677000;     EaMfWeTgpSpgsktRApBtv73677000 = EaMfWeTgpSpgsktRApBtv6752242;     EaMfWeTgpSpgsktRApBtv6752242 = EaMfWeTgpSpgsktRApBtv96344477;     EaMfWeTgpSpgsktRApBtv96344477 = EaMfWeTgpSpgsktRApBtv43606170;     EaMfWeTgpSpgsktRApBtv43606170 = EaMfWeTgpSpgsktRApBtv11004659;     EaMfWeTgpSpgsktRApBtv11004659 = EaMfWeTgpSpgsktRApBtv40474748;     EaMfWeTgpSpgsktRApBtv40474748 = EaMfWeTgpSpgsktRApBtv56450755;     EaMfWeTgpSpgsktRApBtv56450755 = EaMfWeTgpSpgsktRApBtv63337976;     EaMfWeTgpSpgsktRApBtv63337976 = EaMfWeTgpSpgsktRApBtv30090643;     EaMfWeTgpSpgsktRApBtv30090643 = EaMfWeTgpSpgsktRApBtv5466776;     EaMfWeTgpSpgsktRApBtv5466776 = EaMfWeTgpSpgsktRApBtv9651216;     EaMfWeTgpSpgsktRApBtv9651216 = EaMfWeTgpSpgsktRApBtv86356619;     EaMfWeTgpSpgsktRApBtv86356619 = EaMfWeTgpSpgsktRApBtv80812095;     EaMfWeTgpSpgsktRApBtv80812095 = EaMfWeTgpSpgsktRApBtv44260792;     EaMfWeTgpSpgsktRApBtv44260792 = EaMfWeTgpSpgsktRApBtv17352739;     EaMfWeTgpSpgsktRApBtv17352739 = EaMfWeTgpSpgsktRApBtv4963090;     EaMfWeTgpSpgsktRApBtv4963090 = EaMfWeTgpSpgsktRApBtv7632227;     EaMfWeTgpSpgsktRApBtv7632227 = EaMfWeTgpSpgsktRApBtv92724918;     EaMfWeTgpSpgsktRApBtv92724918 = EaMfWeTgpSpgsktRApBtv2392429;     EaMfWeTgpSpgsktRApBtv2392429 = EaMfWeTgpSpgsktRApBtv36295009;     EaMfWeTgpSpgsktRApBtv36295009 = EaMfWeTgpSpgsktRApBtv96048875;     EaMfWeTgpSpgsktRApBtv96048875 = EaMfWeTgpSpgsktRApBtv85108602;     EaMfWeTgpSpgsktRApBtv85108602 = EaMfWeTgpSpgsktRApBtv31845802;     EaMfWeTgpSpgsktRApBtv31845802 = EaMfWeTgpSpgsktRApBtv46176972;     EaMfWeTgpSpgsktRApBtv46176972 = EaMfWeTgpSpgsktRApBtv33810979;}
// Junk Finished
