# simple-audio-player-smtc

Interface between simple-audio-player and the Windows Runtime System Media Transport Control API. This allow the simple-audio-player to send status about playback status and informations and also receive media keys press events.

## Compilation

You need to compile it using the Visual Studio solution to the same compilation type and architecture of the simple-audio-player project, and copy this files into dependencies/simple-audio-player-smtc of the simple-audio-player root directory (buildOutput must be changed with the real binary output) :
   - simple-audio-player-smtc/SAP_SMTC.h
   - *buildOutput*/simple-audio-player-smtc.lib
   - *buildOutput*/simple-audio-player-smtc.dll  
  
  If you do not want or can't compile simple-audio-player-smtc, you can disable it with the cmake option **USE_SMTC** `-DUSE_SMTC=off`. You will still be able to control playback with the media keys.

## License

Simple-audio-player-smtc is licensed under the **MIT** license. Check the [LICENSE](LICENSE) file.