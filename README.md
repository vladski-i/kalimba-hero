# Kalimba Hero

Is an OpenGL C application aiming to provide a similar experience to Synthesia for kalimba players

# Current status

- [x] minimal UI
- [x] tight timing
- [x] reading midi file to get bpm and notes
- [x] piano debug mode
- [x] interface to select midi file
- [ ] detection of notes played in real life
- [ ] stopping time when awaiting notes
- [ ] docker image for building the app
- [ ] own midi parser
- [ ] prettier UI
- [ ] pretty GUI for selecting midi file/track
- [ ] (maybe) send notes from other tracks via midi ports

# Building

You need OpenGL, GLUT, GLFW and `libjack-jackd2-dev` on your system

- cd to /.../kalimba-hero/midi-parser
- run `make` (so the simlink in `kalimba-hero/lib` resolves)
- cd to /.../kalimba-hero/kissfft
- run `make` (so the simlink in `kalimba-hero/lib` resolves)
- ensure that LD_LIBRARY_PATH contains /.../kalimba-hero/lib
- run `make` in kalimba-hero
