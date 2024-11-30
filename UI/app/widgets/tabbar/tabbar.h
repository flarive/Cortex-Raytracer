#pragma once

#include "../../../imgui.h"
#include "../../../imgui_internal.h"


const ImVec4 gray(0.882f, 0.882f, 0.882f, 1.0f);
const ImVec4 blue(0.149f, 0.502f, 0.922f, 1.0f);
const ImVec4 white(1.0f, 1.0f, 1.0f, 1.0f);



template<class T>
const T& maxf(const T& a, const T& b)
{
    return (a < b) ? b : a;
}


bool BeginTabBar(const char* str_id, ImGuiTabBarFlags flags) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 10, 17 });
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.0705f, 0.0705f, 0.0705f, 1.0f));
    bool begin = ImGui::BeginTabBar(str_id, flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    return begin;
}



bool BeginTabItem(const char* label, ImGuiTabItemFlags flags) {
    auto id = ImGui::GetID(label);
    bool isHovered = ImGui::GetCurrentContext()->HoveredIdPreviousFrame == id;
    bool isSelected = false;

    if (auto tabBar = ImGui::GetCurrentTabBar()) {
        isSelected = tabBar->SelectedTabId == id;
    }

    auto drawUnderline = [&](const char* label) {
        if (isSelected) {
            ImGuiContext& g = *GImGui;
            //ImGuiWindow* window = g.CurrentWindow;
            ImGuiTabBar* tab_bar = g.CurrentTabBar;
            auto bbMin = ImGui::GetItemRectMin();
            auto bbMax = ImGui::GetItemRectMax();
            ImRect bb = { bbMin,bbMax };
            ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
            ImRect text_pixel_clip_bb(bb.Min.x + tab_bar->FramePadding.x, bb.Min.y + tab_bar->FramePadding.y, bb.Max.x - tab_bar->FramePadding.x, bb.Max.y);
            if (label_size.x > text_pixel_clip_bb.GetWidth()) {
                label_size.x = text_pixel_clip_bb.GetWidth();
            }
            label_size.x = maxf(0.0f, label_size.x);
            if (label_size.x > 0) {
                auto xMin = bb.GetCenter().x - label_size.x * 0.5f;
                auto xMax = bb.GetCenter().x + label_size.x * 0.5f;
                auto yMin = bb.Max.y - 12;
                //display_draw_list->AddRectFilled(bb.Min, bb.Max, ImGui::ColorConvertFloat4ToU32({1.0f,0.0f,0.0f,0.5f})); //Debug
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(xMin, yMin), { xMax,yMin + 3 }, ImGui::GetColorU32(ImGuiCol_Text));
            }
        }
        };

    ImVec4 col = (isSelected ? ImVec4(0.2f, 0.6117f, 1.f, 1.f) : ImVec4(0.4117f, 0.4117f, 0.4117f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Text, col);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12,0 });
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10, 17 });
    ImGui::PushStyleColor(ImGuiCol_TabHovered, isSelected ? ImGui::ColorConvertFloat4ToU32({ 0.2f, 0.612f, 1.0f, 1.0f }) : ImGui::GetColorU32(ImGuiCol_TabHovered));

    //Make background transparent,
    ImGui::PushStyleColor(ImGuiCol_TabHovered, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_Tab, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_TabActive, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_Text, (isHovered && !isSelected) ? blue : ImGui::GetStyleColorVec4(ImGuiCol_Text)); // aaaaaaaaaaa
    bool begin = ImGui::BeginTabItem(label, nullptr, flags);
    ImGui::PopStyleColor(4);

    drawUnderline(label);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    return begin;
}

