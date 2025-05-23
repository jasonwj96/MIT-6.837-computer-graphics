#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "GL/freeglut.h"
#include "vecmath.h"

// This is the list of points (3D vectors)
std::vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
std::vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
std::vector<std::vector<unsigned> > vecf;

std::vector<int> vertexIndices;
std::vector<int> normalIndices;

int color = 0;
int lightPositionX = 1.0f;
int lightPositionY = 1.0f;

bool is_rotating = false;
int rotation_request = 0;

float angle = 0;
const float rotation_speed = 1;

// You will need more global variables to implement color and position changes

// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f& a) { glVertex3fv(a); }

inline void glNormal(const Vector3f& a) { glNormal3fv(a); }

void parseFaceComponent(const std::string& component, int& vertexIndex,
                        int& normalIndex) {
  size_t firstSlash = component.find('/');
  size_t lastSlash = component.rfind('/');

  vertexIndex = std::stoi(component.substr(0, firstSlash)) - 1;
  normalIndex = std::stoi(component.substr(lastSlash + 1)) - 1;
}

void drawObject() {
  for (const auto& face : vecf) {
    auto a = face[0];
    auto c = face[1];
    auto d = face[2];
    auto f = face[3];
    auto g = face[4];
    auto i = face[5];

    glBegin(GL_TRIANGLES);
    glNormal(vecn[c - 1]);
    glVertex(vecv[a - 1]);
    glNormal(vecn[f - 1]);
    glVertex(vecv[d - 1]);
    glNormal(vecn[i - 1]);
    glVertex(vecv[g - 1]);
    glEnd();
  }
}

void updateFunc(int value) {
  if (value != rotation_request) return;

  glutTimerFunc(1000 / 60, updateFunc, value);

  angle = fmod(angle + rotation_speed, 360);

  glutPostRedisplay();
}

void loadInput(const std::filesystem::path filePath) {
  if (filePath.empty()) {
    std::cout << "The file path is invalid.";
    return;
  }

  std::ifstream file(filePath);

  if (!file.is_open()) {
    std::cerr << "Failed to open the file.\n";
    return;
  }

  vecv.clear();
  vecn.clear();
  vecf.clear();

  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty()) {
      std::istringstream stream(line);
      std::string token;
      std::vector<std::string> tokens(4);
      std::string tag;

      stream >> tag;

      if (tag == "v") {
        float x, y, z = 0.0f;

        stream >> x >> y >> z;

        // printf("v: {%f, %f, %f}\n", x, y, z);

        vecv.push_back(Vector3f(x, y, z));
      }

      if (tag == "vn") {
        float x, y, z = 0.0f;

        stream >> x >> y >> z;

        // printf("vn: {%f, %f, %f}\n", x, y, z);

        vecn.push_back(Vector3f(x, y, z));
      }

      if (tag == "f") {
        std::string abc, def, ghi;
        stream >> abc >> def >> ghi;

        unsigned int a, b, c;
        sscanf(abc.c_str(), "%u/%u/%u", &a, &b, &c);

        unsigned int d, e, f;
        sscanf(def.c_str(), "%u/%u/%u", &d, &e, &f);

        unsigned int g, h, i;
        sscanf(ghi.c_str(), "%u/%u/%u", &g, &h, &i);

        vecf.push_back({a, c, d, f, g, i});
      }
    }
  }

  file.close();
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc(unsigned char key, int x, int y) {
  switch (key) {
    case 27:  // Escape key
      exit(0);
      break;
    case 'c':
      color++;

      if (color > 3) {
        color = 0;
      }
      break;
    case 'r':
      is_rotating = !is_rotating;
      if (is_rotating) {
        updateFunc(rotation_request);
      } else {
        rotation_request++;
      }

      std::cout << (is_rotating ? "Started" : "Stopped") << " rotating."
                << std::endl;

      break;
    default:
      std::cout << "Unhandled key press " << key << "." << std::endl;
  }

  // this will refresh the screen so that the user sees the color change
  glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      lightPositionY++;
      break;
    case GLUT_KEY_DOWN:
      lightPositionY--;
      break;
    case GLUT_KEY_LEFT:
      lightPositionX--;
      break;
    case GLUT_KEY_RIGHT:
      lightPositionX++;
      break;
  }

  // this will refresh the screen so that the user sees the light position
  glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void) {
  int i;

  // Clear the rendering window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Rotate the image
  glMatrixMode(GL_MODELVIEW);  // Current matrix affects objects positions
  glLoadIdentity();            // Initialize to the identity

  // Position the camera at [0,0,5], looking at [0,0,0],
  // with [0,1,0] as the up direction.
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // Set material properties of object

  // Here are some colors you might use - feel free to add more
  GLfloat diffColors[4][4] = {{0.5, 0.5, 0.9, 1.0},
                              {0.9, 0.5, 0.5, 1.0},
                              {0.5, 0.9, 0.3, 1.0},
                              {0.3, 0.8, 0.9, 1.0}};

  // Here we use the first color entry as the diffuse color
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color]);

  // Define specular color and shininess
  GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat shininess[] = {100.0};

  // Note that the specular color and shininess can stay constant
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

  // Set light properties

  // Light color (RGBA)
  GLfloat Lt0diff[] = {1.0, 1.0, 1.0, 1.0};
  // Light position
  GLfloat Lt0pos[] = {lightPositionX, lightPositionY, 5.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
  glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

  // This GLUT method draws a teapot.  You should replace
  // it with code which draws the object you loaded.
  // glutSolidTeapot(1.0);

  drawObject();

  // Dump the image to the screen.
  glutSwapBuffers();
}

// Initialize OpenGL's rendering modes
void initRendering() {
  glEnable(GL_DEPTH_TEST);  // Depth testing must be turned on
  glEnable(GL_LIGHTING);    // Enable lighting calculations
  glEnable(GL_LIGHT0);      // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h) {
  // Always use the largest square viewport possible
  if (w > h) {
    glViewport((w - h) / 2, 0, h, h);
  } else {
    glViewport(0, (h - w) / 2, w, w);
  }

  // Set up a perspective view, with square aspect ratio
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // 50 degree fov, uniform aspect ratio, near = 1, far = 100
  gluPerspective(50.0, 1.0, 1.0, 100.0);
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char** argv) {
  loadInput("garg.obj");

  glutInit(&argc, argv);

  // We're going to animate it, so double buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Initial parameters for window position and size
  glutInitWindowPosition(60, 60);
  glutInitWindowSize(360, 360);
  glutCreateWindow("Assignment 0");

  // Initialize OpenGL parameters.
  initRendering();

  // Set up callback functions for key presses
  glutKeyboardFunc(keyboardFunc);  // Handles "normal" ascii symbols
  glutSpecialFunc(specialFunc);    // Handles "special" keyboard keys

  // Set up the callback function for resizing windows
  glutReshapeFunc(reshapeFunc);

  // Call this whenever window needs redrawing
  glutDisplayFunc(drawScene);

  // Start the main loop.  glutMainLoop never returns.
  glutMainLoop();

  return 0;  // This line is never reached.
}
