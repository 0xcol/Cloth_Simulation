#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "particle.h"
#include <cmath>
#include <limits>

// Constraint������������������֮��ľ��룬�������ȶ��ԡ�
class Constraint {
public:
    Particle *p1;               // ���ӵĵ�һ������
    Particle *p2;               // ���ӵĵڶ�������
    float initial_length;       // ��ʼ���ȣ����ڱ��������Ӽ�Ĺ̶�����
    bool active;                // Լ���Ƿ񼤻��Ϊfalse��Լ��ʧЧ

    // ���캯������ʼ���������Ӽ��Լ��
    Constraint(Particle *p1, Particle *p2) : p1(p1), p2(p2), active(true) {
        // ����������֮��ĳ�ʼ����
        initial_length = std::hypot(p2->position.x - p1->position.x,
                                    p2->position.y - p1->position.y);
    }

    // ����������֮��ľ�����ڳ�ʼ����
    void satisfy() {
        if (!active) return; // ��Լ��δ���ֱ�ӷ���

        // ������������֮��ĵ�ǰ����ʸ��
        sf::Vector2f delta = p2->position - p1->position;
        float current_length = std::hypot(delta.x, delta.y); // ��ǰ����

        // ����������
        if (current_length < std::numeric_limits<float>::epsilon()) {
            return;
        }

        // ���㵱ǰ�������ʼ���ȵĲ�ֵ����
        float difference = (current_length - initial_length) / current_length;

        // ������Ҫ������λ��ʸ��
        sf::Vector2f correction = delta * 0.5f * difference;

        // ������δ���̶���������λ��
        if (!p1->is_pinned) p1->position += correction;
        if (!p2->is_pinned) p2->position -= correction;
    }

    // ����Լ��
    void deactivate() {
        active = false;
    }
};

#endif // CONSTRAINT_H
