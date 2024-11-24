#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "particle.h"
#include "constraint.h"

// 鼠标点击的容差范围，用于判断是否点击到粒子
const float CLICK_TOLERANCE = 5.0f;

class InputHandler {
public:
    /**
     * 处理鼠标事件并相应地更新粒子和约束条件。
     *
     * @param event 当前的SFML鼠标事件
     * @param particles 模拟中的所有粒子
     * @param constraints 模拟中的所有约束
     * @param is_dragging 标记是否正在拖动粒子
     * @param dragged_particle 当前被拖动的粒子指针（如果有）
     * @param is_cutting 标记是否处于切割模式
     * @param cut_path 当前切割路径的顶点数组
     */
    static void handle_mouse_events(
            const sf::Event& event,
            std::vector<Particle>& particles,
            std::vector<Constraint>& constraints,
            bool& is_dragging,
            Particle*& dragged_particle,
            bool& is_cutting,
            sf::VertexArray& cut_path
    ) {
        // 处理鼠标按下事件
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // 左键点击开始拖动粒子
                float mouse_x = static_cast<float>(event.mouseButton.x);
                float mouse_y = static_cast<float>(event.mouseButton.y);
                start_drag(mouse_x, mouse_y, particles, is_dragging, dragged_particle);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // 右键点击开始切割
                is_cutting = true;
                cut_path.clear();  // 清空之前的切割路径
                add_cut_point(event.mouseButton.x, event.mouseButton.y, cut_path);
            }
        }
            // 处理鼠标释放事件
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // 左键释放停止拖动
                stop_drag(is_dragging, dragged_particle);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // 右键释放停止切割
                is_cutting = false;
            }
        }
            // 处理鼠标移动事件
        else if (event.type == sf::Event::MouseMoved) {
            // 更新拖动或切割状态
            if (is_dragging) {
                // 如果在拖动粒子，则更新粒子位置
                float mouse_x = static_cast<float>(event.mouseMove.x);
                float mouse_y = static_cast<float>(event.mouseMove.y);
                drag(mouse_x, mouse_y, dragged_particle);
            }
            else if (is_cutting) {
                // 如果在切割，则更新切割路径并执行切割
                add_cut_point(event.mouseMove.x, event.mouseMove.y, cut_path);
                cut(particles, constraints, cut_path);
            }
        }
    }

private:
    /**
     * 开始拖动粒子：检查鼠标点击是否在粒子附近，若是，则开始拖动。
     *
     * @param mouse_x 鼠标当前位置的x坐标
     * @param mouse_y 鼠标当前位置的y坐标
     * @param particles 所有粒子的列表
     * @param is_dragging 标记是否正在拖动
     * @param dragged_particle 被拖动的粒子指针
     */
    static void start_drag(float mouse_x, float mouse_y, std::vector<Particle>& particles,
                           bool& is_dragging, Particle*& dragged_particle) {
        for (auto& particle : particles) {
            // 判断鼠标是否点击到粒子
            float dx = particle.position.x - mouse_x;
            float dy = particle.position.y - mouse_y;
            if (std::sqrt(dx * dx + dy * dy) < CLICK_TOLERANCE) {
                is_dragging = true;
                dragged_particle = &particle;
                break;
            }
        }
    }

    /**
     * 停止拖动：设置拖动标记为false，释放粒子指针。
     *
     * @param is_dragging 拖动标记
     * @param dragged_particle 被拖动的粒子指针
     */
    static void stop_drag(bool& is_dragging, Particle*& dragged_particle) {
        is_dragging = false;
        dragged_particle = nullptr;
    }

    /**
     * 更新被拖动粒子的位置，使其跟随鼠标。
     *
     * @param mouse_x 鼠标当前位置的x坐标
     * @param mouse_y 鼠标当前位置的y坐标
     * @param dragged_particle 被拖动的粒子指针
     */
    static void drag(float mouse_x, float mouse_y, Particle* dragged_particle) {
        if (dragged_particle) {
            dragged_particle->position.x = mouse_x;
            dragged_particle->position.y = mouse_y;
        }
    }

    /**
     * 向切割路径添加新的点。
     *
     * @param mouse_x 鼠标当前位置的x坐标
     * @param mouse_y 鼠标当前位置的y坐标
     * @param cut_path 当前的切割路径
     */
    static void add_cut_point(int mouse_x, int mouse_y, sf::VertexArray& cut_path) {
        // 将当前鼠标位置添加到切割路径
        cut_path.append(sf::Vertex(sf::Vector2f(static_cast<float>(mouse_x), static_cast<float>(mouse_y)), sf::Color::White));
    }

    /**
     * 执行切割：检查切割路径与每个约束是否相交，如果相交则移除该约束。
     *
     * @param particles 粒子列表
     * @param constraints 约束列表
     * @param cut_path 当前的切割路径
     */
    static void cut(std::vector<Particle>& particles, std::vector<Constraint>& constraints,
                    const sf::VertexArray& cut_path) {
        // 遍历每个约束，检查是否与切割路径相交
        for (auto& constraint : constraints) {
            sf::Vector2f p1 = constraint.p1->position;
            sf::Vector2f p2 = constraint.p2->position;

            // 如果切割路径与约束相交，则删除该约束
            if (check_intersection(p1, p2, cut_path)) {
                constraint.active = false;  // 将约束标记为不活动（即已被切割）
            }
        }
    }

    /**
     * 检查两条线段是否相交。
     *
     * @param p1 约束的第一个粒子位置
     * @param p2 约束的第二个粒子位置
     * @param cut_path 当前的切割路径
     * @return 如果两条线段相交则返回true，否则返回false
     */
    static bool check_intersection(const sf::Vector2f& p1, const sf::Vector2f& p2,
                                   const sf::VertexArray& cut_path) {
        // 遍历切割路径中的每个线段，检查是否与约束线段相交
        for (size_t i = 0; i < cut_path.getVertexCount() - 1; ++i) {
            sf::Vector2f cut_start = cut_path[i].position;
            sf::Vector2f cut_end = cut_path[i + 1].position;
            if (lines_intersect(p1, p2, cut_start, cut_end)) {
                return true;  // 如果相交，返回true
            }
        }
        return false;  // 如果没有相交，返回false
    }

    /**
     * 使用二维线段相交算法检查两条线段是否相交。
     *
     * @param p1 约束的第一个粒子位置
     * @param p2 约束的第二个粒子位置
     * @param q1 切割路径的起点
     * @param q2 切割路径的终点
     * @return 如果线段相交则返回true，否则返回false
     */
    static bool lines_intersect(const sf::Vector2f& p1, const sf::Vector2f& p2,
                                const sf::Vector2f& q1, const sf::Vector2f& q2) {
        // 计算行列式，判断线段是否相交
        float d1 = (p2.x - p1.x) * (q1.y - p1.y) - (p2.y - p1.y) * (q1.x - p1.x);
        float d2 = (p2.x - p1.x) * (q2.y - p1.y) - (p2.y - p1.y) * (q2.x - p1.x);
        float d3 = (q2.x - q1.x) * (p1.y - q1.y) - (q2.y - q1.y) * (p1.x - q1.x);
        float d4 = (q2.x - q1.x) * (p2.y - q1.y) - (q2.y - q1.y) * (p2.x - q1.x);

        return d1 * d2 < 0 && d3 * d4 < 0;  // 判断是否存在交点
    }
};

#endif  // INPUT_HANDLER_H
