project('Rush', 'cpp',
  version : '1.0',
  default_options : [
    'cpp_std=c++17',
    'prefix=/usr/local',  # Important for Nix compatibility
    'libdir=lib',
    'bindir=bin'
  ]
)

# SFML dependencies
sfml_graphics = dependency('sfml-graphics')
sfml_window = dependency('sfml-window')
sfml_system = dependency('sfml-system')
sfml_audio = dependency('sfml-audio')
sfml_network = dependency('sfml-network')

cxxopts = dependency('cxxopts')

# List all source files
sources = [
  'BoomAnim.cpp',
  'Button.cpp',
  'CollisionManager.cpp',
  'Entity.cpp',
  'Help.cpp',
  'main.cpp',
  'Menu.cpp',
  'Rules.cpp'
]

# List all header files (for IDE support)
headers = [
  'BoomAnim.h',
  'Button.h',
  'CollisionManager.h',
  'Entity.h',
  'Help.h',
  'Menu.h',
  'Rules.h'
]

# Déclaration des assets à installer
assets = [
  'fly.ogg',
  'land.ogg',
  'demos_rule_shoot.png',
  'shoot.ogg',
  'home.png',
  'touch_shoot.ogg',
  'bomb2.ogg',
  'demos_rule_degat.png',
  'BebasNeue-Regular.otf',
  'Pixel-Noir Caps.ttf',
  'Fipps-Regular.otf',
  'demos_rule_land.png'
]

full_assetdir = get_option('prefix') / get_option('assetdir')

conf_data = configuration_data()
conf_data.set('assetdir', full_assetdir)
configure_file(input : 'Config.h.in',
               output : 'Config.h',
               configuration : conf_data)

# Build executable
executable('Rush',
  sources,
  dependencies: [sfml_graphics, sfml_window, sfml_system, sfml_audio, sfml_network, cxxopts],
  install : true
)

# Installation des assets
install_data(
  assets,
  install_dir: full_assetdir
)
