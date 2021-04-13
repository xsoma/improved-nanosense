#include "Visuals.hpp"
#include "../Options.hpp"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"
#include "LagCompensation.hpp"
#include "..//SDK.hpp"
#include "..//features/AngryPew.hpp"
#include <comdef.h>
#include <minwindef.h>
#include "..//menu//AntiCheatScan.h"
#include "..//vsonyp0wer/vsonyp0wer.h"
#include <time.h>
#include "..//Install.hpp"
#include "..//Singleton.hpp"
#pragma warning (disable: 4101) // condition expression is constant
#pragma warning (disable: 4838) // condition expression is constant

namespace Visuals
{
	vgui::HFont weapon_font;
	vgui::HFont ui_font;
	vgui::HFont watermark_font;
	vgui::HFont aainfo_font;
	vgui::HFont aa_info;
	vgui::HFont spectatorlist_font;
	vgui::HFont eventlog_font;
	vgui::HFont indicators_font;
	VisualsStruct ESP_ctx;
	float ESP_Fade[64];
	C_BasePlayer* local_observed;
	C_BasePlayer* carrier;
}

RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t &trans = ent->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = ent->m_vecOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			pointstransf[i] = screen_points[i];

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

void Visuals::RenderNadeEsp(C_BaseCombatWeapon* nade)
{
	if (!XSystemCFG.esp_grenades)
		return;

	const model_t* model = nade->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);
	if (!hdr)
		return;

	Color Nadecolor;
	std::string entityName = hdr->szName, icon_character;
	switch (nade->GetClientClass()->m_ClassID)
	{
	case 9:
		if (entityName[16] == 's')
		{
			Nadecolor = Color(255, 255, 0, 200);
			entityName = "Flash";
			icon_character = "G";
		}
		else
		{
			Nadecolor = Color(255, 0, 0, 200);
			entityName = "Frag";
			icon_character = "H";
		}
		break;
	case 134:
		Nadecolor = Color(170, 170, 170, 200);
		entityName = "Smoke";
		icon_character = "P";
		break;
	case 98:
		Nadecolor = Color(255, 0, 0, 200);
		entityName = "Fire";
		icon_character = "P";
		break;
	case 41:
		Nadecolor = Color(255, 255, 0, 200);
		entityName = "Decoy";
		icon_character = "G";
		break;
	default:
		return;
	}

	Vector points_transformed[8];
	RECT size = GetBBox(nade, points_transformed);
	if (size.right == 0 || size.bottom == 0)
		return;

	int width, height, width_icon, height_icon;
	Visuals::GetTextSize(ui_font, entityName.c_str(), width, height);
	Visuals::GetTextSize(weapon_font, icon_character.c_str(), width_icon, height_icon);

	// + distance? just make it customizable
	switch (XSystemCFG.esp_grenades_type)
	{
	case 1:
		g_VGuiSurface->DrawSetColor(Color(20, 20, 20, 240));
		g_VGuiSurface->DrawLine(size.left - 1, size.bottom - 1, size.left - 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.top + 1, size.right + 1, size.bottom - 1);
		g_VGuiSurface->DrawLine(size.left - 1, size.top + 1, size.right + 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.bottom - 1, size.left + -1, size.bottom - 1);

		g_VGuiSurface->DrawSetColor(Nadecolor);
		g_VGuiSurface->DrawLine(size.left, size.bottom, size.left, size.top);
		g_VGuiSurface->DrawLine(size.left, size.top, size.right, size.top);
		g_VGuiSurface->DrawLine(size.right, size.top, size.right, size.bottom);
		g_VGuiSurface->DrawLine(size.right, size.bottom, size.left, size.bottom);
	case 0:
		DrawString(ui_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height * 0.5f + 2, Nadecolor, FONT_CENTER, entityName.c_str());
		break;
	case 3:
		DrawString(ui_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height_icon * 0.5f + 1, Nadecolor, FONT_CENTER, entityName.c_str());
	case 2:
		DrawString(weapon_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom), Nadecolor, FONT_CENTER, icon_character.c_str());
		break;
	}
}

bool Visuals::InitFont()
{
	ui_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(ui_font, "Verdana", 12, 450, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW); 	//g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); // Styles

	watermark_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(watermark_font, "Tahoma", 16, 700, 0, 0, FONTFLAG_DROPSHADOW);

	aainfo_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(aainfo_font, "Tahoma", 32, 700, 0, 0, FONTFLAG_DROPSHADOW);

	aa_info = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(aa_info, "Tahoma", 32, 700, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	weapon_font = g_VGuiSurface->CreateFont_();// 0xA1;
	g_VGuiSurface->SetFontGlyphSet(weapon_font, "Undefeated", 16, 500, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);

	spectatorlist_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(spectatorlist_font, "Verdana", 12, 450, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	eventlog_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(eventlog_font, "Verdana", 14, 350, 0, 0, FONTFLAG_DROPSHADOW);

	indicators_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(indicators_font, "Verdana", 26, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	return true;
}

bool Visuals::IsVisibleScan(C_BasePlayer *player)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	Vector eyePos = g_LocalPlayer->GetEyePos();

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	if (!player->SetupBones2(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
	{
		return false;
	}

	auto studio_model = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (!studio_model)
	{
		return false;
	}

	int scan_hitboxes[] = {
		HITBOX_HEAD,
		HITBOX_LEFT_FOOT,
		HITBOX_RIGHT_FOOT,
		HITBOX_LEFT_CALF,
		HITBOX_RIGHT_CALF,
		HITBOX_CHEST,
		HITBOX_STOMACH
	};
	
	for (int i = 0; i < ARRAYSIZE(scan_hitboxes); i++)
	{
		auto hitbox = studio_model->pHitboxSet(player->m_nHitboxSet())->pHitbox(scan_hitboxes[i]);
		if (hitbox)
		{
			auto
				min = Vector{},
				max = Vector{};

			Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			ray.Init(eyePos, (min + max) * 0.5);
			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == player || tr.fraction > 0.97f)
				return true;
		}
	}
	return false;
}

bool Visuals::ValidPlayer(C_BasePlayer *player, bool count_step)
{
	int idx = player->EntIndex();
	constexpr float frequency = 0.7f / 0.5f;
	float step = frequency * g_GlobalVars->frametime;
	if (!player->IsAlive())
		return false;

	// Don't render esp if in firstperson viewing player.
	if (player == local_observed)
	{
		if (g_LocalPlayer->m_iObserverMode() == 4)
			return false;
	}

	if (player == g_LocalPlayer)
	{
		if (!g_Input->m_fCameraInThirdPerson)
			return false;
	}

	if (count_step)
	{
		if (!player->IsDormant()) {
			if (ESP_Fade[idx] < 1.f)
				ESP_Fade[idx] += step;
		}
		else {
			if (ESP_Fade[idx] > 0.f)
				ESP_Fade[idx] -= step;
		}
		ESP_Fade[idx] = (ESP_Fade[idx] > 1.f ? 1.f : ESP_Fade[idx] < 0.f ? 0.f : ESP_Fade[idx]);
	}

	return (ESP_Fade[idx] > 0.f);
}

bool Visuals::Begin(C_BasePlayer *player)
{
	ESP_ctx.player = player;
	ESP_ctx.bEnemy = !player->IsTeamMate(); // not sure hmmmmm, still tired // g_LocalPlayer->m_iTeamNum() != player->m_iTeamNum();
	ESP_ctx.isVisible = Visuals::IsVisibleScan(player);
	local_observed = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hObserverTarget());

	int idx = player->EntIndex();
	bool playerTeam = player->m_iTeamNum() == 2;

	if (!ESP_ctx.bEnemy && XSystemCFG.esp_enemies_only)
		return false;

	if (!player->m_bGunGameImmunity())
	{
		if (ESP_ctx.isVisible)
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? XSystemCFG.esp_player_fill_color_t_visible : XSystemCFG.esp_player_fill_color_ct_visible);
			ESP_ctx.clr.SetColor(playerTeam ? XSystemCFG.esp_player_bbox_color_t_visible : XSystemCFG.esp_player_bbox_color_ct_visible);
		}
		else
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? XSystemCFG.esp_player_fill_color_t : XSystemCFG.esp_player_fill_color_ct);
			ESP_ctx.clr.SetColor(playerTeam ? XSystemCFG.esp_player_bbox_color_t : XSystemCFG.esp_player_bbox_color_ct);
			ESP_ctx.clr.SetAlpha(255);
		}
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * ESP_Fade[idx]);
		ESP_ctx.clr_fill.SetAlpha(XSystemCFG.esp_fill_amount * ESP_Fade[idx]);
		ESP_ctx.clr_text = Color(245, 245, 245, (int)(ESP_ctx.clr.a() * ESP_Fade[idx]));
	}
	else
	{
		// Set all colors to grey if immune.
		ESP_ctx.clr.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_text.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_fill.SetColor(166, 169, 174, (int)(XSystemCFG.esp_fill_amount * ESP_Fade[idx]));
	}

	// Do some touch ups if local player and scoped
	if (player == g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
	{
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * 0.1f);
		ESP_ctx.clr_text.SetAlpha(ESP_ctx.clr_text.a() * 0.1f);
		ESP_ctx.clr_fill.SetAlpha(ESP_ctx.clr_fill.a() * 0.1f);
	}

	Vector head = player->GetAbsOrigin() + Vector(0, 0, player->GetCollideable()->OBBMaxs().z);
	Vector origin = player->GetAbsOrigin();
	origin.z -= 5;

	if (!Math::WorldToScreen(head, ESP_ctx.head_pos) ||
		!Math::WorldToScreen(origin, ESP_ctx.feet_pos))
		return false;

	auto h = fabs(ESP_ctx.head_pos.y - ESP_ctx.feet_pos.y);
	auto w = h / 1.65f;

	switch (XSystemCFG.esp_player_boundstype)
	{
	case 0:
		ESP_ctx.bbox.left = static_cast<long>(ESP_ctx.feet_pos.x - w * 0.5f);
		ESP_ctx.bbox.right = static_cast<long>(ESP_ctx.bbox.left + w);
		ESP_ctx.bbox.bottom = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.feet_pos.y) : static_cast<long>(ESP_ctx.head_pos.y));
		ESP_ctx.bbox.top = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.head_pos.y) : static_cast<long>(ESP_ctx.feet_pos.y));
		break;
	case 1:
	{
		Vector points_transformed[8];
		RECT BBox = GetBBox(player, points_transformed);
		ESP_ctx.bbox = BBox;
		ESP_ctx.bbox.top = BBox.bottom;
		ESP_ctx.bbox.bottom = BBox.top;
		break;
	}
	}

	return true;
}

void Visuals::RenderFill()
{
	g_VGuiSurface->DrawSetColor(ESP_ctx.clr_fill);
	g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 2);
}

void Visuals::RenderBox()
{
	float
		length_horizontal = (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.2f,
		length_vertical = (ESP_ctx.bbox.bottom - ESP_ctx.bbox.top) * 0.2f;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	switch (XSystemCFG.esp_player_boxtype)
	{
	case 0:
		break;

	case 1:
		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.right, ESP_ctx.bbox.bottom);
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left + 1, ESP_ctx.bbox.top + 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;

	case 2:
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);

		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.bottom - 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom - 2);

		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;
	}
}

void Visuals::GayPornYes()
{
	if (!g_LocalPlayer->IsAlive())
		return;

	g_VGuiSurface->DrawSetColor(Color(XSystemCFG.xhair_color[0], XSystemCFG.xhair_color[1], XSystemCFG.xhair_color[2]));
	g_VGuiSurface->DrawLine(Global::iScreenX / 2 - XSystemCFG.visuals_xhair_x, Global::iScreenY / 2, Global::iScreenX / 2 + XSystemCFG.visuals_xhair_x, Global::iScreenY / 2);
	g_VGuiSurface->DrawLine(Global::iScreenX / 2, Global::iScreenY / 2 - XSystemCFG.visuals_xhair_y, Global::iScreenX / 2, Global::iScreenY / 2 + XSystemCFG.visuals_xhair_y);

}

void Visuals::NightMode()
{
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
	{
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	static ConVar*r_DrawSpecificStaticProp;
	if (OldNightmode != XSystemCFG.visuals_nightmode)
	{

		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_MatSystem->GetMaterial(i);
			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (XSystemCFG.visuals_nightmode)
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
						pMaterial->ColorModulate(XSystemCFG.visuals_others_nightmode_color[0], XSystemCFG.visuals_others_nightmode_color[1], XSystemCFG.visuals_others_nightmode_color[2]);
					else
						pMaterial->ColorModulate(XSystemCFG.visuals_others_nightmode_color[0], XSystemCFG.visuals_others_nightmode_color[1], XSystemCFG.visuals_others_nightmode_color[2]);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}

			if (strstr(pMaterial->GetName(), "models/props"))
			{
				if (XSystemCFG.visuals_nightmode)
					pMaterial->ColorModulate(XSystemCFG.visuals_others_nightmode_color[0], XSystemCFG.visuals_others_nightmode_color[1], XSystemCFG.visuals_others_nightmode_color[2]);
				else 
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}

			if (strstr(pMaterial->GetTextureGroupName(), ("SkyBox")))
			{
				if (XSystemCFG.visuals_nightmode)
					pMaterial->ColorModulate(XSystemCFG.visuals_others_skybox_color[0], XSystemCFG.visuals_others_skybox_color[1], XSystemCFG.visuals_others_skybox_color[2]);
				else 
					pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}
		}
		OldNightmode = XSystemCFG.visuals_nightmode;
	}
}

void Visuals::SpoofZeug()
{
	static auto cl_phys_timescale = g_CVar->FindVar("cl_phys_timescale");
	if (XSystemCFG.cl_phys_timescale)
		cl_phys_timescale->SetValue(XSystemCFG.cl_phys_timescale_value);
	else cl_phys_timescale->SetValue("1.0");

	static auto mp_radar_showall = g_CVar->FindVar("mp_radar_showall");
	if (XSystemCFG.mp_radar_showall)
		mp_radar_showall->SetValue(1);
	else mp_radar_showall->SetValue(0);

	static auto cl_crosshair_recoil = g_CVar->FindVar("cl_crosshair_recoil");
	if (XSystemCFG.cl_crosshair_recoil)
		cl_crosshair_recoil->SetValue(1);
	else cl_crosshair_recoil->SetValue(0);

	static auto fog_override = g_CVar->FindVar("fog_override");
	static auto fog_enable = g_CVar->FindVar("fog_enable");
	if (XSystemCFG.fog_override) {
		fog_enable->SetValue(0);
		fog_override->SetValue(1);
	}
	else {
		fog_enable->SetValue(1);
		fog_override->SetValue(0);
	}

	static auto viewmodel_offset_x = g_CVar->FindVar("viewmodel_offset_x");
	static auto viewmodel_offset_y = g_CVar->FindVar("viewmodel_offset_y");
	static auto viewmodel_offset_z = g_CVar->FindVar("viewmodel_offset_z");
	viewmodel_offset_x->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_y->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_z->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_x->m_fnChangeCallbacks.SetSize(0);
	viewmodel_offset_y->m_fnChangeCallbacks.SetSize(0);
	viewmodel_offset_z->m_fnChangeCallbacks.SetSize(0);
	if (XSystemCFG.change_viewmodel_offset) {
		viewmodel_offset_x->SetValue(XSystemCFG.viewmodel_offset_x);
		viewmodel_offset_y->SetValue(XSystemCFG.viewmodel_offset_y);
		viewmodel_offset_z->SetValue(XSystemCFG.viewmodel_offset_z);
	}
	else {
		viewmodel_offset_x->SetValue(2.5f);
		viewmodel_offset_y->SetValue(2);
		viewmodel_offset_z->SetValue(-2);
	}
}

void Visuals::AsusWalls()
{
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
	{
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	static ConVar*r_DrawSpecificStaticProp;
	if (OldNightmode != XSystemCFG.visuals_asuswalls)
	{
		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_MatSystem->GetMaterial(i);
			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetName(), "models/props"))
			{
				if (XSystemCFG.visuals_asuswalls)
					pMaterial->AlphaModulate(0.5f);
				else
					pMaterial->AlphaModulate(1.f);
			}
		}
		OldNightmode = XSystemCFG.visuals_asuswalls;
	}
}

void Visuals::RenderName()
{
	wchar_t buf[128];
	std::string name = ESP_ctx.player->GetName(),
		s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

	if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0)
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ui_font);
		g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
		g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.top - th + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

void Visuals::DrawAngleLines()
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = g_LocalPlayer;
	Math::AngleVectors(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0), forward);
	src3D = g_LocalPlayer->m_vecOrigin();
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Yellow);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);


	Math::AngleVectors(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0), forward);
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Red);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
}

int getAALAngle()
{
	return static_cast<int>(XSystemCFG.hvh_antiaim_y_desync_start_left);
}

int getAARAngle()
{
	return static_cast<int>(XSystemCFG.hvh_antiaim_y_desync_start_right);
}

int getAABAngle()
{
	return static_cast<int>(XSystemCFG.hvh_antiaim_y_desync_start_back);
}

int getDesyncAmount()
{
	return static_cast<int>(XSystemCFG.hvh_antiaim_y_desync);
}

void Visuals::Indicators()
{
	int alpha;
	int centerWD, centerHD;
	int screenheight, screenwidth;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	centerWD = w / 2;
	centerHD = h / 2;
	screenheight = h;
	screenwidth = w;
	alpha = 255;

	if (XSystemCFG.lgtpew_drawfov)
	{
		g_VGuiSurface->DrawSetColor(Color(255,255,255,255));
		g_VGuiSurface->DrawOutlinedCircle((centerWD), (centerHD), XSystemCFG.lgtpew_fov * 10, 200);
	}

	if(XSystemCFG.indicators_enabled)
	{
		if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_left_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("LEFT"));
		}

		else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_right_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("RIGHT"));
		}

		else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_back_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("BACK"));
		}

		DrawString(indicators_font, 7, screenheight - 260, Color(255, 255, 255, 255), FONT_LEFT, ("L: %d "), getAALAngle());
		DrawString(indicators_font, 7, screenheight - 240, Color(255, 255, 255, 255), FONT_LEFT, ("R: %d "), getAARAngle());
		DrawString(indicators_font, 7, screenheight - 220, Color(255, 255, 255, 255), FONT_LEFT, ("B: %d "), getAABAngle());

		if (XSystemCFG.hvh_antiaim_y_desync > 100)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(255, 0, 30, 255), FONT_LEFT, ("FAKE"));
		}
		else if (XSystemCFG.hvh_antiaim_y_desync > 50)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(255, 150, 10, 255), FONT_LEFT, ("FAKE"));
		}
		else if (XSystemCFG.hvh_antiaim_y_desync > 0)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(116, 154, 30, 255), FONT_LEFT, ("FAKE"));
		}

		if (XSystemCFG.misc_fakelag_enabled)
		{
			DrawString(indicators_font, 7, screenheight - 120, Color(255, 150, 10, 255), FONT_LEFT, ("LC"));
		}

		if (XSystemCFG.hvh_antiaim_x > 0 || XSystemCFG.hvh_antiaim_y > 0)
		{
			DrawString(indicators_font, 7, screenheight - 100, Color(230, 230, 230, 255), FONT_LEFT, ("AA"));
		}

		if (XSystemCFG.hvh_antiaim_y_desync > 0)
		{
			DrawString(ui_font, 48, screenheight - 104, Color(255, 255, 255, 255), FONT_LEFT, ("MAX DSN: %d"), getDesyncAmount());
		}

		if (XSystemCFG.misc_fakelag_value > 6)
		{
			DrawString(ui_font, 48, screenheight - 95, Color(255, 0, 0, 255), FONT_LEFT, ("FL WARNING"));
		}

		if (g_InputSystem->IsButtonDown(XSystemCFG.misc_fakeduck))
		{
			DrawString(indicators_font, 7, screenheight - 80, Color(255, 0, 30, 255), FONT_LEFT, ("FD"));
		}
	}
}

void Visuals::DrawCapsuleOverlay(int idx, float duration)
{
	if (idx == g_EngineClient->GetLocalPlayer())
		return;

	auto player = C_BasePlayer::GetPlayerByIndex(idx);
	if (!player)
		return;

	if (XSystemCFG.angrpwaccboost > 0 && CMBacktracking::Get().m_LagRecord[player->EntIndex()].empty())
		return;

	matrix3x4_t boneMatrix_actual[MAXSTUDIOBONES];
	if (!player->SetupBones2(boneMatrix_actual, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, player->m_flSimulationTime()))
		return;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (studioHdr)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		std::memcpy(boneMatrix, CMBacktracking::Get().current_record[player->EntIndex()].matrix, sizeof(CMBacktracking::Get().current_record[player->EntIndex()].matrix));

		mstudiohitboxset_t *set = studioHdr->pHitboxSet(player->m_nHitboxSet());
		if (set)
		{
			for (int i = 0; i < set->numhitboxes; i++)
			{
				mstudiobbox_t *hitbox = set->pHitbox(i);
				if (hitbox)
				{
					if (hitbox->m_flRadius == -1.0f)
					{
						Vector position, position_actual;
						QAngle angles, angles_actual;
						MatrixAngles(boneMatrix[hitbox->bone], angles, position);
						MatrixAngles(boneMatrix_actual[hitbox->bone], angles_actual, position_actual);

						g_DebugOverlay->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, 255, 255, 255, 150, duration);

						if (XSystemCFG.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddBoxOverlay(position_actual, hitbox->bbmin, hitbox->bbmax, angles_actual, 255, 255, 255, 150, duration);
					}
					else
					{
						Vector min, max,
							min_actual, max_actual;

						Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

						Math::VectorTransform(hitbox->bbmin, boneMatrix_actual[hitbox->bone], min_actual);
						Math::VectorTransform(hitbox->bbmax, boneMatrix_actual[hitbox->bone], max_actual);

						g_DebugOverlay->AddCapsuleOverlay(min, max, hitbox->m_flRadius, 255, 255, 255, 150, duration);

						if (XSystemCFG.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddCapsuleOverlay(min_actual, max_actual, hitbox->m_flRadius, 255, 255, 255, 150, duration);
					}
				}
			}
		}
	}
}

void Visuals::RenderHealth()
{
	int health = ESP_ctx.player->m_iHealth();
	if (health > 100)
		health = 100;

	float box_h = (float)fabs(ESP_ctx.bbox.bottom - ESP_ctx.bbox.top);
	float off = 8;

	auto height = box_h - (((box_h * health) / 100));

	int x = ESP_ctx.bbox.left - off;
	int y = ESP_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	g_VGuiSurface->DrawSetColor(col_black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()])));
	g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
}

void Visuals::RenderWeapon()
{
	wchar_t buf[80];
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	auto weapon = ESP_ctx.player->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (weapon->m_hOwnerEntity().IsValid())
	{
		auto name = clean_item_name(weapon->GetClientClass()->m_pNetworkName);
		std::ostringstream oss;
		oss << name << " " << "[" << weapon->m_iClip1() << " / " << weapon->m_iPrimaryReserveAmmoCount() << "]";
		std::string var = oss.str();

		if (weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_KNIFE 
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_GRENADE 
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_C4
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_UNKNOWN)
		{
			if (MultiByteToWideChar(CP_UTF8, 0, var.c_str(), -1, buf, 80) > 0)
			{
				int tw, th;
				g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

				g_VGuiSurface->DrawSetTextFont(ui_font);
				g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
				g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.bottom + 1);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			}
		}
		else
		{
			if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
			{
				int tw, th;
				g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

				g_VGuiSurface->DrawSetTextFont(ui_font);
				g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
				g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.bottom + 1);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			}
		}
	}
}

void Visuals::RenderSnapline()
{
	g_EngineClient->GetScreenSize(Global::iScreenX, Global::iScreenY);
	int alpha = XSystemCFG.visuals_manual_aa_opacity;
	if (XSystemCFG.visuals_manual_aa)
	{
		if (Global::left) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(66, 134, 244, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(200, 200, 200, alpha), FONT_CENTER, "^");
		}
		if (Global::right) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(66, 134, 244, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(200, 200, 200, alpha), FONT_CENTER, "^");
		}
		if (Global::backwards) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(66, 134, 244, alpha), FONT_CENTER, "^");
		}
	}
}

void Visuals::RenderSkelet()
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (ESP_ctx.player->SetupBones2(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ESP_ctx.player->m_flSimulationTime()))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t *bone = studioHdr->pBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}


void Visuals::RenderBacktrackedSkelet()
{
	if (!XSystemCFG.angrpwaccboost > 0)
		return;

	static float fuckingrgb; // del this if dont work
	fuckingrgb += 0.0005; // del this if dont work

	if (fuckingrgb > 1.f) // del this if dont work
		fuckingrgb = 0.f; // del this if dont work

	Color wcsolja = Color::FromHSB(fuckingrgb, 1.f, 1.f);

	auto records = &CMBacktracking::Get().m_LagRecord[ESP_ctx.player->EntIndex()];
	if (records->size() < 2)
		return;

	Vector previous_screenpos;
	for (auto record = records->begin(); record != records->end(); record++)
	{
		if (!CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(record->m_flSimulationTime)))
			continue;

		Vector screen_pos;
		if (!Math::WorldToScreen(record->m_vecHeadSpot, screen_pos))
			continue;

		if (previous_screenpos.IsValid())
		{
			if (*record == CMBacktracking::Get().m_RestoreLagRecord[ESP_ctx.player->EntIndex()].first)
				g_VGuiSurface->DrawSetColor(Color(wcsolja));
			else
				g_VGuiSurface->DrawSetColor(Color(wcsolja));
			g_VGuiSurface->DrawLine(screen_pos.x, screen_pos.y, previous_screenpos.x, previous_screenpos.y);
		}

		previous_screenpos = screen_pos;
	}
}

void Visuals::RenderWeapon(C_BaseCombatWeapon *entity)
{
	wchar_t buf[80];
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (entity->m_hOwnerEntity().IsValid() ||
		entity->m_vecOrigin() == Vector(0, 0, 0))
		return;

	Vector pointsTransformed[8];
	auto bbox = GetBBox(entity, pointsTransformed);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(XSystemCFG.dropped_weapons_color[0], XSystemCFG.dropped_weapons_color[1],
		XSystemCFG.dropped_weapons_color[2], XSystemCFG.dropped_weapons_color[3]);
	switch (XSystemCFG.esp_dropped_weapons)
	{
	case 1:
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	case 3:
		break;
	}

	auto name = clean_item_name(entity->GetClientClass()->m_pNetworkName);
	if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
	{
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ui_font);
		g_VGuiSurface->DrawSetTextColor(XSystemCFG.dropped_weapons_color[0], XSystemCFG.dropped_weapons_color[1],
			XSystemCFG.dropped_weapons_color[2], XSystemCFG.dropped_weapons_color[3]);
		g_VGuiSurface->DrawSetTextPos(bbox.left + ((bbox.right - bbox.left) / 2) - (tw / 2), bbox.top + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

float bomb_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void Visuals::RenderPlantedC4(C_BaseEntity *entity)
{
	Vector screen_points[8];

//	C_BasePlayer* entity = nullptr;
	for (int i = 66; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		auto temp = C_BasePlayer::GetPlayerByIndex(i);
		if (!temp)
			continue;
		if (temp->GetClientClass()->m_ClassID == ClassId::ClassId_CC4 || temp->GetClientClass()->m_ClassID == ClassId::ClassId_CPlantedC4)
			entity = temp;
	}
	if (entity == nullptr || !entity || (entity->GetClientClass()->m_ClassID != ClassId::ClassId_CC4 && entity->GetClientClass()->m_ClassID != ClassId::ClassId_CPlantedC4))
		return;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)entity;
	CBaseHandle parent = weapon->m_hOwnerEntity();
	if ((parent.IsValid() || (entity->GetAbsOrigin().x == 0 && entity->GetAbsOrigin().y == 0 && entity->GetAbsOrigin().z == 0)) && entity->GetClientClass()->m_ClassID == ClassId::ClassId_CC4)
	{
		auto parentent = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(parent);
		if (parentent && parentent->IsAlive())
			carrier = parentent;
	}
	else
	{
		carrier = nullptr;

		auto bbox = GetBBox(entity, screen_points);

		int w, tw, th;
		if (bbox.right != 0 && bbox.bottom != 0)
		{

			g_VGuiSurface->DrawSetColor(Color::Red);
			g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
			g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
			g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
			g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

			const wchar_t* buf = L"C4";

			w = bbox.right - bbox.left;
			g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(ui_font);
			g_VGuiSurface->DrawSetTextColor(Color::Red);
			g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}

		if (entity->GetClientClass()->m_ClassID == ClassId::ClassId_CPlantedC4)
		{
			C_CSBomb* bomb = (C_CSBomb*)entity;
			float flBlow = bomb->m_flC4Blow();
			float TimeRemaining = flBlow - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AngryPew::Get().GetTickbase())) : (g_GlobalVars->curtime));
			float DefuseTime = bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AngryPew::Get().GetTickbase())) : (g_GlobalVars->curtime));
			std::string temp;  _bstr_t output;

			float flDistance = g_LocalPlayer->m_vecOrigin().DistTo(entity->m_vecOrigin()), a = 450.7f, b = 75.68f, c = 789.2f, d = ((flDistance - b) / c);
			float flDamage = a * exp(-d * d);

			int damage = max(floorf(bomb_Armor(flDamage, g_LocalPlayer->m_ArmorValue())), 0);

			int scrw, scrh; g_EngineClient->GetScreenSize(scrw, scrh);

			bool local_is_t = g_LocalPlayer->m_iTeamNum() == 2;

			Vector boxpos1 = Vector(scrw / 3, scrh - (scrh / 7), 0), boxpos2 = boxpos1; boxpos2.x *= 2; boxpos2.y -= scrh / 112;

			float lenght = boxpos2.x - boxpos1.x;

			float c4time = g_CVar->FindVar("mp_c4timer")->GetFloat();
			float timepercent = TimeRemaining / c4time;
			float defusepercent = DefuseTime / 10;
			C_BasePlayer* defuser = bomb->m_hBombDefuser();

			g_VGuiSurface->DrawSetColor(Color(40, 40, 40));
			g_VGuiSurface->DrawFilledRect(boxpos1.x, boxpos2.y, boxpos2.x, boxpos1.y);
			g_VGuiSurface->DrawSetColor(Color(10, 10, 10));
			g_VGuiSurface->DrawFilledRect(boxpos1.x + 1, boxpos2.y + 1, boxpos2.x - 1, boxpos1.y - 1);

			int offset = 0;

			if (checks::is_bad_ptr(defuser))
			{
				g_VGuiSurface->DrawSetColor(Color(255, 170, 0));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + lenght * timepercent) - 2, boxpos1.y - 2);
			}
			else
			{
				g_VGuiSurface->DrawSetColor(Color(255, 170, 0));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + lenght * timepercent) - 2, boxpos1.y - 2);

				g_VGuiSurface->DrawSetColor(Color(0, 170, 255));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + (lenght * (10 / c4time)) * defusepercent) - 2, boxpos1.y - 2);

				g_VGuiSurface->DrawSetColor(Color(0, 110, 255));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + min(((lenght * (10 / c4time)) * defusepercent), lenght * timepercent) - 2), boxpos1.y - 2);

				if ((bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AngryPew::Get().GetTickbase())) : (g_GlobalVars->curtime))) < TimeRemaining || !local_is_t)
					offset = 10;
			}

			g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 128));
			g_VGuiSurface->DrawLine((boxpos1.x + lenght * (10 / c4time)), boxpos2.y + 3, (boxpos1.x + lenght * (10 / c4time)), boxpos1.y - 3);
			g_VGuiSurface->DrawLine((boxpos1.x + lenght * (5 / c4time)), boxpos2.y + 3, (boxpos1.x + lenght * (5 / c4time)), boxpos1.y - 3);

			g_VGuiSurface->DrawSetTextFont(ui_font);
			g_VGuiSurface->DrawSetTextColor(Color(255, 255, 255));

			temp = (char)(bomb->m_nBombSite() + 0x41); // bombsite netvar is 0 when a, 1 when b. ascii 0x41 = A, 0x42 = B
			temp += " Site";
			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 40 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			temp = "Bomb Timer: ";
			temp += std::to_string(TimeRemaining);
			if (!checks::is_bad_ptr(defuser))
			{
				temp += "  Defuse Timer: ";
				temp += std::to_string(DefuseTime);
			}
			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 30 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			temp = "Bomb Damage: ";
			temp += std::to_string(damage);
			if (damage >= g_LocalPlayer->m_iHealth())
			{
				temp += " (Lethal)"; g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0));
			}
			else g_VGuiSurface->DrawSetTextColor(Color(0, 255, 0));

			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 20 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			if (!checks::is_bad_ptr(defuser))
			{
				g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 20);
				if ((bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AngryPew::Get().GetTickbase())) : (g_GlobalVars->curtime))) < TimeRemaining)
				{
					temp = (local_is_t) ? ("Getting Defused!") : ("Defusable");
					g_VGuiSurface->DrawSetTextColor((local_is_t) ? (Color(255, 0, 0)) : (Color(0, 255, 0)));
				}
				else if (!local_is_t)
				{
					temp = "Defusable";
					g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0));
				}

				output = temp.c_str();
				g_VGuiSurface->DrawPrintText(output, wcslen(output));
			}
		}
	}
}
void Visuals::RenderSpectatorList()
{
	RECT scrn = GetViewport();
	int cnt = 0;
	wchar_t buf[128];

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || player == nullptr)
			continue;

		player_info_t player_info;
		if (player != g_LocalPlayer)
		{
			if (g_EngineClient->GetPlayerInfo(i, &player_info) && !player->IsAlive() && !player->IsDormant())
			{
				auto observer_target = player->m_hObserverTarget();
				if (!observer_target)
					continue;

				auto target = observer_target.Get();
				if (!target)
					continue;

				wchar_t buf[128];

				player_info_t player_info2;
				if (g_EngineClient->GetPlayerInfo(target->EntIndex(), &player_info2))
				{
					if (target->EntIndex() == g_LocalPlayer->EntIndex())
					{
						char player_name[255] = { 0 };
						sprintf_s(player_name, "%s", player_info.szName);

						int w, h;
						int centerW, centerH;
						g_EngineClient->GetScreenSize(w, h);
						centerW = w / 2;
						centerH = h / 2;

						GetTextSize(ui_font, player_name, w, h);

						std::string name = player_name,
							s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

						if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0) //  \/[space between specs]
						{
						DrawString(spectatorlist_font, false, scrn.right - w - 40, ((scrn.bottom / 2) + 25) + (10 * cnt), Color(240, 240, 240, 255), s_name.c_str());
						++cnt;
						}		
					}
				}
			}
		}
	}
}

std::string GetTimeString()
{
	time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

int getfps()
{
	return static_cast<int>(1.f / g_GlobalVars->frametime);
}

void Visuals::DrawWatermark()
{
	//int alpha;
	int centerW, centerH;
	//int monstw, monsth;
	int w, h;
	//g_EngineClient->GetScreenSize(w, h);
	centerW = w / 2;
	centerH = h / 2;
	//monstw = w;
	//monsth = h;
	//alpha = 255;

	/// RGB ///
	//static float rainbow; // del this if dont work
	//rainbow += 0.0005; // del this if dont work

	//if (rainbow > 1.f) // del this if dont work
	//	rainbow = 0.f; // del this if dont work

	//Color wmline = Color::FromHSB(rainbow, 1.f, 1.f);
	/// RECT ///
	//g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 410, 9, (centerW * 2) - 410 + 192, 9 + 29);
	//g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 409, 10, (centerW * 2) - 409 + 190, 10 + 27);
	//g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 408, 11, (centerW * 2) - 408 + 188, 11 + 25);
	//g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 405, 14, (centerW * 2) - 405 + 182, 14 + 19);
	//g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 404, 15, (centerW * 2) - 404 + 180, 15 + 17);

	/// LINE ON TOP OF WATERMARK ///

	//g_VGuiSurface->DrawSetColor(Color(wmline));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 403, 16, (centerW * 2) - 403 + 178, 16 + 1);

	// nanosense GRADIENT COLORS: LEFT(30, 87, 153)  MIDDLE(243, 0, 255)  RIGHT(224, 255, 0)


	/// TEXT ///
	//DrawString(ui_font, (centerW * 2) - 400, 24, Color(240, 240, 240, 255), FONT_LEFT, ("gamesense | %s |"), GetTimeString().c_str());

	//if (getfps() < 50)
	//{
	//	DrawString(ui_font, (centerW * 2) - 274, 24, Color(255, 0, 0, 255), FONT_LEFT, (" fps: %d "), getfps());
	//}
	//else
	//{
	//	DrawString(ui_font, (centerW * 2) - 274, 24, Color(240, 240, 240, 255), FONT_LEFT, (" fps: %d "), getfps());
	//}

	
	/// SPECTATOR LIST ///

	if (XSystemCFG.misc_spectatorlist)
	{
		static float rainbow;
		rainbow += 0.0005;

		if (rainbow > 1.f)
			rainbow = 0.f;

		Color wmline = Color::FromHSB(rainbow, 1.f, 1.f);
		g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 199, centerH + 2, (centerW * 2) - 199 + 190, (centerH + 2) + 137);
		g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 198, centerH + 3, (centerW * 2) - 198 + 188, (centerH + 3) + 135);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 195, centerH + 6, (centerW * 2) - 195 + 182, (centerH + 6) + 129);
		g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 194, centerH + 7, (centerW * 2) - 194 + 180, (centerH + 7) + 127);
		g_VGuiSurface->DrawSetColor(Color(149, 184, 6));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 192, centerH + 21, (centerW * 2) - 193 + 178, (centerH + 7) + 15);
		DrawString(ui_font, (centerW * 2) - 140, centerH + 14, Color(255, 255, 255, 255), FONT_LEFT, ("Spectator List"));
	}
	static auto disable_blur = g_CVar->FindVar("@panorama_disable_blur");
	disable_blur->m_fnChangeCallbacks.m_Size = 0;
	disable_blur->SetValue(1);
}

void Visuals::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = g_VGuiSurface->CreateNewTextureID(true);
	unsigned char buffer[4] = { color.r(), color.g(), color.b(), color.a() };

	g_VGuiSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
	g_VGuiSurface->DrawSetTexture(Texture);

	g_VGuiSurface->DrawTexturedPolygon(count, Vertexs);
}

void Visuals::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int *x, int *y, int count, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawPolyLine(x, y, count);
}

void Visuals::Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
	Vector forward, right, up;
	Math::AngleVectors(angles, forward, right, up);

	Vector points[8];
	points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
	points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
	points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
	points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

	points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
	points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
	points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
	points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

	Vector points_screen[8];
	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(points[i], points_screen[i]))
			return;

	g_VGuiSurface->DrawSetColor(outline);

	// Back frame
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

	// Frame connector
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
	g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

	// Front frame
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
}

void Visuals::FillRGBA(int x, int y, int w, int h, Color c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::BorderBox(int x, int y, int w, int h, Color color, int thickness)
{
	FillRGBA(x, y, w, thickness, color);
	FillRGBA(x, y, thickness, h, color);
	FillRGBA(x + w, y, thickness, h, color);
	FillRGBA(x, y + h, w + thickness, thickness, color);
}

__inline void Visuals::DrawFilledRect(int x, int y, int w, int h)
{
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness)
{
	FillRGBA(x, y, w, h, color);
	BorderBox(x - 1, y - 1, w + 1, h + 1, outlinedColor, thickness);
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	FUNCTION_GUARD;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}

void Visuals::DrawString(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...)
{
	wchar_t *pszStringWide = reinterpret_cast< wchar_t* >(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
}

void Visuals::TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString)
{
	if (center)
	{
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(c);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
}

void Visuals::DrawCircle(int x, int y, float r, int step, Color color)
{
	float Step = PI * 2.0 / step;
	for (float a = 0; a < (PI*2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	}
}

void Visuals::DrawOutlinedRect(int x, int y, int w, int h, Color &c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Visuals::GetTextSize(unsigned long font, const char *txt, int &width, int &height)
{
	FUNCTION_GUARD;

	size_t origsize = strlen(txt) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	int x, y;

	mbstowcs_s(&convertedChars, wcstring, origsize, txt, _TRUNCATE);

	g_VGuiSurface->GetTextSize(font, wcstring, x, y);

	width = x;
	height = y;
}

RECT Visuals::GetViewport()
{
	RECT viewport = { 0, 0, 0, 0 };
	viewport.right = Global::iScreenX; viewport.bottom = Global::iScreenY;

	return viewport;
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void OgUMSRQzztzPyXvIrPpKgVqfPrRFsXkWCYGjYfdw43694240() {     int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73814688 = -318873865;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66364439 = -173906242;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47723282 = -938896795;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85941441 = -135526553;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85343682 = -478512809;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf87521901 = -982323396;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70981499 = -258602322;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70781099 = -707462505;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf63374957 = 52340151;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45899522 = -624048195;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92647838 = -659658877;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73405562 = 50183693;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf67307540 = -550417267;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72549768 = -734792533;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42849295 = -650571848;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93697402 = 36758857;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38323113 = -238678630;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82634279 = -620830602;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf55351292 = -897698411;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36778622 = -479168435;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10513406 = -636813414;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82093028 = -440971455;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf20837974 = -877045654;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10878972 = 20049243;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf58167635 = -717157239;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50664553 = 2798653;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42802931 = -128741291;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70938432 = -369057718;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3863692 = -879859064;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf25279549 = -449445716;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34303878 = -401481217;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf48501153 = -146779558;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf80822522 = -450778;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94655353 = -722645774;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85769839 = -540821729;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf81331391 = -694051490;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf69689099 = -505953385;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43445791 = -425321084;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf79977509 = -64565107;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44574180 = -700928709;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93385649 = 65322837;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11510007 = -70110541;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf21311471 = -492955257;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70782753 = -699732099;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf14905818 = -305811152;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf54503598 = -556329514;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47751619 = 11709943;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92334765 = 7184360;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf40106399 = -851145970;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf71416229 = -938351559;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf96484285 = -734552959;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70056558 = -888650804;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf37371430 = -186924897;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf39060259 = -869773362;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf33180835 = -584836130;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf91721659 = -877902410;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45526465 = -296860589;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36844311 = -858946039;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf27773807 = -418369314;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34679129 = -381311463;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44718971 = -753582105;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43067 = -889544605;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66917407 = -827603441;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38095408 = -498214134;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11595644 = -122566978;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44146686 = -412879320;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92583040 = -949365529;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72652187 = -827771494;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf86779928 = -93970804;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61517904 = -956520358;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf24008303 = -457287758;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94877321 = -813357547;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2656770 = -456265495;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10777113 = -96769703;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43392972 = -444491273;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99003398 = -466702874;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf60781558 = -948016199;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50055221 = -77313556;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf95973153 = -674139605;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3664037 = -60827725;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2912934 = 91088709;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50468165 = -35925652;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf30832033 = -517911749;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32447462 = -941507506;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf28795264 = -714892757;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf64247319 = -512830413;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11129724 = -959854661;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf41762263 = -130677416;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61474519 = -37809645;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94048179 = -662919320;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf35804926 = -297190902;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32844789 = -647007347;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf15671985 = 93048230;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45298380 = -683253645;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99855209 = -947346604;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93342582 = -45132558;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44592600 = -242507100;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf83216063 = -994741124;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf59187109 = -477165121;    int IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70759132 = -318873865;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73814688 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66364439;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66364439 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47723282;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47723282 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85941441;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85941441 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85343682;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85343682 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf87521901;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf87521901 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70981499;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70981499 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70781099;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70781099 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf63374957;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf63374957 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45899522;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45899522 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92647838;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92647838 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73405562;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73405562 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf67307540;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf67307540 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72549768;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72549768 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42849295;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42849295 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93697402;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93697402 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38323113;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38323113 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82634279;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82634279 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf55351292;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf55351292 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36778622;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36778622 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10513406;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10513406 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82093028;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf82093028 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf20837974;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf20837974 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10878972;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10878972 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf58167635;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf58167635 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50664553;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50664553 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42802931;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf42802931 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70938432;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70938432 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3863692;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3863692 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf25279549;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf25279549 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34303878;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34303878 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf48501153;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf48501153 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf80822522;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf80822522 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94655353;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94655353 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85769839;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf85769839 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf81331391;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf81331391 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf69689099;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf69689099 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43445791;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43445791 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf79977509;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf79977509 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44574180;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44574180 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93385649;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93385649 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11510007;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11510007 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf21311471;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf21311471 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70782753;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70782753 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf14905818;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf14905818 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf54503598;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf54503598 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47751619;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf47751619 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92334765;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92334765 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf40106399;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf40106399 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf71416229;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf71416229 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf96484285;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf96484285 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70056558;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70056558 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf37371430;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf37371430 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf39060259;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf39060259 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf33180835;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf33180835 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf91721659;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf91721659 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45526465;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45526465 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36844311;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf36844311 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf27773807;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf27773807 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34679129;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf34679129 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44718971;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44718971 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43067;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43067 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66917407;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf66917407 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38095408;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf38095408 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11595644;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11595644 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44146686;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44146686 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92583040;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf92583040 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72652187;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf72652187 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf86779928;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf86779928 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61517904;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61517904 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf24008303;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf24008303 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94877321;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94877321 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2656770;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2656770 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10777113;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf10777113 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43392972;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf43392972 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99003398;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99003398 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf60781558;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf60781558 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50055221;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50055221 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf95973153;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf95973153 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3664037;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf3664037 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2912934;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf2912934 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50468165;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf50468165 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf30832033;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf30832033 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32447462;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32447462 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf28795264;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf28795264 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf64247319;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf64247319 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11129724;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf11129724 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf41762263;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf41762263 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61474519;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf61474519 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94048179;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf94048179 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf35804926;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf35804926 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32844789;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf32844789 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf15671985;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf15671985 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45298380;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf45298380 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99855209;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf99855209 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93342582;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf93342582 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44592600;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf44592600 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf83216063;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf83216063 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf59187109;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf59187109 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70759132;     IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf70759132 = IRHIyRGTOnXKxstsqoxaALGcIdNaRLjMmsBOOhgxlyqFAaYJeNtjiCwxKfAUEfBDDKYFrf73814688;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jNiOSdgwRtEXYKJsdTRAvRgtQpFnElsGVikyHfBRFFtexTJFdPA95739656() {     float XQtZSgoxdYsrgAxCpZKkxRAvqXCw27043903 = -85978153;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw33307451 = -823800802;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw3477332 = -207447856;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw18457249 = -828652235;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw32375749 = -403143493;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw94145160 = -337676805;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw21338989 = -572894973;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw66456229 = -10345882;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw65030967 = -775803807;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw45609637 = 54844133;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw95336366 = -651792848;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw65450965 = -338287903;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw9782589 = -745565694;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw84083926 = -193122315;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw59723882 = -131529697;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw41881066 = -960739505;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw58924664 = -72869844;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw37604149 = -703483880;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw5817415 = -614558321;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw55044101 = -445109990;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw44704586 = 22692203;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw91405048 = -542361458;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw96509801 = -831571667;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw76552143 = -706097639;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw56781680 = -466803017;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw28235803 = -75550249;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw50103977 = -158228874;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw69165421 = -135028003;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw21629142 = -308966566;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw13544444 = -680862251;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw20020915 = -885569200;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw2315816 = -251463299;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw80085230 = -94007794;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw22752996 = -735361992;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw46502192 = -942468287;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw74024539 = -899130088;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw92382683 = -247359625;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw22037792 = -424568677;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw31555139 = -300274663;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw93518688 = -381413110;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw39102573 = -600519098;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw89564437 = -889120258;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw87421391 = -742811661;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw19867798 = -165946952;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw72757347 = -128710432;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw68458629 = -152216308;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw3047702 = -534401853;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw9631930 = -42893218;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw1276413 = -466903191;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw3758065 = -495007940;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw97098071 = -970911097;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw44876740 = -804405280;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw2267283 = -990158144;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw36323000 = -517820176;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw51035070 = -45587451;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw35638854 = -543616696;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw36797649 = -992229136;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw26925188 = -501350217;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw61675568 = -261849218;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw4139947 = -227593245;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw44041183 = -79447932;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw52173568 = -337866970;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw44827088 = -701379316;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw51486524 = 5058444;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw25588722 = -59586668;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw93020551 = -300329550;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw85365734 = -144280109;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw87029592 = 89796297;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw37581735 = -250654028;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw85699343 = -232399610;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw49498382 = -613379880;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw36886872 = -648301167;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw6049010 = -303209217;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw12298727 = -133145212;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw15941528 = -844590893;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw55140149 = -88187540;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw3983658 = -799549797;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw76642003 = -565624715;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw3794797 = -477387207;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw88323051 = -214586710;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw25188101 = -541148396;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw40472047 = -15335656;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw67889008 = -668124812;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw17871077 = -813958626;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw16446372 = -709951154;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw75144175 = 18836080;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw48533 = -261305155;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw43762230 = -576187618;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw71717925 = -589774541;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw10863339 = -298851592;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw37226890 = -906900953;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw65457496 = -746009408;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw60362224 = -62719460;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw27415192 = 27318581;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw49477505 = -201965179;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw86929005 = -162652128;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw44737349 = -87740943;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw35934867 = -647870105;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw94279075 = -6360284;    float XQtZSgoxdYsrgAxCpZKkxRAvqXCw79736795 = -85978153;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw27043903 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw33307451;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw33307451 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw3477332;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw3477332 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw18457249;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw18457249 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw32375749;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw32375749 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw94145160;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw94145160 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw21338989;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw21338989 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw66456229;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw66456229 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw65030967;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw65030967 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw45609637;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw45609637 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw95336366;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw95336366 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw65450965;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw65450965 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw9782589;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw9782589 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw84083926;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw84083926 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw59723882;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw59723882 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw41881066;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw41881066 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw58924664;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw58924664 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw37604149;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw37604149 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw5817415;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw5817415 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw55044101;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw55044101 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw44704586;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw44704586 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw91405048;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw91405048 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw96509801;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw96509801 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw76552143;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw76552143 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw56781680;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw56781680 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw28235803;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw28235803 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw50103977;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw50103977 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw69165421;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw69165421 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw21629142;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw21629142 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw13544444;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw13544444 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw20020915;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw20020915 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw2315816;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw2315816 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw80085230;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw80085230 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw22752996;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw22752996 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw46502192;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw46502192 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw74024539;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw74024539 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw92382683;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw92382683 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw22037792;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw22037792 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw31555139;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw31555139 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw93518688;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw93518688 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw39102573;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw39102573 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw89564437;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw89564437 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw87421391;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw87421391 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw19867798;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw19867798 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw72757347;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw72757347 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw68458629;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw68458629 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw3047702;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw3047702 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw9631930;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw9631930 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw1276413;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw1276413 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw3758065;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw3758065 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw97098071;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw97098071 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw44876740;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw44876740 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw2267283;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw2267283 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw36323000;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw36323000 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw51035070;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw51035070 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw35638854;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw35638854 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw36797649;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw36797649 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw26925188;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw26925188 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw61675568;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw61675568 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw4139947;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw4139947 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw44041183;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw44041183 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw52173568;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw52173568 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw44827088;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw44827088 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw51486524;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw51486524 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw25588722;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw25588722 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw93020551;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw93020551 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw85365734;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw85365734 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw87029592;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw87029592 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw37581735;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw37581735 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw85699343;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw85699343 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw49498382;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw49498382 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw36886872;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw36886872 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw6049010;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw6049010 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw12298727;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw12298727 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw15941528;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw15941528 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw55140149;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw55140149 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw3983658;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw3983658 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw76642003;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw76642003 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw3794797;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw3794797 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw88323051;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw88323051 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw25188101;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw25188101 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw40472047;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw40472047 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw67889008;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw67889008 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw17871077;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw17871077 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw16446372;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw16446372 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw75144175;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw75144175 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw48533;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw48533 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw43762230;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw43762230 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw71717925;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw71717925 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw10863339;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw10863339 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw37226890;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw37226890 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw65457496;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw65457496 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw60362224;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw60362224 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw27415192;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw27415192 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw49477505;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw49477505 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw86929005;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw86929005 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw44737349;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw44737349 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw35934867;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw35934867 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw94279075;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw94279075 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw79736795;     XQtZSgoxdYsrgAxCpZKkxRAvqXCw79736795 = XQtZSgoxdYsrgAxCpZKkxRAvqXCw27043903;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jPUzKIpcgjANOJLwUCOETAefy70587554() {     long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs84640212 = -936908815;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs39737471 = -102234418;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs4427933 = -558011834;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69402120 = -667474273;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs91469601 = -544572867;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs45614490 = -222223050;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37640107 = -263148055;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98831386 = -460722581;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16196766 = -111835376;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10191194 = -161408495;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs8563848 = 81545903;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11119306 = -91010517;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72955017 = -445156516;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16143539 = -783604484;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69962884 = -741325921;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62613083 = 56656332;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs43060403 = -299644263;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs30695448 = -607494455;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs79503137 = 50266755;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46296138 = -784741330;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs957812 = -89030809;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85985237 = -150681906;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89399299 = -582032863;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81780183 = -965028733;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs66675569 = -779051561;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35114987 = -30408033;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50034725 = -302769297;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62028241 = -403772280;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9303280 = 10783921;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46642098 = -637569448;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35940220 = -171055432;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs6340049 = -73418628;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs33233530 = -777170780;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs52293874 = -286556707;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs5809654 = -149494639;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50107417 = -436538893;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs48558054 = -347267092;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41201172 = -101150944;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16423772 = -863123508;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41596727 = -962942360;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs94479885 = -35349910;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7563661 = -225784817;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85652617 = -780739189;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs21696536 = -929286063;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9572751 = -902844437;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83621337 = -811527786;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2276197 = -169830133;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50514627 = -793174686;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50085079 = -597913360;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs55457496 = -450469744;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72566226 = -392807517;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81258217 = -94114450;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs86931630 = -773189310;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32558491 = -693357489;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69629743 = -730130729;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98654974 = -686226910;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50338171 = -520201555;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs22647749 = -592983101;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2726552 = -888422712;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs56354615 = -414164834;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs95579764 = -919453753;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs75611865 = -859375776;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89528106 = -371506502;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69554668 = -474265929;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74250973 = -990353063;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2223800 = -845035469;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs77885775 = -313839737;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs20661144 = -58599809;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10333885 = -534109845;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19855467 = -204787028;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14055029 = -596076577;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs1859231 = -98493320;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14271677 = -744370948;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37906410 = 13209115;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs51816252 = -649391421;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs93394151 = -863245993;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs332620 = -369942717;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs67702764 = -652746801;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72207432 = 37815703;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83054231 = -967523776;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32838790 = -860577901;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99520097 = -509594612;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11943163 = -805858920;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs53845784 = -538746336;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74075872 = -144761931;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs54682003 = 23059017;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19408419 = -300229319;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs675040 = 16186708;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs82664131 = -556425979;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7154679 = -463267729;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99769245 = -916337338;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs25910305 = -754283992;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs38474621 = -212728232;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs60069156 = -348958674;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46016963 = 56511392;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18868020 = -175974135;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18035554 = -854278316;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16097950 = -206473261;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs47445562 = -938933000;    long HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7348952 = -936908815;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs84640212 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs39737471;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs39737471 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs4427933;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs4427933 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69402120;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69402120 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs91469601;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs91469601 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs45614490;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs45614490 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37640107;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37640107 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98831386;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98831386 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16196766;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16196766 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10191194;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10191194 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs8563848;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs8563848 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11119306;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11119306 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72955017;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72955017 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16143539;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16143539 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69962884;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69962884 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62613083;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62613083 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs43060403;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs43060403 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs30695448;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs30695448 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs79503137;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs79503137 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46296138;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46296138 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs957812;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs957812 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85985237;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85985237 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89399299;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89399299 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81780183;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81780183 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs66675569;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs66675569 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35114987;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35114987 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50034725;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50034725 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62028241;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs62028241 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9303280;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9303280 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46642098;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46642098 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35940220;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs35940220 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs6340049;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs6340049 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs33233530;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs33233530 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs52293874;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs52293874 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs5809654;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs5809654 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50107417;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50107417 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs48558054;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs48558054 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41201172;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41201172 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16423772;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16423772 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41596727;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs41596727 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs94479885;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs94479885 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7563661;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7563661 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85652617;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs85652617 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs21696536;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs21696536 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9572751;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs9572751 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83621337;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83621337 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2276197;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2276197 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50514627;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50514627 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50085079;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50085079 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs55457496;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs55457496 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72566226;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72566226 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81258217;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs81258217 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs86931630;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs86931630 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32558491;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32558491 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69629743;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69629743 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98654974;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs98654974 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50338171;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs50338171 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs22647749;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs22647749 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2726552;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2726552 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs56354615;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs56354615 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs95579764;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs95579764 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs75611865;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs75611865 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89528106;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs89528106 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69554668;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs69554668 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74250973;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74250973 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2223800;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs2223800 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs77885775;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs77885775 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs20661144;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs20661144 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10333885;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs10333885 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19855467;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19855467 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14055029;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14055029 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs1859231;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs1859231 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14271677;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs14271677 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37906410;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs37906410 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs51816252;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs51816252 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs93394151;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs93394151 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs332620;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs332620 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs67702764;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs67702764 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72207432;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs72207432 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83054231;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs83054231 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32838790;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs32838790 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99520097;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99520097 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11943163;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs11943163 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs53845784;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs53845784 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74075872;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs74075872 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs54682003;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs54682003 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19408419;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs19408419 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs675040;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs675040 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs82664131;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs82664131 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7154679;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7154679 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99769245;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs99769245 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs25910305;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs25910305 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs38474621;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs38474621 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs60069156;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs60069156 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46016963;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs46016963 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18868020;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18868020 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18035554;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs18035554 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16097950;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs16097950 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs47445562;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs47445562 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7348952;     HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs7348952 = HNRBzGRBUkSMuRQTICoeFtHABCIonRjloACvYagdjMuZs84640212;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qrsbBGfuaQSYLxKrYfDkieFxkDixnMVlNgHxY61909387() {     double dqgxcABITeIrbiaFxWMlv91949220 = -234138268;    double dqgxcABITeIrbiaFxWMlv38656880 = -729642278;    double dqgxcABITeIrbiaFxWMlv89842438 = -505204116;    double dqgxcABITeIrbiaFxWMlv67341117 = -745890255;    double dqgxcABITeIrbiaFxWMlv88173569 = -130365692;    double dqgxcABITeIrbiaFxWMlv27755493 = -506811294;    double dqgxcABITeIrbiaFxWMlv98784149 = -127468897;    double dqgxcABITeIrbiaFxWMlv38731467 = -458024370;    double dqgxcABITeIrbiaFxWMlv29097941 = -108901188;    double dqgxcABITeIrbiaFxWMlv57012205 = -994004808;    double dqgxcABITeIrbiaFxWMlv53718988 = -34286050;    double dqgxcABITeIrbiaFxWMlv47645872 = -121560059;    double dqgxcABITeIrbiaFxWMlv93007513 = -945649428;    double dqgxcABITeIrbiaFxWMlv75762173 = -694135461;    double dqgxcABITeIrbiaFxWMlv43342227 = -372838885;    double dqgxcABITeIrbiaFxWMlv66125917 = -580894069;    double dqgxcABITeIrbiaFxWMlv90898564 = -445402058;    double dqgxcABITeIrbiaFxWMlv94311590 = -269297860;    double dqgxcABITeIrbiaFxWMlv75187858 = -817411635;    double dqgxcABITeIrbiaFxWMlv92900176 = -52696310;    double dqgxcABITeIrbiaFxWMlv45098695 = -403899298;    double dqgxcABITeIrbiaFxWMlv14581005 = -199488692;    double dqgxcABITeIrbiaFxWMlv63036635 = -641685823;    double dqgxcABITeIrbiaFxWMlv25134722 = 42296776;    double dqgxcABITeIrbiaFxWMlv92096685 = -180989418;    double dqgxcABITeIrbiaFxWMlv30061158 = -958065916;    double dqgxcABITeIrbiaFxWMlv40605251 = -7518281;    double dqgxcABITeIrbiaFxWMlv96277892 = -67238688;    double dqgxcABITeIrbiaFxWMlv26988430 = -640362728;    double dqgxcABITeIrbiaFxWMlv30137104 = -28809339;    double dqgxcABITeIrbiaFxWMlv30356264 = 90932612;    double dqgxcABITeIrbiaFxWMlv29582242 = -301149016;    double dqgxcABITeIrbiaFxWMlv72611544 = -393422122;    double dqgxcABITeIrbiaFxWMlv88622094 = -796109110;    double dqgxcABITeIrbiaFxWMlv29188068 = -144037524;    double dqgxcABITeIrbiaFxWMlv22253482 = -158282310;    double dqgxcABITeIrbiaFxWMlv63510722 = -955249165;    double dqgxcABITeIrbiaFxWMlv87958692 = -681138862;    double dqgxcABITeIrbiaFxWMlv83813759 = -271163039;    double dqgxcABITeIrbiaFxWMlv64821901 = -587207862;    double dqgxcABITeIrbiaFxWMlv3558187 = -326619442;    double dqgxcABITeIrbiaFxWMlv28833020 = -772572131;    double dqgxcABITeIrbiaFxWMlv25004968 = -766232524;    double dqgxcABITeIrbiaFxWMlv67274566 = -216069767;    double dqgxcABITeIrbiaFxWMlv65367966 = -295695568;    double dqgxcABITeIrbiaFxWMlv44658121 = -265474334;    double dqgxcABITeIrbiaFxWMlv28963393 = 32482487;    double dqgxcABITeIrbiaFxWMlv91555549 = -66007669;    double dqgxcABITeIrbiaFxWMlv44189631 = -244446287;    double dqgxcABITeIrbiaFxWMlv81672859 = -564717675;    double dqgxcABITeIrbiaFxWMlv47775209 = -443026717;    double dqgxcABITeIrbiaFxWMlv89818840 = -442712330;    double dqgxcABITeIrbiaFxWMlv59636469 = -896102936;    double dqgxcABITeIrbiaFxWMlv70350973 = -400498815;    double dqgxcABITeIrbiaFxWMlv16810693 = -42557282;    double dqgxcABITeIrbiaFxWMlv77368215 = 65350423;    double dqgxcABITeIrbiaFxWMlv75620245 = 12043545;    double dqgxcABITeIrbiaFxWMlv64707717 = -447500892;    double dqgxcABITeIrbiaFxWMlv75244431 = -464900837;    double dqgxcABITeIrbiaFxWMlv58112412 = -172299776;    double dqgxcABITeIrbiaFxWMlv87150241 = -399293014;    double dqgxcABITeIrbiaFxWMlv2506257 = 39769790;    double dqgxcABITeIrbiaFxWMlv11743038 = -817661642;    double dqgxcABITeIrbiaFxWMlv98960837 = 19908150;    double dqgxcABITeIrbiaFxWMlv26655941 = -984937421;    double dqgxcABITeIrbiaFxWMlv24136746 = -733137034;    double dqgxcABITeIrbiaFxWMlv75034327 = -728137937;    double dqgxcABITeIrbiaFxWMlv4385419 = -49540318;    double dqgxcABITeIrbiaFxWMlv46574105 = -450097938;    double dqgxcABITeIrbiaFxWMlv21088745 = -114556576;    double dqgxcABITeIrbiaFxWMlv2615195 = -625644904;    double dqgxcABITeIrbiaFxWMlv2939872 = -764263197;    double dqgxcABITeIrbiaFxWMlv10497831 = -998134822;    double dqgxcABITeIrbiaFxWMlv10365958 = -130203774;    double dqgxcABITeIrbiaFxWMlv89341989 = -726076868;    double dqgxcABITeIrbiaFxWMlv16265675 = -631327168;    double dqgxcABITeIrbiaFxWMlv89576037 = -433256169;    double dqgxcABITeIrbiaFxWMlv95762068 = -325616056;    double dqgxcABITeIrbiaFxWMlv59766755 = -662007656;    double dqgxcABITeIrbiaFxWMlv47438564 = -915515085;    double dqgxcABITeIrbiaFxWMlv1097765 = -890548403;    double dqgxcABITeIrbiaFxWMlv49049702 = -941510613;    double dqgxcABITeIrbiaFxWMlv52088262 = -822792401;    double dqgxcABITeIrbiaFxWMlv45315570 = 24354947;    double dqgxcABITeIrbiaFxWMlv82361895 = -585782623;    double dqgxcABITeIrbiaFxWMlv40537424 = -466355058;    double dqgxcABITeIrbiaFxWMlv69945773 = -405046081;    double dqgxcABITeIrbiaFxWMlv2260572 = -992923308;    double dqgxcABITeIrbiaFxWMlv71811402 = -653551829;    double dqgxcABITeIrbiaFxWMlv51819853 = -109387947;    double dqgxcABITeIrbiaFxWMlv46633237 = -70325855;    double dqgxcABITeIrbiaFxWMlv98803005 = -407748273;    double dqgxcABITeIrbiaFxWMlv12714261 = -116238025;    double dqgxcABITeIrbiaFxWMlv25701348 = 1136737;    double dqgxcABITeIrbiaFxWMlv77671659 = -87914848;    double dqgxcABITeIrbiaFxWMlv1051930 = -266389233;    double dqgxcABITeIrbiaFxWMlv17089983 = -954910489;    double dqgxcABITeIrbiaFxWMlv26044130 = -686140674;    double dqgxcABITeIrbiaFxWMlv40618625 = -231132347;    double dqgxcABITeIrbiaFxWMlv41231220 = -234138268;     dqgxcABITeIrbiaFxWMlv91949220 = dqgxcABITeIrbiaFxWMlv38656880;     dqgxcABITeIrbiaFxWMlv38656880 = dqgxcABITeIrbiaFxWMlv89842438;     dqgxcABITeIrbiaFxWMlv89842438 = dqgxcABITeIrbiaFxWMlv67341117;     dqgxcABITeIrbiaFxWMlv67341117 = dqgxcABITeIrbiaFxWMlv88173569;     dqgxcABITeIrbiaFxWMlv88173569 = dqgxcABITeIrbiaFxWMlv27755493;     dqgxcABITeIrbiaFxWMlv27755493 = dqgxcABITeIrbiaFxWMlv98784149;     dqgxcABITeIrbiaFxWMlv98784149 = dqgxcABITeIrbiaFxWMlv38731467;     dqgxcABITeIrbiaFxWMlv38731467 = dqgxcABITeIrbiaFxWMlv29097941;     dqgxcABITeIrbiaFxWMlv29097941 = dqgxcABITeIrbiaFxWMlv57012205;     dqgxcABITeIrbiaFxWMlv57012205 = dqgxcABITeIrbiaFxWMlv53718988;     dqgxcABITeIrbiaFxWMlv53718988 = dqgxcABITeIrbiaFxWMlv47645872;     dqgxcABITeIrbiaFxWMlv47645872 = dqgxcABITeIrbiaFxWMlv93007513;     dqgxcABITeIrbiaFxWMlv93007513 = dqgxcABITeIrbiaFxWMlv75762173;     dqgxcABITeIrbiaFxWMlv75762173 = dqgxcABITeIrbiaFxWMlv43342227;     dqgxcABITeIrbiaFxWMlv43342227 = dqgxcABITeIrbiaFxWMlv66125917;     dqgxcABITeIrbiaFxWMlv66125917 = dqgxcABITeIrbiaFxWMlv90898564;     dqgxcABITeIrbiaFxWMlv90898564 = dqgxcABITeIrbiaFxWMlv94311590;     dqgxcABITeIrbiaFxWMlv94311590 = dqgxcABITeIrbiaFxWMlv75187858;     dqgxcABITeIrbiaFxWMlv75187858 = dqgxcABITeIrbiaFxWMlv92900176;     dqgxcABITeIrbiaFxWMlv92900176 = dqgxcABITeIrbiaFxWMlv45098695;     dqgxcABITeIrbiaFxWMlv45098695 = dqgxcABITeIrbiaFxWMlv14581005;     dqgxcABITeIrbiaFxWMlv14581005 = dqgxcABITeIrbiaFxWMlv63036635;     dqgxcABITeIrbiaFxWMlv63036635 = dqgxcABITeIrbiaFxWMlv25134722;     dqgxcABITeIrbiaFxWMlv25134722 = dqgxcABITeIrbiaFxWMlv92096685;     dqgxcABITeIrbiaFxWMlv92096685 = dqgxcABITeIrbiaFxWMlv30061158;     dqgxcABITeIrbiaFxWMlv30061158 = dqgxcABITeIrbiaFxWMlv40605251;     dqgxcABITeIrbiaFxWMlv40605251 = dqgxcABITeIrbiaFxWMlv96277892;     dqgxcABITeIrbiaFxWMlv96277892 = dqgxcABITeIrbiaFxWMlv26988430;     dqgxcABITeIrbiaFxWMlv26988430 = dqgxcABITeIrbiaFxWMlv30137104;     dqgxcABITeIrbiaFxWMlv30137104 = dqgxcABITeIrbiaFxWMlv30356264;     dqgxcABITeIrbiaFxWMlv30356264 = dqgxcABITeIrbiaFxWMlv29582242;     dqgxcABITeIrbiaFxWMlv29582242 = dqgxcABITeIrbiaFxWMlv72611544;     dqgxcABITeIrbiaFxWMlv72611544 = dqgxcABITeIrbiaFxWMlv88622094;     dqgxcABITeIrbiaFxWMlv88622094 = dqgxcABITeIrbiaFxWMlv29188068;     dqgxcABITeIrbiaFxWMlv29188068 = dqgxcABITeIrbiaFxWMlv22253482;     dqgxcABITeIrbiaFxWMlv22253482 = dqgxcABITeIrbiaFxWMlv63510722;     dqgxcABITeIrbiaFxWMlv63510722 = dqgxcABITeIrbiaFxWMlv87958692;     dqgxcABITeIrbiaFxWMlv87958692 = dqgxcABITeIrbiaFxWMlv83813759;     dqgxcABITeIrbiaFxWMlv83813759 = dqgxcABITeIrbiaFxWMlv64821901;     dqgxcABITeIrbiaFxWMlv64821901 = dqgxcABITeIrbiaFxWMlv3558187;     dqgxcABITeIrbiaFxWMlv3558187 = dqgxcABITeIrbiaFxWMlv28833020;     dqgxcABITeIrbiaFxWMlv28833020 = dqgxcABITeIrbiaFxWMlv25004968;     dqgxcABITeIrbiaFxWMlv25004968 = dqgxcABITeIrbiaFxWMlv67274566;     dqgxcABITeIrbiaFxWMlv67274566 = dqgxcABITeIrbiaFxWMlv65367966;     dqgxcABITeIrbiaFxWMlv65367966 = dqgxcABITeIrbiaFxWMlv44658121;     dqgxcABITeIrbiaFxWMlv44658121 = dqgxcABITeIrbiaFxWMlv28963393;     dqgxcABITeIrbiaFxWMlv28963393 = dqgxcABITeIrbiaFxWMlv91555549;     dqgxcABITeIrbiaFxWMlv91555549 = dqgxcABITeIrbiaFxWMlv44189631;     dqgxcABITeIrbiaFxWMlv44189631 = dqgxcABITeIrbiaFxWMlv81672859;     dqgxcABITeIrbiaFxWMlv81672859 = dqgxcABITeIrbiaFxWMlv47775209;     dqgxcABITeIrbiaFxWMlv47775209 = dqgxcABITeIrbiaFxWMlv89818840;     dqgxcABITeIrbiaFxWMlv89818840 = dqgxcABITeIrbiaFxWMlv59636469;     dqgxcABITeIrbiaFxWMlv59636469 = dqgxcABITeIrbiaFxWMlv70350973;     dqgxcABITeIrbiaFxWMlv70350973 = dqgxcABITeIrbiaFxWMlv16810693;     dqgxcABITeIrbiaFxWMlv16810693 = dqgxcABITeIrbiaFxWMlv77368215;     dqgxcABITeIrbiaFxWMlv77368215 = dqgxcABITeIrbiaFxWMlv75620245;     dqgxcABITeIrbiaFxWMlv75620245 = dqgxcABITeIrbiaFxWMlv64707717;     dqgxcABITeIrbiaFxWMlv64707717 = dqgxcABITeIrbiaFxWMlv75244431;     dqgxcABITeIrbiaFxWMlv75244431 = dqgxcABITeIrbiaFxWMlv58112412;     dqgxcABITeIrbiaFxWMlv58112412 = dqgxcABITeIrbiaFxWMlv87150241;     dqgxcABITeIrbiaFxWMlv87150241 = dqgxcABITeIrbiaFxWMlv2506257;     dqgxcABITeIrbiaFxWMlv2506257 = dqgxcABITeIrbiaFxWMlv11743038;     dqgxcABITeIrbiaFxWMlv11743038 = dqgxcABITeIrbiaFxWMlv98960837;     dqgxcABITeIrbiaFxWMlv98960837 = dqgxcABITeIrbiaFxWMlv26655941;     dqgxcABITeIrbiaFxWMlv26655941 = dqgxcABITeIrbiaFxWMlv24136746;     dqgxcABITeIrbiaFxWMlv24136746 = dqgxcABITeIrbiaFxWMlv75034327;     dqgxcABITeIrbiaFxWMlv75034327 = dqgxcABITeIrbiaFxWMlv4385419;     dqgxcABITeIrbiaFxWMlv4385419 = dqgxcABITeIrbiaFxWMlv46574105;     dqgxcABITeIrbiaFxWMlv46574105 = dqgxcABITeIrbiaFxWMlv21088745;     dqgxcABITeIrbiaFxWMlv21088745 = dqgxcABITeIrbiaFxWMlv2615195;     dqgxcABITeIrbiaFxWMlv2615195 = dqgxcABITeIrbiaFxWMlv2939872;     dqgxcABITeIrbiaFxWMlv2939872 = dqgxcABITeIrbiaFxWMlv10497831;     dqgxcABITeIrbiaFxWMlv10497831 = dqgxcABITeIrbiaFxWMlv10365958;     dqgxcABITeIrbiaFxWMlv10365958 = dqgxcABITeIrbiaFxWMlv89341989;     dqgxcABITeIrbiaFxWMlv89341989 = dqgxcABITeIrbiaFxWMlv16265675;     dqgxcABITeIrbiaFxWMlv16265675 = dqgxcABITeIrbiaFxWMlv89576037;     dqgxcABITeIrbiaFxWMlv89576037 = dqgxcABITeIrbiaFxWMlv95762068;     dqgxcABITeIrbiaFxWMlv95762068 = dqgxcABITeIrbiaFxWMlv59766755;     dqgxcABITeIrbiaFxWMlv59766755 = dqgxcABITeIrbiaFxWMlv47438564;     dqgxcABITeIrbiaFxWMlv47438564 = dqgxcABITeIrbiaFxWMlv1097765;     dqgxcABITeIrbiaFxWMlv1097765 = dqgxcABITeIrbiaFxWMlv49049702;     dqgxcABITeIrbiaFxWMlv49049702 = dqgxcABITeIrbiaFxWMlv52088262;     dqgxcABITeIrbiaFxWMlv52088262 = dqgxcABITeIrbiaFxWMlv45315570;     dqgxcABITeIrbiaFxWMlv45315570 = dqgxcABITeIrbiaFxWMlv82361895;     dqgxcABITeIrbiaFxWMlv82361895 = dqgxcABITeIrbiaFxWMlv40537424;     dqgxcABITeIrbiaFxWMlv40537424 = dqgxcABITeIrbiaFxWMlv69945773;     dqgxcABITeIrbiaFxWMlv69945773 = dqgxcABITeIrbiaFxWMlv2260572;     dqgxcABITeIrbiaFxWMlv2260572 = dqgxcABITeIrbiaFxWMlv71811402;     dqgxcABITeIrbiaFxWMlv71811402 = dqgxcABITeIrbiaFxWMlv51819853;     dqgxcABITeIrbiaFxWMlv51819853 = dqgxcABITeIrbiaFxWMlv46633237;     dqgxcABITeIrbiaFxWMlv46633237 = dqgxcABITeIrbiaFxWMlv98803005;     dqgxcABITeIrbiaFxWMlv98803005 = dqgxcABITeIrbiaFxWMlv12714261;     dqgxcABITeIrbiaFxWMlv12714261 = dqgxcABITeIrbiaFxWMlv25701348;     dqgxcABITeIrbiaFxWMlv25701348 = dqgxcABITeIrbiaFxWMlv77671659;     dqgxcABITeIrbiaFxWMlv77671659 = dqgxcABITeIrbiaFxWMlv1051930;     dqgxcABITeIrbiaFxWMlv1051930 = dqgxcABITeIrbiaFxWMlv17089983;     dqgxcABITeIrbiaFxWMlv17089983 = dqgxcABITeIrbiaFxWMlv26044130;     dqgxcABITeIrbiaFxWMlv26044130 = dqgxcABITeIrbiaFxWMlv40618625;     dqgxcABITeIrbiaFxWMlv40618625 = dqgxcABITeIrbiaFxWMlv41231220;     dqgxcABITeIrbiaFxWMlv41231220 = dqgxcABITeIrbiaFxWMlv91949220;}
// Junk Finished
