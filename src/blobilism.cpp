#include <iostream>
#include <vector>
#include <algorithm>
#include "tinygl-cpp.h"
using namespace tinygl;

class Color {
  public:
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
    float * arr() {
      static float arr[3] = {r, g, b};
      return arr;
    }
    float r;
    float g;
    float b;
};

class Circle {
  public:
    Circle(float x_, float y_, float radius_, Color color_, float alpha_) : x(x_), y(y_), radius(radius_), color(color_), alpha(alpha_) {}
    void draw(Window * window) {
      window->color(color.r, color.g, color.b, alpha);
      window->circle(x, y, radius);
    }
    float x;
    float y;
    float radius;
    Color color;
    float alpha;
};

class Stroke {
  public:
    Stroke(float p1_[2], float p2_[2], float width_, Color color_, float alpha_) : width(width_), color(color_), alpha(alpha_) {
      start[0] = p1_[0];
      start[1] = p1_[1];
      end[0] = p2_[0];
      end[1] = p2_[1];
    }
    void draw(Window * window) {
      window->color(color.r, color.g, color.b, alpha);
      window->circle(start[0], start[1], width);
      window->circle(end[0], end[1], width);
      window->line(start, end, width);
    }
    float start[2];
    float end[2];
    float width;
    Color color;
    float alpha;
};

class CircleButton {
  public:
    CircleButton(float x_, float y_, float radius_, Color color_, float alpha_) : x(x_), y(y_), radius(radius_), color(color_), alpha(alpha_) {
      originalRadius = radius;
    }
    void draw(Window * window) {
      window->color(color.r, color.g, color.b, alpha);
      window->circle(x, y, radius);
    }
    bool wasClicked(float mx, float my) {
      float dx = mx - x;
      float dy = my - y;
      return dx * dx + dy * dy < radius * radius;
    }
    float x;
    float y;
    float originalRadius;
    float radius;
    Color color;
    float alpha;
};

class MyWindow : public Window {
 public:
  MyWindow(int w, int h) : Window(w, h) {}

  void setup() override {
    std::cout << "Window size: " << width() << ", " << height() << std::endl;
    float buttonRadius = 40.0f;
    float x = 50;
    float y = 35;
    for(Color c : palleteColors) {
      palleteButtons.push_back(CircleButton(x, y, buttonRadius, c, brushAlpha));
      x += 1.42 * buttonRadius;
    }
  }

  virtual void mouseMotion(int x, int y, int dx, int dy) {
    if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
      // todo: store a circle with the current color, size, x, y
      if(lastBrushX == NULL || lastBrushY == NULL) {
        lastBrushX = x;
        lastBrushY = y;
      }

      float p1[] = {lastBrushX, lastBrushY};
      float p2[] = {(float) x, (float) y};

      strokes.push_back(Stroke(p1, p2, (int) brushSize, brushColor, brushAlpha));

      lastBrushX = x;
      lastBrushY = y;
    }
    else {
      lastBrushX = NULL;
      lastBrushY = NULL;
    }
  }
  
  virtual void mouseDown(int button, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      float mx = mouseX();
      float my = mouseY();
      for (CircleButton b : palleteButtons) {
        if(b.wasClicked(mx, my)){
          changeBrushColor(b.color);
        };
      }
    }
  }

  void keyDown(int key, int mods) {
    if (key == GLFW_KEY_UP) {
      // increase size of circle
      brushSize = std::min(100.0f, ((float) brushSize * 1.33f));
      std::cout << "brushSize: " << brushSize << std::endl;
    } 
    else if (key == GLFW_KEY_DOWN) {
      // decrease size of circle
      brushSize = std::max(1.0f, ((float) brushSize * 0.75f));
      std::cout << "brushSize: " << brushSize << std::endl;
    }
    else if (key == GLFW_KEY_LEFT) {
      // decrease alpha
      brushAlpha = std::max(0.0f, brushAlpha * 0.75f);
    }
    else if (key == GLFW_KEY_RIGHT) {
      // increase alpha
      brushAlpha = std::min(1.0f, brushAlpha / 0.75f);
    }
    else if (key == GLFW_KEY_C) {
      // clear vector of circles
      strokes = {};
    }

    for(CircleButton b : palleteButtons) {
      b.radius = brushSize;
      b.alpha = brushAlpha;
    }
  }

  void draw() override {
    background(0.95f, 0.95f, 0.95f);

    for (Stroke s : strokes) {
      s.draw(this);
    }

    color(0.1f, 0.1f, 0.1f);
    square(width()/2.0f, 35, width(), 70);
    for (CircleButton b : palleteButtons) {
      b.draw(this);
    }
  }

  void changeBrushColor(Color c) {
    brushColor = c;
  }
  
 private:
  // color pallete
  std::vector<Color> palleteColors = {
    Color(1, 0, 0),
    Color(0, 1, 0),
    Color(0, 0, 1),
    Color(1, 1, 0),
    Color(1, 0, 1),
    Color(0, 1, 1),
    Color(1, 1, 1),
    Color(0, 0, 0)
  };
  // todo: create member variables for 
  // current circle size
  float brushSize = 20;
  // current transparency
  float brushAlpha = 1.0f;
  // current color
  Color brushColor{1.0f, 0.0f, 0.0f};
  // list of circles to draw each frame
  std::vector<Circle> circles;
  // list of strokes to draw each frame
  std::vector<Stroke> strokes;
  // list of buttons
  std::vector<CircleButton> palleteButtons;
  // Last brush position
  float lastBrushX = NULL;
  float lastBrushY = NULL;
};

int main() {
  MyWindow window(500, 500);
  window.run();
}
