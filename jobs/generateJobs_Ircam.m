datasets = {'50words','Adiac','ArrowHead','ARSim','Beef','BeetleFly',...
    'BirdChicken','Car','CBF','Coffee','Computers','Chlorine',...
    'CinECG','Cricket_X','Cricket_Y','Cricket_Z','DiatomSize',...
    'DistalPhalanxOutlineAgeGroup','DistalPhalanxOutlineCorrect',...
    'ECGFiveDays','Earthquakes','ElectricDevices','FaceAll','FaceFour',...
    'FacesUCR','Fish','FordA','FordB','Gun_Point','HandOutlines', ...
    'DistalPhalanxTW','Herring','LargeKitchenAppliances',...
    'Haptics','InlineSkate','ItalyPower','Lighting2',...
    'Lighting7','MALLAT','MedicalImages','MoteStrain','NonInv_ECG1',...
    'MiddlePhalanxOutlineAgeGroup','MiddlePhalanxOutlineCorrect',...
    'MiddlePhalanxTW','PhalangesOutlinesCorrect','Plane',...
    'ProximalPhalanxOutlineAgeGroup','ProximalPhalanxOutlineCorrect',...
    'ProximalPhalanxTW','RefrigerationDevices',...
    'NonInv_ECG2','OliveOil','OSULeaf','SonyAIBO1','SonyAIBO2',...
    'ScreenType','ShapeletSim','ShapesAll','SmallKitchenAppliances',...
    'StarLight','SwedishLeaf','Symbols','Synthetic','Trace',...
    'Two_Patterns','TwoLeadECG','uWGestureX','uWGestureY','uWGestureZ',...
    'ToeSegmentation1','ToeSegmentation2','Wafer','WordsSynonyms','Yoga'};
%
% Change queue name here
%
fprintf(fullSH, '#!/bin/bash\n');
sizesStr = cell(length(datasets), 1);
sizesVal = zeros(length(datasets), 1);
for i = 1:length(datasets)
   curDir = [dSetDir '/' datasets{i}];
   [~, out] = unix(['du -h ' curDir]);
   sizeS = strsplit(out, '\t');
   sizeS = sizeS{1};
   if (sizeS(end) == 'K')
       fact = 1;
   else
       fact = 1000;
   end
   valSize = str2double(sizeS(1:(end-1)));
   sizesVal(i) = valSize * fact;
   sizesStr{i} = sizeS;
end
[sizesVal, sizeID] = sort(sizesVal, 'ascend');
sizesStr = sizesStr(sizeID);
datasets = datasets(sizeID);
for i = 1:57
    fprintf('"%s" ', datasets{i});
%    curQueue = queueSelect(i);
%     fprintf(fullSH, 'qsub hvMots_%s.sh\n', datasets{i});
%     confID = fopen(['config' datasets{i}], 'w');
%     fprintf(confID, 'Directory\tdatasets\n');
%     fprintf(confID, datasets{i});
%     fprintf(confID, '\tTRAIN\t');
%     fprintf(confID, '%s/%s_TRAIN\t', datasets{i}, datasets{i});
%     fprintf(confID, '%s/%s_TEST\n', datasets{i}, datasets{i});
%     fprintf(confID, datasets{i});
%     fprintf(confID, '\tTRAIN\t');
%     fprintf(confID, '%s/%s_TRAIN\t', datasets{i}, datasets{i});
%     fprintf(confID, '%s/%s_TEST\n', datasets{i}, datasets{i});
%     fclose(confID);
%     confID = fopen(['hvMots_' datasets{i} '.sh'], 'w');
%     fprintf(confID, '#!/bin/bash\n');
%     fprintf(confID, '#\n');
%     fprintf(confID, '#PBS -N HV-%.10s\n', datasets{i});
%     fprintf(confID, '#PBS -e HV-%s.err.txt\n', datasets{i});
%     fprintf(confID, '#PBS -o HV-%s.out.txt\n', datasets{i});
%     fprintf(confID, '#PBS -l walltime=%s,select=1:ncpus=%d\n', wallTimes{curQueue}, cores(curQueue));
%     fprintf(confID, '#PBS -q %s\n', queueNames{curQueue});
%     fprintf(confID, 'cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config%s', datasets{i});
%     fclose(confID);
end
fprintf('\n');
for i = 58:length(datasets)
    fprintf('"%s" ', datasets{i});
end
fprintf('\n');