#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "particle.h"         // 粒子类的定义和实现
#include "constraint.h"       // 约束类的定义和实现
#include "input_handler.h"    // 输入处理逻辑

// 配置常量
const int WIDTH = 1000;       // 窗口宽度
const int HEIGHT = 800;       // 窗口高度
const float PARTICLE_RADIUS = 10.0f;  // 粒子的半径，用于显示
const float GRAVITY = 10.0f;  // 重力加速度
const float TIME_STEP = 0.131f; // 时间步长，控制模拟的张力

const int ROW = 140;          // 布料网格的行数
const int COL = 140;          // 布料网格的列数
const float REST_DISTANCE = 4.0f; // 粒子之间的初始间隔

int main() {
    // 创建SFML窗口
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cloth Simulation");
    window.setFramerateLimit(60); // 限制帧率为60

    // 粒子和约束的容器
    std::vector<Particle> particles;  // 粒子数组
    std::vector<Constraint> constraints;  // 约束数组

    // 初始化粒子
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            // 计算粒子的初始位置
            float x = col * REST_DISTANCE + WIDTH / 3.8;
            float y = row * REST_DISTANCE + HEIGHT / 8;

            // 是否固定粒子（网格边缘的粒子固定）
            bool pinned = (row == ROW - 1 || row == 0 || col == 0 || col == COL - 1);

            // 创建粒子并加入容器
            particles.emplace_back(x, y, pinned);
        }
    }

    // 初始化约束
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            // 水平约束：连接当前粒子和右边的粒子
            if (col < COL - 1) {
                constraints.emplace_back(&particles[row * COL + col], &particles[row * COL + col + 1]);
            }
            // 垂直约束：连接当前粒子和下方的粒子
            if (row < ROW - 1) {
                constraints.emplace_back(&particles[row * COL + col], &particles[(row + 1) * COL + col]);
            }
        }
    }

    // 鼠标拖拽和切割功能的状态变量
    bool is_dragging = false;           // 是否正在拖拽粒子
    Particle* dragged_particle = nullptr; // 当前被拖拽的粒子
    bool is_cutting = false;           // 是否正在进行切割操作
    sf::VertexArray cut_path(sf::LinesStrip);  // 记录切割路径

    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // 关闭窗口
            }

            // 处理鼠标事件（左键拖拽，右键切割）
            InputHandler::handle_mouse_events(event, particles, constraints, is_dragging, dragged_particle,
                                              is_cutting, cut_path);
        }

        // 更新粒子状态
        for (auto& particle : particles) {
            // 应用重力
            particle.apply_force(sf::Vector2f(-20, GRAVITY));

            // 更新粒子位置
            particle.update(TIME_STEP);

            // 约束粒子在窗口边界内
            particle.constrain_to_bounds(WIDTH, HEIGHT);
        }

        // 多次满足约束（模拟张力）
        for (size_t i = 0; i < 5; i++) {
            for (auto& constraint : constraints) {
                constraint.satisfy(); // 调整粒子位置满足约束
            }
        }

        // 绘制部分
        window.clear(sf::Color::Black); // 清空窗口，设置背景为黑色

        // 绘制粒子
        for (const auto& particle : particles) {
            sf::Vertex point(particle.position, sf::Color::White);
            window.draw(&point, 1, sf::Points);
        }

        // 绘制约束（连线）
        for (const auto& constraint : constraints) {
            if (!constraint.active) {
                continue; // 跳过非激活状态的约束
            }
            sf::Vertex line[] = {
                    sf::Vertex(constraint.p1->position, sf::Color::White),
                    sf::Vertex(constraint.p2->position, sf::Color::White),
            };
            window.draw(line, 2, sf::Lines);
        }

        // 绘制切割路径
        window.draw(cut_path);

        // 显示窗口内容
        window.display();
    }

    return 0;
}
