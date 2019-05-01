:run.bat

cd  ClientGUI/bin/debug

START ClientGUI.exe "localhost" "8082" "8080"
START ClientGUI.exe "localhost" "8083" "8080" "clien2"
cd ../../..


cd Debug/
START ServerPrototype.exe

cd ../../..

@echo:

cd ../..