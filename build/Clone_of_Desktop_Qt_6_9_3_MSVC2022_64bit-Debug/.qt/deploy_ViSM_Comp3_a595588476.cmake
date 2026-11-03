include("A:/School/Vizualisation and Simulation/Comps/ViSM_Comp3/build/Clone_of_Desktop_Qt_6_9_3_MSVC2022_64bit-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ViSM_Comp3-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "A:/School/Vizualisation and Simulation/Comps/ViSM_Comp3/build/Clone_of_Desktop_Qt_6_9_3_MSVC2022_64bit-Debug/ViSM_Comp3.exe"
    GENERATE_QT_CONF
)
