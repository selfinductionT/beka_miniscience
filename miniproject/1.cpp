#include <iostream>
#include <list>
#include <math.h>

class Point {
public:
  float x, y;
  float vx, vy;

  Point(float x, float y, float vx, float vy) {
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
  }

  void move(float dt) {
    x+= vx*dt;
    y+= vy*dt;
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

  void move(float dt) {
    std::list<Point>::iterator i;
    for (i = points.begin(); i != points.end(); ++i) {
      i->move(dt);
    }

    // if distance between points is to large, add new point
    i = points.begin();
    while (i != points.end()) {
      Point a = *i;
      Point b = *(++i);
      if (length(a, b) < 0.1) {
	float new_x = (a.x + b.x) / 2;
	float new_y = (a.y + b.y) / 2;

	float vx = (a.vx + b.vx) / 2;
	float vy = (a.vy + b.vy) / 2;
	float v = std::sqrt(std::pow(vx, 2) + std::pow(vy, 2));
	vx *= v_ph / v;
	vy *= v_ph / v;
	points.insert(i, Point(new_x, new_y, vx, vy)); // or "--i" ?
	std::cout << i->x << std::endl;
      }	
    }
  }
};


int main(){
  Front F = Front(100, 0, 0, 0.02);
  for (int i = 0; i < 100; i++) {
    F.move(1);
  }
}
