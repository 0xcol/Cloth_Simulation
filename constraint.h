#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "particle.h"
#include <cmath>
#include <limits>

// Constraint类用于限制两个粒子之间的距离，保持其稳定性。
class Constraint {
public:
    Particle *p1;               // 连接的第一个粒子
    Particle *p2;               // 连接的第二个粒子
    float initial_length;       // 初始长度，用于保持两粒子间的固定距离
    bool active;                // 约束是否激活，若为false则约束失效

    // 构造函数，初始化两个粒子间的约束
    Constraint(Particle *p1, Particle *p2) : p1(p1), p2(p2), active(true) {
        // 计算两粒子之间的初始距离
        initial_length = std::hypot(p2->position.x - p1->position.x,
                                    p2->position.y - p1->position.y);
    }

    // 保持两粒子之间的距离等于初始长度
    void satisfy() {
        if (!active) return; // 若约束未激活，直接返回

        // 计算两个粒子之间的当前距离矢量
        sf::Vector2f delta = p2->position - p1->position;
        float current_length = std::hypot(delta.x, delta.y); // 当前距离

        // 避免除零错误
        if (current_length < std::numeric_limits<float>::epsilon()) {
            return;
        }

        // 计算当前长度与初始长度的差值比例
        float difference = (current_length - initial_length) / current_length;

        // 计算需要调整的位置矢量
        sf::Vector2f correction = delta * 0.5f * difference;

        // 若粒子未被固定，调整其位置
        if (!p1->is_pinned) p1->position += correction;
        if (!p2->is_pinned) p2->position -= correction;
    }

    // 禁用约束
    void deactivate() {
        active = false;
    }
};

#endif // CONSTRAINT_H
