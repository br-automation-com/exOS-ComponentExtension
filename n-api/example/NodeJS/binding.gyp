{
  "targets": [
    {
      "target_name": "NodeJS",
      "sources": [
        "libnodejs.c",
        "libnodejs_wrap.cpp"
      ],
      "include_dirs": [
          '/usr/include'
      ],  
      'link_settings': {
          'libraries': [
            '-lexos-api',
            '-lzmq'
          ]
      }    
    }
  ]
}