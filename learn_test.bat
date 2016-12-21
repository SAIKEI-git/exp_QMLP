@echo off

://////////////////////////////////////////////////////////

SET learning=
SET test=

SET rate=20
SET count=500

SET bmpdir=image\
SET bmpfile1=ham
SET bmpfile2=chart
SET bmpfile3=SII
SET bmpfile4=marblemachine
SET bmpfile5=chart
SET numfile=1
SET shuffle=1
SET gloomy=2

SET CARNEL=3
SET hidden=3
SET CARNEL2=1

://///////////////C/////////////////////////////////////////

if %CARNEL2%==1 (
	SET OUTPUT=1
)
if %CARNEL%==2 (
	SET INPUT=4
)
if %CARNEL2%==2 (
	SET OUTPUT=4
)
if %CARNEL%==3 (
	SET INPUT=9
)
if %CARNEL2%==3 (
	SET OUTPUT=9
)
if %CARNEL%==4 (
	SET INPUT=16
)
if %CARNEL2%==4 (
	SET OUTPUT=16
)
if %CARNEL%==5 (
	SET INPUT=25
)
if %CARNEL2%==5 (
	SET OUTPUT=25
)
if %CARNEL%==6 (
	SET INPUT=36
)

if %numfile%==1 (
	SET bmpfileset=%bmpfile1%
	SET bmpfileset2=%bmpfile1%
	SET teachbmp=%bmpdir%%bmpfile1%-%gloomy%.bmp %bmpdir%%bmpfile1%01.bmp
)
if %numfile%==2 (
	SET bmpfileset=%bmpfile1% %bmpfile2%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%
	SET teachbmp=%bmpdir%%bmpfile1%-%gloomy%.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-%gloomy%.bmp %bmpdir%%bmpfile2%01.bmp
)
if %numfile%==3 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%
	SET teachbmp=%bmpdir%%bmpfile1%-%gloomy%.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-%gloomy%.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-%gloomy%.bmp %bmpdir%%bmpfile3%01.bmp
)
if %numfile%==4 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3% %bmpfile4%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%+%bmpfile4%
	SET teachbmp=%bmpdir%%bmpfile1%-%gloomy%.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-%gloomy%.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-%gloomy%.bmp %bmpdir%%bmpfile3%01.bmp %bmpdir%%bmpfile4%-%gloomy%.bmp %bmpdir%%bmpfile4%01.bmp
)
if %numfile%==5 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3% %bmpfile4% %bmpfile5%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%+%bmpfile4%+%bmpfile5%
	SET teachbmp=%bmpdir%%bmpfile1%-%groomy%.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-1.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-1.bmp %bmpdir%%bmpfile3%01.bmp %bmpdir%%bmpfile4%-1.bmp %bmpdir%%bmpfile4%01.bmp %bmpdir%%bmpfile5%-1.bmp %bmpdir%%bmpfile5%01.bmp
)
:*************************
: lerning
:*************************

SET prams=-s 0 -r %rate% -x %CARNEL% -y %CARNEL2% -d %hidden% -l %count% -n %numfile% -R %shuffle% -g %groomy%

echo qmlp_train.exe %prams% %bmpfileset% %bmpfileset2% %teachbmp%
%learning% qmlp_train.exe %prams% %bmpfileset% %bmpfileset2% %teachbmp%

:*************************
: testing
:*************************

SET prams=-i %CARNEL% -m %hidden% -o %CARNEL2%
SET weight=weight\weight_%bmpfileset2%-%groomy%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%rate%_sfl%shuffle%_.txt
SET output=%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%rate%_s%shuffle%_g%gloomy%.bmp

SET testfile=testdata
echo qmlp_test.exe %prams% %weight% image\%testfile%-%gloomy%.bmp %testfile%_%output%

SET testfile=ham
%test% qmlp_test.exe %prams% %weight% image\%testfile%-%gloomy%.bmp %testfile%_%output%
SET testfile=chart
%test% qmlp_test.exe %prams% %weight% image\%testfile%-%gloomy%.bmp %testfile%_%output%
SET testfile=SII
%test% qmlp_test.exe %prams% %weight% image\%testfile%-%gloomy%.bmp %testfile%_%output%
SET testfile=ND
%test% qmlp_test.exe %prams% %weight% image\%testfile%-%gloomy%.bmp %testfile%_%output%

pause 
