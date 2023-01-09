How to create windows installer

1. Install NSIS
2. Compile texttrainer
3. Copy texttrainer.exe from build directory to texttrainer/bin/
4. Use windeployqt.exe tool from the bin folder of the qt installation, with target texttrainer/bin
5. Copy following dlls to this directory (texttrainer/windows):
	libgcc_s_dw2-1.dll
	libstdc++-6.dll
	libwinpthread-1.dll
6. Run NSIS on texttrainer.nsis
