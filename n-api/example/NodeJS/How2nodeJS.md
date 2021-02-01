1) generate a python template
2) use the lib____.c,h, i and exos_____.h file
3) create a binding.gyp file
{
  "targets": [
    {
      "target_name": "NodeJS",
      "sources": [
        "lib____.c",
        "lib_____wrap.cpp"
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
4) run command "swig -c++ -javascript -node -o lib_____wrap.cpp lib_____.i
5) run command "node-gyp rebuild"
Now the executable NodeJS module is in subfolder /build/Release/  as the ____.node file. This can now be included in the NodeJS code. 
Example:

const myModule = require('./NodeJS/build/Release/NodeJS');