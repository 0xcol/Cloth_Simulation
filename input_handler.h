#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "particle.h"
#include "constraint.h"

// ��������ݲΧ�������ж��Ƿ���������
const float CLICK_TOLERANCE = 5.0f;

class InputHandler {
public:
    /**
     * ��������¼�����Ӧ�ظ������Ӻ�Լ��������
     *
     * @param event ��ǰ��SFML����¼�
     * @param particles ģ���е���������
     * @param constraints ģ���е�����Լ��
     * @param is_dragging ����Ƿ������϶�����
     * @param dragged_particle ��ǰ���϶�������ָ�루����У�
     * @param is_cutting ����Ƿ����и�ģʽ
     * @param cut_path ��ǰ�и�·���Ķ�������
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
        // ������갴���¼�
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // ��������ʼ�϶�����
                float mouse_x = static_cast<float>(event.mouseButton.x);
                float mouse_y = static_cast<float>(event.mouseButton.y);
                start_drag(mouse_x, mouse_y, particles, is_dragging, dragged_particle);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // �Ҽ������ʼ�и�
                is_cutting = true;
                cut_path.clear();  // ���֮ǰ���и�·��
                add_cut_point(event.mouseButton.x, event.mouseButton.y, cut_path);
            }
        }
            // ��������ͷ��¼�
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // ����ͷ�ֹͣ�϶�
                stop_drag(is_dragging, dragged_particle);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // �Ҽ��ͷ�ֹͣ�и�
                is_cutting = false;
            }
        }
            // ��������ƶ��¼�
        else if (event.type == sf::Event::MouseMoved) {
            // �����϶����и�״̬
            if (is_dragging) {
                // ������϶����ӣ����������λ��
                float mouse_x = static_cast<float>(event.mouseMove.x);
                float mouse_y = static_cast<float>(event.mouseMove.y);
                drag(mouse_x, mouse_y, dragged_particle);
            }
            else if (is_cutting) {
                // ������и������и�·����ִ���и�
                add_cut_point(event.mouseMove.x, event.mouseMove.y, cut_path);
                cut(particles, constraints, cut_path);
            }
        }
    }

private:
    /**
     * ��ʼ�϶����ӣ����������Ƿ������Ӹ��������ǣ���ʼ�϶���
     *
     * @param mouse_x ��굱ǰλ�õ�x����
     * @param mouse_y ��굱ǰλ�õ�y����
     * @param particles �������ӵ��б�
     * @param is_dragging ����Ƿ������϶�
     * @param dragged_particle ���϶�������ָ��
     */
    static void start_drag(float mouse_x, float mouse_y, std::vector<Particle>& particles,
                           bool& is_dragging, Particle*& dragged_particle) {
        for (auto& particle : particles) {
            // �ж�����Ƿ���������
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
     * ֹͣ�϶��������϶����Ϊfalse���ͷ�����ָ�롣
     *
     * @param is_dragging �϶����
     * @param dragged_particle ���϶�������ָ��
     */
    static void stop_drag(bool& is_dragging, Particle*& dragged_particle) {
        is_dragging = false;
        dragged_particle = nullptr;
    }

    /**
     * ���±��϶����ӵ�λ�ã�ʹ�������ꡣ
     *
     * @param mouse_x ��굱ǰλ�õ�x����
     * @param mouse_y ��굱ǰλ�õ�y����
     * @param dragged_particle ���϶�������ָ��
     */
    static void drag(float mouse_x, float mouse_y, Particle* dragged_particle) {
        if (dragged_particle) {
            dragged_particle->position.x = mouse_x;
            dragged_particle->position.y = mouse_y;
        }
    }

    /**
     * ���и�·������µĵ㡣
     *
     * @param mouse_x ��굱ǰλ�õ�x����
     * @param mouse_y ��굱ǰλ�õ�y����
     * @param cut_path ��ǰ���и�·��
     */
    static void add_cut_point(int mouse_x, int mouse_y, sf::VertexArray& cut_path) {
        // ����ǰ���λ����ӵ��и�·��
        cut_path.append(sf::Vertex(sf::Vector2f(static_cast<float>(mouse_x), static_cast<float>(mouse_y)), sf::Color::White));
    }

    /**
     * ִ���и����и�·����ÿ��Լ���Ƿ��ཻ������ཻ���Ƴ���Լ����
     *
     * @param particles �����б�
     * @param constraints Լ���б�
     * @param cut_path ��ǰ���и�·��
     */
    static void cut(std::vector<Particle>& particles, std::vector<Constraint>& constraints,
                    const sf::VertexArray& cut_path) {
        // ����ÿ��Լ��������Ƿ����и�·���ཻ
        for (auto& constraint : constraints) {
            sf::Vector2f p1 = constraint.p1->position;
            sf::Vector2f p2 = constraint.p2->position;

            // ����и�·����Լ���ཻ����ɾ����Լ��
            if (check_intersection(p1, p2, cut_path)) {
                constraint.active = false;  // ��Լ�����Ϊ��������ѱ��и
            }
        }
    }

    /**
     * ��������߶��Ƿ��ཻ��
     *
     * @param p1 Լ���ĵ�һ������λ��
     * @param p2 Լ���ĵڶ�������λ��
     * @param cut_path ��ǰ���и�·��
     * @return ��������߶��ཻ�򷵻�true�����򷵻�false
     */
    static bool check_intersection(const sf::Vector2f& p1, const sf::Vector2f& p2,
                                   const sf::VertexArray& cut_path) {
        // �����и�·���е�ÿ���߶Σ�����Ƿ���Լ���߶��ཻ
        for (size_t i = 0; i < cut_path.getVertexCount() - 1; ++i) {
            sf::Vector2f cut_start = cut_path[i].position;
            sf::Vector2f cut_end = cut_path[i + 1].position;
            if (lines_intersect(p1, p2, cut_start, cut_end)) {
                return true;  // ����ཻ������true
            }
        }
        return false;  // ���û���ཻ������false
    }

    /**
     * ʹ�ö�ά�߶��ཻ�㷨��������߶��Ƿ��ཻ��
     *
     * @param p1 Լ���ĵ�һ������λ��
     * @param p2 Լ���ĵڶ�������λ��
     * @param q1 �и�·�������
     * @param q2 �и�·�����յ�
     * @return ����߶��ཻ�򷵻�true�����򷵻�false
     */
    static bool lines_intersect(const sf::Vector2f& p1, const sf::Vector2f& p2,
                                const sf::Vector2f& q1, const sf::Vector2f& q2) {
        // ��������ʽ���ж��߶��Ƿ��ཻ
        float d1 = (p2.x - p1.x) * (q1.y - p1.y) - (p2.y - p1.y) * (q1.x - p1.x);
        float d2 = (p2.x - p1.x) * (q2.y - p1.y) - (p2.y - p1.y) * (q2.x - p1.x);
        float d3 = (q2.x - q1.x) * (p1.y - q1.y) - (q2.y - q1.y) * (p1.x - q1.x);
        float d4 = (q2.x - q1.x) * (p2.y - q1.y) - (q2.y - q1.y) * (p2.x - q1.x);

        return d1 * d2 < 0 && d3 * d4 < 0;  // �ж��Ƿ���ڽ���
    }
};

#endif  // INPUT_HANDLER_H
