{
  "targets": [{
    "target_name": "l_MyApp",
    "sources": [
      "libmyapp.c"
    ],
    'link_settings': {
        'libraries': [
          '-lexos-api',
          '-lzmq'
        ]
      },
  }]
}
