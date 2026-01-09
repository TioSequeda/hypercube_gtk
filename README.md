# Hypercube GTK

Visualización en tiempo real de un hipercubo (tesseracto) en C++ usando `gtkmm` y `cairomm`.

## Descripción
Pequeña aplicación que renderiza un hipercubo 4D en movimiento. Demuestra rotaciones en varios planos 4D y proyección en perspectiva a 2D.

## Características
- Renderizado alrededor de ~60 FPS.
- Rotaciones combinadas en planos 4D.
- Ajuste automático al tamaño de ventana.

## Dependencias
- Compilador C++ (g++).
- `pkg-config`
- `gtkmm-4.0`
- `cairomm-1.16`

En Debian/Ubuntu:

sudo apt update 
sudo apt install build-essential pkg-config libgtkmm-4.0-dev libcairomm-1.16-dev

En Archlinux:
sudo pacman -Syu 
sudo pacman -S base-devel pkgconf gtkmm-4.0 cairomm-1.16

## Compilar
Desde la raíz del proyecto (donde está `hypercube_gtk.cpp`):

g++ hypercube_gtk.cpp -o hypercube_gtk $(pkg-config gtkmm-4.0 cairomm-1.16 --cflags --libs)

## Ejecutar

./hypercube_gtk

La ventana mostrará el hipercubo; se puede redimensionar y el render se ajusta automáticamente.

## Estructura del repositorio
- `hypercube_gtk.cpp` — código fuente.
- `README.md` — este fichero.
- `.editorconfig` — reglas de estilo del proyecto.
- `CONTRIBUTING.md` — normas de contribución.

## Contribuir
1. Crea una rama: `git checkout -b feature/mi-cambio`.
2. Haz cambios y pruebas.
3. `git add . && git commit -m "Descripción"`.
4. Push y abre un pull request.

Revisa `.editorconfig` y `CONTRIBUTING.md` antes de contribuir.

## Publicar en GitHub desde Visual Studio 2026
1. Inicia sesión en tu cuenta GitHub (__Sign in__ / cuenta arriba a la derecha).
2. Inicializa el repo local si hace falta: __Git > Create Git Repository...__ o __Add Solution to Source Control__.
3. Publica: __Publish to GitHub__ (elige cuenta, nombre y visibilidad).
Alternativa por terminal integrada:

git add README.md git commit -m "Add README" git push origin master
4. Verifica en GitHub que el repo se ha creado correctamente.