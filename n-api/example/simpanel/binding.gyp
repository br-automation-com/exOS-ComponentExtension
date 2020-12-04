{
  "targets": [{
    "target_name": "simpanel",
    "sources": [
      "simpanel.c"
    ],
    'link_settings': {
        'libraries': [
          '-lexos-api',
          '-lzmq'
        ]
      },
  }]
}
