#include <iostream>
#include <iomanip>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
struct triangle_t {
    std::vector<glm::vec3> vertices; //size = 3
};
class TrainWindow;
class surface {
public:
    float t2Height = 0;
    float t1Height = 0;
    float height = 0;
};
class water_simulation {
public:
    water_simulation(int, TrainWindow* tw);
    float slowdown = 0.7;
    float speed = 0.8;
    float baisHeight = 12.0f;
    int count = 0;
    surface** surfaces;
    TrainWindow* tw;
    
    void fresh();
    void tickBack();
    void get_matrix(std::vector<triangle_t>&);
private:
};



//void fresh(surface** su, int x, int y) {
//    float l = su[x - 1][y].t1Height;
//    float r = su[x + 1][y].t1Height;
//    float u = su[x][y + 1].t1Height;
//    float d = su[x][y - 1].t1Height;
//
//    float slowdown = 0.7;
//    float speed = 0.8;
//    float tickLength = 1;
//
//    su[x][y].height = (1 + slowdown) * su[x][y].t1Height -
//        slowdown * su[x][y].t2Height +
//
//    0.5f * speed * speed * (
//        l + r + u + d - 4 * su[x][y].t1Height
//        ) * tickLength * tickLength;
//}

//void tickBack(surface** su, int row, int column) {
//    for (int i = 0; i < row; i++) {
//        for (int j = 0; j < column; j++) {
//            su[i][j].t2Height = su[i][j].t1Height;
//            su[i][j].t1Height = su[i][j].height;
//        }
//    }
//}

// int main() {
//     int Count = 9;
//     surface** sf = new surface * [Count + 2];

//     //init
//     for (int i = 0; i < Count + 2; i++) {
//         sf[i] = new surface[Count + 2];
//         //            for(int j = 0; j < Count + 2; j++){
//         //                sf[i][j] = new surface;
//         //            }
//     }

//     int _i, _j, h;
//     std::cin >> _i >> _j >> h;
//     sf[_i][_j].height = h;
//     int z = 100;
//     while (z--) {
//         if (_i == -1) {
//             break;
//         }
//         //        if(_i != -2){

//         //            //get
//         //            for(int i = 0; i <= Count; i++){
//         //                for(int j = 0; j <= Count; j++){
//         //                    std::cout << std::setw(7) << sf[i][j].height << " ";
//         //                }
//         //                std::cout << "\n";
//         //            }
//         //            std::cout << "\n";
//         //        }

//                 //tickBack
//         tickBack(sf, Count + 2, Count + 2);

//         //fresh
//         for (int i = 1; i <= Count; i++) {
//             for (int j = 1; j <= Count; j++) {
//                 fresh(sf, i, j);
//             }
//         }

//         //get
//         for (int i = 0; i < Count + 2; i++) {
//             for (int j = 0; j < Count + 2; j++) {
//                 std::cout << std::setw(15) << sf[i][j].height << " ";
//             }
//             std::cout << "\n";
//         }
//         std::cout << "\n";
//     }



//     return 0;
// }