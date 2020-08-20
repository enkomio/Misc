@echo off
cls

@rem install fake
dotnet tool install fake-cli --tool-path fake

".\fake\fake.exe" run build.fsx %*