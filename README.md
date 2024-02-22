Whenever I wanted to learn a new language, I seem to struggle with what to build. I thought back to when I first starting writing code and remember my first real ambitious attepmpt was to create a TETRIS clone in BASIC. I realized now that was an excellent way to start. TETRIS has well defined rules, requires minimal graphic capability, and presents enough problems to solve to ensure you capture a great deal of concepts in a language.

So this project will be a collection of the same ported TETRIS clone written in different languages across different platforms. There are a few constraints I will adhere to:
1) terminal or lowest level of rendering only - I tried with an other language a while back using OpenGL and found I spent more time working out pipeline/rendering issues related to the OpenGL API instead of building the damn game. So, we're going low level rendering
2) maintain general architecture of the game, unless the platform can't handle it - You get the game working once and you know what works, you stick with it. I'll be sticking with that as close as possible, but there will be exceptions, but there's really no need to write the game completely from scratch without thinking back to previous designs.
3) minimal features - pieces should move/drop, be constrianed within the container. Score, level and next piece should be visible. Cleared rows should collapse. Game over is shown when you lose. As fun as it may be to add more to this (3D graphics, network play, sound..), keeping the scope small so I can complete and move to the next language is key.

Languages completed:
* Javascript - written to run in NodeJS in the terminal and in a browser sharing the same code base.
* C - Very familiar with C, but wanted to use this to learn how to implement more complex ideas in a purely procedural language.
* C++ - Haven't seriously touched c++ in 20 years; I last used the C++98 standard and was thrown into the future by building with C++23.

Future Languages and platforms:
* Rust - I attempted this is Rust before, never completed it and lost the code (probably for the best). Now with C++ completed, I feel this may be more approachable now.
* Zig - Zig is pretty awesome and has many features that improve upon my C implementation without going full blown C++.
* Arduino - Years ago I created a PONG clone that runs on an Arduino Uno that actually generated a VGA video signal plus sound. I'd like to take what I learned from my C TETRIS implementation and improve upon my PONG code. I'd like to still output a VGA signal, but will likely create forks trying to implement SPI and/or i2c and connect to different outputs.
* FORTRAN - Only because a friend dared me to.
* Java - I haven't touched Java in a while and though it's similar enough to other languages I use, it's changes much since I last used it.

