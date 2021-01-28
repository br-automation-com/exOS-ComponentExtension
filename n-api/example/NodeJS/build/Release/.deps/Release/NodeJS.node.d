cmd_Release/NodeJS.node := ln -f "Release/obj.target/NodeJS.node" "Release/NodeJS.node" 2>/dev/null || (rm -rf "Release/NodeJS.node" && cp -af "Release/obj.target/NodeJS.node" "Release/NodeJS.node")
