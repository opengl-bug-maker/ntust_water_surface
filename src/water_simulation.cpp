#include "water_simulation.h"
#include "TrainWindow.h"

water_simulation::water_simulation(int count, TrainWindow* tw) {
    this->tw = tw;
    this->count = count;
    surfaces = new surface* [count + 2];
    for (int i = 0; i < count + 2; ++i) {
        surfaces[i] = new surface[count + 2];
    }
}

void water_simulation::tickBack() {
    for (int i = 0; i < count + 2; i++) {
        for (int j = 1; j < count + 2; j++) {
            surfaces[i][j].t2Height = surfaces[i][j].t1Height;
            surfaces[i][j].t1Height = surfaces[i][j].height;
        }
    }
}

void water_simulation::fresh() {
    for (int i = 1; i <= count; ++i) {
        for (int j = 1; j <= count; ++j) {
            float l = surfaces[i - 1][j].t1Height;
            float r = surfaces[i + 1][j].t1Height;
            float u = surfaces[i][j + 1].t1Height;
            float d = surfaces[i][j - 1].t1Height;

            // float slowdown = 0.7;
            // float speed = 0.8;
            float tickLength = 1;

            // surfaces[i][j].height = (1 + slowdown) * surfaces[i][j].t1Height -
            //     slowdown * surfaces[i][j].t2Height +

            //     0.5f * speed * speed * (
            //         l + r + u + d - 4 * surfaces[i][j].t1Height
            //         ) * tickLength * tickLength;
            surfaces[i][j].height = (1 + tw->SlowDown->value()) * surfaces[i][j].t1Height -
                tw->SlowDown->value() * surfaces[i][j].t2Height +

                0.5f * tw->Speed->value() * tw->Speed->value() * (
                    l + r + u + d - 4 * surfaces[i][j].t1Height
                    ) * tickLength * tickLength;
        }
    }
}
void water_simulation::get_matrix(std::vector<triangle_t>& triangles) {
    static const float min_x = -10.0f, max_x = 10.0f;
    static const float min_y = -10.0f, max_y = 10.0f;
    const float step_x = (max_x - min_x) / this->count,
        step_y = (max_y - min_y) / this->count;

    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count; ++j) {
            triangle_t t;
            t.vertices.push_back(glm::vec3(min_x + i * step_x, baisHeight + surfaces[i][j].height, min_y + j * step_y));
            t.vertices.push_back(glm::vec3(min_x + (i + 1) * step_x, baisHeight + surfaces[i + 1][j].height, min_y + j * step_y));
            t.vertices.push_back(glm::vec3(min_x + i * step_x, baisHeight + surfaces[i][j + 1].height, min_y + (j + 1) * step_y));
            triangles.push_back(t);
            t.vertices.clear();

            t.vertices.push_back(glm::vec3(min_x + (i + 1) * step_x, baisHeight + surfaces[i + 1][j].height, min_y + j * step_y));
            t.vertices.push_back(glm::vec3(min_x + (i + 1) * step_x, baisHeight + surfaces[i + 1][j + 1].height, min_y + (j + 1) * step_y));
            t.vertices.push_back(glm::vec3(min_x + i * step_x, baisHeight + surfaces[i][j + 1].height, min_y + (j + 1) * step_y));
            triangles.push_back(t);
            t.vertices.clear();
        }
    }
}