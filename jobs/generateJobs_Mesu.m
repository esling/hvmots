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
sort(datasets);
for i = 1:length(datasets)
    fprintf('%s\n', datasets{i});
end
return;
%
% Change queue name here
%
dSetDir = '/Users/esling/Research/Coding/HV-MOTS-Univariate/datasets_Raw';
queueNames = {'f16c', 'f32c', 'f48c', 'f64c', 'p_f128c', 'np_f64c', 'np_f128c', 'np_f256c'};
wallTimes = {'01:00:00', '06:00:00', '12:00:00', '04:00:00', '04:00:00', '12:00:00', '12:00:00', '12:00:00'};
cores = [16, 32, 48, 64, 128, 64, 128, 256];
maxRuns = [16, 10, 2, 10, 4, 8, 4, 2];
queueSelect = [];
for i = 1:length(maxRuns)
    queueSelect = [queueSelect ; (ones(maxRuns(i), 1) * i)];
end
queueName = 'f16c';
fullSH = fopen('hvMots.sh', 'w');
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
queueSelect = queueSelect(end:-1:1);
for i = 1:3
    curQueue = 7;
    fprintf(fullSH, 'qsub -J 0-24 hvMots_Batch_%d.sh\n', i);
    confID = fopen(['hvMots_Batch_' num2str(i) '.sh'], 'w');
    fprintf(confID, '#!/bin/bash\n');
    fprintf(confID, '#\n');
    fprintf(confID, '#PBS -N HV-%d\n', i);
    fprintf(confID, '#PBS -e HV-%s.err.txt\n', datasets{i});
    fprintf(confID, '#PBS -o HV-%s.out.txt\n', datasets{i});
    fprintf(confID, '#PBS -l walltime=%s,select=1:ncpus=5\n', wallTimes{curQueue});
    fprintf(confID, '#PBS -q %s\n', queueNames{curQueue});
    fprintf(confID, '#PBS -J 0-24\n');
    fprintf(confID, 'datasets=(');
    for j = i:3:length(datasets)
        fprintf(confID, '"%s" ', datasets{j});
    end
    fprintf(confID, ')\n\n');
    fprintf(confID, 'cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config${datasets[$PBS_ARRAY_INDEX]}\n');
    fclose(confID);
end
% 
% for i = 1:length(queueSelect)
%     curQueue = queueSelect(i);
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
% end
fclose(fullSH);