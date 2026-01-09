#include <gtkmm.h>
#include <cairomm/context.h>
#include <cmath>
#include <vector>
// compilar con:
// g++ hypercube_gtk.cpp -o hypercube_gtk $(pkg-config gtkmm-4.0 cairomm-1.16 --cflags --libs)
// 
// --- Constantes globales ---
constexpr int WINDOW_DEFAULT_WIDTH = 800;
constexpr int WINDOW_DEFAULT_HEIGHT = 600;

constexpr int FRAME_INTERVAL_MS = 16;     // ~60 FPS
constexpr double ROTATION_SPEED = 0.03;  // incremento de ángulo por frame

constexpr double LINE_WIDTH = 1.6;   // grosor de aristas
constexpr double PERSPECTIVE_D = 2.2;   // parámetro de proyección en W
constexpr double SCALE_DIVISOR = 4.0;   // factor de escala relativo al tamaño ventana
constexpr double Z_SHIFT_FACTOR = 0.15;  // sesgo por Z para efecto 3D

// Ángulos base para cada plano de rotación (coeficientes y desfases)
constexpr double ANGLE_XY_COEFF = 0.7;
constexpr double ANGLE_XZ_COEFF = 0.5;
constexpr double ANGLE_YZ_COEFF = 0.6;
constexpr double ANGLE_XW_COEFF = 0.9;
constexpr double ANGLE_YW_COEFF = 0.4;
constexpr double ANGLE_ZW_COEFF = 0.8;

constexpr double ANGLE_XZ_OFFSET = 0.8;
constexpr double ANGLE_YZ_OFFSET = 1.6;
constexpr double ANGLE_XW_OFFSET = 0.3;
constexpr double ANGLE_YW_OFFSET = 2.1;
constexpr double ANGLE_ZW_OFFSET = 0.5;

struct Vec4 { double x, y, z, w; };
struct Vec2 { double x, y; };

static inline void rotate_plane(double& a, double& b, double c, double s) {
    double na = a * c - b * s;
    double nb = a * s + b * c;
    a = na; b = nb;
}

class HypercubeArea : public Gtk::DrawingArea {
public:
    HypercubeArea() {
        vertices = hypercubeVertices();
        t = 0.0;

        set_draw_func(sigc::mem_fun(*this, &HypercubeArea::on_draw));
        Glib::signal_timeout().connect(sigc::mem_fun(*this, &HypercubeArea::on_timeout), FRAME_INTERVAL_MS);
    }

private:
    std::vector<Vec4> vertices;
    double t;

    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        // Fondo negro
        cr->set_source_rgb(0, 0, 0);
        cr->paint();

        // Ángulos de rotación en varios planos 4D
        double aXY = ANGLE_XY_COEFF * t;
        double aXZ = ANGLE_XZ_COEFF * t + ANGLE_XZ_OFFSET;
        double aYZ = ANGLE_YZ_COEFF * t + ANGLE_YZ_OFFSET;
        double aXW = ANGLE_XW_COEFF * t + ANGLE_XW_OFFSET;
        double aYW = ANGLE_YW_COEFF * t + ANGLE_YW_OFFSET;
        double aZW = ANGLE_ZW_COEFF * t + ANGLE_ZW_OFFSET;

        double cXY = std::cos(aXY), sXY = std::sin(aXY);
        double cXZ = std::cos(aXZ), sXZ = std::sin(aXZ);
        double cYZ = std::cos(aYZ), sYZ = std::sin(aYZ);
        double cXW = std::cos(aXW), sXW = std::sin(aXW);
        double cYW = std::cos(aYW), sYW = std::sin(aYW);
        double cZW = std::cos(aZW), sZW = std::sin(aZW);

        // Rotar todos los vértices
        std::vector<Vec4> rot;
        rot.reserve(vertices.size());
        for (const auto& p0 : vertices) {
            Vec4 p = p0;
            rotate_plane(p.x, p.y, cXY, sXY);
            rotate_plane(p.x, p.z, cXZ, sXZ);
            rotate_plane(p.y, p.z, cYZ, sYZ);
            rotate_plane(p.x, p.w, cXW, sXW);
            rotate_plane(p.y, p.w, cYW, sYW);
            rotate_plane(p.z, p.w, cZW, sZW);
            rot.push_back(p);
        }

        // Proyección en 2D
        std::vector<Vec2> proj;
        proj.reserve(rot.size());
        for (const auto& p : rot) {
            proj.push_back(project(p, width, height));
        }

        // Dibujar aristas
        cr->set_line_width(LINE_WIDTH);
        for (int i = 0; i < (int)vertices.size(); ++i) {
            for (int j = i + 1; j < (int)vertices.size(); ++j) {
                int diff = (vertices[i].x != vertices[j].x) +
                    (vertices[i].y != vertices[j].y) +
                    (vertices[i].z != vertices[j].z) +
                    (vertices[i].w != vertices[j].w);
                if (diff == 1) {
                    double wmean = 0.5 * (std::fabs(rot[i].w) + std::fabs(rot[j].w));
                    double alpha = 0.25 + 0.75 * (1.0 - std::min(1.0, wmean / 1.5));
                    cr->set_source_rgba(1.0, 1.0, 1.0, alpha);

                    cr->move_to(proj[i].x, proj[i].y);
                    cr->line_to(proj[j].x, proj[j].y);
                    cr->stroke();
                }
            }
        }
    }

    bool on_timeout() {
        t += ROTATION_SPEED;
        queue_draw();
        return true;
    }

    std::vector<Vec4> hypercubeVertices() {
        std::vector<Vec4> v;
        v.reserve(16);
        for (int i = -1; i <= 1; i += 2)
            for (int j = -1; j <= 1; j += 2)
                for (int k = -1; k <= 1; k += 2)
                    for (int l = -1; l <= 1; l += 2)
                        v.push_back({ (double)i, (double)j, (double)k, (double)l });
        return v;
    }

    Vec2 project(const Vec4& p, int width, int height) {
        double factor = PERSPECTIVE_D / (PERSPECTIVE_D - p.w);

        // Escala dinámica según tamaño de ventana
        double scale = std::min(width, height) / SCALE_DIVISOR;

        double X = p.x * factor * scale + width / 2.0;
        double Y = p.y * factor * scale + height / 2.0;

        // Pequeño sesgo por Z para dar sensación 3D
        double zshift = p.z * scale * Z_SHIFT_FACTOR;
        return { X + zshift, Y + zshift * Z_SHIFT_FACTOR };
    }
};

class HypercubeWindow : public Gtk::Window {
public:
    HypercubeWindow() {
        set_title("Hipercubo 4D");
        set_default_size(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
        set_decorated(true);
        set_resizable(true);
        set_child(area);
    }

private:
    HypercubeArea area;
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("eduardo.hypercube.gtk", Gio::Application::Flags::NONE);

    app->signal_activate().connect([&app]() {
        auto* window = new HypercubeWindow();
        app->add_window(*window);
        window->present();
        });

    return app->run(argc, argv);
}
