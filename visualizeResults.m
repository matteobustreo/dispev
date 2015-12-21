close all;
clear all;

path = 'DOCS/middlesbury_motorcycle_q/';

%% Reading results
dispMap         = dlmread([path 'im0.png_dispMap.txt']);
bestCorrVals    = dlmread([path 'im0.png_bestCorrVals.txt']);
gt              = imread([path 'disp0GT.png']);
mask0nocc       = imread([path 'mask0nocc.png']);

% Converting disparity map
dispMapU8 = uint8(dispMap);
immElNum = size(gt,1)*size(gt,2);

% Calculating the holes area
holePxNum = sum(sum(dispMapU8==0));
holePerc = 100*holePxNum/immElNum;
disp(['holePerc: ' num2str(holePerc)]);

%% Evaluating the error (bigger than 1px)
difference = gt-dispMapU8;
errImm = difference>1;
imwrite(errImm, [path 'errImm.png']);

% Calculating the percentage error
wrongPxNum = sum(sum(errImm));
wrongPerc = 100*wrongPxNum/immElNum;
disp(['wrongPerc: ' num2str(wrongPerc)]);

% Calculating the mean error
meanErr = sum(sum(difference))/immElNum;
disp(['meanErr: ' num2str(meanErr)]);

%% Evaluating the error in non occluded area (bigger than 1px)
differenceNO = gt-dispMapU8;
immO = mask0nocc<200;
errImmNO = (errImm - immO)>0;
imwrite(errImmNO, [path 'errImmNO.png']);

% Calculating the percentage error
wrongPxNONum = sum(sum(errImmNO));
immElNONum = sum(sum(mask0nocc>200));
wrongPercNO = 100*wrongPxNONum/immElNONum;
disp(['wrongPercNO: ' num2str(wrongPercNO)]);

%% Visualizing results
figure(1)
subplot(1,2,1)
imshow(dispMapU8);
title ('Calculated Disparity Map')
subplot(1,2,2)
imshow(gt);
title ('Ground Truth')

figure(2)
subplot(1,2,1)
imshow(errImm);
title ('Pixels with error > 1px')
subplot(1,2,2)
imshow(errImmNO);
title(sprintf('Pixels with error > 1px \n(non occluded only)'))



