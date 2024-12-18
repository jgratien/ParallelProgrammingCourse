# Recherche des bibliothèques nécessaires
find_package(glfw3 3.3 REQUIRED)
find_package(glad REQUIRED)
find_package(fmt REQUIRED)
find_package(PNG REQUIRED)
find_package(TBB REQUIRED) # Ajout de TBB

# Recherche et ajout d'OpenMP
find_package(OpenMP REQUIRED)

# Récupérer tous les fichiers .cpp
file(GLOB_RECURSE ALL_CPP_FILES "*.cpp")

# Liste des fichiers à exclure
list(REMOVE_ITEM ALL_CPP_FILES "main.cpp")

# Création de la bibliothèque avec les fichiers .cpp restants
add_library(my_libraries ${ALL_CPP_FILES})

# Ajouter les répertoires d'inclusion
target_include_directories(my_libraries PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/laws
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/agents
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/graphics/shaders
)

# Lier les bibliothèques nécessaires à my_libraries
target_link_libraries(my_libraries PUBLIC glfw glad::glad PNG::PNG fmt::fmt TBB::tbb)

# Création de l'exécutable à partir de main.cpp
add_executable(freebirds main.cpp)

# Lier les bibliothèques nécessaires à l'exécutable
target_link_libraries(freebirds PUBLIC my_libraries fmt::fmt TBB::tbb)

# Vérifiez si OpenMP est trouvé et ajoutez les bibliothèques nécessaires
if(OpenMP_CXX_FOUND)
    target_link_libraries(freebirds PUBLIC OpenMP::OpenMP_CXX)
endif()
