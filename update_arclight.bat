@echo off

mkdir updtemp
cd updtemp
git clone https://github.com/Arclight-Team/Arclight.git --depth 1

cd ..

move src\arclight\core\net updtemp\net
rmdir /s /q src\arclight
move updtemp\Arclight\src\arclight src\arclight
move updtemp\net src\arclight\core\net
rmdir /s /q updtemp

echo Arclight updated
