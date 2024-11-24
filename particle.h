#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

// Particle类用于模拟一个实体类描述类，包括位置、动量和被外部力作用时的变化。
class Particle {
public:
    sf::Vector2f position;             // 实体当前的位置
    sf::Vector2f previous_position;   // 实体上一次更新时的位置
    sf::Vector2f acceleration;        // 实体当前受到的动量加速度
    bool is_pinned;                   // 表示实体是否被固定，如果为true，位置不会变化

    // 构造函数，用于初始化实体的位置和固定状态
    Particle(float x, float y, bool pinned = false) :
            position(x, y),
            previous_position(x, y),
            acceleration(0, 0),
            is_pinned(pinned) {}

    // 应用一个力场举，增加实体的加速度
    void apply_force(const sf::Vector2f& force) {
        if (!is_pinned) {
            acceleration += force;  // 如果实体未固定，增加力场应用对加速度的影响
        }
    }

    // 刷新实体的位置（使用Verlet积分法）
    void update(float time_step) {
        if (!is_pinned) {
            sf::Vector2f velocity = position - previous_position; // 计算实体当前速度
            previous_position = position; // 将当前位置保存为上一次位置
            position += velocity + acceleration * time_step * time_step; // Verlet积分更新方程
            acceleration = sf::Vector2f(0, 0); // 刷新后重置加速度
        }
    }

    // 限制实体在指定的矩形进与进出
    void constrain_to_bounds(float width, float height) {
        if (position.x < 0) position.x = 0;            // 如果实体跨过左边界，将位置调整到边界
        if (position.x > width) position.x = width;    // 跨过右边界时调整
        if (position.y < 0) position.y = 0;            // 跨过上边界时调整
        if (position.y > height) position.y = height;  // 跨过下边界时调整
    }
};

#endif // PARTICLE_H
