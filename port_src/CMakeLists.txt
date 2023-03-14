add_library(step_port
  base64_dec.cpp
  cmdline.cpp    ../port/cmdline.h
  config.cpp     ../port/config.h
  license.cpp    ../port/license.h
  paUtils.cpp    ../port/paUtils.h
  play.cpp       ../port/play.h
  rngs.cpp       ../port/rngs.h
  session.cpp    ../port/session.h
  testAB.cpp     ../port/testAB.h
  testABX.cpp    ../port/testABX.h
  testBS1116.cpp ../port/testBS1116.h
  testBase.cpp   ../port/testBase.h
  testMOS.cpp    ../port/testMOS.h
  testMushra.cpp ../port/testMushra.h
  testTrain.cpp  ../port/testTrain.h
  transBuff.cpp  ../port/transBuff.h
  trialAudioFiles.cpp  ../port/trialAudioFiles.h
  utils.cpp      ../port/utils.h
  verify_signature.cpp
  wav.cpp        ../port/wav.h

  ../port/allTestIncludes.h
  ../port/defineValues.h
  ../port/message.h
  ../port/portControls.h
  ../port/step_cmdline.h
  ../port/sysFiles.h
  ../port/transition.h
  ../port/tty_interface.h
  ../port/video.h
)

target_link_libraries(step_port PUBLIC OpenSSL::Crypto portaudio_static)
target_include_directories(step_port PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../port
  ${CMAKE_CURRENT_SOURCE_DIR}/../Step
)
