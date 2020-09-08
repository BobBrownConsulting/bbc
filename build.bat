cd ext\boost\
call bootstrap.bat
b2.exe --with-system link=static threading=multi toolset=msvc address-model=64
b2.exe --with-thread link=static threading=multi toolset=msvc address-model=64
b2.exe --with-log link=static threading=multi toolset=msvc address-model=64
cd ..\..\
