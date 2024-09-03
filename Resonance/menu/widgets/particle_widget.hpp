#pragma once

#include "../libs/imgui/imgui.h"
#include <vector>
#include <random>
#include <iostream>

inline float calc_dist(const ImVec2 &pos1, const ImVec2 &pos2) {
    return std::sqrt(
        std::pow(pos1.x - pos2.x, 2) +
        std::pow(pos1.y - pos2.y, 2)
    );
}

template<bool layer_over = false>
class particle_widget {
public:
    std::uint64_t n_circles{};
    ImVec2 pos, sz;
    float rounding{}, circle_radius{};
    ImColor backfill{}, main_color{};
    std::vector<ImVec2> circle_pos;
    int ticker{0}; // once every 30
    std::vector<std::pair<bool, bool> > circle_dir; // amazing opt opportunities here

    particle_widget() = default;

    particle_widget(const ImVec2 &_pos, const ImVec2 &_sz, std::uint64_t _ncircles, float _rounding,
                    float _circleradius, ImColor _backfill, ImColor _maincolor)
        : pos{_pos}, sz{_sz}, n_circles{_ncircles}, rounding{_rounding}, circle_radius{_circleradius},
          backfill{_backfill}, main_color{_maincolor} {
    }

    void setup_circles() {
        const auto min_x = pos.x;
        const auto min_y = pos.y;
        const auto max_x = min_x + sz.x;
        const auto max_y = min_y + sz.y;
        std::random_device rd{};
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> direction(0, 9), pos_y(min_y, max_y), pos_x(min_x, max_x);
        for (std::size_t i = 0; i < n_circles; i++) {
            circle_pos.emplace_back(pos_x(gen), pos_y(gen));
            circle_dir.emplace_back(direction(gen) > 4, direction(gen) > 4);
        }
    }

    void draw_circle(const ImVec2 &loc, const ImColor &color) {
        ImGui::GetWindowDrawList()->AddCircleFilled(loc, circle_radius - 0.8f, color);
    }

    void draw_line(const ImVec2 &pos1, const ImVec2 &pos2, const ImColor &col, float thickness) {
        ImGui::GetWindowDrawList()->AddLine(pos1, pos2, col, thickness);
    }

    void draw_background() {
        ImGui::GetWindowDrawList()->AddRectFilled({pos.x, pos.y}, {pos.x + sz.x, pos.y + sz.y}, backfill, rounding);
    }

    void move_widget(const int idx) {
        const auto new_y = 35 + 146 + (36 * idx);
        pos.y = new_y;
        circle_pos.clear();
        circle_dir.clear();
        setup_circles();
    }

    void move_circles() {
        if (ticker < 4) {
            ticker++;
            return;
        }
        ticker = 0;

        const auto min_x = pos.x;
        const auto min_y = pos.y;
        const auto max_x = min_x + sz.x;
        const auto max_y = min_y + sz.y;
        std::random_device rd{};
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> off_y(-2, -1), off_x(-2, -1);
        int idx = 0;
        for (auto &c: circle_pos) {
            const auto next_x = c.x + (circle_dir[idx].first ? std::abs(off_x(gen)) : off_x(gen));
            if (next_x > max_x - 2 || next_x < min_x + 2)
                circle_dir[idx].first = !circle_dir[idx].first;
            else
                c.x = next_x;

            const auto next_y = c.y + (circle_dir[idx].second ? std::abs(off_y(gen)) : off_y(gen));
            if (next_y > max_y - 2 || next_y < min_y + 2)
                circle_dir[idx].second = !circle_dir[idx].second;
            else
                c.y = next_y;

            idx++;
        }
    }

    void render() {
        move_circles();
        if constexpr (!layer_over)
            draw_background();
        for (std::size_t i = 0; i < n_circles; i++) {
            const auto &c = circle_pos[i];
            draw_circle(c, main_color);
            for (std::size_t j = i + 1; j < n_circles; j++) {
                const auto &d = circle_pos[j];

                if (const auto dist = calc_dist(c, d); dist < 75) {
                    const auto multiplier = std::abs((dist - 75)) / 75;
                    const auto col = main_color.Value;
                    draw_line(c, d, {col.x, col.y, col.z, multiplier}, 1);
                }
            }
        }
        if constexpr (layer_over)
            draw_background();
    }
};
