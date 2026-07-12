{
	'variables': {
		'bin': '<!(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printBin())")',
		'steam_include': '<!(node ../scripts/print-sdk-path.ts include)',
		'steam_redist': '<!(node ../scripts/print-sdk-path.ts redist)',
	},
	'targets': [
		{
			'target_name': 'steam-api',
			'includes': ['common.gypi'],
			'sources': [
				'cpp/apps.cpp',
				'cpp/bindings.cpp',
				'cpp/callbacks.cpp',
				'cpp/steam-api.cpp',
				'cpp/user-stats.cpp',
				'cpp/user.cpp',
				'cpp/utils.cpp',
			],
			'include_dirs': [
				'<!@(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printInclude())")',
				'<(steam_include)',
			],
			'conditions': [
				['OS=="linux"', {
					'libraries': [
						"-Wl,-rpath,'$$ORIGIN'",
						'<(steam_redist)/linux64/libsteam_api.so',
					],
				}],
				['OS=="mac"', {
					'libraries': [
						'-Wl,-rpath,@loader_path',
						'<(steam_redist)/osx/libsteam_api.dylib',
					],
				}],
				['OS=="win"', {
					'library_dirs': ['<(steam_redist)/win64'],
					'libraries': ['steam_api64.lib'],
				}],
			],
		},
	],
}
