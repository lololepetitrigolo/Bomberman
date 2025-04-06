project('bomberman', 'c')

sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('sdl2_image')
sdl2_ttf_dep = dependency('sdl2_ttf')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m')

cc = meson.get_compiler('c')

deps = [
        sdl2_dep,
        sdl2_image_dep,
        sdl2_ttf_dep,
        m_dep,
]

src = files([
        'main.c',
        'message.c',
        'network.c',
        'player.c',
        'grid.c',
        'graphic.c',
        'game.c',
        'collision.c',
        'powerup.c',
        'teleporter.c',
        'player_list.c',
        'bomb_list.c',
        'debug.c',
        'bomb.c'
        ])

executable('bomberman',
        src,
        dependencies : deps)
