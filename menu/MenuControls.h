#pragma once
#include "Menu.h"
#include "../vsonyp0wer/vsonyp0wer.h"

vsonyp0werStyle* style = &vsonyp0wer::GetStyle();

// =====================================
// - Custom controls
// =====================================

#define InsertSpacer(x1) style->Colors[vsonyp0werCol_ChildBg] = ImColor(0, 0, 0, 0); vsonyp0wer::BeginChild(x1, ImVec2(210.f, 18.f), false); {} vsonyp0wer::EndChild(); style->Colors[vsonyp0werCol_ChildBg] = ImColor(49, 49, 49, 255);
#define InsertGroupboxSpacer(x1) style->Colors[vsonyp0werCol_ChildBg] = ImColor(0, 0, 0, 0); vsonyp0wer::BeginChild(x1, ImVec2(210.f, 9.f), false); {} vsonyp0wer::EndChild(); style->Colors[vsonyp0werCol_ChildBg] = ImColor(49, 49, 49, 255);
#define InsertGroupboxTitle(x1) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(11.f); vsonyp0wer::GroupBoxTitle(x1);

#define InsertGroupBoxLeft(x1,x2) vsonyp0wer::NewLine(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBox(x1, ImVec2(258.f, x2), true);
#define InsertGroupBoxCentered(x1,x2) vsonyp0wer::NewLine(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBox(x1, ImVec2(535.f, x2), true);
#define InsertGroupBoxRight(x1,x2) vsonyp0wer::NewLine(); vsonyp0wer::SameLine(10.f); vsonyp0wer::BeginGroupBox(x1, ImVec2(258.f, x2), true);
#define InsertEndGroupBoxLeft(x1,x2) vsonyp0wer::EndGroupBox(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBoxScroll(x1, x2, ImVec2(258.f, 11.f), true); vsonyp0wer::EndGroupBoxScroll();
#define InsertEndGroupBoxCentered(x1,x2) vsonyp0wer::EndGroupBox(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBoxScroll(x1, x2, ImVec2(526.f, 11.f), true); vsonyp0wer::EndGroupBoxScroll();
#define InsertEndGroupBoxRight(x1,x2) vsonyp0wer::EndGroupBox(); vsonyp0wer::SameLine(10.f); vsonyp0wer::BeginGroupBoxScroll(x1, x2, ImVec2(258.f, 11.f), true); vsonyp0wer::EndGroupBoxScroll();

#define InsertGroupBoxTop(x1,x2) vsonyp0wer::NewLine(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBox(x1, x2, true);
#define InsertEndGroupBoxTop(x1,x2,x3) vsonyp0wer::EndGroupBox(); vsonyp0wer::SameLine(19.f); vsonyp0wer::BeginGroupBoxScroll(x1, x2, x3, true); vsonyp0wer::EndGroupBoxScroll();

// =====================================
// - Default controls
// =====================================

#define InsertCheckbox(x1,x2) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(19.f); vsonyp0wer::Checkbox(x1, &x2);
#define InsertButton(x1) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::Button(x1);
//#define InsertButton(x1,x2) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f);vsonyp0wer:Button(x1, &x2);// vsonyp0wer::PopItemWidth(); vsonyp0wer::CustomSpacing(1.f);
#define InsertSlider(x1,x2,x3,x4,x5) vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f); vsonyp0wer::SliderFloat(x1, &x2, x3, x4, x5); vsonyp0wer::PopItemWidth();// was 159
#define InsertSliderWithoutText(x1,x2,x3,x4,x5) vsonyp0wer::Spacing(); vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(159.f); vsonyp0wer::SliderFloat(x1, &x2, x3, x4, x5); vsonyp0wer::PopItemWidth();
#define InsertCombo(x1,x2,x3) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f); vsonyp0wer::Combo(x1, &x2, x3, IM_ARRAYSIZE(x3)); vsonyp0wer::PopItemWidth(); vsonyp0wer::CustomSpacing(1.f);
#define InsertComboWithoutText(x1,x2,x3) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f); vsonyp0wer::Combo(x1, &x2, x3, IM_ARRAYSIZE(x3)); vsonyp0wer::PopItemWidth(); vsonyp0wer::CustomSpacing(1.f);
#define InsertMultiCombo(x1,x2,x3,x4) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f); vsonyp0wer::MultiCombo(x1, x2, x3, x4); vsonyp0wer::PopItemWidth(); vsonyp0wer::CustomSpacing(1.f);
#define InsertMultiComboWithoutText(x1,x2,x3,x4) vsonyp0wer::Spacing(); vsonyp0wer::NewLine(); vsonyp0wer::SameLine(42.f); vsonyp0wer::PushItemWidth(158.f); vsonyp0wer::MultiCombo(x1, x2, x3, x4); vsonyp0wer::PopItemWidth(); vsonyp0wer::CustomSpacing(1.f);

#define InsertColorPicker(x1,x2,x3) vsonyp0wer::SameLine(219.f); Menu::ColorPicker(x1, x2, x3);
