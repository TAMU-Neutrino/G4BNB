file(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.cc")
add_executable(NuBeam ${sources})

target_include_directories(NuBeam
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/physics
)

# Prefer modern targets if they exist (newer G4); otherwise use legacy variables.
if(TARGET Geant4::G4global)
  target_link_libraries(NuBeam
    PRIVATE
      Geant4::G4global
      Geant4::G4gdml
      ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist
      ROOT::Physics ROOT::GenVector ROOT::MathCore
      ROOT::Matrix
      DK2NU::Tree
  )
  if(TARGET Geant4::G4UI_all)
    target_link_libraries(NuBeam PRIVATE Geant4::G4UI_all)
  endif()
  if(TARGET Geant4::G4vis_all)
    target_link_libraries(NuBeam PRIVATE Geant4::G4vis_all)
  endif()
else()
  # Legacy (G4 10.4): Geant4_USE_FILE already added include dirs / defs.
  target_link_libraries(NuBeam
    PRIVATE
      ${Geant4_LIBRARIES}
      ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist
      ROOT::Physics ROOT::GenVector ROOT::MathCore
      ROOT::Matrix
      DK2NU::Tree
  )
endif()

install(TARGETS NuBeam RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

