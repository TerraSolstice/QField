function(set_library_target NAMESPACE LIB_NAME DEBUG_LIB_FILE_NAME RELEASE_LIB_FILE_NAME INCLUDE_DIR)
    add_library(${NAMESPACE}::${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${NAMESPACE}::${LIB_NAME} PROPERTIES
                          IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
                          IMPORTED_LOCATION_RELEASE "${RELEASE_LIB_FILE_NAME}"
                          IMPORTED_LOCATION_DEBUG "${DEBUG_LIB_FILE_NAME}"
                          INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIR}"
                          )
    set(${NAMESPACE}_${LIB_NAME}_FOUND 1)
    set(${NAMESPACE}_${LIB_NAME}_LIBRARY ${NAMESPACE}::${LIB_NAME})
    set(${NAMESPACE}_${LIB_NAME}_INCLUDE_DIR ${INCLUDE_DIR})
endfunction()

get_filename_component(ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(ROOT "${ROOT}" PATH)
get_filename_component(ROOT "${ROOT}" PATH)

if (CMAKE_HOST_WIN32)
  set_library_target("QGIS" "CORE" "${ROOT}/debug/lib/qgis_core.lib" "${ROOT}/lib/qgis_core.lib" "${ROOT}/include/qgis/")
  set_library_target("QGIS" "ANALYSIS" "${ROOT}/debug/lib/qgis_analysis.lib" "${ROOT}/lib/qgis_analysis.lib" "${ROOT}/include/qgis/")
else ()
  set_library_target("QGIS" "CORE" "${ROOT}/debug/lib/libqgis_core.a" "${ROOT}/lib/libqgis_core.a" "${ROOT}/include/qgis/")
  set_library_target("QGIS" "ANALYSIS" "${ROOT}/debug/lib/libqgis_analysis.a" "${ROOT}/lib/libqgis_analysis.a" "${ROOT}/include/qgis/")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/libwmsprovider_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libprovider_wms_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_basic_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_esritoken_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_identcert_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_oauth2_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_pkcs12_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libauthmethod_pkipaths_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libprovider_postgres_a.a")
  target_link_libraries(QGIS::CORE INTERFACE "${ROOT}/debug/lib/qgis/plugins/libprovider_postgresraster_a.a")
endif ()

function(find_and_link_library TARGET LIBRARY)
  find_library(${LIBRARY}-LIBRARY ${LIBRARY})
  target_link_libraries(${TARGET} INTERFACE ${${LIBRARY}-LIBRARY})
endfunction()

set(QGIS_INCLUDE_DIR "${ROOT}/include/qgis/")
# This should be done by the set_library_target function, but doesnt work for whatever reason ...
set(QGIS_CORE_LIBRARY QGIS::CORE)
set(QGIS_ANALYSIS_LIBRARY QGIS::ANALYSIS)
find_package(GDAL REQUIRED)
target_link_libraries(QGIS::CORE INTERFACE GDAL::GDAL)
find_package(Protobuf REQUIRED)
target_link_libraries(QGIS::CORE INTERFACE protobuf::libprotobuf)
find_package(exiv2 REQUIRED)
target_link_libraries(QGIS::CORE INTERFACE exiv2lib)

# Qca does not support manifest mode? The QcaTargets.cmake file will look for
# the lib in the vcpkg install folder and not the manifest install folder
#find_package(Qca REQUIRED)
#target_link_libraries(QGIS::CORE INTERFACE qca)

find_and_link_library(QGIS::CORE qca)
find_and_link_library(QGIS::CORE spatialite)
find_and_link_library(QGIS::CORE spatialindex)
find_and_link_library(QGIS::CORE tasn1)
find_and_link_library(QGIS::CORE zip)
find_and_link_library(QGIS::CORE rttopo)
find_and_link_library(QGIS::CORE freexl)

target_link_libraries(QGIS::ANALYSIS INTERFACE QGIS::CORE)
