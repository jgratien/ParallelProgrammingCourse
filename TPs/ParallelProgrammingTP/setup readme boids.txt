1. Create a Python Virtual Environment & Install Conan


python3.7 -m venv conan_env
source conan_env/bin/activate
pip install --upgrade pip
pip install conan
2. Set Up the Conan Profile
You can either auto-detect the system or create a new profile:



conan profile new default --detect
# or
conan profile detect
Later, ensure your Conan profile (e.g. at ~/.conan2/profiles/default) contains:

ini

[conf]
tools.system.package_manager:mode=install
tools.system.package_manager:sudo=False
3. Load Required Modules
Load the modules for the libraries you need:



module load gtk+3.22
module load glib2.60
module load gobject-introspection1.60
module load harfbuzz2.3
module load pango1.43
module load libpng1.6
module load gcc7.3
module load cmake3.28
module load fontconfig2.13
(Load any additional modules as required for your system.)

4. Set Environment Variables
a. Update the PATH


export PATH=$PATH:/softs/gtk+/3.22/bin:/softs/glib/2.60/bin:/softs/pango/1.43/bin
echo 'export PATH=$PATH:/softs/gtk+/3.22/bin:/softs/glib/2.60/bin:/softs/pango/1.43/bin' >> ~/.bashrc
b. Update LD_LIBRARY_PATH


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/softs/gtk+/3.22/lib:/softs/glib/2.60/lib64:/softs/pango/1.43/lib64
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/softs/gtk+/3.22/lib:/softs/glib/2.60/lib64:/softs/pango/1.43/lib64' >> ~/.bashrc
c. Update PKG_CONFIG_PATH
Basic setup:



export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/softs/gtk+/3.22/lib/pkgconfig:/softs/glib/2.60/lib64/pkgconfig:/softs/pango/1.43/lib64/pkgconfig
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/softs/gtk+/3.22/lib/pkgconfig:/softs/glib/2.60/lib64/pkgconfig:/softs/pango/1.43/lib64/pkgconfig' >> ~/.bashrc
Then add additional paths for other dependencies:



export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/softs/freetype/2.10/lib/pkgconfig:/softs/pixman/0.38/lib/pkgconfig:/softs/cairo/1.16/lib/pkgconfig:/softs/gdk-pixbuf/2.38/lib64/pkgconfig:/softs/util-linux/2.33/lib/pkgconfig:/softs/epoxy/1.5/lib64/pkgconfig:/softs/atk/2.32/lib64/pkgconfig:/softs/at-spi2-atk/2.32/lib64/pkgconfig:/softs/at-spi2-core/2.32/lib64/pkgconfig:/softs/libXtst/1.2.3/lib/pkgconfig
(You can also merge all these into one long export command if desired.)

d. Update Other Environment Variables
For example, to set the XDG data directories:



export XDG_DATA_DIRS=$XDG_DATA_DIRS:/softs/gtk+/3.22/lib/share:/softs/pango/1.43/share
echo 'export XDG_DATA_DIRS=$XDG_DATA_DIRS:/softs/gtk+/3.22/lib/share:/softs/pango/1.43/share' >> ~/.rc
And to update the CMAKE_PREFIX_PATH:



export CMAKE_PREFIX_PATH=/softs/gtk+/3.22:/softs/glib/2.60:/softs/pango/1.43:$CMAKE_PREFIX_PATH
(Adjust the paths to match your system’s installation locations.)

Verify Library Installations
Use pkg-config to check that the correct versions are detected:

pkg-config --modversion gtk+-3.0
pkg-config --modversion glib-2.0
pkg-config --modversion pango
pkg-config --modversion cairo

Run Optional Setup Scripts
If you have a setup script, make it executable and run it:

chmod +x setup_env.sh
./setup_env.sh
pip freeze > requirements.txt

Install Project Dependencies with Conan
Using your profile and building missing packages locally:

conan install . --build=missing --no-remote

Or with an explicit profile:

conan install .. --profile /gext/shubhankar.shubhanka/.conan2/profiles/default --build=missing --no-remote

(Optional) Update and Verify SQLite
If your project uses SQLite:

export LD_LIBRARY_PATH=/softs/sqlite/lib:$LD_LIBRARY_PATH
python3 -c "import sqlite3; print(sqlite3.sqlite_version)"

(Optional) Final Merge for All pkg-config Paths
For a comprehensive configuration, merge all required paths:

export PKG_CONFIG_PATH=/softs/gtk+/3.22/lib/pkgconfig:\
/softs/atk/2.32/lib64/pkgconfig:\
/softs/pango/1.43/lib64/pkgconfig:\
/softs/glib/2.60/lib64/pkgconfig:\
/softs/dbus/1.12/lib/pkgconfig:\
/softs/libXtst/1.2.3/lib/pkgconfig:\
/softs/fontconfig/2.13/lib/pkgconfig:\
/softs/libpng/1.6/lib/pkgconfig:\
/softs/harfbuzz/2.3/lib/pkgconfig:\
/softs/gobject-introspection/1.60/lib/pkgconfig:\
/softs/freetype/2.10/lib/pkgconfig:\
/softs/cairo/1.16/lib/pkgconfig:\
/softs/pixman/0.38/lib/pkgconfig:\
/softs/gdk-pixbuf/2.38/lib64/pkgconfig:\
/softs/util-linux/2.33/lib/pkgconfig:\
/softs/epoxy/1.5/lib64/pkgconfig:\
/softs/at-spi2-atk/2.32/lib64/pkgconfig:\
/softs/at-spi2-core/2.32/lib64/pkgconfig:\
/softs/gtk+/2.24/lib/pkgconfig:$PKG_CONFIG_PATH

Build the Project (Using CMake)
A sample CMakeLists.txt is provided that finds MPI, Eigen3, OpenMP, TBB, and Boost. Use it to configure and build your project:

cmake -S . -B build
cmake --build build
