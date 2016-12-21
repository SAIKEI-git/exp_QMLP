@echo off

SET bmpdir=image\
SET teachname=ham+chart_9-3-1_c1000_r20

SET testfile=ham
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s0.bmp no
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s1.bmp rand

SET testfile=chart
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s0.bmp no
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s1.bmp rand

SET testfile=SII
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s0.bmp no
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s1.bmp rand

SET testfile=ND
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s0.bmp no
psnr.exe %bmpdir%%testfile%01.bmp bmpoutput\%testfile%_%teachname%_s1.bmp rand

pause