@echo off

://////////////////////////////////////////////////////////

SET learning=rem
SET test=

SET i=20
SET j=20
SET count=100

SET bmpdir=image\
SET bmpfile1=ham
SET bmpfile2=hamR
SET bmpfile3=marblemachine
SET bmpfile4=marblemachine
SET bmpfile5=chart
SET numfile=1

SET hidden=3
SET CARNEL=6
SET CARNEL2=1

://////////////////////////////////////////////////////////

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
	SET teachbmp=%bmpdir%%bmpfile1%-1.bmp %bmpdir%%bmpfile1%01.bmp
)
if %numfile%==2 (
	SET bmpfileset=%bmpfile1% %bmpfile2%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%
	SET teachbmp=%bmpdir%%bmpfile1%-1.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-1.bmp %bmpdir%%bmpfile2%01.bmp
)
if %numfile%==3 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%
	SET teachbmp=%bmpdir%%bmpfile1%-1.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-1.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-1.bmp %bmpdir%%bmpfile3%01.bmp
)
if %numfile%==4 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3% %bmpfile4%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%+%bmpfile4%
	SET teachbmp=%bmpdir%%bmpfile1%-1.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-1.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-1.bmp %bmpdir%%bmpfile3%01.bmp %bmpdir%%bmpfile4%-1.bmp %bmpdir%%bmpfile4%01.bmp
)
if %numfile%==5 (
	SET bmpfileset=%bmpfile1% %bmpfile2% %bmpfile3% %bmpfile4% %bmpfile5%
	SET bmpfileset2=%bmpfile1%+%bmpfile2%+%bmpfile3%+%bmpfile4%+%bmpfile5%
	SET teachbmp=%bmpdir%%bmpfile1%-1.bmp %bmpdir%%bmpfile1%01.bmp %bmpdir%%bmpfile2%-1.bmp %bmpdir%%bmpfile2%01.bmp %bmpdir%%bmpfile3%-1.bmp %bmpdir%%bmpfile3%01.bmp %bmpdir%%bmpfile4%-1.bmp %bmpdir%%bmpfile4%01.bmp %bmpdir%%bmpfile5%-1.bmp %bmpdir%%bmpfile5%01.bmp
)

%learning% Release\QNN_train -s 0 -r %i% -x %CARNEL% -y %CARNEL2% -d %hidden% -l %count% -n %numfile% %bmpfileset% %bmpfileset2% %teachbmp%

SET testfile=ham
%test% Release\QNN_test.exe -i %CARNEL% -m %hidden% -o %CARNEL2% weight\weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

pause 









SET testfile=fukusou03
%test% Release\QNN_test.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=fukusou07
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp
pause








SET testfile=trafic3
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=trafic4
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=tokunaga
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=Bsub
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=load
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=track
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=trafic
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=trafic2
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=trafic
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=trafic2
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=SII
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

SET testfile=ND
%test% qmlp_test%INPUT%-%OUTPUT%.exe weight_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.txt image\%testfile%-1.bmp %testfile%_%bmpfileset2%_%INPUT%-%hidden%-%OUTPUT%_c%count%_r%j%.bmp

pause