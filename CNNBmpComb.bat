echo off

SET teach=ham

SET option=_%teach%_9-4-1_c1000_r20
SET shuffle=1


SET basebmp=ham
falseCNN bmpoutput\%basebmp%%option% %shuffle%
SET basebmp=chart
falseCNN bmpoutput\%basebmp%%option% %shuffle%
SET basebmp=SII
falseCNN bmpoutput\%basebmp%%option% %shuffle%
SET basebmp=ND
falseCNN bmpoutput\%basebmp%%option% %shuffle%
SET basebmp=marblemachine
falseCNN bmpoutput\%basebmp%%option% %shuffle%

SET bmpdir=image\
SET basebmp=%option%
echo %basebmp%

SET testfile=ham
echo %testfile%
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%.bmp original-NN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-CNN.bmp original-CNN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-s%shuffle%-CNN.bmp original-CNN

SET testfile=chart
echo %testfile%
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%.bmp original-NN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-CNN.bmp original-CNN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-s%shuffle%-CNN.bmp original-CNN

SET testfile=SII
echo %testfile%
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%.bmp original-NN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-CNN.bmp original-CNN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-s%shuffle%-CNN.bmp original-CNN

SET testfile=ND
echo %testfile%
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%.bmp original-NN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-CNN.bmp original-CNN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-s%shuffle%-CNN.bmp original-CNN
pause

SET testfile=marblemachine
echo %testfile%
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%.bmp original-NN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-CNN.bmp original-CNN
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%%basebmp%-s%shuffle%-CNN.bmp original-CNN

pause