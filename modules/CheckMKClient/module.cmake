IF(HAVE_LUA)
	SET (BUILD_MODULE 1)
ELSE(HAVE_LUA)
	MESSAGE(STATUS "Disabling since Lua was not foun")
ENDIF(HAVE_LUA)
