set( TARGET_NAME server )

set( SOURCE_FILES
	${GAME_COMMON_SRCS}
	CMakeLists.txt

	wpn_shared/hl_wpn_glock.cpp
	
	activity.h
	activitymap.h	
	aflock.cpp
	agrunt.cpp
	airtank.cpp
	AmxxOffsetClassList.txt
	animating.cpp
	animation.cpp
	animation.h
	apache.cpp
	barnacle.cpp
	barney.cpp
	basemonster.h
	bigmomma.cpp
	bloater.cpp
	bmodels.cpp
	bullsquid.cpp
	buttons.cpp
	cbase.cpp
	cbase.h
	cdll_dll.h
	client.cpp
	client.h
	combat.cpp
	controller.cpp
	crossbow.cpp
	crowbar.cpp
	cvar.cpp
	cvar.h
	decals.h
	defaultai.cpp
	defaultai.h
	doors.cpp
	doors.h
	effects.cpp
	effects.h
	egon.cpp
	enginecallback.h
	explode.cpp
	explode.h
	extdll.h
	flyingmonster.cpp
	flyingmonster.h
	func_break.cpp
	func_break.h
	func_tank.cpp
	game.cpp
	gamerules.cpp
	gamerules.h
	gargantua.cpp
	gauss.cpp
	genericmonster.cpp
	ggrenade.cpp
	globals.cpp
	gman.cpp
	h_ai.cpp
	h_battery.cpp
	h_cine.cpp
	h_cycler.cpp
	h_export.cpp
	handgrenade.cpp
	hassassin.cpp
	headcrab.cpp
	healthkit.cpp
	hgrunt.cpp
	hornet.cpp
	hornet.h
	hornetgun.cpp
	houndeye.cpp
	ichthyosaur.cpp
	islave.cpp
	items.cpp
	leech.cpp
	lights.cpp
	maprules.cpp
	monstermaker.cpp
	monsters.cpp
	monsters.h
	monsterstate.cpp
	mortar.cpp
	mp5.cpp
	multiplay_gamerules.cpp
	nihilanth.cpp
	nodes.cpp
	nodes.h
	observer.cpp
	osprey.cpp
	pathcorner.cpp
	plane.cpp
	plane.h
	plats.cpp
	player.cpp
	player.h
	python.cpp
	rat.cpp
	roach.cpp
	rpg.cpp
	satchel.cpp
	saverestore.h
	schedule.cpp
	schedule.h
	scientist.cpp
	scripted.cpp
	scripted.h
	scriptevent.h
	shotgun.cpp
	singleplay_gamerules.cpp
	skill.cpp
	skill.h
	sound.cpp
	soundent.cpp
	soundent.h
	spectator.cpp
	spectator.h
	squadmonster.cpp
	squadmonster.h
	squeakgrenade.cpp
	subs.cpp
	talkmonster.cpp
	talkmonster.h
	teamplay_gamerules.cpp
	teamplay_gamerules.h
	tempmonster.cpp
	tentacle.cpp
	trains.h
	triggers.cpp
	tripmine.cpp
	turret.cpp
	util.cpp
	util.h
	voice_gamemgr.cpp
	voice_gamemgr.h
	weapons.cpp
	weapons.h
	world.cpp
	xen.cpp
	zombie.cpp

	agadmin.cpp
	agadmin.h
	agadmincache.cpp
	agadmincache.h
	agarena.cpp
	agarena.h
	agclient.cpp
	agclient.h
	agcommand.cpp
	agcommand.h
	agctf.cpp
	agctf.h
	agdom.cpp
	agdom.h
	agflood.cpp
	agflood.h
	aggame.cpp
	aggame.h
	aggamemode.cpp
	aggamemode.h
	aggamerules.cpp
	aggamerules.h
	agglobal.cpp
	agglobal.h
	aginfointermission.cpp
	aginfointermission.h
	aglms.cpp
	aglms.h
	agmatch.cpp
	agmatch.h
	agrandom.cpp
	agrandom.h
	agscore.cpp
	agscore.h
	agscorecache.cpp
	agscorecache.h
	agscorelog.cpp
	agscorelog.h
	agsettings.cpp
	agsettings.h
	agspectator.cpp
	agsuddendeath.cpp
	agsuddendeath.h
	agtimeout.cpp
	agtimeout.h
	agtimer.cpp
	agtimer.h
	agvote.cpp
	agvote.h
	spawnchooser.cpp
	spawnchooser.h
	speedrunstats.cpp
	speedrunstats.h
)

if( PLATFORM_WINDOWS )
	set( SOURCE_FILES
		${SOURCE_FILES}
		ag.def
	)
endif()

source_group( TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES} )

add_library( ${TARGET_NAME} SHARED ${SOURCE_FILES} )
add_auto_deploy( ${TARGET_NAME} server PublishPathServer.txt )

target_include_directories( ${TARGET_NAME} PRIVATE
	.
	${GAME_COMMON_INCLUDE_PATHS}
	${SOURCE_SDK_INCLUDE_PATHS} # For mathlib
)

target_compile_definitions( ${TARGET_NAME} PRIVATE
	${GAME_COMMON_DEFINES}
	${SOURCE_SDK_DEFINES}
	SERVER_DLL
	MATHLIB_USE_C_ASSERT
	MATHLIB_VECTOR_NONTRIVIAL
)

# Dependencies
if( WIN32 )
	set( SERVER_PLAT_LIBS )
else()
	set( SERVER_PLAT_LIBS
		dl
	)
endif()

# Linker flags
set( SERVER_LINK_FLAGS "")
set( SERVER_COMPILE_FLAGS "")

set_target_properties( ${TARGET_NAME} PROPERTIES
	COMPILE_FLAGS "${SERVER_COMPILE_FLAGS}" 
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${SERVER_LINK_FLAGS}"
)

# Remove "lib" prefix
set_target_properties( ${TARGET_NAME} PROPERTIES PREFIX "" )

# Set the original name
set_target_properties( ${TARGET_NAME} PROPERTIES OUTPUT_NAME "ag" )

if( GENERATE_AMXX_OFFSETS )
	set( AMXX_OFFSET_GENERATOR_VERSION "1.0.1" )

	set( AMXX_OFFSET_GENERATOR_DIR "${CMAKE_BINARY_DIR}/amxx-offset-generator" )
	set( AMXX_OFFSETS_SCRIPT "${AMXX_OFFSET_GENERATOR_DIR}/create_amxx_files.py" )
	set( AMXX_OFFSETS_CLASS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/AmxxOffsetClassList.txt" )
	set( AMXX_OFFSETS_OUT "${CMAKE_BINARY_DIR}/amxx-offsets" )

	# Check if platform is supported
	if( PLATFORM_WINDOWS )
		set( AMXX_OFFSETS_EXEC "${AMXX_OFFSET_GENERATOR_DIR}/win64/OffsetExporter.Pdb.exe" )
		set( AMXX_OFFSETS_DBG_FILE $<TARGET_PDB_FILE:server> )
		set( AMXX_OFFSETS_DBG_FILE_ARG --pdb )
		set( AMXX_OFFSETS_JSON "${CMAKE_BINARY_DIR}/ag-amxx-offsets-windows.json" )
		set( AMXX_OFFSETS_SCRIPT_ARG "--windows=${AMXX_OFFSETS_JSON}" )
	elseif( PLATFORM_LINUX )
		set( AMXX_OFFSETS_EXEC "${AMXX_OFFSET_GENERATOR_DIR}/linux64/OffsetExporter.Dwarf" )
		set( AMXX_OFFSETS_DBG_FILE $<TARGET_FILE:server> )
		set( AMXX_OFFSETS_DBG_FILE_ARG --so )
		set( AMXX_OFFSETS_JSON "${CMAKE_BINARY_DIR}/ag-amxx-offsets-linux.json" )
		set( AMXX_OFFSETS_SCRIPT_ARG "--linux=${AMXX_OFFSETS_JSON}" )
	else()
		message( FATAL_ERROR "GENERATE_AMXX_OFFSETS is not supported on this platform" )
	endif()

	# Download the release
	FetchContent_Declare(
		amxx-offset-generator
		URL https://github.com/tmp64/amxx-offset-generator/releases/download/v${AMXX_OFFSET_GENERATOR_VERSION}/amxx-offset-generator-v${AMXX_OFFSET_GENERATOR_VERSION}.zip
		URL_HASH SHA256=8fb1b0dd311cdbade408621bad66bf58c2e3472a8c68d3126a5b7b1b26c6da8e
		SOURCE_DIR ${AMXX_OFFSET_GENERATOR_DIR}
	)

	FetchContent_MakeAvailable( amxx-offset-generator )

	if( PLATFORM_LINUX )
		# File doesn't have +x set in the ZIP. Fix that.
		file(
			CHMOD ${AMXX_OFFSETS_EXEC}
			PERMISSIONS
				OWNER_READ OWNER_WRITE OWNER_EXECUTE
				GROUP_WRITE GROUP_EXECUTE
				WORLD_READ WORLD_EXECUTE
		)
	endif()

	# Add command to generate the JSON file and the offset files
	add_custom_command(
		TARGET server POST_BUILD
		COMMAND
			${AMXX_OFFSETS_EXEC}
			--class-list ${AMXX_OFFSETS_CLASS_LIST}
			${AMXX_OFFSETS_DBG_FILE_ARG} ${AMXX_OFFSETS_DBG_FILE}
			--out ${AMXX_OFFSETS_JSON}
		COMMAND
			Python::Interpreter
			${AMXX_OFFSETS_SCRIPT}
			${AMXX_OFFSETS_SCRIPT_ARG}
			--out ${AMXX_OFFSETS_OUT}
			--banner "AG offset file. Generated using amxx-offset-generator."
			--file-prefix "ag"
		COMMENT "Generating AMXX offsets"
	)

	# Recompile when any of that changes
	target_sources( server PRIVATE
		${AMXX_OFFSETS_EXEC}
		${AMXX_OFFSETS_CLASS_LIST}
	)
endif()

if( GENERATE_HAMDATA_INI AND PLATFORM_LINUX )
	set( HAMDATA_GENERATOR_REPO_DIR "${CMAKE_BINARY_DIR}/hamdata_ini_generator" )
	set( HAMDATA_GENERATOR_DIR "${HAMDATA_GENERATOR_REPO_DIR}/target/release" )
	set( HAMDATA_GENERATOR_EXEC "${HAMDATA_GENERATOR_DIR}/vtable" )

	FetchContent_Declare(
        hamdata_ini_generator
        GIT_REPOSITORY https://github.com/ag-community/hamdata_ini_generator
        SOURCE_DIR ${HAMDATA_GENERATOR_REPO_DIR}
    )

    FetchContent_MakeAvailable( hamdata_ini_generator )

	add_custom_command(
		TARGET server POST_BUILD
		COMMAND make -C ${HAMDATA_GENERATOR_REPO_DIR} release
		COMMENT "Compiling hamdata.ini generator"
	)

	add_custom_command(
		TARGET server POST_BUILD
		COMMAND ${HAMDATA_GENERATOR_EXEC} vtable-dump $<TARGET_FILE:server>
		WORKING_DIRECTORY ${HAMDATA_GENERATOR_DIR}
		COMMENT "Generating hamdata.ini"
	)
endif()