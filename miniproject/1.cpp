#include <iostream>
#include <array>
#include <list>
#include <math.h>
#include <fstream>


// Obstacle is a line segment. Other types of obstacles do not exist.
// Complex obstacles like square are just sets of line obstacles
// Curve obstacles do not exist for now. Maybe we will add them later

// Each obstacle has unique id
class Obstacle {
public:
  float x_1, x_2, y_1, y_2;
  // Ax + By = C
  float A, B;

  // maybe this will not be needed
  float n_x, n_y;
  
  Obstacle(float x_1, float y_1, float x_2, float y_2) {
    this->x_1 = x_1;
    this->x_2 = x_2;
    this->y_1 = y_1;
    this->y_2 = y_2;

    this->A = (y_2 - y_1)/(x_1*y_2 - y_1*x_2);
    this->B = (x_1 - x_2)/(x_1*y_2 - y_1*x_2);

    float n = std::sqrt(std::pow(A, 2) + std::pow(B, 2));
    this->n_x = A / n;
    this->n_y = B / n;
  }

};


class Point {
public:
  float x, y;
  float vx, vy;
  std::list<Obstacle*> hist; // points on obstacle, from which point was reflected

  Point(float x, float y, float vx, float vy) {
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
    this->hist = {};
  }

  Point(float x, float y, float vx, float vy, std::list<Obstacle*> hist) {
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
    this->hist = hist;
  }

  void reflect(float n_x, float n_y) {
    float vx_norm = (vx*n_x + vy*n_y) * n_x;
    float vy_norm = (vx*n_x + vy*n_y) * n_y;

    // float vx_par = vx - vx_norm;
    // float vy_par = vy - vy_norm;

    vx -= 2* vx_norm;
    vy -= 2* vy_norm;
  }

  bool segments_crosses(Obstacle obst, float x, float y, float x_new, float y_new) {
    // firstly check if old and new points are on the different sizes of obstacle line
    if ((obst.A * x + obst.B * y - 1) * (obst.A * x_new + obst.B * y_new - 1) < 0) {
      // find line equation for [x, y, x_new, y_new]
      float A = (y_new - y)/(x*y_new - y*x_new);
      float B = (x - x_new)/(x*y_new - y*x_new);

      // and check if old ends of the obstacle are on the different sizes of this line
      if ((A * obst.x_1 + B * obst.y_1 - 1) * (A * obst.x_2 + B * obst.y_2 - 1) < 0) {
	return 1;
      }
    }
    return 0;
  }

  template<int N>
  void move(float dt, std::array<Obstacle, N> arr) {
    float x_new = x + vx*dt;
    float y_new = y + vy*dt;

    bool reflected = 0;
    // reflection
    for (int i = 0; i < N; i++) {
      Obstacle obstacle = arr[i];
      
      if (segments_crosses(obstacle, x, y, x_new, y_new)) {
	  hist.push_front(&obstacle);
	  reflect(obstacle.n_x, obstacle.n_y);
	  reflected = 1;
	  break;
	}
    }
    if (reflected == 0) {
      x = x_new;
      y = y_new;
    }
  }
};

float length(Point a, Point b) {
  float dx = std::pow(a.x - b.x, 2);
  float dy = std::pow(a.y - b.y, 2);
  return std::sqrt(dx + dy);
}

class Front {
public:
  std::list<Point> points;
  float v_ph;
  int N;

  Front(int N, std::list<Point> points, float v_ph) {
    this->points = points;
    this->N = N;
    this->v_ph = v_ph;
  }

  Front(int N, float origin_x, float origin_y, float v_ph) {
    this->N = N;
    this->v_ph = v_ph;

    std::list<Point> points;
    float phi = 2*M_PI / N;
    // e is a unit vector in direction of point velocity
    float e_x = 1;
    float e_y = 0;
    for (int i = 0; i < N; i++) {
      float x = origin_x + 0.01 * e_x; //TODO
      float y = origin_y + 0.01 * e_y;
      float vx = v_ph * e_x;
      float vy = v_ph * e_y; 
      points.push_front(Point(x, y, vx, vy));

      // rotate e on phi
      float new_ex = e_x*std::cos(phi) - e_y*std::sin(phi);
      float new_ey = e_x*std::sin(phi) + e_y*std::cos(phi);
      e_x = new_ex;
      e_y = new_ey;
    }
    this->points = points;
  }

  template<int N>
  void move(float dt, std::array<Obstacle, N> arr) {
    std::list<Point>::iterator i;
    for (i = points.begin(); i != points.end(); ++i) {
      i->move<N>(dt, arr);
    }

    // if distance between points is to large, add new point
    i = points.begin();
    std::list<Point>::iterator end = points.end();
    --end;
    while (i != end) {
      Point a = *i;
      Point b = *(++i);
      float dist = length(a, b);      
      if ((.05 < dist) and (dist < .1) and (a.hist == b.hist)) {
	float new_x = (a.x + b.x) / 2;
	float new_y = (a.y + b.y) / 2;

	float vx = (a.vx + b.vx) / 2;
	float vy = (a.vy + b.vy) / 2;
	float v = std::sqrt(std::pow(vx, 2) + std::pow(vy, 2));
	vx *= v_ph / v;
	vy *= v_ph / v;
	points.insert(i, Point(new_x, new_y, vx, vy, a.hist)); // or "--i" ?
      }	
    }
  }

  void write(std::string file) {
    std::ofstream target;
    target.open(file);

    std::list<Point>::iterator i;
    for (i = points.begin(); i != points.end(); ++i) {
      target << i->x << ";" << i->y << "\n";
    }

    target.close();
  }
};


int main(){
  Front F = Front(10, 0, 0, 0.05);
  std::array<Obstacle, 1> obst = {Obstacle(-5, -100, -5, 100)};
  for (int i = 0; i < 1000; i++) {
    F.move<1>(1, obst);
    if (i % 10 == 0)
      std::cout << i << std::endl;
      F.write("./1/" + std::to_string(i) + ".csv");
  }
}
