# Kalimba Hero

Is an OpenGL C application aiming to provide a similar experience to Synthesia for kalimba players

# Current status

- [x] tight timing
- [x] reading midi file to get bpm and notes
- [x] piano debug mode
- [ ] interface to select midi file
- [ ] detection of notes played in real life
- [ ] stopping time when awaiting notes
- [ ] docker image for building the app
- [ ] own midi parser

# Building

You need OpenGL, GLUT, GLFW on your system

- cd to /.../kalimba-hero/midi-parser
- run `make` (so the simlink in `kalimba-hero/lib` resolves)
- run `make` in kalimba-hero
