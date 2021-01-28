{
  "targets": [
    {
      "target_name": "NodeJS",
      "sources": [
        "libnodejs.cpp"
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