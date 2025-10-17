set( TXT_UNICODE_CONVERT_REGEX "\\.utf8\\.txt$" )
set( TXT_UNICODE_CONVERT_REPLACE ".txt" )

set( SERVER_FILES
	dom/crossfire.dom
	dom/datacore.dom

	gamemodes/arcade.cfg
	gamemodes/arena.cfg
	gamemodes/ctf.cfg
	gamemodes/ctf2.cfg
	gamemodes/dom.cfg
	gamemodes/ffa.cfg
	gamemodes/ffax.cfg
	gamemodes/gamemodes.txt
	gamemodes/instagib.cfg
	gamemodes/lms.cfg
	gamemodes/lts.cfg
	gamemodes/sgbow.cfg
	gamemodes/tdm.cfg
	gamemodes/tdm2.cfg
	gamemodes/tdm3.cfg
	gamemodes/tdmx.cfg
	gamemodes/tournament.cfg

	gfx/detail/endworld/afloor1_4.tga
	gfx/detail/endworld/altar1_7.tga
	gfx/detail/endworld/ceiling5.tga
	gfx/detail/endworld/city1_4.tga
	gfx/detail/endworld/city4_2.tga
	gfx/detail/endworld/city5_3.tga
	gfx/detail/endworld/city5_8.tga
	gfx/detail/endworld/crate02b.tga
	gfx/detail/endworld/ground1_8.tga
	gfx/detail/endworld/met5_3.tga
	gfx/detail/endworld/metal1_2.tga
	gfx/detail/endworld/metal1_7.tga
	gfx/detail/endworld/metal2_2.tga
	gfx/detail/endworld/metal4_4.tga
	gfx/detail/endworld/metall1.tga
	gfx/detail/endworld/mmetal1_1.tga
	gfx/detail/endworld/rock5_2.tga
	gfx/detail/endworld/sfloor4_2.tga
	gfx/detail/endworld/wizwood1_3.tga

	gfx/env/destbk.tga
	gfx/env/destdn.tga
	gfx/env/destft.tga
	gfx/env/destlf.tga
	gfx/env/destrt.tga
	gfx/env/destup.tga
	gfx/env/fullmoonbk.tga
	gfx/env/fullmoondn.tga
	gfx/env/fullmoonft.tga
	gfx/env/fullmoonlf.tga
	gfx/env/fullmoonrt.tga
	gfx/env/fullmoonup.tga
	gfx/env/greenbk.tga
	gfx/env/greendn.tga
	gfx/env/greenft.tga
	gfx/env/greenlf.tga
	gfx/env/greenrt.tga
	gfx/env/greenup.tga

	locs/ag_crossfire.loc
	locs/boot_camp.loc
	locs/crossfire.loc
	locs/dm_dust.loc
	locs/elixir.loc
	locs/frenzy.loc
	locs/isotonic.loc
	locs/moss.loc
	locs/pwrcore.loc
	locs/scary_1.loc
	locs/stalkyard.loc
	locs/vengeance.loc
	locs/ag_hidden_lab.loc
	locs/boot_campx.loc
	locs/dabo.loc
	locs/dm_dust2.loc
	locs/endcamp.loc
	locs/gasworks.loc
	locs/lambda_bunker.loc
	locs/no_remorse.loc
	locs/rapidcore.loc
	locs/scary_2.loc
	locs/subtransit.loc
	locs/xbounce.loc
	locs/ag_park.loc
	locs/bounce.loc
	locs/darn.loc
	locs/doublecross.loc
	locs/endworld.loc
	locs/havoc.loc
	locs/last_call.loc
	locs/obsolete.loc
	locs/rats.loc
	locs/semonz.loc
	locs/thunderstruck.loc
	locs/xbounce2.loc
	locs/agony.loc
	locs/cabrito.loc
	locs/datacore.loc
	locs/echo.loc
	locs/farewell.loc
	locs/hlcs_italy.loc
	locs/lost_village.loc
	locs/olvidada_muerte.loc
	locs/rebellion.loc
	locs/snark_pit.loc
	locs/undertow.loc
	locs/awol.loc
	locs/cold_faces.loc
	locs/de_railroad.loc
	locs/eden.loc
	locs/fling.loc
	locs/homeworld.loc
	locs/lost_village2.loc
	locs/outcry.loc
	locs/rustmill.loc
	locs/stalkx.loc
	locs/urethane.loc

	maps/ag_crossfire.bsp
	maps/ag_crossfire.res
	maps/ag_hidden_lab.bsp
	maps/ag_hidden_lab.res
	maps/ag_park.bsp
	maps/ag_park.res
	maps/agony.bsp
	maps/agony.res
	maps/awol.bsp
	maps/awol.res
	maps/battlegrounds.bsp
	maps/boot_camp.bsp
	maps/boot_camp.res
	maps/boot_campx.bsp
	maps/boot_campx.res
	maps/bounce.bsp
	maps/bounce.res
	maps/cabrito.bsp
	maps/cabrito.res
	maps/cold_faces.bsp
	maps/cold_faces.res
	maps/crossfire.bsp
	maps/crossfire.res
	maps/cyanidestalkyard.bsp
	maps/dabo.bsp
	maps/dabo.res
	maps/darn.bsp
	maps/darn.res
	maps/datacore.bsp
	maps/datacore.res
	maps/de_railroad.bsp
	maps/de_railroad.res
	maps/dm_delve.bsp
	maps/dm_delve.res
	maps/dm_dust.bsp
	maps/dm_dust.res
	maps/dm_dust2.bsp
	maps/dm_dust2.res
	maps/dockingbay.bsp
	maps/doublecross.bsp
	maps/doublecross.res
	maps/echo.bsp
	maps/echo.res
	maps/eden.bsp
	maps/eden.res
	maps/elixir.bsp
	maps/elixir.res
	maps/endcamp.bsp
	maps/endcamp.res
	maps/endworld.bsp
	maps/endworld.res
	maps/farewell.bsp
	maps/farewell.res
	maps/fling.bsp
	maps/fling.res
	maps/frenzied.bsp
	maps/frenzy.bsp
	maps/frenzy.res
	maps/gasworks.bsp
	maps/gasworks.res
	maps/havoc.bsp
	maps/havoc.res
	maps/hlcs_italy.bsp
	maps/hlcs_italy.res
	maps/homeworld.bsp
	maps/homeworld.res
	maps/isotonic.bsp
	maps/isotonic.res
	maps/lambda_bunker.bsp
	maps/lambda_bunker.res
	maps/last_call.bsp
	maps/last_call.res
	maps/lost_village.bsp
	maps/lost_village.res
	maps/lost_village2.bsp
	maps/lost_village2.res
	maps/moss.bsp
	maps/moss.res
	maps/no_remorse.bsp
	maps/no_remorse.res
	maps/obsolete.bsp
	maps/obsolete.res
	maps/olvidada_muerte.bsp
	maps/olvidada_muerte.res
	maps/outcry.bsp
	maps/outcry.res
	maps/pwrcore.bsp
	maps/pwrcore.res
	maps/rapidcore.bsp
	maps/rapidcore.res
	maps/rats.bsp
	maps/rats.res
	maps/rebellion.bsp
	maps/rebellion.res
	maps/rustmill.bsp
	maps/rustmill.res
	maps/scary_1.bsp
	maps/scary_1.res
	maps/scary_2.bsp
	maps/scary_2.res
	maps/semonz.bsp
	maps/semonz.res
	maps/snark_pit.bsp
	maps/snark_pit.res
	maps/stalkx.bsp
	maps/stalkx.res
	maps/stalkyard.bsp
	maps/stalkyard.res
	maps/subtransit.bsp
	maps/subtransit.res
	maps/the_beach.bsp
	maps/the_tube.bsp
	maps/thunderstruck.bsp
	maps/thunderstruck.res
	maps/undertow.bsp
	maps/undertow.res
	maps/undyz.bsp
	maps/urethane.bsp
	maps/urethane.res
	maps/vengeance.bsp
	maps/vengeance.res
	maps/xbounce.bsp
	maps/xbounce.res
	maps/xbounce2.bsp
	maps/xbounce2.res

	models/bizl_fence1.mdl
	models/by_rubble_set.mdl
	models/chick.mdl
	models/feather.mdl
	models/palmtree.mdl
	models/pi_tree1.mdl
	models/pred_plant.mdl
	models/tree_noleaves2.mdl
	models/winebottle.mdl

	sound/ambience/guit1.wav
	sound/ambience/opera.wav
	sound/ambience/pulsemachine.wav
	sound/common/npc_step1.wav
	sound/common/npc_step2.wav
	sound/common/npc_step3.wav
	sound/common/npc_step4.wav
	sound/fvox/powermove_on.wav
	sound/items/gunpickup2.wav
	sound/misc/killchicken.wav
	sound/misc/sheep.wav
	sound/player/pl_dirt1.wav
	sound/player/pl_dirt2.wav
	sound/player/pl_dirt3.wav
	sound/player/pl_dirt4.wav
	sound/player/pl_duct1.wav
	sound/player/pl_duct2.wav
	sound/player/pl_duct3.wav
	sound/player/pl_duct4.wav
	sound/player/pl_fallpain3.wav
	sound/player/pl_grate1.wav
	sound/player/pl_grate2.wav
	sound/player/pl_grate3.wav
	sound/player/pl_grate4.wav
	sound/player/pl_ladder1.wav
	sound/player/pl_ladder2.wav
	sound/player/pl_ladder3.wav
	sound/player/pl_ladder4.wav
	sound/player/pl_metal1.wav
	sound/player/pl_metal2.wav
	sound/player/pl_metal3.wav
	sound/player/pl_metal4.wav
	sound/player/pl_pain2.wav
	sound/player/pl_slosh1.wav
	sound/player/pl_slosh2.wav
	sound/player/pl_slosh3.wav
	sound/player/pl_slosh4.wav
	sound/player/pl_step1.wav
	sound/player/pl_step2.wav
	sound/player/pl_step3.wav
	sound/player/pl_step4.wav
	sound/player/pl_swim1.wav
	sound/player/pl_swim2.wav
	sound/player/pl_swim3.wav
	sound/player/pl_swim4.wav
	sound/weapons/egon_off1.wav
	sound/weapons/egon_run3.wav
	sound/weapons/egon_windup2.wav

	sprites/flare1.spr

	Av3.wad
	cstrike.wad
	de_turkey.wad
	decals.wad
	gorge_cz.wad
	halflife.wad
	itsitaly.wad
	jos.wad
	opfor.wad

	startup_server_ctf.cfg
	startup_server_lms.cfg
	startup_server.cfg

	liblist.gam

	delta.lst
)

function(ag_install_files component_name file_list)
	set( converted_txt_files )

	foreach( rel_file_path IN LISTS file_list)
		cmake_path( HAS_PARENT_PATH rel_file_path has_parent_path )

		if( has_parent_path)
			cmake_path( GET rel_file_path PARENT_PATH parent_path )
		else()
			set( parent_path "." )
		endif()

		if( rel_file_path MATCHES ${TXT_UNICODE_CONVERT_REGEX})
			# Convert file into UTF-16 LE for VGUI2
			string( REGEX REPLACE ${TXT_UNICODE_CONVERT_REGEX} ${TXT_UNICODE_CONVERT_REPLACE} converted_rel_file_path ${rel_file_path} )

			file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${parent_path}" )
			set( file_to_copy "${CMAKE_CURRENT_BINARY_DIR}/${converted_rel_file_path}" )

			add_custom_command(
				OUTPUT ${file_to_copy}
				COMMAND
					Python::Interpreter
					"${CMAKE_SOURCE_DIR}/scripts/reencode_text_file.py"
					"${CMAKE_CURRENT_SOURCE_DIR}/${rel_file_path}"
					${file_to_copy}
				DEPENDS
					"${CMAKE_SOURCE_DIR}/scripts/reencode_text_file.py"
					"${CMAKE_CURRENT_SOURCE_DIR}/${rel_file_path}"
			)

			list( APPEND converted_txt_files ${file_to_copy} )
		else()
			# Copy the file as-is
			set( file_to_copy "${CMAKE_CURRENT_SOURCE_DIR}/${rel_file_path}" )
		endif()

		install(
			FILES ${file_to_copy}
			DESTINATION ${parent_path}
			COMPONENT ${component_name}
		)
	endforeach()

	if( converted_txt_files )
		# Create custom target to convert the files
		add_custom_target( convert_utf_files_${component_name} DEPENDS ${converted_txt_files} )
	endif()
endfunction()


function(ag_install_target component_name target_name install_dir)
	install(
		FILES $<TARGET_FILE:${target_name}>
		DESTINATION ${install_dir}
		COMPONENT ${component_name}
	)

	if( CMAKE_CXX_LINKER_SUPPORTS_PDB )
		install(
			FILES $<TARGET_PDB_FILE:${target_name}>
			DESTINATION ${install_dir}
			COMPONENT ${component_name}
			OPTIONAL
		)
	endif()
endfunction()

# Server
ag_install_files( server "${SERVER_FILES}" )
ag_install_target( server server "dlls" )

if( GENERATE_AMXX_OFFSETS )
	# Generate the install script
	configure_file(
		${CMAKE_SOURCE_DIR}/cmake/InstallAmxxOffsets.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/InstallAmxxOffsets.cmake
		@ONLY
	)

	# Install offset files
	install(
		SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/InstallAmxxOffsets.cmake
		COMPONENT server
	)
endif()

if ( GENERATE_HAMDATA_INI AND PLATFORM_LINUX )
	# Generate the install script
	configure_file(
		${CMAKE_SOURCE_DIR}/cmake/InstallHamdataIni.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/InstallHamdataIni.cmake
		@ONLY
	)

	# Install hamdata.ini file
	install(
		SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/InstallHamdataIni.cmake
		COMPONENT server
	)
endif()
