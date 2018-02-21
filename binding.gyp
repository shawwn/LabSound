{
  'targets': [
    {
      'target_name': 'labsound',
      'sources': [
        'main.cpp',
      ],
      'include_dirs': [
        '<(module_root_dir)/include',
      ],
      'library_dirs': [
        '<(module_root_dir)/osx/build/Release',
      ],
      'libraries': [
        '-llabsound',
      ],
      'link_settings': {
        'libraries': [
        ],
      },
      'copies': [
        {
          'destination': '<(module_root_dir)/build/Release/',
          'files': [
          ]
        }
      ],
    }
  ]
}

