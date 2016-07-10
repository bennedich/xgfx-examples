# emscripten-renderer-examples
Examples using the [emscripten-renderer](https://github.com/bennedich/emscripten-renderer).

- `ex1` - Barebones rendering of a quad.
- `ex2` - Barebones fps camera navigation using `glm`.

## Build and run

Files from cmake/emcmake end up in subfolders of `./build/`.
<br/>
Files from make/emmake end up in subfolders of `./bin/`.

#### Web
From project root folder build and run first example in firefox:
```
shl/build_web.sh ex1
shl/run_web.sh ex1
```

#### Desktop
From project root folder build and run first example in osx:
```
shl/build_desktop.sh ex1
shl/run_desktop.sh ex1
```

## Troubleshooting

- WebGL stops working in Firefox.
  - Open Firefox `about:config` and reset `gfx.blacklist.webgl.opengl`.
  - Open Firefox `about:support` and click `Refresh Firefox...`.

<br/>
<p align="center">&#x02767;</p>