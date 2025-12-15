# Silence CMP0167 by using CONFIG mode.
find_package(Boost 1.66 CONFIG REQUIRED COMPONENTS program_options)

add_executable(beamHist beamHist.cc)

target_link_libraries(beamHist
  PRIVATE
    ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist
    ROOT::Physics ROOT::GenVector ROOT::MathCore ROOT::Matrix
    DK2NU::Tree
    Boost::program_options
)

target_include_directories(beamHist
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS beamHist RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(PROGRAMS submitBeam.py compare.py mergeHist.py
        DESTINATION ${CMAKE_INSTALL_BINDIR})

