#pragma once

#include "interfaces/IInputSystem.h"

#pragma warning (disable: 4305) // condition expression is constant

#define OPTION(type, var, value) type var = value

class Options
{

public:

	OPTION(int, angrpwaimtype, 0);
	OPTION(int, angrpwaccboost, 0);
	OPTION(int, angrpwfacboost, 0);
	OPTION(int, angrpwqstypes, 0);
	OPTION(int, angrpwresastype, 0);
	OPTION(bool, messagesends, false);
	OPTION(bool, misc_devinfo, false);
	OPTION(bool, misc_antikick, false);
	OPTION(bool, misc_radio, false);
	OPTION(int, misc_radiostation, 0);
	OPTION(bool, misc_autoaccept, false);
	OPTION(bool, misc_revealAllRanks, false);
	OPTION(bool, esp_enemies_only, true);
	OPTION(bool, esp_farther, false);
	OPTION(bool, onlytextnocheckbox, false);
	OPTION(bool, onlytextboxfill, false);
	OPTION(float, esp_fill_amount, 0.f);
	float esp_player_fill_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_fill_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_fill_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_fill_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(int, esp_player_boundstype, 0);
	OPTION(int, esp_player_boxtype, 0);
	float esp_player_bbox_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_bbox_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_bbox_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_bbox_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(bool, esp_player_name, false);
	OPTION(bool, esp_player_health, false);
	OPTION(bool, esp_player_weapons, false);
	OPTION(bool, esp_player_ammo, false);
	OPTION(bool, esp_player_snaplines, false);
	OPTION(bool, esp_player_chams, false);
	OPTION(int, esp_player_chams_type, 0);
	OPTION(bool, esp_localplayer_chams, false);
	OPTION(bool, fake_chams, false);
	OPTION(bool, force_backtrack, false);
	OPTION(bool, esp_localplayer_chams_xyz, false);
	OPTION(int, esp_localplayer_chams_type, 0);
	OPTION(int, esp_localplayer_fakechams_type, 0);
	OPTION(int, esp_localplayer_viewmodel_materials, 0);
	OPTION(bool, esp_localplayer_viewmodel_chams, false);
	float esp_player_chams_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_visible[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_localplayer_chams_color_visible[4] = { 0.15f, 0.7f, 1.f, 1.0f }; //
	float esp_localplayer_chams_color_invisible[4] = { 0.f, 0.2f, 1.f, 1.f }; //
	float esp_localplayer_fakechams_color_visible[4] = { 1.f, 0.f, 0.f, 1.0f };
	float esp_localplayer_viewmodel_chams_color[4] = { 1.f, 0.f, 0.f, 1.0f }; // 
	float esp_localplayer_fakechams_color_invisible[4] = { 1.f, 0.f, 0.f, 1.f };
	float esp_player_chams_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	OPTION(bool, esp_localplayer_viewmodel_chams_wireframe, false);
	OPTION(bool, esp_player_skelet, false);
	OPTION(bool, esp_player_anglelines, false);
	OPTION(int, esp_dropped_weapons, 0);
	OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_grenades, false);
	OPTION(bool, autothrow_grenades, false);
	OPTION(int, autothrow_grenades_dmg, 0);
	OPTION(int, esp_grenades_type, 0);
	OPTION(float, visuals_others_player_fov, 0);
	OPTION(float, slowwalk_value, 0);
	OPTION(float, aspectratio, 0);//aspectratio
	OPTION(float, visuals_others_player_fov_viewmodel, 0);

	OPTION(float, viewmodel_offset_x, 1);
	OPTION(float, viewmodel_offset_y, 2);
	OPTION(float, viewmodel_offset_z, -2);

	OPTION(bool, visuals_others_watermark, true);//
	OPTION(bool, visuals_manual_aa, false);
	OPTION(float, visuals_manual_aa_opacity, 200);
	OPTION(bool, visuals_nightmode, false);
	OPTION(bool, visuals_asuswalls, false);
	OPTION(int, visuals_weapon_type, 0);
	OPTION(bool, removals_zoom, false);
	OPTION(float, visuals_asuswalls_value, 90);	
	OPTION(bool, visuals_asuswalls_staticprops, false);
	OPTION(bool, visuals_asuswalls_world, false);
	OPTION(bool, visuals_others_grenade_pred, false);
	OPTION(int, visuals_others_sky, 0);
	OPTION(int, visuals_nightmode_type, 0);
	OPTION(bool, glow_enabled, false);
	OPTION(bool, indicators_enabled, false);
	OPTION(bool, glow_players, false);
	float glow_player_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float glow_player_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float glow_player_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float glow_player_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	float nightmode_color[4] = { 0.15f, 0.15f, 0.15f, 1.f };
	float dropped_weapons_color[4] = { 255,0,0,255 };
	OPTION(bool, droppedwepcolorz, false);
	OPTION(int, glow_players_style, 0);
	OPTION(bool, glow_others, false);
	OPTION(int, glow_others_style, 0);
	OPTION(bool, visuals_others_hitmarker, false);
	OPTION(bool, visuals_others_bulletimpacts, false);
	float visuals_others_bulletimpacts_color[4] = { 0.012f, 0.788f, 0.663f, 1.f };
	float xhair_color[4] = { 255, 255, 255, 255 };
	OPTION(bool, removals_scope, false);
	OPTION(bool, removals_novisualrecoil, false);
	OPTION(bool, angrpwaimstep, false);
	OPTION(bool, removals_postprocessing, false);
	OPTION(bool, removals_flash, false);
	OPTION(bool, removals_smoke, false);
	OPTION(int, removals_smoke_type, 0);

	float visuals_others_nightmode_color[4] = { 0.15f, 0.15f, 0.15f, 1.f };
	float visuals_others_skybox_color[4] = { 0.15f, 0.15f, 0.15f, 1.f };

	OPTION(bool, anti_obs, true);
	OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_autostrafe, false);
	OPTION(bool, minecraft_graphics, false);
	OPTION(bool, removals_crosshair, false);
	OPTION(bool, misc_auto_pistol, false);
	OPTION(bool, misc_infinite_duck, false);
	OPTION(bool, misc_chatspamer, false);
	OPTION(bool, misc_toxic_chatspamer, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(ButtonCode_t, misc_thirdperson_bind, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, misc_fakeduck, BUTTON_CODE_NONE);
	OPTION(bool, misc_fakewalk, false);
	OPTION(float, misc_fakewalk_speed, 0);
	OPTION(ButtonCode_t, misc_fakewalk_bind, BUTTON_CODE_NONE);
	OPTION(bool, lgtpew_on_press, false);
	OPTION(bool, tickbase_manipulation, false);
	OPTION(bool, tickbase_freeze, false);
	OPTION(ButtonCode_t, tickbase_manipulation_key, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, tickbase_freeze_key, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_aa_left_bind, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_aa_right_bind, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_aa_back_bind, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, menu_open_alternative, BUTTON_CODE_NONE);
	OPTION(bool, misc_fakelag_enabled, false);
	OPTION(bool, misc_namestealer, false);
	OPTION(float, misc_fakelag_value, 0);
	OPTION(int, misc_fakelag_activation_type, 0);
	OPTION(bool, misc_fakelag_adaptive, false);
	OPTION(bool, misc_animated_clantag, false);
	OPTION(bool, misc_become_gay, false);
	OPTION(bool, misc_aimware_clantag, false);
	OPTION(bool, misc_spectatorlist, false);
	OPTION(bool, misc_instant_defuse_plant, false);
	OPTION(bool, misc_logevents, false);
	OPTION(bool, lgtpew_enabled, false);
	OPTION(bool, lgtpew_on_key, false);
	OPTION(bool, lgtpew_drawfov, false);
	OPTION(ButtonCode_t, lgtpew_aim_keybind, BUTTON_CODE_NONE);
	OPTION(bool, lgtpew_rcs, false);
	OPTION(bool, lgtpew_trigger, false);
	OPTION(bool, lgtpew_trigger_with_aimkey, false);
	OPTION(int, lgtpew_preaim, 0);

	OPTION(bool, cl_phys_timescale, false);
	OPTION(bool, mp_radar_showall, false);
	OPTION(bool, cl_crosshair_recoil, false);
	OPTION(bool, fog_override, false);
	OPTION(float, cl_phys_timescale_value, 0.1f);
	OPTION(float, clantag_speed, 1);
	OPTION(bool, change_viewmodel_offset, false);


	OPTION(bool, rank_enable, false);
	OPTION(int, chosen_rank, 1);
	OPTION(float, friendly_count, 0);
	OPTION(float, teaching_count, 0);
	OPTION(float, leader_count, 0);
	OPTION(float, wins_count, 0);
	OPTION(float, private_level, 0);
	OPTION(float, private_experience, 0);

	OPTION(float, lgtpew_rcsy, 0);
	OPTION(float, lgtpew_rcsx, 0);
	OPTION(bool, lgtpew_hsonly, false);
	OPTION(float, lgtpew_aftershots, 3);
	OPTION(int, lgtpew_afteraim, 0);
	OPTION(bool, legit_backtrack, false);
	OPTION(bool, visuals_draw_xhair, false);
	OPTION(float, visuals_xhair_x, 1);
	OPTION(float, visuals_xhair_y, 1);
	OPTION(float, lgtpew_smooth_factor, 1); //1.f
	OPTION(float, lgtpew_fov, 0.1); //1.f
	OPTION(bool, angrpwenabled, false);
	OPTION(float, angrpwfov, 0);
	OPTION(ButtonCode_t, angrpwaimkey, BUTTON_CODE_NONE);
	OPTION(bool, angrpwsilent, false);
	OPTION(bool, angrpwnorecoil, false);
	OPTION(bool, angrpwautoshoot, false);
	OPTION(bool, angrpwautoscope, false);
	OPTION(bool, angrpwautocrouch, false);
	OPTION(bool, angrpwautostop, false);
	OPTION(bool, angrpwautobaim, false);
	OPTION(bool, angrpwautocockrevolver, false);
	OPTION(int, angrpwbaim_after_x_shots, 0);
	OPTION(bool, angrpwlagcompensation, false);
	OPTION(bool, trigger_on_press, false);
	OPTION(ButtonCode_t, trigger_keybind, BUTTON_CODE_NONE);
	OPTION(bool, rcs_on_press, false);
	OPTION(ButtonCode_t, rcs_keybind, BUTTON_CODE_NONE);
	OPTION(int, test_slider, 0);
	OPTION(bool, angrpwfixup_entities, false);

	OPTION(float, misc_thirdperson_distance, 20);


	OPTION(float, angrpwmindmg, 0.f);

	OPTION(float, angrpwhitchance_amount, 0.f);

	OPTION(float, angrpwmindmg_amount, 0.f);

	OPTION(float, angrpwpointscale_amount, 0.1f);


	OPTION(int, iangrpwhitbox, 0);

	OPTION(bool, bangrpwmultipoint, false);

	OPTION(bool, bangrpwprioritize, false);

	bool angrpwmultiHitboxesAll[14] =
	{
		true, true, true, true, true,
		true, true, true, true, true,
		true, true, true, true
	};


	OPTION(bool, esp_backtracked_player_skelet, false);
	OPTION(bool, esp_lagcompensated_hitboxes, false);
	OPTION(int, esp_lagcompensated_hitboxes_type, 0);
	OPTION(int, hvh_antiaim_x, 0);
	OPTION(int, hvh_antiaim_y, 0);
	OPTION(int, hvh_antiaim_y_move, 0);
	OPTION(float, hvh_antiaim_y_move_trigger_speed, 0.1);
	OPTION(float, hvh_antiaim_y_custom_left, 0.f);
	OPTION(float, hvh_antiaim_y_custom_right, 0.f);
	OPTION(float, hvh_antiaim_y_custom_back, 0.f);
	OPTION(float, hvh_antiaim_y_desync, 0.f);
	OPTION(float, hvh_antiaim_y_desync_start_left, 0.f);
	OPTION(float, hvh_antiaim_y_desync_start_right, 0.f);
	OPTION(float, hvh_antiaim_y_desync_start_back, 0.f);
	OPTION(float, hvh_antiaim_y_custom_realmove_left, 0.f);
	OPTION(float, hvh_antiaim_y_custom_realmove_right, 0.f);
	OPTION(float, hvh_antiaim_y_custom_realmove_back, 0.f);
	OPTION(bool, hvh_antiaim_freestand_aggresive, false);

	OPTION(int, hvh_antiaim_base, 0);
	OPTION(int, hvh_antiaim_y_fake, 0);
	OPTION(int, hvh_antiaim_y_moving, 0);
	OPTION(int, hvh_antiaim_y_movingfake, 0);
	OPTION(int, hvh_antiaim_y_onair, 0);
	OPTION(bool, hvh_antiaim_legit, false;)
	OPTION(ButtonCode_t, hvh_antiaim_menkey, BUTTON_CODE_NONE);
	OPTION(bool, hvh_antiaim_freestand, false);
	OPTION(bool, hvh_antiaim_lby_breaker, false);
	OPTION(float, hvh_antiaim_lby_delta, 0);
	OPTION(float, hvh_antiaim_real_delta, 0);
	OPTION(float, hvh_antiaim_fake_delta, 0);
	OPTION(bool, hvh_antiaim_lby_dynamic, false);
	int hvh_antiaim_new_options[2][2][2][2];	//[real][yaw][moving][freestand]
	float hvh_antiaim_new_offset[2][2][2][2];	//[real][yaw][moving][freestand] base offset
	float hvh_antiaim_new_extra[2][2][2][2];	//[real][yaw][moving][freestand] used for many diffrent things. (jitter range, spin speed, etc)
	bool hvh_antiaim_new_breakLBY[2];			//[freestand]
	float hvh_antiaim_new_LBYDelta[2];			//[freestand]
	OPTION(int, hvh_show_real_angles, false);
	OPTION(bool, hvh_resolver, false);
	OPTION(bool, hvh_resolver_custom, false);
	OPTION(bool, hvh_resolver_override, false);
	OPTION(bool, hvh_disable_antiut, false);

	// SCHIEBE CONFIG

	OPTION(int, configstuff, 0);

	//

	//Resolver scheiﬂe
	/* eye lby */
	OPTION(bool, resolver_eyelby_running, false);
	OPTION(bool, resolver_eyelby_notrunning, false);
	OPTION(bool, resolver_eyelby_always, false);
	OPTION(float, resolver_eyelby_running_value, false);
	OPTION(bool, resolver_eyelby, false);
	OPTION(int, resolver_eyelby_missed_shots, 0);
	OPTION(bool, resolver_eyelby_after_x_shots, false);

	OPTION(bool, resolver_autoflip, false);
	OPTION(float, resolver_flip_after_missed_shots, 0);
	OPTION(bool, resolver_flip_after_x_shots, false);

	OPTION(ButtonCode_t, hvh_resolver_override_key, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_resolver_disable_key, BUTTON_CODE_NONE);
	OPTION(bool, hvh_resolver_disabled_draw, false);
	OPTION(bool, skinchanger_enabled, false);
	OPTION(bool, debug_window, false);
	OPTION(bool, debug_showposes, false);
	OPTION(bool, debug_showactivities, false);
	OPTION(bool, debug_headbox, false);
	OPTION(bool, debug_fliponkey, false);
	OPTION(ButtonCode_t, debug_flipkey, BUTTON_CODE_NONE);



	OPTION(int, medaltest, 0);
	OPTION(float, medaltrick, 0.f);
	OPTION(int, misc_region_changer, 0);

	//gonna put the bool here for ease of access yes

	OPTION(bool, unload, 0);
};


extern const char *opt_EspType[];
extern const char *opt_BoundsType[];
extern const char *opt_WeaponBoxType[];
extern const char *opt_GrenadeESPType[];
extern const char *opt_AimHitboxSpot[];
extern const char *opt_GlowStyles[];
extern const char *opt_NightmodeType[];
extern const char *opt_Chams_Local_Mode[];
extern const char *opt_Chams[];
extern const char *opt_Chams_Mat[];
extern const char *opt_AimSpot[];
extern const char *opt_MultiHitboxes[14];
extern const char *opt_AApitch[];
extern const char *opt_AAyaw[];
extern const char *opt_AAyaw_move[];
extern const char *opt_Skynames[];
extern const char *opt_nosmoketype[];
extern int realAimSpot[];
extern int realHitboxSpot[];
extern const char *opt_LagCompType[];
extern bool input_shouldListen;
extern ButtonCode_t* input_receivedKeyval;
extern bool menuOpen;
extern Options XSystemCFG;
