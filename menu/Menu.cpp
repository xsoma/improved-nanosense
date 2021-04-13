#include "Menu.h"
#include "MenuControls.h"
#include "Dropdown.h"


#include "../vsonyp0wer/vsonyp0wer.h"
#include "../vsonyp0wer/vsonyp0wer_impl_dx9.h"
#include "../vsonyp0wer/vsonyp0wer_impl_win32.h"
#include "../vsonyp0wer/vsonyp0wer_internal.h"
#include <d3d9.h>
#include <string>
#include "../globals/HanaSong.h"
#include "../Options.hpp"
#include <functional>
#include <experimental/filesystem> 
#include "..//helpers/Config.hpp"
#include "..//Install.hpp"
#include "..//Structs.hpp"
#include "..//features/Miscellaneous.hpp"
#include "..//features/KitParser.hpp"
#include "..//features/Skinchanger.hpp"
#include "..//BASS/bass.h"
#include "..//BASS/API.h"
#include "..//Install.hpp"
#include "..//Handlers.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

static int tab = 0;
static int weapontab = 0;


bool input_shouldListen;
ButtonCode_t* input_receivedKeyval;

extern bool unload;

Menu _menu;

void Menu::ColorPicker(const char* name, float* color, bool alpha) {

	vsonyp0werWindow* window = vsonyp0wer::GetCurrentWindow();
	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();

	auto alphaSliderFlag = alpha ? vsonyp0werColorEditFlags_AlphaBar : vsonyp0werColorEditFlags_NoAlpha;

	vsonyp0wer::SameLine(219.f);
	vsonyp0wer::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | vsonyp0werColorEditFlags_NoInputs | vsonyp0werColorEditFlags_NoTooltip);
}
namespace vsonyp0wer
{

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	static bool ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
	{
		return vsonyp0wer::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
			{
				*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
				return true;
			}, &lambda, items_count, height_in_items);
	}

	bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
	{
		vsonyp0werWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
		char Buf[64];
		_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

		vsonyp0werContext& g = *Gvsonyp0wer;
		const vsonyp0werStyle& style = g.Style;
		const vsonyp0werID id = window->GetID(unique_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
		ItemSize(check_bb, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);

		const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			* v = !(*v);

		if (label_size.x > 0.0f)
			RenderText(check_bb.GetTL(), Buf);

		return pressed;
	}

	void KeyBindButton(ButtonCode_t* key)
	{
		bool clicked = false;

		std::string text = g_InputSystem->ButtonCodeToString(*key);
		std::string unique_id = std::to_string((int)key);

		if (*key <= BUTTON_CODE_NONE)
			text = " ";

		if (input_shouldListen && input_receivedKeyval == key) {
			clicked = true;
			text = "   ";
		}
		text += "]";

		vsonyp0wer::SameLine(213.f);
		vsonyp0wer::LabelClick("   [", "   [", text.c_str(), &clicked, unique_id.c_str());

		if (clicked)
		{
			input_shouldListen = true;
			input_receivedKeyval = key;
			g_InputSystem->EnableInput(true);
		}

		if (*key == KEY_DELETE)
		{
			*key = BUTTON_CODE_NONE;
		}

		if (vsonyp0wer::IsItemHovered())
			vsonyp0wer::SetTooltip("Bind the \"del\" key to remove current bind.");
	}
}

void Menu::Render() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();

	style->WindowPadding = ImVec2(6, 6);

	vsonyp0wer::PushFont(globals::menuFont);

	vsonyp0wer::SetNextWindowSize(ImVec2(660.f, 640.f));
	vsonyp0wer::BeginMenuBackground("ChainMail", &Menu::Get().isOpen, vsonyp0werWindowFlags_NoCollapse | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoScrollbar | vsonyp0werWindowFlags_NoScrollWithMouse | vsonyp0werWindowFlags_NoTitleBar); {

		vsonyp0wer::BeginChild("Complete Border", ImVec2(648.f, 628.f), false); {

			vsonyp0wer::Image(globals::menuBg, ImVec2(648.f, 628.f));

		} vsonyp0wer::EndChild();

		vsonyp0wer::SameLine(6.f);

		style->Colors[vsonyp0werCol_ChildBg] = ImColor(0, 0, 0, 0);

		vsonyp0wer::BeginChild("Menu Contents", ImVec2(648.f, 628.f), false); {

			vsonyp0wer::ColorBar("unicorn", ImVec2(648.f, 2.f));

			style->ItemSpacing = ImVec2(0.f, -1.f);

			vsonyp0wer::BeginTabs("Tabs", ImVec2(0.0000001f, 622.f), false); {

				style->ItemSpacing = ImVec2(0.f, 0.f);

				style->ButtonTextAlign = ImVec2(0.5f, 0.47f);

				vsonyp0wer::PopFont();
				vsonyp0wer::PushFont(globals::tabFont);

				switch (tab) {

				case 0:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::SelectedTab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 1:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::SelectedTab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 2:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::SelectedTab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 3:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::SelectedTab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 4:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::SelectedTab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 5:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::SelectedTab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 6:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::SelectedTab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 7:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::SelectedTab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::Tab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				case 8:
					vsonyp0wer::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (vsonyp0wer::Tab("A", ImVec2(75.f, 66.f))) tab = 0;
					if (vsonyp0wer::Tab("G", ImVec2(75.f, 66.f))) tab = 1;
					if (vsonyp0wer::Tab("B", ImVec2(75.f, 66.f))) tab = 2;
					if (vsonyp0wer::Tab("C", ImVec2(75.f, 66.f))) tab = 3;
					if (vsonyp0wer::Tab("D", ImVec2(75.f, 66.f))) tab = 4;
					if (vsonyp0wer::Tab("E", ImVec2(75.f, 66.f))) tab = 5;
					if (vsonyp0wer::Tab("F", ImVec2(75.f, 66.f))) tab = 6;
					if (vsonyp0wer::Tab("H", ImVec2(75.f, 66.f))) tab = 7;
					if (vsonyp0wer::SelectedTab("I", ImVec2(75.f, 66.f))) tab = 8;

					vsonyp0wer::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 18.f));
					break;
				}

				vsonyp0wer::PopFont();
				vsonyp0wer::PushFont(globals::menuFont);

				style->ButtonTextAlign = ImVec2(0.5f, 0.5f);

			} vsonyp0wer::EndTabs();

			vsonyp0wer::SameLine(75.f);

			vsonyp0wer::BeginChild("Tab Contents", ImVec2(572.f, 622.f), false); {

				style->Colors[vsonyp0werCol_Border] = ImColor(0, 0, 0, 0);

				switch (tab) {

				case 0:
					Aimbot();
					break;
				case 1:
					Antiaim();
					break;
				case 2:
					lgtpew();
					break;
				case 3:
					Visuals();
					break;
				case 4:
					Misc();
					break;
				case 5:
					Skins();
					break;
				case 6:
					Players();
					break;
				case 7:
					Config();
					break;
				case 8:
					LuaTab();
				}

				style->Colors[vsonyp0werCol_Border] = ImColor(10, 10, 10, 255);

			} vsonyp0wer::EndChild();

			style->ItemSpacing = ImVec2(4.f, 4.f);
			style->Colors[vsonyp0werCol_ChildBg] = ImColor(17, 17, 17, 255);

		} vsonyp0wer::EndChild();
		vsonyp0wer::PopFont();
		
	} vsonyp0wer::End();
}

void Menu::Shutdown() {

	vsonyp0wer_ImplDX9_Shutdown();
	vsonyp0wer_ImplWin32_Shutdown();
}

void Menu::Aimbot() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Aimbot", 586.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Enable", XSystemCFG.angrpwenabled);
			InsertCombo("Target selection", XSystemCFG.angrpwaimtype, hnlytypes);
			InsertCombo("Target hitbox (priority)", XSystemCFG.iangrpwhitbox, rgbthitboxes);
			InsertCheckbox("Prioritize selected hitbox", XSystemCFG.bangrpwprioritize);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Multi-point");
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::BeginChild("#MULTIPOINTS", ImVec2(158, 80), true, vsonyp0werWindowFlags_AlwaysVerticalScrollbar);
			for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
			{
				vsonyp0wer::Selectable(opt_MultiHitboxes[i], &XSystemCFG.angrpwmultiHitboxesAll[i]);
			}
			vsonyp0wer::EndChild();

			InsertCheckbox("Dynamic multi-point", XSystemCFG.bangrpwmultipoint);
			InsertSlider("Hitbox scale", XSystemCFG.angrpwpointscale_amount, 0.1f, 1.0f, "%0.1fx%");

			InsertCheckbox("Automatic fire", XSystemCFG.angrpwautoshoot);
			InsertCheckbox("Silent aim", XSystemCFG.angrpwsilent);
			InsertSlider("Minimum hit chance", XSystemCFG.angrpwhitchance_amount, 0.f, 100.f, "%1.f%%");
			InsertSlider("Minimum damage", XSystemCFG.angrpwmindmg_amount, 0.f, 100.f, "%1.fhp%");
			InsertSlider("Field of view", XSystemCFG.angrpwfov, 0.f, 180.0f, "%1.f%");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);



		} InsertEndGroupBoxLeft("Aimbot Cover", "Aimbot");
	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("Settings", 586.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);


			InsertCheckbox("Use resolver", XSystemCFG.hvh_resolver);
			InsertCheckbox("Remove recoil", XSystemCFG.angrpwnorecoil);
			InsertCombo("Accuracy boost", XSystemCFG.angrpwaccboost, AccuracyBoost);
			InsertCombo("Accuracy boost options", XSystemCFG.angrpwfacboost, ForceACBoost);
			InsertCheckbox("Force body aim", XSystemCFG.angrpwautobaim);
			InsertCheckbox("Resolver assist", XSystemCFG.resolver_eyelby_always);
			InsertCombo("Resolver assist mode", XSystemCFG.angrpwresastype, resastypes);
			InsertCheckbox("Override resolver", XSystemCFG.hvh_resolver_override);
			InsertCheckbox("Reverse resolver", XSystemCFG.hvh_resolver_custom);
			InsertCheckbox("Resolver flip", XSystemCFG.resolver_flip_after_x_shots);
			InsertSlider("Shots to flip after:", XSystemCFG.resolver_flip_after_missed_shots, 0.f, 10.f, "%1.f%");
			InsertCheckbox("Automatic Resolver Flip", XSystemCFG.resolver_autoflip);
			InsertCheckbox("Override R8", XSystemCFG.angrpwautocockrevolver);
			InsertCheckbox("Automatic scope", XSystemCFG.angrpwautoscope);
			InsertCheckbox("Automatic crouch", XSystemCFG.angrpwautocrouch);


			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxRight("Settings Cover", "Other");
	}
}

void Menu::Antiaim() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Anti-aimbot angles", 586.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCombo("Pitch", XSystemCFG.hvh_antiaim_x, aaxaxis);
			InsertCombo("Yaw", XSystemCFG.hvh_antiaim_y, aayaxis);

			InsertSlider("Desync angle left", XSystemCFG.hvh_antiaim_y_desync_start_left, -180.f, 180.f, "%0.f%");
			InsertSlider("Desync angle right", XSystemCFG.hvh_antiaim_y_desync_start_right, -180.f, 180.f, "%0.f%");
			InsertSlider("Desync angle back", XSystemCFG.hvh_antiaim_y_desync_start_back, -180.f, 180.f, "%0.f%");
			InsertSlider("Desync amount", XSystemCFG.hvh_antiaim_y_desync, -180.f, 180.f, "%0.f%");

			InsertSlider("Custom yaw left", XSystemCFG.hvh_antiaim_y_custom_left, -180.f, 180.f, "%0.f%");
			InsertSlider("Custom yaw right", XSystemCFG.hvh_antiaim_y_custom_right, -180.f, 180.f, "%0.f%");
			InsertSlider("Custom Yaw Back", XSystemCFG.hvh_antiaim_y_custom_back, -180.f, 180.f, "%0.f%");

			InsertCombo("Yaw move", XSystemCFG.hvh_antiaim_y_move, aayaxismove);
			InsertSlider("Do when velocity is over", XSystemCFG.hvh_antiaim_y_move_trigger_speed, 0.1f, 130.f, "%0.1f%");
			InsertSlider("Custom yaw move left", XSystemCFG.hvh_antiaim_y_custom_realmove_left, -180.f, 180.f, "%0.f%");
			InsertSlider("Custom yaw move right", XSystemCFG.hvh_antiaim_y_custom_realmove_right, -180.f, 180.f, "%0.f%");
			InsertSlider("Custom yaw move back", XSystemCFG.hvh_antiaim_y_custom_realmove_back, -180.f, 180.f, "%0.f%");


			//	InsertCheckbox("Static Desync", XSystemCFG.hnlyaa_staticdesync);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxLeft("Anti-aimbot angles Cover", "Anti-aimbot angles");

	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("Fake lag", 360.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Enable fakelag", XSystemCFG.misc_fakelag_enabled);
			InsertCheckbox("Enable adaptive fakelag", XSystemCFG.misc_fakelag_adaptive);
			InsertCombo("Activation type", XSystemCFG.misc_fakelag_activation_type, fakelagact);
			InsertSlider("Fakelag amount", XSystemCFG.misc_fakelag_value, 0.f, 14.f, "%0.ft"); // ovde
			InsertCombo("Quick stop", XSystemCFG.angrpwqstypes, hqstypes);
			InsertCheckbox("Anti-aim correction", XSystemCFG.resolver_eyelby);
			InsertCheckbox("Anti-aim correction override", XSystemCFG.resolver_eyelby_running);
			InsertCheckbox("Infinite crouch", XSystemCFG.misc_infinite_duck);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fakeduck key"); vsonyp0wer::KeyBindButton(&XSystemCFG.misc_fakeduck);
		//	InsertSlider("Fakeduck speed", XSystemCFG.misc_fakeduck_value, 0.f, 14.f, "%0.ft");
			InsertCheckbox("Slowwalk", XSystemCFG.misc_fakewalk);
			vsonyp0wer::KeyBindButton(&XSystemCFG.misc_fakewalk_bind);
			InsertSlider("Slowwalk speed", XSystemCFG.misc_fakewalk_speed, 0.f, 130.f, "%0.f%");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxRight("Fake lag Cover", "Fake lag");

		InsertSpacer("Fake lag - Other Spacer");

		InsertGroupBoxRight("Manual & Exploit", 205.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Tickbase manipulation", XSystemCFG.tickbase_manipulation);
			vsonyp0wer::KeyBindButton(&XSystemCFG.tickbase_manipulation_key);
			InsertCheckbox("Tickfreeze exploit", XSystemCFG.tickbase_freeze);
			vsonyp0wer::KeyBindButton(&XSystemCFG.tickbase_freeze_key);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Left AA Bind"); vsonyp0wer::KeyBindButton(&XSystemCFG.hvh_aa_left_bind);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Right AA Bind"); vsonyp0wer::KeyBindButton(&XSystemCFG.hvh_aa_right_bind);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Back AA Bind"); vsonyp0wer::KeyBindButton(&XSystemCFG.hvh_aa_back_bind);
			InsertCheckbox("On Screen Indicator", XSystemCFG.visuals_manual_aa);
			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxRight("Manual & Exploit Cover", "Manual & Exploit");
	}
}

void Menu::lgtpew() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	InsertGroupBoxTop("Weapon Selection", ImVec2(535.f, 61.f)); {

		vsonyp0wer::Columns(6, nullptr, false);
		vsonyp0wer::PopFont();
		vsonyp0wer::PushFont(globals::weaponzFont);
		style->ButtonTextAlign = ImVec2(0.5f, 0.75f);
		{
			vsonyp0wer::TabButton("G", ImVec2(88, 45));

			vsonyp0wer::NextColumn();
			{
				vsonyp0wer::TabButton("P", ImVec2(88, 45));
			}
			vsonyp0wer::NextColumn();
			{
				vsonyp0wer::TabButton("W", ImVec2(88, 45));
			}
			vsonyp0wer::NextColumn();
			{
				vsonyp0wer::TabButton("d", ImVec2(88, 45));
			}
			vsonyp0wer::NextColumn();
			{
				vsonyp0wer::TabButton("f", ImVec2(88, 45));
			}
			vsonyp0wer::NextColumn();
			{
				vsonyp0wer::TabButton("a", ImVec2(88, 45));
			}
		}


	} InsertEndGroupBoxTop("Weapon Selection Cover", "Weapon Selection", ImVec2(536.f, 11.f));

	InsertSpacer("Weapon Selection - Main Group boxes Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Aimbot", 507.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Enable", XSystemCFG.lgtpew_enabled);
			InsertCheckbox("Silent mode", XSystemCFG.angrpwsilent);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Allows you to aim without your screen locking on");
			InsertCheckbox("On Keybind", XSystemCFG.lgtpew_on_press);
			vsonyp0wer::KeyBindButton(&XSystemCFG.lgtpew_aim_keybind);
			InsertCheckbox("Draw fov", XSystemCFG.lgtpew_drawfov);
			InsertSlider("Field of view", XSystemCFG.lgtpew_fov, 0.1f, 25.f, "%0.1f%");
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("How strong you want it to lock on");
			InsertSlider("Smooth factor", XSystemCFG.lgtpew_smooth_factor, 1.f, 20.f, "%1.f%");
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("How smooth does it flick?");
			InsertCheckbox("Auto pistol", XSystemCFG.misc_auto_pistol);
			InsertCheckbox("Headshot only", XSystemCFG.lgtpew_hsonly);
			InsertCombo("Pre-aim spot", XSystemCFG.lgtpew_preaim, lgtpewpreaim);
			InsertCombo("After-aim spot", XSystemCFG.lgtpew_afteraim, lgtpewafteraim);
			InsertSlider("Bullets after aim", XSystemCFG.lgtpew_aftershots, 0.f, 10.f, "%3.fx%");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Aimbot Cover", "Aimbot");
	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("Other", 507.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Legit accuracy boost", XSystemCFG.legit_backtrack);
			InsertCombo("Accuracy boost type", XSystemCFG.angrpwaccboost, LegitACBoost);
			InsertCombo("Accuracy boost options", XSystemCFG.angrpwfacboost, LegitFACBoost);

			InsertCheckbox("Use recoil control system", XSystemCFG.lgtpew_rcs);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Stabilises recoil");
			InsertSlider("RCS X", XSystemCFG.lgtpew_rcsx, 0, 1, "%0.01f%");
			InsertSlider("RCS Y", XSystemCFG.lgtpew_rcsy, 0, 1, "%0.01f%");
			InsertCheckbox("Use RCS only with key", XSystemCFG.rcs_on_press);
			vsonyp0wer::KeyBindButton(&XSystemCFG.rcs_keybind);
			InsertCheckbox("Trigger", XSystemCFG.lgtpew_trigger);
			InsertCheckbox("Trigger on key only", XSystemCFG.trigger_on_press);
			vsonyp0wer::KeyBindButton(&XSystemCFG.trigger_keybind);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Visuals() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Player ESP", 371.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Enable Boxes", XSystemCFG.onlytextnocheckbox);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Visible T Color");
			InsertColorPicker("##colortvb", XSystemCFG.esp_player_bbox_color_t_visible, false);//
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Visible CT Color");
			InsertColorPicker("##colorctvb", XSystemCFG.esp_player_bbox_color_ct_visible, false);//
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Behind Wall T Color");
			InsertColorPicker("##colortbw", XSystemCFG.esp_player_bbox_color_t, false);//
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Behind Wall CT Color");
			InsertColorPicker("##colorctbw", XSystemCFG.esp_player_bbox_color_ct, false);//
			InsertCombo("Box Type", XSystemCFG.esp_player_boxtype, espType);
			InsertCombo("Box Bound Type", XSystemCFG.esp_player_boundstype, espBounds);

			//InsertCheckbox("Box Fill", XSystemCFG.onlytextboxfill);
			InsertSlider("Box Fill Amount", XSystemCFG.esp_fill_amount, 0.f, 100.f, "%1.f%%");
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fill Color T Visible");
			InsertColorPicker("##colortfv", XSystemCFG.esp_player_fill_color_t_visible, false);//
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fill Color CT Visible");
			InsertColorPicker("##colorctfv", XSystemCFG.esp_player_fill_color_ct_visible, false);//
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fill Color T Invisible");
			InsertColorPicker("##colortbfw", XSystemCFG.esp_player_fill_color_t, false); //
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fill Color CT Invisible");
			InsertColorPicker("##colorctbfw", XSystemCFG.esp_player_fill_color_ct, false);//

			InsertSlider("Aspect ratio changer", XSystemCFG.aspectratio, 0.f, 3.f, "%0.1f%");
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Change ratio"))
			{
				float ratio = (XSystemCFG.aspectratio);
				if (ratio > 0.001)
					g_CVar->FindVar("r_aspectratio")->SetValue(ratio);
				else
					g_CVar->FindVar("r_aspectratio")->SetValue((35 * 0.1f) / 2);

			}

			InsertCheckbox("Dormant", XSystemCFG.esp_farther);
			InsertCheckbox("Enemies only", XSystemCFG.esp_enemies_only);
			InsertCheckbox("Player name", XSystemCFG.esp_player_name);
			InsertCheckbox("Player health", XSystemCFG.esp_player_health);
			InsertCheckbox("Player weapons", XSystemCFG.esp_player_weapons);
			InsertCheckbox("Player ammo", XSystemCFG.esp_player_ammo);
			InsertCheckbox("Player snap-lines", XSystemCFG.esp_player_snaplines);
			InsertCheckbox("Lag compensated hitboxes", XSystemCFG.esp_lagcompensated_hitboxes);
			InsertCombo("Lagcomp hitboxes type", XSystemCFG.esp_lagcompensated_hitboxes_type, lagcomphitboxez);
			InsertCheckbox("Skeleton", XSystemCFG.esp_player_skelet);
			InsertCheckbox("Backtracked skeleton", XSystemCFG.esp_backtracked_player_skelet);
			InsertCheckbox("Antiaim lines", XSystemCFG.esp_player_anglelines);
		
			

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Player ESP Cover", "Player ESP");

		InsertSpacer("Player ESP - Colored models Spacer");

		InsertGroupBoxLeft("Colored models", 197.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);	

			InsertCheckbox("Enable Chams", XSystemCFG.esp_player_chams);
			InsertCombo("Chams Type", XSystemCFG.esp_player_chams_type, espChams);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Chams Color T Visible");
			InsertColorPicker("##colorCHAMST", XSystemCFG.esp_player_chams_color_t_visible, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Chams Color CT Visible");
			InsertColorPicker("##colorCHAMSCT", XSystemCFG.esp_player_chams_color_ct_visible, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Chams Color T Invisible");
			InsertColorPicker("##colorCHAMSTINVIS", XSystemCFG.esp_player_chams_color_t, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Chams Color CT Invisible");
			InsertColorPicker("##colorCHAMSCTINVIS", XSystemCFG.esp_player_chams_color_ct, false);

			InsertCheckbox("Glow Enable", XSystemCFG.glow_enabled);
			InsertCheckbox("Glow Players", XSystemCFG.glow_players);
			InsertCombo("Glow Type", XSystemCFG.glow_others_style, glowstyles);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Visible T Color");
			InsertColorPicker("##colorglowtinvisible", XSystemCFG.glow_player_color_t_visible, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Visible CT Color");
			InsertColorPicker("##colorglowctinvisible", XSystemCFG.glow_player_color_ct_visible, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Invisible T Color");
			InsertColorPicker("##colorglowtvisible", XSystemCFG.glow_player_color_t, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Invisible CT Color");
			InsertColorPicker("##colorglowctvisible", XSystemCFG.glow_player_color_ct, false);



			InsertCheckbox("Enable localplayer chams", XSystemCFG.esp_localplayer_chams);
			InsertCombo("Local chams material", XSystemCFG.esp_localplayer_chams_type, localChams);
			//InsertCheckbox("Localplayer chams xqz", XSystemCFG.esp_localplayer_chams_xyz);
			//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//vsonyp0wer::Text("Local chams visible color");
			//InsertColorPicker("##colorlocalvisible", XSystemCFG.esp_localplayer_chams_color_visible, false);
			//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//vsonyp0wer::Text("Local chams invisible color");
			//("##colorlocalinvisible", XSystemCFG.esp_localplayer_chams_color_invisible, false);

			InsertCheckbox("Enable localplayer VM chams", XSystemCFG.esp_localplayer_viewmodel_chams);
			//InsertColorPicker("##colorlocalvc", XSystemCFG.esp_localplayer_viewmodel_chams_color, false);
			InsertCombo("Localplayer VM material", XSystemCFG.esp_localplayer_viewmodel_materials, localChams);
			InsertCheckbox("Enable fakeangle chams", XSystemCFG.fake_chams);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("FakeAngle visible color");
			InsertColorPicker("##colorfakevisible", XSystemCFG.esp_localplayer_fakechams_color_visible, false);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//vsonyp0wer::Text("FakeAngle invisible color");
			//InsertColorPicker("##colorfakeinvisible", XSystemCFG.esp_localplayer_fakechams_color_invisible, false);

			//InsertCheckbox("Enable localplayer wireframe", XSystemCFG.esp_localplayer_viewmodel_chams_wireframe);
			
	
			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Colored models Cover", "Colored models");
	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("Other ESP", 289.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			InsertCheckbox("Indicators enable", XSystemCFG.indicators_enabled);
			InsertCheckbox("Grenade prediction", XSystemCFG.visuals_others_grenade_pred); 
			InsertCheckbox("Planted C4", XSystemCFG.esp_planted_c4); 
			InsertCheckbox("Thrown grenades", XSystemCFG.esp_grenades);
			InsertCombo("Grenade esp type", XSystemCFG.esp_grenades_type, grenadeesptype);
			InsertCheckbox("Disable post processing", XSystemCFG.removals_postprocessing);
			InsertSlider("Field of view", XSystemCFG.visuals_others_player_fov, 0.f, 60.f, "%0.f%");
			InsertSlider("VM field of view", XSystemCFG.visuals_others_player_fov_viewmodel, 0.f, 60.f, "%0.f%");
			InsertCheckbox("Enable nightmode", XSystemCFG.visuals_nightmode);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("NightMode Color");
			InsertColorPicker("##nightmodecolor", XSystemCFG.visuals_others_nightmode_color, false);
			InsertCombo("Nightmode type", XSystemCFG.visuals_nightmode_type, nightmodetype); 
			//InsertCombo("Sky type", XSystemCFG.visuals_others_sky, skytype);
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Sky color");
			InsertColorPicker("##skycolor", XSystemCFG.visuals_others_skybox_color, false);
			InsertCheckbox("Asus walls", XSystemCFG.visuals_asuswalls);
			InsertCheckbox("Asus wall props", XSystemCFG.visuals_asuswalls_staticprops);
			InsertCheckbox("Asus wall world", XSystemCFG.visuals_asuswalls_world);
			InsertSlider("Asus walls transparency", XSystemCFG.visuals_asuswalls_value, 0.f, 100.f, "%1.f%%");
			InsertCheckbox("Dropped weapons", XSystemCFG.droppedwepcolorz);
			InsertSlider("DW red", XSystemCFG.dropped_weapons_color[0], 0.f, 255.f, "%1.f%%");
			InsertSlider("DW green", XSystemCFG.dropped_weapons_color[1], 0.f, 255.f, "%1.f%%");
			InsertSlider("DW blue", XSystemCFG.dropped_weapons_color[2], 0.f, 255.f, "%1.f%%");
			InsertCombo("Dropped weapons type", XSystemCFG.esp_dropped_weapons, wepBox);




			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other ESP Cover", "Other ESP");

		InsertSpacer("Other ESP - Effects Spacer");

		InsertGroupBoxRight("Effects", 279.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);
			
			InsertCheckbox("Remove flashbang effects", XSystemCFG.removals_flash);
			InsertCheckbox("Remove recoil", XSystemCFG.removals_novisualrecoil);
			InsertCheckbox("Remove scope overlay", XSystemCFG.removals_scope);
			InsertCheckbox("Remove scope zoom", XSystemCFG.removals_zoom);
			InsertCheckbox("Remove smoke", XSystemCFG.removals_smoke);
			InsertCombo("Smoke removal type", XSystemCFG.removals_smoke_type, nosmoketype);
			InsertCheckbox("Hit marker", XSystemCFG.visuals_others_hitmarker);
			InsertCheckbox("Bullet impacts", XSystemCFG.visuals_others_bulletimpacts);
			InsertColorPicker("##bulletimpacts_color", XSystemCFG.visuals_others_bulletimpacts_color, false);

			InsertCheckbox("Enable viewmodel changer", XSystemCFG.change_viewmodel_offset);
			InsertSlider("Viewmodel X", XSystemCFG.viewmodel_offset_x, -20.f, 20.f, "%1.f%");
			InsertSlider("Viewmodel Y", XSystemCFG.viewmodel_offset_y, -20.f, 20.f, "%1.f%");
			InsertSlider("Viewmodel Z", XSystemCFG.viewmodel_offset_z, -20.f, 20.f, "%1.f%");

			InsertCheckbox("Enable custom crosshair", XSystemCFG.visuals_draw_xhair);
			InsertColorPicker("##crosshaircolor", XSystemCFG.xhair_color, false);
			InsertSlider("X Length", XSystemCFG.visuals_xhair_x, 1.f, 15.f, "%1.f%");
			InsertSlider("Y Length", XSystemCFG.visuals_xhair_y, 1.f, 15.f, "%1.f%");


			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Effects Cover", "Effects");
	}
}
void Menu::Misc() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Miscellaneous", 586.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
			style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

			//style->ButtonTextAlign = ImVec2(0.5f, 0.5f);

			InsertCheckbox("Disable anti-untrusted", XSystemCFG.hvh_disable_antiut);
			InsertCheckbox("Anti-OBS", XSystemCFG.anti_obs);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Hide your client while streaming.");
			InsertCheckbox("Reveal competitive ranks", XSystemCFG.misc_revealAllRanks);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("owo i wonder what ranks the enemies are");
			InsertCheckbox("Auto-accept matchmaking", XSystemCFG.misc_autoaccept);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Accepts mm for you if youre afk");
			InsertCheckbox("Event logs", XSystemCFG.misc_logevents);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Shows damage dealt to players top left");
			InsertCheckbox("Bunny hop", XSystemCFG.misc_bhop);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Spams jump so you dont have to");
			InsertCheckbox("Auto strafe", XSystemCFG.misc_autostrafe);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Lets u bhop easier");

			InsertCheckbox("RTX Mode", XSystemCFG.minecraft_graphics)
				if (XSystemCFG.minecraft_graphics)
				{
					g_CVar->FindVar("mat_showlowresimage")->SetValue(1);
				}
				else
				{
					g_CVar->FindVar("mat_showlowresimage")->SetValue(0);
				}

			InsertCheckbox("Namestealer (NN First)", XSystemCFG.misc_namestealer);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("be a toxic faggot");
			InsertCheckbox("Spectator list", XSystemCFG.misc_spectatorlist);
			InsertCheckbox("Radar show all", XSystemCFG.mp_radar_showall);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Shows all players on radar");
			InsertCheckbox("Override fog", XSystemCFG.fog_override);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Removes Fog");
			InsertCheckbox("Dev information", XSystemCFG.misc_devinfo);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Currently Disabled!");
			InsertCheckbox("Anti vote kick", XSystemCFG.misc_antikick);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Currently Disabled!");

			InsertCheckbox("Enable physics timescale", XSystemCFG.cl_phys_timescale);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Allows you to change speed of ragdolls");
			InsertSlider("Physics Timescale Amount", XSystemCFG.cl_phys_timescale_value, 0.1f, 5.0f, "%0.1f%");



			
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Anti-cheat detector##ACD"))
			{
				acinfo output;
				Miscellaneous::Get().DetectAC(output);
				auto print_bool = [](bool in)
				{
					if (in)
						g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "True\n");
					else
						g_CVar->ConsoleColorPrintf(Color(0, 255, 0, 255), "False\n");
				};
				g_EngineClient->ExecuteClientCmd("toggleconsole");

				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Core       : "); print_bool(output.smac_core);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Aimbot     : "); print_bool(output.smac_aimbot);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Triggerbot : "); print_bool(output.smac_autotrigger);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Client     : "); print_bool(output.smac_client);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Commands   : "); print_bool(output.smac_commands);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC ConVars    : "); print_bool(output.smac_cvars);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Eyeangle   : "); print_bool(output.smac_eyetest);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Speedhack  : "); print_bool(output.smac_speedhack);
				g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "SMAC Spinbot    : "); print_bool(output.smac_spinhack);
			}

			//skytype //visuals_others_sky
			vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Custom Cky Changer");
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f);
			vsonyp0wer::ListBox("", &XSystemCFG.visuals_others_sky, skytype,15,15);

			//vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//if (vsonyp0wer::Button("Apply"))

			//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//vsonyp0wer::Text("Sky color");
			//InsertColorPicker("##skycolor", XSystemCFG.visuals_others_skybox_color, false);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Miscellaneous Cover", "Miscellaneous");

	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("Thirdperson", 156.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);


			InsertCheckbox("Third person", XSystemCFG.misc_thirdperson);
			vsonyp0wer::KeyBindButton(&XSystemCFG.misc_thirdperson_bind);
			InsertCombo("Third person mode", XSystemCFG.hvh_show_real_angles, thirdpersonmode);
			if (vsonyp0wer::IsItemHovered())
				vsonyp0wer::SetTooltip("Type of thirdperson");
			InsertSlider("Thirdperson distance", XSystemCFG.misc_thirdperson_distance, 20, 200, "%1.f%");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Thirdperson Cover", "Thirdperson");

		InsertSpacer("Settings - Other Spacer");

		InsertGroupBoxRight("Other", 412.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);
			style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
			style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

			std::string zstations[] = {
			"http://air.radiorecord.ru:805/synth_320",
			"https://shoutcast1.hardcoreradio.n1:9000/hcr-mp3",
			"http://air.radiorecord.ru:805/tm_320",
			"http://air.radiorecord.ru:805/mix_320",
			"http://air2.radiorecord.ru:805/rr_320",
			"http://air.radiorecord.ru:805/dub_320",
			"http://air.radiorecord.ru:805/teo_320",
			"http://air.radiorecord.ru:805/dc_320",
			"http://air.radiorecord.ru:805/goa_320",
			"http://air.radiorecord.ru:805/hbass_320",
			"http://air.radiorecord.ru:805/1980_320",
			"http://air.radiorecord.ru:805/rock_320",
			"http://air.radiorecord.ru:805/cadillac_320",
			"http://air.radiorecord.ru:805/eurodance_320",
			"http://antenaradio.org:4550/",
			"http://streaming.tdiradio.com/tdiradiobezreklama-mp3"
			};



			InsertCheckbox("Radio", XSystemCFG.misc_radio);
			vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Radio Station");
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f);
			if (vsonyp0wer::ListBox("", &XSystemCFG.misc_radiostation, radio_name, IM_ARRAYSIZE(radio_name),17))
			{
				BASS_ChannelStop(stream);
				stream = NULL;
				stream = BASS_StreamCreateURL(zstations[XSystemCFG.misc_radiostation].c_str(), 0, 0, NULL, 0);
			}
			if (XSystemCFG.misc_radio)
			{
				static bool radioInit = false;

				if (!radioInit) {
					BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
					stream = BASS_StreamCreateURL(zstations[XSystemCFG.misc_radiostation].c_str(), 0, 0, NULL, 0);
					radioInit = true;
				}
				static float vol = 0.1f;
				BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol / 100.0f);
				BASS_ChannelPlay(stream, false);
				InsertSlider("Radio volume", vol, 0.f, 100.0f, "%0.f%");
			}
			else if (!XSystemCFG.misc_radio)
			{
				BASS_ChannelStop(stream);
				stream = NULL;
			}


			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Skins() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

		InsertGroupBoxCentered("Skin Configuration", 586.f); {

			style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
			style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Checkbox("Enabled##Skinchanger", &XSystemCFG.skinchanger_enabled))
			Skinchanger::Get().bForceFullUpdate = true;
			std::vector<EconomyItem_t>& entries = Skinchanger::Get().GetItems();
			// If the user deleted the only config let's add one
			if (entries.size() == 0)
				entries.push_back(EconomyItem_t());
			static int selected_id = 0;
			vsonyp0wer::Columns(2, nullptr, false);
			// Config selection
			{
				char element_name[64];
				vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				vsonyp0wer::ListBox("##skinchangerconfigs", &selected_id, [&element_name, &entries](int idx)
					{
						sprintf_s(element_name, "%s (%s)", entries.at(idx).name, k_weapon_names.at(entries.at(idx).definition_vector_index).name);
						return element_name;
					}, entries.size(), 26);
				vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::Button("Add item"))
				{
					entries.push_back(EconomyItem_t());
					selected_id = entries.size() - 1;
				}
				vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::Button("Remove item"))
					entries.erase(entries.begin() + selected_id);

				vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::Button("Force update##Skinchanger"))
					Skinchanger::Get().bForceFullUpdate = true;

				vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::Button("Save##Skinchanger"))
					Skinchanger::Get().SaveSkins();

				vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::Button("Load##Skinchanger"))
					Skinchanger::Get().LoadSkins();

			}
			vsonyp0wer::NextColumn();
			selected_id = selected_id < int(entries.size()) ? selected_id : entries.size() - 1;
			EconomyItem_t& selected_entry = entries[selected_id];
			{
				// Name
				vsonyp0wer::InputText("Name", selected_entry.name, 32);
				vsonyp0wer::Dummy(ImVec2(1, 4));
				// Item to change skins for
				vsonyp0wer::CustomSpacing(6.f);
				vsonyp0wer::Combo("Item", &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_weapon_names[idx].name;
						return true;
					}, nullptr, k_weapon_names.size(), 5);
				vsonyp0wer::Dummy(ImVec2(1, 3));
				// Enabled
				vsonyp0wer::Checkbox("Enabled", &selected_entry.enabled);
				vsonyp0wer::Dummy(ImVec2(1, 3));
				// Pattern Seed & Stattrak
				vsonyp0wer::Spacing();
				vsonyp0wer::Text("Seed                Stattrak");
				vsonyp0wer::InputInt2("", &selected_entry.seed);
				vsonyp0wer::Dummy(ImVec2(1, 4));
				vsonyp0wer::CustomSpacing(9.f);
				// Wear Float
				vsonyp0wer::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5); // was .10
				vsonyp0wer::Dummy(ImVec2(1, 4));
				vsonyp0wer::CustomSpacing(13.f);
				// Paint kit
				if (selected_entry.definition_index != GLOVE_T_SIDE)
				{
					vsonyp0wer::ListBox("Paintkit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) // PAINTKIT LISTBOX
						{
							*out_text = k_skins[idx].name.c_str();
							return true;
						}, nullptr, k_skins.size(), 16);
				}
				else
				{
					vsonyp0wer::ListBox("Paintkit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) // PAINTKIT LISTBOX
						{
							*out_text = k_gloves[idx].name.c_str();
							return true;
						}, nullptr, k_gloves.size(), 16);
				}
				vsonyp0wer::Dummy(ImVec2(1, 4));
				vsonyp0wer::CustomSpacing(6.f);
				// Quality
				vsonyp0wer::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_quality_names[idx].name;
						return true;
					}, nullptr, k_quality_names.size(), 5);
				vsonyp0wer::Dummy(ImVec2(1, 4));
				vsonyp0wer::CustomSpacing(6.f);
				// Yes we do it twice to decide knifes
				selected_entry.UpdateValues();
				// Item defindex override
				if (selected_entry.definition_index == WEAPON_KNIFE)
				{
					vsonyp0wer::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = k_knife_names.at(idx).name;
							return true;
						}, nullptr, k_knife_names.size(), 5);
				}
				else if (selected_entry.definition_index == GLOVE_T_SIDE)
				{
					vsonyp0wer::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = k_glove_names.at(idx).name;
							return true;
						}, nullptr, k_glove_names.size(), 5);
				}
				else
				{
					// We don't want to override weapons other than knives or gloves
					static auto unused_value = 0;
					selected_entry.definition_override_vector_index = 0;
					vsonyp0wer::Combo("Unavailable", &unused_value, "Only knives or gloves!\0");
				}
				vsonyp0wer::Dummy(ImVec2(1, 4));
				vsonyp0wer::CustomSpacing(6.f);
				selected_entry.UpdateValues();
				// Custom Name tag
				vsonyp0wer::InputText("Nametag", selected_entry.custom_name, 32);
				vsonyp0wer::Dummy(ImVec2(1, 4));
			}
			vsonyp0wer::NextColumn();
			
			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxCentered("Skin Configuration Cover", "Skin Configuration");
}

void Menu::Players() {

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Trolling Stuff", 586.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			std::stringstream fakeunbox;
			std::stringstream fakeban;
			std::stringstream fakevote;

			static char pFakeVote[127] = "";
			static char pFakeBan[127] = "";
			static char pFakeUB[127] = "";

			fakeban << u8" \x01\x0B\x07" << pFakeBan << u8"\x01\x0B\x07 has been permanently banned from official CS:GO servers. \x01";
			fakeunbox << u8" \x01\x0B\x10" << pFakeUB << u8"\x01 has opened a container and found:\x02 Souvenir AWP | Dragon Lore \x01";
			fakevote << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << pFakeVote << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

			std::string fakevotestring(pFakeVote);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fake ban name");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::InputText("##PFAKEBANINPUT", pFakeBan, 127);
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Set fake ban (NN First)"))
				Miscellaneous::Get().ChangeName(fakeban.str().c_str());

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fake unbox name");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::InputText("##PFAKEUBINPUT", pFakeUB, 127);
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Set Fake Unbox (NN First)"))
				Miscellaneous::Get().ChangeName(fakeunbox.str().c_str());

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Fake-vote text");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::InputText("##PFAKEVOTEINPUT", pFakeVote, 127);
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Set Fake-Vote (NN First)"))
				Miscellaneous::Get().ChangeName(fakevote.str().c_str());


			InsertCheckbox("Sex joke chat spam", XSystemCFG.misc_chatspamer);
			InsertCheckbox("Toxic chat [enable resolver]", XSystemCFG.misc_toxic_chatspamer);


			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Trolling Stuff Cover", "Trolling Stuff");
	}
	vsonyp0wer::NextColumn(); 
	
	
	InsertGroupBoxRight("Name Adjustments", 371.f); {

		style->ItemSpacing = ImVec2(4, 2);
		style->WindowPadding = ImVec2(4, 4);
		vsonyp0wer::CustomSpacing(9.f);

		static char nName[127] = "";
		static char ClanChanger[127] = "";
		vsonyp0wer::Columns(1);
		vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Text("Custom clan tag");
		vsonyp0wer::Separator();
		vsonyp0wer::Columns(1, NULL, true);
		{
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::InputText("##CLANINPUT", ClanChanger, 127);
		}
		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		if (vsonyp0wer::Button("Set clan-tag"))
			Utils::SetClantag(ClanChanger);
		std::string allStringedUp = ClanChanger;
		Miscellaneous::Get().gladTag = allStringedUp;
		InsertCheckbox("Animate clantag (If no text, crash!)", XSystemCFG.misc_animated_clantag);

		vsonyp0wer::Columns(1);
		vsonyp0wer::Separator();
		vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Text("Custom nickname");
		vsonyp0wer::Separator();
		vsonyp0wer::Columns(1, NULL, true);
		{
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::InputText("##NNAMEINPUT", nName, 127);
		}

		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		if (vsonyp0wer::Button("Set nickname##Nichnamechanger"))
			Miscellaneous::Get().ChangeName(nName);

		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		if (vsonyp0wer::Button("No name##NNChanger"))
			Miscellaneous::Get().ChangeName("\n\xAD\xAD\xAD");

		style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
		style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);


	} InsertEndGroupBoxRight("Name Adjustments Cover", "Name Adjustments");

	InsertSpacer("Name Adjustments - Other Spacer");

	InsertGroupBoxRight("Cheat Info", 99.f); {


		style->ItemSpacing = ImVec2(4, 2);
		style->WindowPadding = ImVec2(4, 4);
		vsonyp0wer::CustomSpacing(9.f);

		vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Text("Status: Beta");
		vsonyp0wer::Separator();
		vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Text("Cheat version: V1");
		vsonyp0wer::Separator();
		//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		//vsonyp0wer::Text("Cheat subscription: Lifetime ");
		//vsonyp0wer::Separator();
		//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		//vsonyp0wer::Text("V1");
		//vsonyp0wer::Separator();
		//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		//vsonyp0wer::Text("///////////////");
		//vsonyp0wer::Separator();
		//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		//vsonyp0wer::Text("Website: https://shoppy.gg/@PsychOwO");
		//vsonyp0wer::Separator();
		//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		//vsonyp0wer::Text("Contact Mail: rxhook@gmail.com");
		//vsonyp0wer::Separator();

		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);


	} 	InsertEndGroupBoxRight("Cheat Info Cover", "Cheat Info"); 

	InsertSpacer("Cheat Info - Other Spacer");

	InsertGroupBoxRight("Panic Button", 80.f); {

		style->ItemSpacing = ImVec2(4, 2);
		style->WindowPadding = ImVec2(4, 4);
		vsonyp0wer::CustomSpacing(9.f);

		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		if (vsonyp0wer::Button("Unload##UNLOAD"))
		{
			XSystemCFG.unload = true;
			//Installer::UnLoadvsonyp0wer(); // here
		}


	}InsertEndGroupBoxRight("Panic Button Cover", "Panic Button");

}
void Menu::Config()
{
	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");
	vsonyp0wer::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Matchmaking", 586.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
			style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
			style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

			vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("Matchmaking region changer");
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f);
			vsonyp0wer::ListBox("", &XSystemCFG.misc_region_changer, mmregions,48,36);
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Apply region"))
			{
				Miscellaneous::Get().ChangeRegion();
			}

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		}InsertEndGroupBoxLeft("Matchmaking Cover", "Matchmaking");
	}
	vsonyp0wer::NextColumn();
	InsertGroupBoxRight("Config", 586.f); {
		style->ItemSpacing = ImVec2(4, 2);
		style->WindowPadding = ImVec2(4, 4);
		style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
		vsonyp0wer::CustomSpacing(9.f);

		style->Colors[vsonyp0werCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
		style->Colors[vsonyp0werCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		style->Colors[vsonyp0werCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		style->Colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		style->Colors[vsonyp0werCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

		static std::vector<std::string> configItems = Config::Get().GetAllConfigs();
		static int configItemCurrent = -1;

		static char fName[128] = "default";

		vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Text("Config (click to load)");
		vsonyp0wer::Separator();
		vsonyp0wer::Columns(1, NULL, true);
		{

			{
				vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				if (vsonyp0wer::ListBox("", &configItemCurrent, configItems, 7))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".cfg";
					Config::Get().LoadConfig(fPath);
				}
			}
			{
				vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
				vsonyp0wer::InputText("", fName, 128);
			}

			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Add##Config"))
			{
				std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + fName + ".cfg";
				Config::Get().SaveConfig(fPath);

				configItems = Config::Get().GetAllConfigs();
				configItemCurrent = -1;
			}
			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Refresh##Config"))
				configItems = Config::Get().GetAllConfigs();

			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Save##Config"))
			{
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".cfg";
					Config::Get().SaveConfig(fPath);
					g_CVar->ConsoleColorPrintf(Color(238, 75, 181), "[ChaiSense] Saved Config.");
				}
			}

			vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			if (vsonyp0wer::Button("Remove##Config"))
			{
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".cfg";
					std::remove(fPath.c_str());
					g_CVar->ConsoleColorPrintf(Color(238, 75, 181), "[ChaiSense] Removed Config.");

					configItems = Config::Get().GetAllConfigs();
					configItemCurrent = -1;
				}
			}
		}

		static int luasciptCurrent = -1;
		static std::vector<std::string> luascriptItems = Config::Get().GetAllLuas();

		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		if (vsonyp0wer::ListBox("##luas", &luasciptCurrent, luascriptItems, 3))
		{
			std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\lua\\" + luascriptItems[luasciptCurrent] + ".lua";
		}
		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Button("Load lua script##LOADLUA");
		vsonyp0wer::CustomSpacing(6.f); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
		vsonyp0wer::Button("Unload lua script##UNLOADLUA");
		style->ItemSpacing = ImVec2(0, 0);
		style->WindowPadding = ImVec2(6, 6);
	}InsertEndGroupBoxRight("Config Cover", "Config");
}
void Menu::LuaTab()
{

	vsonyp0werStyle* style = &vsonyp0wer::GetStyle();
	InsertSpacer("Top Spacer");

	vsonyp0wer::Columns(2, NULL, false); {

		InsertGroupBoxLeft("A", 586.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);

			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text(" ");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text(" ");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text(" ");
			vsonyp0wer::Separator();
			vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			vsonyp0wer::Text("");
			vsonyp0wer::Separator();
			//vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);
			//vsonyp0wer::Text("///////////////");
			//vsonyp0wer::Separator();

		} InsertEndGroupBoxLeft("Info cheat and stuff", "Info cheat and stuff");
	}
	vsonyp0wer::NextColumn(); {

		InsertGroupBoxRight("B", 586.f); {


			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			vsonyp0wer::CustomSpacing(9.f);


			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);


		} InsertEndGroupBoxRight("B Cover", "B");
	}
}


































































// Junk Code By Troll Face & Thaisen's Gen
void CrJvqRGNFREXvoEuHCdqAivqIpWZnVOilnSAbZNN48275552() {     int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67135921 = -654521274;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30825305 = -877064697;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97982483 = -67063255;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82695188 = -782884152;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL73988219 = -109879198;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87837182 = 80954465;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63083710 = 41300070;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97642953 = -106084137;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL56722888 = -974403527;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL75869747 = -54146346;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL55751655 = -361618557;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94715615 = -590107236;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL31152727 = -569013411;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL36835696 = -352345479;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83043332 = -278117461;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68624548 = -434542773;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60186438 = -920871131;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL80629746 = -776750375;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL96662908 = -120971713;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22784935 = -54863893;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL38396678 = -666684166;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL7010632 = -394030648;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72986065 = -830996009;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL15839378 = -763291516;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL88637060 = -969952128;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68928698 = -47734364;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL85179545 = -946896783;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL69797920 = -74639049;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72396564 = -207130001;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5278876 = -150177489;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL32229450 = -619025257;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19878738 = -465239338;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72666156 = -870125688;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL12833801 = -934455920;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87721016 = -958493841;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19075474 = -689163113;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82099025 = -269159972;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL24708500 = -270101021;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL782773 = -735660151;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63620727 = -194098615;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19407067 = -306392911;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14504500 = -732629608;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4920020 = -289021777;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL52409803 = -813349459;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72876871 = -234828449;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53456616 = -736294197;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL29494863 = -331471475;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33927112 = -210500427;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83705662 = -668926775;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67445596 = -894863233;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33746525 = -791794833;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL74510617 = -241410409;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14307260 = -190792215;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5704821 = -748671542;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL9878875 = -926272342;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60125289 = -160490626;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57839240 = 53931312;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82143105 = -303771740;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94058127 = -812932025;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5059521 = 37855165;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2657637 = 27851248;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL93285790 = -884060881;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL25246389 = -898954137;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51444013 = -724226039;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL43640297 = -435121090;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35872918 = -896379220;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22049459 = -719981549;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL18318926 = -634557491;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL49114680 = -393851639;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63967858 = -588954349;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86525523 = -65382802;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35477938 = -550770111;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL79846974 = 58909775;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33042181 = -926873098;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL3377869 = -748470982;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL23892179 = -934054558;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2090613 = -5008872;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL20576262 = 82353449;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL42962507 = -428463068;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35180444 = -133657931;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL39433836 = -716262889;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51252434 = -636396357;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86092257 = -405712275;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4950969 = -312266768;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL71532350 = -358382657;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57718833 = -277614849;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5571478 = -174447123;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL66961336 = -21454146;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2954927 = 91816422;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL27595727 = -698003216;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL61236233 = -643094425;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL99955919 = -965388233;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30650372 = -457168996;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL95723251 = -673515317;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60963090 = -121949863;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL26121276 = -422332031;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL89258110 = -833675472;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53476007 = -564795739;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL8769506 = -278228370;    int ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL37003954 = -654521274;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67135921 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30825305;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30825305 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97982483;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97982483 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82695188;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82695188 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL73988219;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL73988219 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87837182;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87837182 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63083710;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63083710 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97642953;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL97642953 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL56722888;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL56722888 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL75869747;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL75869747 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL55751655;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL55751655 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94715615;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94715615 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL31152727;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL31152727 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL36835696;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL36835696 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83043332;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83043332 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68624548;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68624548 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60186438;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60186438 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL80629746;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL80629746 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL96662908;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL96662908 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22784935;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22784935 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL38396678;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL38396678 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL7010632;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL7010632 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72986065;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72986065 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL15839378;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL15839378 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL88637060;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL88637060 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68928698;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL68928698 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL85179545;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL85179545 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL69797920;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL69797920 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72396564;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72396564 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5278876;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5278876 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL32229450;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL32229450 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19878738;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19878738 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72666156;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72666156 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL12833801;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL12833801 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87721016;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL87721016 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19075474;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19075474 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82099025;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82099025 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL24708500;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL24708500 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL782773;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL782773 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63620727;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63620727 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19407067;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL19407067 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14504500;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14504500 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4920020;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4920020 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL52409803;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL52409803 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72876871;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL72876871 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53456616;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53456616 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL29494863;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL29494863 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33927112;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33927112 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83705662;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL83705662 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67445596;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67445596 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33746525;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33746525 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL74510617;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL74510617 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14307260;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL14307260 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5704821;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5704821 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL9878875;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL9878875 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60125289;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60125289 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57839240;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57839240 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82143105;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL82143105 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94058127;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL94058127 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5059521;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5059521 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2657637;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2657637 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL93285790;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL93285790 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL25246389;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL25246389 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51444013;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51444013 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL43640297;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL43640297 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35872918;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35872918 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22049459;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL22049459 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL18318926;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL18318926 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL49114680;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL49114680 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63967858;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL63967858 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86525523;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86525523 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35477938;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35477938 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL79846974;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL79846974 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33042181;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL33042181 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL3377869;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL3377869 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL23892179;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL23892179 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2090613;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2090613 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL20576262;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL20576262 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL42962507;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL42962507 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35180444;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL35180444 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL39433836;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL39433836 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51252434;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL51252434 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86092257;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL86092257 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4950969;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL4950969 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL71532350;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL71532350 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57718833;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL57718833 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5571478;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL5571478 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL66961336;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL66961336 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2954927;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL2954927 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL27595727;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL27595727 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL61236233;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL61236233 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL99955919;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL99955919 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30650372;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL30650372 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL95723251;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL95723251 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60963090;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL60963090 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL26121276;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL26121276 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL89258110;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL89258110 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53476007;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL53476007 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL8769506;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL8769506 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL37003954;     ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL37003954 = ivdIzHwpByuFFERTvTnAvhajCVlIVEShoQHFEnrAsgXobJdhWzEImMSAUtGdVoTxfNRoxL67135921;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void EPwekhdBGCFoHWAfpiLHGJoHjZFJFMXhBiwwCvPCVCxvWcCmMDj53650341() {     float YuaHubpvKcTwteXmZaEduTJmDLsk20361899 = -907887633;    float YuaHubpvKcTwteXmZaEduTJmDLsk67054475 = -448256819;    float YuaHubpvKcTwteXmZaEduTJmDLsk4393140 = -964178622;    float YuaHubpvKcTwteXmZaEduTJmDLsk7750925 = -836280249;    float YuaHubpvKcTwteXmZaEduTJmDLsk25440338 = -268687542;    float YuaHubpvKcTwteXmZaEduTJmDLsk11863157 = -224115351;    float YuaHubpvKcTwteXmZaEduTJmDLsk97714868 = -675260814;    float YuaHubpvKcTwteXmZaEduTJmDLsk27925109 = -361056172;    float YuaHubpvKcTwteXmZaEduTJmDLsk99748980 = -9370520;    float YuaHubpvKcTwteXmZaEduTJmDLsk73899619 = -843657454;    float YuaHubpvKcTwteXmZaEduTJmDLsk68600485 = -333488564;    float YuaHubpvKcTwteXmZaEduTJmDLsk93906856 = -846318014;    float YuaHubpvKcTwteXmZaEduTJmDLsk89554232 = -224991075;    float YuaHubpvKcTwteXmZaEduTJmDLsk25291965 = 16748213;    float YuaHubpvKcTwteXmZaEduTJmDLsk22503974 = -840431737;    float YuaHubpvKcTwteXmZaEduTJmDLsk4090486 = 51561040;    float YuaHubpvKcTwteXmZaEduTJmDLsk8714369 = -664869630;    float YuaHubpvKcTwteXmZaEduTJmDLsk97099882 = -507939464;    float YuaHubpvKcTwteXmZaEduTJmDLsk7343166 = -146822756;    float YuaHubpvKcTwteXmZaEduTJmDLsk77087295 = -326817673;    float YuaHubpvKcTwteXmZaEduTJmDLsk12422571 = -962411736;    float YuaHubpvKcTwteXmZaEduTJmDLsk5470785 = -200416592;    float YuaHubpvKcTwteXmZaEduTJmDLsk67097091 = -465000943;    float YuaHubpvKcTwteXmZaEduTJmDLsk3337605 = -329898659;    float YuaHubpvKcTwteXmZaEduTJmDLsk51472450 = -382944832;    float YuaHubpvKcTwteXmZaEduTJmDLsk4786987 = -80186230;    float YuaHubpvKcTwteXmZaEduTJmDLsk77591382 = -26784223;    float YuaHubpvKcTwteXmZaEduTJmDLsk85726671 = -145092829;    float YuaHubpvKcTwteXmZaEduTJmDLsk96501237 = -509272661;    float YuaHubpvKcTwteXmZaEduTJmDLsk81588704 = -219309711;    float YuaHubpvKcTwteXmZaEduTJmDLsk34652826 = -196659857;    float YuaHubpvKcTwteXmZaEduTJmDLsk66055328 = -765833960;    float YuaHubpvKcTwteXmZaEduTJmDLsk14655077 = -514654812;    float YuaHubpvKcTwteXmZaEduTJmDLsk41072862 = -335513004;    float YuaHubpvKcTwteXmZaEduTJmDLsk89632387 = -389797362;    float YuaHubpvKcTwteXmZaEduTJmDLsk33182717 = -384124584;    float YuaHubpvKcTwteXmZaEduTJmDLsk60763293 = -610392901;    float YuaHubpvKcTwteXmZaEduTJmDLsk38259341 = -266979953;    float YuaHubpvKcTwteXmZaEduTJmDLsk70017200 = -777710415;    float YuaHubpvKcTwteXmZaEduTJmDLsk98501681 = -229298859;    float YuaHubpvKcTwteXmZaEduTJmDLsk59051824 = -466206795;    float YuaHubpvKcTwteXmZaEduTJmDLsk85407759 = -548535366;    float YuaHubpvKcTwteXmZaEduTJmDLsk51171620 = -818443309;    float YuaHubpvKcTwteXmZaEduTJmDLsk81110797 = -58046534;    float YuaHubpvKcTwteXmZaEduTJmDLsk72737426 = -844357429;    float YuaHubpvKcTwteXmZaEduTJmDLsk87625631 = -788203326;    float YuaHubpvKcTwteXmZaEduTJmDLsk98639841 = -18223582;    float YuaHubpvKcTwteXmZaEduTJmDLsk38916066 = -14958684;    float YuaHubpvKcTwteXmZaEduTJmDLsk4204871 = -616565928;    float YuaHubpvKcTwteXmZaEduTJmDLsk9810143 = -245032058;    float YuaHubpvKcTwteXmZaEduTJmDLsk40989996 = -817430475;    float YuaHubpvKcTwteXmZaEduTJmDLsk56604427 = -898237759;    float YuaHubpvKcTwteXmZaEduTJmDLsk66927286 = -390052466;    float YuaHubpvKcTwteXmZaEduTJmDLsk8092697 = -112678282;    float YuaHubpvKcTwteXmZaEduTJmDLsk41227770 = -265473303;    float YuaHubpvKcTwteXmZaEduTJmDLsk14891115 = -607471041;    float YuaHubpvKcTwteXmZaEduTJmDLsk99957383 = -983255877;    float YuaHubpvKcTwteXmZaEduTJmDLsk1055536 = -534279964;    float YuaHubpvKcTwteXmZaEduTJmDLsk56278474 = -353335417;    float YuaHubpvKcTwteXmZaEduTJmDLsk20653351 = -88501313;    float YuaHubpvKcTwteXmZaEduTJmDLsk34271775 = -97331128;    float YuaHubpvKcTwteXmZaEduTJmDLsk11988198 = -430167985;    float YuaHubpvKcTwteXmZaEduTJmDLsk31423871 = -851783512;    float YuaHubpvKcTwteXmZaEduTJmDLsk18160276 = -790060809;    float YuaHubpvKcTwteXmZaEduTJmDLsk39246793 = -546997598;    float YuaHubpvKcTwteXmZaEduTJmDLsk2545157 = -567654604;    float YuaHubpvKcTwteXmZaEduTJmDLsk79251780 = -231663203;    float YuaHubpvKcTwteXmZaEduTJmDLsk48481370 = -889478071;    float YuaHubpvKcTwteXmZaEduTJmDLsk35659577 = -593454426;    float YuaHubpvKcTwteXmZaEduTJmDLsk89321257 = -356307154;    float YuaHubpvKcTwteXmZaEduTJmDLsk43327192 = -338046060;    float YuaHubpvKcTwteXmZaEduTJmDLsk70455028 = -297889677;    float YuaHubpvKcTwteXmZaEduTJmDLsk27082683 = -730229049;    float YuaHubpvKcTwteXmZaEduTJmDLsk8841484 = -917523898;    float YuaHubpvKcTwteXmZaEduTJmDLsk18035472 = -860610878;    float YuaHubpvKcTwteXmZaEduTJmDLsk27014811 = -313876370;    float YuaHubpvKcTwteXmZaEduTJmDLsk54299165 = -381973284;    float YuaHubpvKcTwteXmZaEduTJmDLsk85986294 = -306954409;    float YuaHubpvKcTwteXmZaEduTJmDLsk30600178 = -485541230;    float YuaHubpvKcTwteXmZaEduTJmDLsk63846819 = -594741507;    float YuaHubpvKcTwteXmZaEduTJmDLsk6147146 = 38037352;    float YuaHubpvKcTwteXmZaEduTJmDLsk38675316 = 88174460;    float YuaHubpvKcTwteXmZaEduTJmDLsk81521800 = -528526902;    float YuaHubpvKcTwteXmZaEduTJmDLsk86691095 = -164240603;    float YuaHubpvKcTwteXmZaEduTJmDLsk40598708 = -401879237;    float YuaHubpvKcTwteXmZaEduTJmDLsk78048399 = -298422099;    float YuaHubpvKcTwteXmZaEduTJmDLsk99128042 = -275781494;    float YuaHubpvKcTwteXmZaEduTJmDLsk6562380 = -301976530;    float YuaHubpvKcTwteXmZaEduTJmDLsk99845092 = 29960298;    float YuaHubpvKcTwteXmZaEduTJmDLsk74741273 = -782326321;    float YuaHubpvKcTwteXmZaEduTJmDLsk33225333 = -400868707;    float YuaHubpvKcTwteXmZaEduTJmDLsk59707758 = 23887062;    float YuaHubpvKcTwteXmZaEduTJmDLsk81980866 = -913644537;    float YuaHubpvKcTwteXmZaEduTJmDLsk49363849 = -589209103;    float YuaHubpvKcTwteXmZaEduTJmDLsk64229907 = -31967731;    float YuaHubpvKcTwteXmZaEduTJmDLsk47063627 = 63961190;    float YuaHubpvKcTwteXmZaEduTJmDLsk53983889 = -696751854;    float YuaHubpvKcTwteXmZaEduTJmDLsk33011344 = 71617500;    float YuaHubpvKcTwteXmZaEduTJmDLsk41864004 = -511048937;    float YuaHubpvKcTwteXmZaEduTJmDLsk70192269 = -907887633;     YuaHubpvKcTwteXmZaEduTJmDLsk20361899 = YuaHubpvKcTwteXmZaEduTJmDLsk67054475;     YuaHubpvKcTwteXmZaEduTJmDLsk67054475 = YuaHubpvKcTwteXmZaEduTJmDLsk4393140;     YuaHubpvKcTwteXmZaEduTJmDLsk4393140 = YuaHubpvKcTwteXmZaEduTJmDLsk7750925;     YuaHubpvKcTwteXmZaEduTJmDLsk7750925 = YuaHubpvKcTwteXmZaEduTJmDLsk25440338;     YuaHubpvKcTwteXmZaEduTJmDLsk25440338 = YuaHubpvKcTwteXmZaEduTJmDLsk11863157;     YuaHubpvKcTwteXmZaEduTJmDLsk11863157 = YuaHubpvKcTwteXmZaEduTJmDLsk97714868;     YuaHubpvKcTwteXmZaEduTJmDLsk97714868 = YuaHubpvKcTwteXmZaEduTJmDLsk27925109;     YuaHubpvKcTwteXmZaEduTJmDLsk27925109 = YuaHubpvKcTwteXmZaEduTJmDLsk99748980;     YuaHubpvKcTwteXmZaEduTJmDLsk99748980 = YuaHubpvKcTwteXmZaEduTJmDLsk73899619;     YuaHubpvKcTwteXmZaEduTJmDLsk73899619 = YuaHubpvKcTwteXmZaEduTJmDLsk68600485;     YuaHubpvKcTwteXmZaEduTJmDLsk68600485 = YuaHubpvKcTwteXmZaEduTJmDLsk93906856;     YuaHubpvKcTwteXmZaEduTJmDLsk93906856 = YuaHubpvKcTwteXmZaEduTJmDLsk89554232;     YuaHubpvKcTwteXmZaEduTJmDLsk89554232 = YuaHubpvKcTwteXmZaEduTJmDLsk25291965;     YuaHubpvKcTwteXmZaEduTJmDLsk25291965 = YuaHubpvKcTwteXmZaEduTJmDLsk22503974;     YuaHubpvKcTwteXmZaEduTJmDLsk22503974 = YuaHubpvKcTwteXmZaEduTJmDLsk4090486;     YuaHubpvKcTwteXmZaEduTJmDLsk4090486 = YuaHubpvKcTwteXmZaEduTJmDLsk8714369;     YuaHubpvKcTwteXmZaEduTJmDLsk8714369 = YuaHubpvKcTwteXmZaEduTJmDLsk97099882;     YuaHubpvKcTwteXmZaEduTJmDLsk97099882 = YuaHubpvKcTwteXmZaEduTJmDLsk7343166;     YuaHubpvKcTwteXmZaEduTJmDLsk7343166 = YuaHubpvKcTwteXmZaEduTJmDLsk77087295;     YuaHubpvKcTwteXmZaEduTJmDLsk77087295 = YuaHubpvKcTwteXmZaEduTJmDLsk12422571;     YuaHubpvKcTwteXmZaEduTJmDLsk12422571 = YuaHubpvKcTwteXmZaEduTJmDLsk5470785;     YuaHubpvKcTwteXmZaEduTJmDLsk5470785 = YuaHubpvKcTwteXmZaEduTJmDLsk67097091;     YuaHubpvKcTwteXmZaEduTJmDLsk67097091 = YuaHubpvKcTwteXmZaEduTJmDLsk3337605;     YuaHubpvKcTwteXmZaEduTJmDLsk3337605 = YuaHubpvKcTwteXmZaEduTJmDLsk51472450;     YuaHubpvKcTwteXmZaEduTJmDLsk51472450 = YuaHubpvKcTwteXmZaEduTJmDLsk4786987;     YuaHubpvKcTwteXmZaEduTJmDLsk4786987 = YuaHubpvKcTwteXmZaEduTJmDLsk77591382;     YuaHubpvKcTwteXmZaEduTJmDLsk77591382 = YuaHubpvKcTwteXmZaEduTJmDLsk85726671;     YuaHubpvKcTwteXmZaEduTJmDLsk85726671 = YuaHubpvKcTwteXmZaEduTJmDLsk96501237;     YuaHubpvKcTwteXmZaEduTJmDLsk96501237 = YuaHubpvKcTwteXmZaEduTJmDLsk81588704;     YuaHubpvKcTwteXmZaEduTJmDLsk81588704 = YuaHubpvKcTwteXmZaEduTJmDLsk34652826;     YuaHubpvKcTwteXmZaEduTJmDLsk34652826 = YuaHubpvKcTwteXmZaEduTJmDLsk66055328;     YuaHubpvKcTwteXmZaEduTJmDLsk66055328 = YuaHubpvKcTwteXmZaEduTJmDLsk14655077;     YuaHubpvKcTwteXmZaEduTJmDLsk14655077 = YuaHubpvKcTwteXmZaEduTJmDLsk41072862;     YuaHubpvKcTwteXmZaEduTJmDLsk41072862 = YuaHubpvKcTwteXmZaEduTJmDLsk89632387;     YuaHubpvKcTwteXmZaEduTJmDLsk89632387 = YuaHubpvKcTwteXmZaEduTJmDLsk33182717;     YuaHubpvKcTwteXmZaEduTJmDLsk33182717 = YuaHubpvKcTwteXmZaEduTJmDLsk60763293;     YuaHubpvKcTwteXmZaEduTJmDLsk60763293 = YuaHubpvKcTwteXmZaEduTJmDLsk38259341;     YuaHubpvKcTwteXmZaEduTJmDLsk38259341 = YuaHubpvKcTwteXmZaEduTJmDLsk70017200;     YuaHubpvKcTwteXmZaEduTJmDLsk70017200 = YuaHubpvKcTwteXmZaEduTJmDLsk98501681;     YuaHubpvKcTwteXmZaEduTJmDLsk98501681 = YuaHubpvKcTwteXmZaEduTJmDLsk59051824;     YuaHubpvKcTwteXmZaEduTJmDLsk59051824 = YuaHubpvKcTwteXmZaEduTJmDLsk85407759;     YuaHubpvKcTwteXmZaEduTJmDLsk85407759 = YuaHubpvKcTwteXmZaEduTJmDLsk51171620;     YuaHubpvKcTwteXmZaEduTJmDLsk51171620 = YuaHubpvKcTwteXmZaEduTJmDLsk81110797;     YuaHubpvKcTwteXmZaEduTJmDLsk81110797 = YuaHubpvKcTwteXmZaEduTJmDLsk72737426;     YuaHubpvKcTwteXmZaEduTJmDLsk72737426 = YuaHubpvKcTwteXmZaEduTJmDLsk87625631;     YuaHubpvKcTwteXmZaEduTJmDLsk87625631 = YuaHubpvKcTwteXmZaEduTJmDLsk98639841;     YuaHubpvKcTwteXmZaEduTJmDLsk98639841 = YuaHubpvKcTwteXmZaEduTJmDLsk38916066;     YuaHubpvKcTwteXmZaEduTJmDLsk38916066 = YuaHubpvKcTwteXmZaEduTJmDLsk4204871;     YuaHubpvKcTwteXmZaEduTJmDLsk4204871 = YuaHubpvKcTwteXmZaEduTJmDLsk9810143;     YuaHubpvKcTwteXmZaEduTJmDLsk9810143 = YuaHubpvKcTwteXmZaEduTJmDLsk40989996;     YuaHubpvKcTwteXmZaEduTJmDLsk40989996 = YuaHubpvKcTwteXmZaEduTJmDLsk56604427;     YuaHubpvKcTwteXmZaEduTJmDLsk56604427 = YuaHubpvKcTwteXmZaEduTJmDLsk66927286;     YuaHubpvKcTwteXmZaEduTJmDLsk66927286 = YuaHubpvKcTwteXmZaEduTJmDLsk8092697;     YuaHubpvKcTwteXmZaEduTJmDLsk8092697 = YuaHubpvKcTwteXmZaEduTJmDLsk41227770;     YuaHubpvKcTwteXmZaEduTJmDLsk41227770 = YuaHubpvKcTwteXmZaEduTJmDLsk14891115;     YuaHubpvKcTwteXmZaEduTJmDLsk14891115 = YuaHubpvKcTwteXmZaEduTJmDLsk99957383;     YuaHubpvKcTwteXmZaEduTJmDLsk99957383 = YuaHubpvKcTwteXmZaEduTJmDLsk1055536;     YuaHubpvKcTwteXmZaEduTJmDLsk1055536 = YuaHubpvKcTwteXmZaEduTJmDLsk56278474;     YuaHubpvKcTwteXmZaEduTJmDLsk56278474 = YuaHubpvKcTwteXmZaEduTJmDLsk20653351;     YuaHubpvKcTwteXmZaEduTJmDLsk20653351 = YuaHubpvKcTwteXmZaEduTJmDLsk34271775;     YuaHubpvKcTwteXmZaEduTJmDLsk34271775 = YuaHubpvKcTwteXmZaEduTJmDLsk11988198;     YuaHubpvKcTwteXmZaEduTJmDLsk11988198 = YuaHubpvKcTwteXmZaEduTJmDLsk31423871;     YuaHubpvKcTwteXmZaEduTJmDLsk31423871 = YuaHubpvKcTwteXmZaEduTJmDLsk18160276;     YuaHubpvKcTwteXmZaEduTJmDLsk18160276 = YuaHubpvKcTwteXmZaEduTJmDLsk39246793;     YuaHubpvKcTwteXmZaEduTJmDLsk39246793 = YuaHubpvKcTwteXmZaEduTJmDLsk2545157;     YuaHubpvKcTwteXmZaEduTJmDLsk2545157 = YuaHubpvKcTwteXmZaEduTJmDLsk79251780;     YuaHubpvKcTwteXmZaEduTJmDLsk79251780 = YuaHubpvKcTwteXmZaEduTJmDLsk48481370;     YuaHubpvKcTwteXmZaEduTJmDLsk48481370 = YuaHubpvKcTwteXmZaEduTJmDLsk35659577;     YuaHubpvKcTwteXmZaEduTJmDLsk35659577 = YuaHubpvKcTwteXmZaEduTJmDLsk89321257;     YuaHubpvKcTwteXmZaEduTJmDLsk89321257 = YuaHubpvKcTwteXmZaEduTJmDLsk43327192;     YuaHubpvKcTwteXmZaEduTJmDLsk43327192 = YuaHubpvKcTwteXmZaEduTJmDLsk70455028;     YuaHubpvKcTwteXmZaEduTJmDLsk70455028 = YuaHubpvKcTwteXmZaEduTJmDLsk27082683;     YuaHubpvKcTwteXmZaEduTJmDLsk27082683 = YuaHubpvKcTwteXmZaEduTJmDLsk8841484;     YuaHubpvKcTwteXmZaEduTJmDLsk8841484 = YuaHubpvKcTwteXmZaEduTJmDLsk18035472;     YuaHubpvKcTwteXmZaEduTJmDLsk18035472 = YuaHubpvKcTwteXmZaEduTJmDLsk27014811;     YuaHubpvKcTwteXmZaEduTJmDLsk27014811 = YuaHubpvKcTwteXmZaEduTJmDLsk54299165;     YuaHubpvKcTwteXmZaEduTJmDLsk54299165 = YuaHubpvKcTwteXmZaEduTJmDLsk85986294;     YuaHubpvKcTwteXmZaEduTJmDLsk85986294 = YuaHubpvKcTwteXmZaEduTJmDLsk30600178;     YuaHubpvKcTwteXmZaEduTJmDLsk30600178 = YuaHubpvKcTwteXmZaEduTJmDLsk63846819;     YuaHubpvKcTwteXmZaEduTJmDLsk63846819 = YuaHubpvKcTwteXmZaEduTJmDLsk6147146;     YuaHubpvKcTwteXmZaEduTJmDLsk6147146 = YuaHubpvKcTwteXmZaEduTJmDLsk38675316;     YuaHubpvKcTwteXmZaEduTJmDLsk38675316 = YuaHubpvKcTwteXmZaEduTJmDLsk81521800;     YuaHubpvKcTwteXmZaEduTJmDLsk81521800 = YuaHubpvKcTwteXmZaEduTJmDLsk86691095;     YuaHubpvKcTwteXmZaEduTJmDLsk86691095 = YuaHubpvKcTwteXmZaEduTJmDLsk40598708;     YuaHubpvKcTwteXmZaEduTJmDLsk40598708 = YuaHubpvKcTwteXmZaEduTJmDLsk78048399;     YuaHubpvKcTwteXmZaEduTJmDLsk78048399 = YuaHubpvKcTwteXmZaEduTJmDLsk99128042;     YuaHubpvKcTwteXmZaEduTJmDLsk99128042 = YuaHubpvKcTwteXmZaEduTJmDLsk6562380;     YuaHubpvKcTwteXmZaEduTJmDLsk6562380 = YuaHubpvKcTwteXmZaEduTJmDLsk99845092;     YuaHubpvKcTwteXmZaEduTJmDLsk99845092 = YuaHubpvKcTwteXmZaEduTJmDLsk74741273;     YuaHubpvKcTwteXmZaEduTJmDLsk74741273 = YuaHubpvKcTwteXmZaEduTJmDLsk33225333;     YuaHubpvKcTwteXmZaEduTJmDLsk33225333 = YuaHubpvKcTwteXmZaEduTJmDLsk59707758;     YuaHubpvKcTwteXmZaEduTJmDLsk59707758 = YuaHubpvKcTwteXmZaEduTJmDLsk81980866;     YuaHubpvKcTwteXmZaEduTJmDLsk81980866 = YuaHubpvKcTwteXmZaEduTJmDLsk49363849;     YuaHubpvKcTwteXmZaEduTJmDLsk49363849 = YuaHubpvKcTwteXmZaEduTJmDLsk64229907;     YuaHubpvKcTwteXmZaEduTJmDLsk64229907 = YuaHubpvKcTwteXmZaEduTJmDLsk47063627;     YuaHubpvKcTwteXmZaEduTJmDLsk47063627 = YuaHubpvKcTwteXmZaEduTJmDLsk53983889;     YuaHubpvKcTwteXmZaEduTJmDLsk53983889 = YuaHubpvKcTwteXmZaEduTJmDLsk33011344;     YuaHubpvKcTwteXmZaEduTJmDLsk33011344 = YuaHubpvKcTwteXmZaEduTJmDLsk41864004;     YuaHubpvKcTwteXmZaEduTJmDLsk41864004 = YuaHubpvKcTwteXmZaEduTJmDLsk70192269;     YuaHubpvKcTwteXmZaEduTJmDLsk70192269 = YuaHubpvKcTwteXmZaEduTJmDLsk20361899;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HohqqSxwUjJZnzpCNpmHAoIUR83244649() {     long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn93949007 = -711963983;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33732709 = -161364046;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn25893225 = -228781061;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12272004 = -679679095;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn40372943 = -989443345;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn73963283 = -480524723;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79841514 = -866933400;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn57181593 = -361859046;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn91745586 = -205542118;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75455164 = -719011034;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65786438 = -69167241;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36648732 = -463858695;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn589648 = -52237124;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn2076401 = -7811640;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn30411031 = -555569184;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42148155 = -303662797;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42723930 = -146843920;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65888622 = -734623390;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81944338 = -521356341;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81565249 = -375473623;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69306587 = -125197111;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88490415 = -483570121;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn62338962 = -655519705;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn84636920 = -363110366;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58180801 = -424878465;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17596881 = -37825603;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54014574 = -312457855;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68526241 = -419876001;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89098634 = -749705830;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35512916 = -119085384;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39351278 = -168800484;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68323269 = -896411685;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68545283 = -790206009;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41605659 = -746798326;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn74817967 = -585221158;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn24760501 = -932530086;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97967029 = -48120333;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67155650 = -949008985;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17963070 = -527020711;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89569516 = -279559559;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn6398688 = -260450226;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn80912977 = -340848990;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67652983 = -461749825;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39685334 = -976645395;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn29540878 = -67879632;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn94288541 = -949107015;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35223620 = -223944900;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97369245 = -748479430;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn34770612 = -397373738;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn45140821 = -930508333;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22793305 = -807238521;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn60022516 = -24246416;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn50387636 = -253020224;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47390005 = -485130458;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13938062 = -421948091;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5458592 = -128393862;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn71393747 = -505844341;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41256305 = -865670695;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54091202 = -154800631;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22776063 = -851617743;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn19948710 = -68066868;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11315273 = -347057400;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68082959 = -612153216;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56232671 = 13543266;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36103887 = -450210551;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97463168 = -172755557;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68103449 = -673652687;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58983988 = -305438799;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27258433 = -422590482;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5650530 = -623039098;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn44181125 = -155542464;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75568279 = -197834935;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47925553 = -107602679;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn92374822 = -141796782;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75166561 = -15023397;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88393610 = -784348121;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn20837432 = 78179703;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22653629 = -678874311;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn55096043 = -195230734;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn63892260 = -475771450;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn82373260 = -813880703;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56645328 = -563978426;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33755629 = 77497736;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn43957813 = -819197498;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12719611 = -311846864;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79328761 = -44554068;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17935633 = -543391461;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn21155278 = -205075551;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27667598 = -224850236;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69359375 = -356827296;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn53366754 = -326685746;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56710725 = -182449638;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13064449 = -694208355;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95187007 = -675402968;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69620806 = -111492692;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95083414 = -913392827;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12830018 = -728695774;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11420313 = -375293091;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn3581448 = -426434844;    long qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn32077709 = -711963983;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn93949007 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33732709;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33732709 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn25893225;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn25893225 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12272004;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12272004 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn40372943;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn40372943 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn73963283;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn73963283 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79841514;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79841514 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn57181593;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn57181593 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn91745586;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn91745586 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75455164;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75455164 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65786438;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65786438 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36648732;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36648732 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn589648;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn589648 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn2076401;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn2076401 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn30411031;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn30411031 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42148155;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42148155 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42723930;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn42723930 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65888622;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn65888622 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81944338;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81944338 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81565249;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn81565249 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69306587;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69306587 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88490415;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88490415 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn62338962;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn62338962 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn84636920;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn84636920 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58180801;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58180801 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17596881;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17596881 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54014574;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54014574 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68526241;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68526241 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89098634;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89098634 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35512916;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35512916 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39351278;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39351278 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68323269;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68323269 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68545283;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68545283 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41605659;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41605659 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn74817967;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn74817967 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn24760501;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn24760501 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97967029;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97967029 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67155650;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67155650 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17963070;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17963070 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89569516;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn89569516 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn6398688;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn6398688 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn80912977;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn80912977 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67652983;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn67652983 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39685334;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn39685334 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn29540878;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn29540878 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn94288541;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn94288541 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35223620;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn35223620 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97369245;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97369245 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn34770612;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn34770612 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn45140821;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn45140821 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22793305;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22793305 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn60022516;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn60022516 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn50387636;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn50387636 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47390005;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47390005 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13938062;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13938062 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5458592;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5458592 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn71393747;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn71393747 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41256305;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn41256305 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54091202;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn54091202 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22776063;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22776063 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn19948710;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn19948710 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11315273;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11315273 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68082959;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68082959 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56232671;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56232671 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36103887;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn36103887 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97463168;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn97463168 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68103449;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn68103449 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58983988;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn58983988 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27258433;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27258433 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5650530;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn5650530 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn44181125;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn44181125 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75568279;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75568279 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47925553;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn47925553 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn92374822;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn92374822 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75166561;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn75166561 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88393610;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn88393610 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn20837432;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn20837432 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22653629;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn22653629 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn55096043;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn55096043 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn63892260;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn63892260 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn82373260;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn82373260 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56645328;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56645328 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33755629;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn33755629 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn43957813;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn43957813 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12719611;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12719611 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79328761;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn79328761 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17935633;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn17935633 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn21155278;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn21155278 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27667598;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn27667598 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69359375;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69359375 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn53366754;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn53366754 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56710725;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn56710725 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13064449;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn13064449 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95187007;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95187007 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69620806;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn69620806 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95083414;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn95083414 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12830018;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn12830018 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11420313;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn11420313 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn3581448;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn3581448 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn32077709;     qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn32077709 = qWeZbBmQcDBDHFdIGevhHqvBWfNxgRTgMZAvSWshXJLwn93949007;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ajBIBkrCxizlSXaUIpqqtmTIHQyKJWbCitgPh74566482() {     double DyJqTaNPbDVOLbsCkFnQs1258016 = -9193436;    double DyJqTaNPbDVOLbsCkFnQs32652119 = -788771906;    double DyJqTaNPbDVOLbsCkFnQs11307731 = -175973343;    double DyJqTaNPbDVOLbsCkFnQs10211000 = -758095077;    double DyJqTaNPbDVOLbsCkFnQs37076911 = -575236171;    double DyJqTaNPbDVOLbsCkFnQs56104287 = -765112966;    double DyJqTaNPbDVOLbsCkFnQs40985557 = -731254242;    double DyJqTaNPbDVOLbsCkFnQs97081673 = -359160835;    double DyJqTaNPbDVOLbsCkFnQs4646763 = -202607929;    double DyJqTaNPbDVOLbsCkFnQs22276176 = -451607347;    double DyJqTaNPbDVOLbsCkFnQs10941578 = -184999194;    double DyJqTaNPbDVOLbsCkFnQs73175298 = -494408237;    double DyJqTaNPbDVOLbsCkFnQs20642143 = -552730037;    double DyJqTaNPbDVOLbsCkFnQs61695035 = 81657383;    double DyJqTaNPbDVOLbsCkFnQs3790374 = -187082148;    double DyJqTaNPbDVOLbsCkFnQs45660989 = -941213197;    double DyJqTaNPbDVOLbsCkFnQs90562091 = -292601715;    double DyJqTaNPbDVOLbsCkFnQs29504765 = -396426795;    double DyJqTaNPbDVOLbsCkFnQs77629060 = -289034731;    double DyJqTaNPbDVOLbsCkFnQs28169288 = -743428603;    double DyJqTaNPbDVOLbsCkFnQs13447471 = -440065600;    double DyJqTaNPbDVOLbsCkFnQs17086183 = -532376908;    double DyJqTaNPbDVOLbsCkFnQs35976298 = -715172665;    double DyJqTaNPbDVOLbsCkFnQs27991459 = -455784857;    double DyJqTaNPbDVOLbsCkFnQs83601917 = -926816322;    double DyJqTaNPbDVOLbsCkFnQs12543053 = -965483485;    double DyJqTaNPbDVOLbsCkFnQs44585100 = -17206839;    double DyJqTaNPbDVOLbsCkFnQs2775893 = -83342409;    double DyJqTaNPbDVOLbsCkFnQs6783784 = -300852478;    double DyJqTaNPbDVOLbsCkFnQs19007922 = -610325275;    double DyJqTaNPbDVOLbsCkFnQs33767322 = 93187561;    double DyJqTaNPbDVOLbsCkFnQs91565462 = -24142073;    double DyJqTaNPbDVOLbsCkFnQs7923298 = -406457351;    double DyJqTaNPbDVOLbsCkFnQs77933880 = -156350730;    double DyJqTaNPbDVOLbsCkFnQs98196381 = -579764043;    double DyJqTaNPbDVOLbsCkFnQs96906566 = -654273503;    double DyJqTaNPbDVOLbsCkFnQs12919698 = -656102406;    double DyJqTaNPbDVOLbsCkFnQs13913171 = -428996904;    double DyJqTaNPbDVOLbsCkFnQs85353057 = 64939758;    double DyJqTaNPbDVOLbsCkFnQs12794691 = 96174940;    double DyJqTaNPbDVOLbsCkFnQs15476989 = -551719759;    double DyJqTaNPbDVOLbsCkFnQs2182337 = -887636304;    double DyJqTaNPbDVOLbsCkFnQs7005334 = -447243160;    double DyJqTaNPbDVOLbsCkFnQs85263364 = -263429099;    double DyJqTaNPbDVOLbsCkFnQs85336092 = -560730764;    double DyJqTaNPbDVOLbsCkFnQs55325324 = -403053564;    double DyJqTaNPbDVOLbsCkFnQs61910817 = -21632281;    double DyJqTaNPbDVOLbsCkFnQs38410168 = -21312413;    double DyJqTaNPbDVOLbsCkFnQs28875164 = -43906665;    double DyJqTaNPbDVOLbsCkFnQs71356184 = 55243736;    double DyJqTaNPbDVOLbsCkFnQs98002287 = -857457720;    double DyJqTaNPbDVOLbsCkFnQs68583140 = -372844296;    double DyJqTaNPbDVOLbsCkFnQs23092476 = -375933851;    double DyJqTaNPbDVOLbsCkFnQs85182487 = -192271784;    double DyJqTaNPbDVOLbsCkFnQs61119011 = -834374644;    double DyJqTaNPbDVOLbsCkFnQs84171832 = -476816529;    double DyJqTaNPbDVOLbsCkFnQs96675820 = 26400759;    double DyJqTaNPbDVOLbsCkFnQs83316272 = -720188486;    double DyJqTaNPbDVOLbsCkFnQs26609083 = -831278755;    double DyJqTaNPbDVOLbsCkFnQs24533859 = -609752686;    double DyJqTaNPbDVOLbsCkFnQs11519187 = -647906128;    double DyJqTaNPbDVOLbsCkFnQs38209664 = -547911833;    double DyJqTaNPbDVOLbsCkFnQs90297890 = 41691643;    double DyJqTaNPbDVOLbsCkFnQs85638840 = -592282655;    double DyJqTaNPbDVOLbsCkFnQs88508854 = -444794908;    double DyJqTaNPbDVOLbsCkFnQs19376115 = -60857122;    double DyJqTaNPbDVOLbsCkFnQs65252000 = 12049113;    double DyJqTaNPbDVOLbsCkFnQs42708263 = -296379308;    double DyJqTaNPbDVOLbsCkFnQs63498653 = -338578575;    double DyJqTaNPbDVOLbsCkFnQs6883808 = -532808646;    double DyJqTaNPbDVOLbsCkFnQs32741291 = -185110792;    double DyJqTaNPbDVOLbsCkFnQs76648921 = -863604812;    double DyJqTaNPbDVOLbsCkFnQs44151707 = -361366554;    double DyJqTaNPbDVOLbsCkFnQs64834369 = -285209671;    double DyJqTaNPbDVOLbsCkFnQs12692300 = -91708845;    double DyJqTaNPbDVOLbsCkFnQs11265134 = -552429296;    double DyJqTaNPbDVOLbsCkFnQs10080850 = 14866251;    double DyJqTaNPbDVOLbsCkFnQs50712933 = -351743567;    double DyJqTaNPbDVOLbsCkFnQs42655366 = -895054093;    double DyJqTaNPbDVOLbsCkFnQs28276593 = -423762759;    double DyJqTaNPbDVOLbsCkFnQs50632236 = -843851205;    double DyJqTaNPbDVOLbsCkFnQs6174932 = -995894426;    double DyJqTaNPbDVOLbsCkFnQs73900729 = 60564256;    double DyJqTaNPbDVOLbsCkFnQs35427599 = -256096215;    double DyJqTaNPbDVOLbsCkFnQs21005634 = -752867555;    double DyJqTaNPbDVOLbsCkFnQs65184182 = -533968144;    double DyJqTaNPbDVOLbsCkFnQs68472987 = -648208223;    double DyJqTaNPbDVOLbsCkFnQs22740810 = -114185567;    double DyJqTaNPbDVOLbsCkFnQs16814869 = -321976086;    double DyJqTaNPbDVOLbsCkFnQs14024550 = -2947514;    double DyJqTaNPbDVOLbsCkFnQs230746 = -580674263;    double DyJqTaNPbDVOLbsCkFnQs29603426 = -935913920;    double DyJqTaNPbDVOLbsCkFnQs87304088 = -597718149;    double DyJqTaNPbDVOLbsCkFnQs60819199 = -325307557;    double DyJqTaNPbDVOLbsCkFnQs1275504 = -255918933;    double DyJqTaNPbDVOLbsCkFnQs77267324 = 96192074;    double DyJqTaNPbDVOLbsCkFnQs11884447 = -829327948;    double DyJqTaNPbDVOLbsCkFnQs21366493 = -854960505;    double DyJqTaNPbDVOLbsCkFnQs96754510 = -818634191;    double DyJqTaNPbDVOLbsCkFnQs65959978 = -9193436;     DyJqTaNPbDVOLbsCkFnQs1258016 = DyJqTaNPbDVOLbsCkFnQs32652119;     DyJqTaNPbDVOLbsCkFnQs32652119 = DyJqTaNPbDVOLbsCkFnQs11307731;     DyJqTaNPbDVOLbsCkFnQs11307731 = DyJqTaNPbDVOLbsCkFnQs10211000;     DyJqTaNPbDVOLbsCkFnQs10211000 = DyJqTaNPbDVOLbsCkFnQs37076911;     DyJqTaNPbDVOLbsCkFnQs37076911 = DyJqTaNPbDVOLbsCkFnQs56104287;     DyJqTaNPbDVOLbsCkFnQs56104287 = DyJqTaNPbDVOLbsCkFnQs40985557;     DyJqTaNPbDVOLbsCkFnQs40985557 = DyJqTaNPbDVOLbsCkFnQs97081673;     DyJqTaNPbDVOLbsCkFnQs97081673 = DyJqTaNPbDVOLbsCkFnQs4646763;     DyJqTaNPbDVOLbsCkFnQs4646763 = DyJqTaNPbDVOLbsCkFnQs22276176;     DyJqTaNPbDVOLbsCkFnQs22276176 = DyJqTaNPbDVOLbsCkFnQs10941578;     DyJqTaNPbDVOLbsCkFnQs10941578 = DyJqTaNPbDVOLbsCkFnQs73175298;     DyJqTaNPbDVOLbsCkFnQs73175298 = DyJqTaNPbDVOLbsCkFnQs20642143;     DyJqTaNPbDVOLbsCkFnQs20642143 = DyJqTaNPbDVOLbsCkFnQs61695035;     DyJqTaNPbDVOLbsCkFnQs61695035 = DyJqTaNPbDVOLbsCkFnQs3790374;     DyJqTaNPbDVOLbsCkFnQs3790374 = DyJqTaNPbDVOLbsCkFnQs45660989;     DyJqTaNPbDVOLbsCkFnQs45660989 = DyJqTaNPbDVOLbsCkFnQs90562091;     DyJqTaNPbDVOLbsCkFnQs90562091 = DyJqTaNPbDVOLbsCkFnQs29504765;     DyJqTaNPbDVOLbsCkFnQs29504765 = DyJqTaNPbDVOLbsCkFnQs77629060;     DyJqTaNPbDVOLbsCkFnQs77629060 = DyJqTaNPbDVOLbsCkFnQs28169288;     DyJqTaNPbDVOLbsCkFnQs28169288 = DyJqTaNPbDVOLbsCkFnQs13447471;     DyJqTaNPbDVOLbsCkFnQs13447471 = DyJqTaNPbDVOLbsCkFnQs17086183;     DyJqTaNPbDVOLbsCkFnQs17086183 = DyJqTaNPbDVOLbsCkFnQs35976298;     DyJqTaNPbDVOLbsCkFnQs35976298 = DyJqTaNPbDVOLbsCkFnQs27991459;     DyJqTaNPbDVOLbsCkFnQs27991459 = DyJqTaNPbDVOLbsCkFnQs83601917;     DyJqTaNPbDVOLbsCkFnQs83601917 = DyJqTaNPbDVOLbsCkFnQs12543053;     DyJqTaNPbDVOLbsCkFnQs12543053 = DyJqTaNPbDVOLbsCkFnQs44585100;     DyJqTaNPbDVOLbsCkFnQs44585100 = DyJqTaNPbDVOLbsCkFnQs2775893;     DyJqTaNPbDVOLbsCkFnQs2775893 = DyJqTaNPbDVOLbsCkFnQs6783784;     DyJqTaNPbDVOLbsCkFnQs6783784 = DyJqTaNPbDVOLbsCkFnQs19007922;     DyJqTaNPbDVOLbsCkFnQs19007922 = DyJqTaNPbDVOLbsCkFnQs33767322;     DyJqTaNPbDVOLbsCkFnQs33767322 = DyJqTaNPbDVOLbsCkFnQs91565462;     DyJqTaNPbDVOLbsCkFnQs91565462 = DyJqTaNPbDVOLbsCkFnQs7923298;     DyJqTaNPbDVOLbsCkFnQs7923298 = DyJqTaNPbDVOLbsCkFnQs77933880;     DyJqTaNPbDVOLbsCkFnQs77933880 = DyJqTaNPbDVOLbsCkFnQs98196381;     DyJqTaNPbDVOLbsCkFnQs98196381 = DyJqTaNPbDVOLbsCkFnQs96906566;     DyJqTaNPbDVOLbsCkFnQs96906566 = DyJqTaNPbDVOLbsCkFnQs12919698;     DyJqTaNPbDVOLbsCkFnQs12919698 = DyJqTaNPbDVOLbsCkFnQs13913171;     DyJqTaNPbDVOLbsCkFnQs13913171 = DyJqTaNPbDVOLbsCkFnQs85353057;     DyJqTaNPbDVOLbsCkFnQs85353057 = DyJqTaNPbDVOLbsCkFnQs12794691;     DyJqTaNPbDVOLbsCkFnQs12794691 = DyJqTaNPbDVOLbsCkFnQs15476989;     DyJqTaNPbDVOLbsCkFnQs15476989 = DyJqTaNPbDVOLbsCkFnQs2182337;     DyJqTaNPbDVOLbsCkFnQs2182337 = DyJqTaNPbDVOLbsCkFnQs7005334;     DyJqTaNPbDVOLbsCkFnQs7005334 = DyJqTaNPbDVOLbsCkFnQs85263364;     DyJqTaNPbDVOLbsCkFnQs85263364 = DyJqTaNPbDVOLbsCkFnQs85336092;     DyJqTaNPbDVOLbsCkFnQs85336092 = DyJqTaNPbDVOLbsCkFnQs55325324;     DyJqTaNPbDVOLbsCkFnQs55325324 = DyJqTaNPbDVOLbsCkFnQs61910817;     DyJqTaNPbDVOLbsCkFnQs61910817 = DyJqTaNPbDVOLbsCkFnQs38410168;     DyJqTaNPbDVOLbsCkFnQs38410168 = DyJqTaNPbDVOLbsCkFnQs28875164;     DyJqTaNPbDVOLbsCkFnQs28875164 = DyJqTaNPbDVOLbsCkFnQs71356184;     DyJqTaNPbDVOLbsCkFnQs71356184 = DyJqTaNPbDVOLbsCkFnQs98002287;     DyJqTaNPbDVOLbsCkFnQs98002287 = DyJqTaNPbDVOLbsCkFnQs68583140;     DyJqTaNPbDVOLbsCkFnQs68583140 = DyJqTaNPbDVOLbsCkFnQs23092476;     DyJqTaNPbDVOLbsCkFnQs23092476 = DyJqTaNPbDVOLbsCkFnQs85182487;     DyJqTaNPbDVOLbsCkFnQs85182487 = DyJqTaNPbDVOLbsCkFnQs61119011;     DyJqTaNPbDVOLbsCkFnQs61119011 = DyJqTaNPbDVOLbsCkFnQs84171832;     DyJqTaNPbDVOLbsCkFnQs84171832 = DyJqTaNPbDVOLbsCkFnQs96675820;     DyJqTaNPbDVOLbsCkFnQs96675820 = DyJqTaNPbDVOLbsCkFnQs83316272;     DyJqTaNPbDVOLbsCkFnQs83316272 = DyJqTaNPbDVOLbsCkFnQs26609083;     DyJqTaNPbDVOLbsCkFnQs26609083 = DyJqTaNPbDVOLbsCkFnQs24533859;     DyJqTaNPbDVOLbsCkFnQs24533859 = DyJqTaNPbDVOLbsCkFnQs11519187;     DyJqTaNPbDVOLbsCkFnQs11519187 = DyJqTaNPbDVOLbsCkFnQs38209664;     DyJqTaNPbDVOLbsCkFnQs38209664 = DyJqTaNPbDVOLbsCkFnQs90297890;     DyJqTaNPbDVOLbsCkFnQs90297890 = DyJqTaNPbDVOLbsCkFnQs85638840;     DyJqTaNPbDVOLbsCkFnQs85638840 = DyJqTaNPbDVOLbsCkFnQs88508854;     DyJqTaNPbDVOLbsCkFnQs88508854 = DyJqTaNPbDVOLbsCkFnQs19376115;     DyJqTaNPbDVOLbsCkFnQs19376115 = DyJqTaNPbDVOLbsCkFnQs65252000;     DyJqTaNPbDVOLbsCkFnQs65252000 = DyJqTaNPbDVOLbsCkFnQs42708263;     DyJqTaNPbDVOLbsCkFnQs42708263 = DyJqTaNPbDVOLbsCkFnQs63498653;     DyJqTaNPbDVOLbsCkFnQs63498653 = DyJqTaNPbDVOLbsCkFnQs6883808;     DyJqTaNPbDVOLbsCkFnQs6883808 = DyJqTaNPbDVOLbsCkFnQs32741291;     DyJqTaNPbDVOLbsCkFnQs32741291 = DyJqTaNPbDVOLbsCkFnQs76648921;     DyJqTaNPbDVOLbsCkFnQs76648921 = DyJqTaNPbDVOLbsCkFnQs44151707;     DyJqTaNPbDVOLbsCkFnQs44151707 = DyJqTaNPbDVOLbsCkFnQs64834369;     DyJqTaNPbDVOLbsCkFnQs64834369 = DyJqTaNPbDVOLbsCkFnQs12692300;     DyJqTaNPbDVOLbsCkFnQs12692300 = DyJqTaNPbDVOLbsCkFnQs11265134;     DyJqTaNPbDVOLbsCkFnQs11265134 = DyJqTaNPbDVOLbsCkFnQs10080850;     DyJqTaNPbDVOLbsCkFnQs10080850 = DyJqTaNPbDVOLbsCkFnQs50712933;     DyJqTaNPbDVOLbsCkFnQs50712933 = DyJqTaNPbDVOLbsCkFnQs42655366;     DyJqTaNPbDVOLbsCkFnQs42655366 = DyJqTaNPbDVOLbsCkFnQs28276593;     DyJqTaNPbDVOLbsCkFnQs28276593 = DyJqTaNPbDVOLbsCkFnQs50632236;     DyJqTaNPbDVOLbsCkFnQs50632236 = DyJqTaNPbDVOLbsCkFnQs6174932;     DyJqTaNPbDVOLbsCkFnQs6174932 = DyJqTaNPbDVOLbsCkFnQs73900729;     DyJqTaNPbDVOLbsCkFnQs73900729 = DyJqTaNPbDVOLbsCkFnQs35427599;     DyJqTaNPbDVOLbsCkFnQs35427599 = DyJqTaNPbDVOLbsCkFnQs21005634;     DyJqTaNPbDVOLbsCkFnQs21005634 = DyJqTaNPbDVOLbsCkFnQs65184182;     DyJqTaNPbDVOLbsCkFnQs65184182 = DyJqTaNPbDVOLbsCkFnQs68472987;     DyJqTaNPbDVOLbsCkFnQs68472987 = DyJqTaNPbDVOLbsCkFnQs22740810;     DyJqTaNPbDVOLbsCkFnQs22740810 = DyJqTaNPbDVOLbsCkFnQs16814869;     DyJqTaNPbDVOLbsCkFnQs16814869 = DyJqTaNPbDVOLbsCkFnQs14024550;     DyJqTaNPbDVOLbsCkFnQs14024550 = DyJqTaNPbDVOLbsCkFnQs230746;     DyJqTaNPbDVOLbsCkFnQs230746 = DyJqTaNPbDVOLbsCkFnQs29603426;     DyJqTaNPbDVOLbsCkFnQs29603426 = DyJqTaNPbDVOLbsCkFnQs87304088;     DyJqTaNPbDVOLbsCkFnQs87304088 = DyJqTaNPbDVOLbsCkFnQs60819199;     DyJqTaNPbDVOLbsCkFnQs60819199 = DyJqTaNPbDVOLbsCkFnQs1275504;     DyJqTaNPbDVOLbsCkFnQs1275504 = DyJqTaNPbDVOLbsCkFnQs77267324;     DyJqTaNPbDVOLbsCkFnQs77267324 = DyJqTaNPbDVOLbsCkFnQs11884447;     DyJqTaNPbDVOLbsCkFnQs11884447 = DyJqTaNPbDVOLbsCkFnQs21366493;     DyJqTaNPbDVOLbsCkFnQs21366493 = DyJqTaNPbDVOLbsCkFnQs96754510;     DyJqTaNPbDVOLbsCkFnQs96754510 = DyJqTaNPbDVOLbsCkFnQs65959978;     DyJqTaNPbDVOLbsCkFnQs65959978 = DyJqTaNPbDVOLbsCkFnQs1258016;}
// Junk Finished
