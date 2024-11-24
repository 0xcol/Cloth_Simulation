#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

// Particle������ģ��һ��ʵ���������࣬����λ�á������ͱ��ⲿ������ʱ�ı仯��
class Particle {
public:
    sf::Vector2f position;             // ʵ�嵱ǰ��λ��
    sf::Vector2f previous_position;   // ʵ����һ�θ���ʱ��λ��
    sf::Vector2f acceleration;        // ʵ�嵱ǰ�ܵ��Ķ������ٶ�
    bool is_pinned;                   // ��ʾʵ���Ƿ񱻹̶������Ϊtrue��λ�ò���仯

    // ���캯�������ڳ�ʼ��ʵ���λ�ú͹̶�״̬
    Particle(float x, float y, bool pinned = false) :
            position(x, y),
            previous_position(x, y),
            acceleration(0, 0),
            is_pinned(pinned) {}

    // Ӧ��һ�������٣�����ʵ��ļ��ٶ�
    void apply_force(const sf::Vector2f& force) {
        if (!is_pinned) {
            acceleration += force;  // ���ʵ��δ�̶�����������Ӧ�öԼ��ٶȵ�Ӱ��
        }
    }

    // ˢ��ʵ���λ�ã�ʹ��Verlet���ַ���
    void update(float time_step) {
        if (!is_pinned) {
            sf::Vector2f velocity = position - previous_position; // ����ʵ�嵱ǰ�ٶ�
            previous_position = position; // ����ǰλ�ñ���Ϊ��һ��λ��
            position += velocity + acceleration * time_step * time_step; // Verlet���ָ��·���
            acceleration = sf::Vector2f(0, 0); // ˢ�º����ü��ٶ�
        }
    }

    // ����ʵ����ָ���ľ��ν������
    void constrain_to_bounds(float width, float height) {
        if (position.x < 0) position.x = 0;            // ���ʵ������߽磬��λ�õ������߽�
        if (position.x > width) position.x = width;    // ����ұ߽�ʱ����
        if (position.y < 0) position.y = 0;            // ����ϱ߽�ʱ����
        if (position.y > height) position.y = height;  // ����±߽�ʱ����
    }
};

#endif // PARTICLE_H
