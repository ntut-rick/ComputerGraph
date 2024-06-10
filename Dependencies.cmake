cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCH_CONTENT_QUIET FALSE)

FetchContent_Declare(
  freeglut_static
  URL      https://github.com/freeglut/freeglut/releases/download/v3.4.0/freeglut-3.4.0.tar.gz
  URL_HASH MD5=f1621464e6525d0368976870cab8f418
  SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freeglut
)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        0bc88af4de5fb022db643c2d8e549a0927749354 # stb_image - v2.29
)
