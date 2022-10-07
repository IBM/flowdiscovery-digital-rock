set(debug ${CMAKE_SOURCE_DIR}/bin/digital-rock.x.dSYM
          ${CMAKE_SOURCE_DIR}/bin/digital-rock-test.x.dSYM
          ${CMAKE_SOURCE_DIR}/bin/digital-rock-stress-test.x.dSYM
)

foreach(folder ${debug})
  if (EXISTS ${folder})
     file(REMOVE_RECURSE ${folder})
  endif()
endforeach(folder)
