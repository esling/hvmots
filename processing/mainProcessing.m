mainDirectory = '../output';
variety = {'HV-MOTS'};
varSimp = {'Resample'};
criteria = {'1-NN', '5-NN', 'NC', 'MOTS', 'NP-MOTS', 'HV-MOTS'};
data = importdata('results_SOA.txt');
datasets = data.textdata(2:end, 1);
disp(datasets);
trainSet = data.data(:, 1);
testSet = data.data(:, 2);
tsLength = data.data(:, 3);
classes = data.data(:, 4);
resultsL2 = data.data(:, 5);
resultsDTW = data.data(:, 6);
resultsWDTW = data.data(:, 7);
resultsLCSS = data.data(:, 8);
resultsMSM = data.data(:, 9);
resultsTWED = data.data(:, 10);
resultsERP = data.data(:, 11);
resultsEnsembles = data.data(:, 12);
resultsSOABest = data.data(:, 13);
distances = {'L1', 'L2', 'LI', 'LP', 'ERP', 'EDR', 'LCSS', 'TWED', 'MSM', ...
    'DTW', 'WDTW', 'DDTW', 'WDDTW', 'OSB', 'FFT(L2)', 'FFT(DTW)', ...
    'ACF(L2)', 'ACF(DTW)', 'CDM'};
familyGlobal = [1 1 1 1 2 2 2 2 2 1 1 1 1 3 3 3 3 4];
familyProperties = [1 1 1 1 3 3 3 3 3 2 2 3 3 3 4 4 4 4 5];
familyProperties2 = [1 1 1 1 3 3 3 3 3 2 2 3 3 3 4 4 5 5 6];
families = [familyGlobal; familyProperties; familyProperties2];
%for i = [23 21 20 13]
%    datasets(i) = []; resultsL2(i) = []; resultsDTWb(i) = [];
%    resultsDTWf(i) = []; classes(i) = []; tsLength(i) = [];
%    trainSet(i) = []; testSet(i) = [];
%end
TTRatio = trainSet ./ testSet;
properties = {classes, tsLength, TTRatio, trainSet, testSet};
propNames = {'Classes', 'Length', 'Ratio', 'Train', 'Test'};
bestResults = ones(length(datasets), length(criteria));
spaceResults = ones(length(datasets), length(criteria));
distResults = ones(length(datasets), length(distances));
nbComb = (2^length(distances)) - 1;
disp(nbComb);
allResults = ones(length(datasets) * nbComb, length(criteria));
distSpaceSize = zeros(length(datasets), length(criteria), length(distances), 2);
distOccur = zeros(length(distances), length(datasets), length(criteria), 2);
distOccurCumul = zeros(length(distances), length(datasets), length(criteria), 2);
distCoOccur = zeros(length(distances), length(distances), length(criteria), 2);
combosReal = ones(length(distances), nbComb);
spaceCombosHV = {};
bestCombosHV = {};
resultsLearn = [];
for v = 1
for c = 1:length(criteria)
    fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_Occur_Best.csv'], 'w');
    fprintf(fID, '.');
    for d = 1:length(distances)
        fprintf(fID, '\t%s', distances{d});
    end
    fprintf(fID, '\n');
    fclose(fID);
    fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_Occur_Space.csv'], 'w');
    fprintf(fID, '.');
    for d = 1:length(distances)
        fprintf(fID, '\t%s', distances{d});
    end
    fprintf(fID, '\n');
    fclose(fID);
end
wID = fopen(['distances/' varSimp{v} '_distanceParams.txt'], 'w');
fclose(wID);
end
for v = 1
    disp(['Variety : ' variety{v}]);
    for i = 1:length(datasets)
        curDataset = datasets{i};
        fprintf('- Processing %s\n', curDataset);
        resultFolder = [mainDirectory '/' variety{v} '/' curDataset '/results'];
        resID = fopen([resultFolder '/globalResults.txt'], 'r');
        if (resID == -1)
            fprintf('   *** No results ***\n');
            continue;
        end
        curComb = 1;
        % Global results import
        while (1)
            if (feof(resID))
                break;
            end
            k = curComb;
            comb = zeros(length(distances), 1);
            for j = 1:length(distances)
                comb(j) = bitand(k, 1); 
                k = bitshift(k, -1);
            end
            dists = fgetl(resID);
            results = fgetl(resID);
            if (results == -1)
                break;
            end
            strFloats = regexp(results, ' ', 'split');
            resFloats = str2double(strFloats(1:(end-1)));
            distSplit = regexp(dists, ' ', 'split');
            if (sum(comb) == 1)
                distResults(i, comb ~= 0) = resFloats(1);
            end
            if (strcmp(distSplit(1), 'Best'))
                bestResults(i, :) = resFloats;
            end
            if (strcmp(distSplit(1), 'Space'))
                spaceResults(i, :) = resFloats;
            end
            if (curComb <= nbComb)
                allResults(curComb + ((i - 1) * nbComb), :) = resFloats;
                combosReal(:, curComb) = comb;
            end
            curComb = curComb + 1;
        end
        fclose(resID);
        % For each distance we retrieve the best parameters
        wID = fopen(['distances/' varSimp{v} '_distanceParams.txt'], 'a');
        fprintf(wID, '%s\n', datasets{i});
        for d = 1:length(distances)
            fid = fopen([mainDirectory '/' variety{v} '/' curDataset '/optimize_distance/' distances{d} '.txt'], 'r');
            if (fid == -1)
                continue;
            end
            bestVals = [];
            bestErr = 2.0;
            disp(distances{d});
            if (feof(fid))
                continue;
            end
            results = fgetl(fid);
            if (results == -1)
                continue;
            end
            distOptim = regexp(results, ' ', 'split');
            distOptimNb = str2double(distOptim(end));
            for dO = 1:distOptimNb
                curStr = fgetl(fid);
                curVals = regexp(curStr, '\t', 'split');
                err = regexp(curVals(end), '=', 'split');
                curErr = str2double(err{1}(end));
                if (curErr < bestErr)
                    bestErr = curErr;
                    bestVals = curStr;
                end
            end
            fclose(fid);
            fprintf(wID, '%s\t: %s\n', distances{d}, curStr);
        end
        fclose(wID);
        % Based on best and space results, find back corresponding combos
        for c = 1:length(criteria)
            for r = 1:nbComb
                if (allResults(r + ((i - 1) * nbComb), c) == bestResults(i, c))
                    fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_Occur_Best.csv'], 'a');
                    fprintf(fID, '%s', datasets{i});
                    distOccurCumul(:, i, c, 1) = distOccurCumul(:, i, c, 1) + combosReal(:, r);
                    distSpaceSize(i, c, sum(combosReal(:, r)), 1) = distSpaceSize(i, c, sum(combosReal(:, r)), 1) + 1;
                    for d1 = 1:length(distances)
                        fprintf(fID, '\t%d', combosReal(d1, r));
                        for d2 = 1:length(distances)
                            if (d1 ~= d2 && combosReal(d1, r) == 1 && combosReal(d2, r) == 1)
                                distCoOccur(d1, d2, c, 1) = distCoOccur(d1, d2, c, 1) + 1;
                            end
                        end
                    end
                    if (c == 6)
                        bestCombosHV = {bestCombosHV ; combosReal(:, r)};
                    end
                    fprintf(fID, '\n');
                    fclose(fID);
                end
                if (allResults(r + ((i - 1) * nbComb), c) == spaceResults(i, c))
                    fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_Occur_Space.csv'], 'a');
                    fprintf(fID, '%s', datasets{i});
                    distOccurCumul(:, i, c, 2) = distOccurCumul(:, i, c, 2) + combosReal(:, r);
                    distSpaceSize(i, c, sum(combosReal(:, r)), 2) = distSpaceSize(i, c, sum(combosReal(:, r)), 1) + 2;
                    for d1 = 1:length(distances)
                        fprintf(fID, '\t%d', combosReal(d1, r));
                        for d2 = 1:length(distances)
                            if (d1 ~= d2 && combosReal(d1, r) == 1 && combosReal(d2, r) == 1)
                                distCoOccur(d1, d2, c, 2) = distCoOccur(d1, d2, c, 2) + 1;
                            end
                        end
                    end
                    if (c == 6)
                        spaceCombosHV = {spaceCombosHV ; combosReal(:, r)};
                    end
                    fprintf(fID, '\n');
                    fclose(fID);
                end
            end
        end
        % Per-criteria results import
        for c = 1:length(criteria)
            resID = fopen([resultFolder '/' criteria{c} '.txt'], 'r');
            if (resID == -1)
                fprintf('   *** No results for %s ***\n', criteria{c});
                continue;
            end
            while (1)
                if (feof(resID))
                    break;
                end
                results = fgetl(resID);
                if (results == -1)
                    break;
                end
                distSplit = regexp(results, ' ', 'split');
                errors = regexp(fgetl(resID), ': ', 'split');
                errors = str2double(errors(2));
                rate = regexp(fgetl(resID), ': ', 'split');
                rate = str2double(rate(2));
                classesErrors = str2num(fgetl(resID));
                confMatrix = zeros(classes(i), classes(i));
                for cl = 1:classes(i)
                    curConfusion = fgetl(resID);
                    confMatrix(cl, :) = str2num(curConfusion);
                end
                break;
            end
            fclose(resID);
        end
    end
    minResults = [min(distResults, [], 2) bestResults(:, end)];
    bestResults = bestResults * 100;
    spaceResults = spaceResults * 100;
    distResults = distResults * 100;
    minResults = minResults * 100;
    disp(bestResults);
    disp(spaceResults);
    %%
    figure;
    scatter(bestResults(:, 1), spaceResults(:, 1));
    setPaperQuality();
    figure;
    bar(reshape(mean(distSpaceSize(:, end, 1:end-1, 1), 1), length(distances) - 1, 1));
    setPaperQuality();
    figure;
    bar(reshape(mean(distSpaceSize(:, end, end-1:-1:1, 2), 1), length(distances) - 1, 1));
    setPaperQuality();
    %%
    figure;
    bar(reshape(mean(distSpaceSize(:, end, 1:end-1, 2), 1), length(distances) - 1, 1));
    figure;
    bar(reshape(mean(distSpaceSize(:, end-2, 1:end-1, 2), 1), length(distances) - 1, 1));
    bar(sum(distOccurCumul(:, :, end, 1), 2));
    figure;
    bar(sum(distOccurCumul(:, :, end, 2), 2));
    figure;
    imagesc(distCoOccur(:, :, end, 1));
    figure;
    imagesc(distCoOccur(:, :, end, 2));
    families = [1 1 1 1 2 2 2 1 1 3 3 3 3 4 ; ...
                1 1 1 1 3 3 3 2 3 4 4 4 4 5 ; ...
                1 1 1 1 3 3 3 2 3 4 4 5 5 6];
    for i = 1:3
        curFamilies = families(i, :);
        for t = 1:2
            coOccurFamily = zeros(max(curFamilies), max(curFamilies));
            for f1 = 1:max(curFamilies)
                for f2 = 1:max(curFamilies)
                    coOccurFamily(f1, f2) = mean(mean(distCoOccur(find(curFamilies == f1), find(curFamilies == f2), end, t)));
                end
            end
            figure;
            imagesc(coOccurFamily);
            colormap hot;
        end
    end
    %        curCoOccur = 
    %        for f = 1:max
    %        end
    %    end
    %end
    return;
    %%
    % First compute critical differences (inside our criteria)
    figure; criticaldifference(bestResults, criteria);
    title('Best results'); setPaperQuality();
    figure; criticaldifference(spaceResults, criteria);
    title('Space results'); setPaperQuality();
    % Then compute critical differences (over all combinations)
    figure; criticaldifference(allResults, criteria);
    title('All results'); setPaperQuality();
    % Compute critical differences with best (against state-of-art)
    SOAResults = [resultsL2' resultsDTW' resultsTSE' bestResults(:, [1 end])];
    SOANames = {'L2', 'DTW', 'TSE', '1NNm', 'HVm'};
    figure; criticaldifference(SOAResults, SOANames);
    title('State-of-art vs. best'); setPaperQuality();
    % Compute critical differences with space (against state-of-art)
    SOAResults = [resultsL2' resultsDTW' resultsTSE' spaceResults(:, [1 end])];
    SOANames = {'L2', 'DTW', 'TSE', '1NNm', 'HVm'};
    figure; criticaldifference(SOAResults, SOANames);
    title('State-of-art vs. best'); setPaperQuality();
    %% Export the pairwise data (for scatterplot analysis)
    cResID = fopen('classif.results.space.csv', 'w');
    SOAResults = [resultsL2' resultsDTW' resultsTSE' spaceResults(:, end)];
    SOANames = {'L2', 'DTW', 'TSE', 'HVm'};
    fprintf(cResID, 'dataset');
    for c = 1:length(SOANames)
        fprintf(cResID, ',%s', SOANames{c});
    end
    fprintf(cResID, '\n');
    for i = 1:length(datasets)
        fprintf(cResID, '%s', datasets{i});
        for j = 1:length(SOANames)
            fprintf(cResID, ',%f', SOAResults(i, j) / 10.0);
        end
        fprintf(cResID, '\n');
    end
    fclose(cResID);
    SOAResults = [resultsL2' resultsDTW' resultsTSE' bestResults(:, end)];
    SOANames = {'L2', 'DTW', 'TSE', 'HVm'};
    cResID = fopen('classif.results.best.csv', 'w');
    fprintf(cResID, 'dataset');
    for c = 1:length(SOANames)
        fprintf(cResID, ',%s', SOANames{c});
    end
    fprintf(cResID, '\n');
    for i = 1:length(datasets)
        fprintf(cResID, '%s', datasets{i});
        for j = 1:length(SOANames)
            fprintf(cResID, ',%f', SOAResults(i, j) / 10.0);
        end
        fprintf(cResID, '\n');
    end
    fclose(cResID);
    %% Dataset properties analysis (best)
    deltaError = bestResults - repmat(mean(bestResults(:, [1 2 4 5 6]), 2), 1, size(bestResults, 2));
    colorsCrit = distinguishable_colors(length(criteria));
    for p = 1:length(properties)
        f = figure;
        xVals = properties{p}';
        linePointsX = [min(xVals) max(xVals)];
        hold on;
        for c = [1 2 4 5 6]
            plot(0, 0, 'Color', colorsCrit(c,:));
        end
        for c = [1 2 4 5 6]
            yVals = deltaError(:, c);
            pPoly = polyfit(xVals, yVals, 1);
            linePointsY = polyval(pPoly, linePointsX);
            scatter(xVals, yVals, repmat(40, length(yVals), 1), 'o', 'MarkerEdgeColor',colorsCrit(c,:),'MarkerFaceColor',colorsCrit(c, :),'LineWidth',1.5);
            plot(linePointsX, linePointsY, '-.', 'Color', colorsCrit(c,:));
            [R, P] = corrcoef(xVals, yVals);
            fprintf('[%s] %s : R = %f / p = %f\n', propNames{p}, criteria{c}, R(2), P(2));
        end
        hold off;
        legend(criteria);
        title(propNames{p});
        setPaperQuality();
    end
    % Distance-wise analysis (scatterplot distance against best / space)
    figure;
    hold on;
    colorsCrit = distinguishable_colors(length(distances));
    xVals = bestResults(:, end);
    % First do it with best
    for d = 1:length(distances)
        scatter(xVals, distResults(:, d), repmat(40, length(xVals), 1), 'o', 'MarkerEdgeColor',colorsCrit(d,:),'MarkerFaceColor',colorsCrit(d, :),'LineWidth',1.5);
    end
    hold off;
    title('Distances against best');
    legend(distances);
    setPaperQuality();
    % Then do it with space
    figure;
    hold on;
    xVals = spaceResults(:, end);
    for d = 1:length(distances)
        scatter(xVals, distResults(:, d), repmat(40, length(xVals), 1), 'o', 'MarkerEdgeColor',colorsCrit(d,:),'MarkerFaceColor',colorsCrit(d, :),'LineWidth',1.5);
    end
    hold off;
    title('Distances against space');
    legend(distances);
    setPaperQuality();
    % Similarity space analysis
    testNames = {'Best', 'Space'};
    for c = 1:length(criteria)
        for i = 1:2
            fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_Size_' testNames{i} '.csv'], 'w');
            fprintf(fID, '.');
            for d = 1:length(datasets)
                fprintf(fID, '\t%s', datasets{d});
            end
            fprintf(fID, '\n');
            for di = 1:length(distances)
                fprintf(fID, '%d', di);
                for da = 1:length(datasets)
                    fprintf(fID, '\t%d', distSpaceSize(da, c, di, i));
                end
                fprintf(fID, '\n');
            end
            fclose(fID);
            fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_OccurCumul_' testNames{i} '.csv'], 'w');
            fprintf(fID, '.');
            for d = 1:length(datasets)
                fprintf(fID, '\t%s', datasets{d});
            end
            fprintf(fID, '\n');
            for di = 1:length(distances)
                fprintf(fID, '%s', distances{di});
                for da = 1:length(datasets)
                    fprintf(fID, '\t%d', distOccurCumul(di, da, c, i));
                end
                fprintf(fID, '\n');
            end
            fclose(fID);
            fID = fopen(['distances/' varSimp{v} '_' criteria{c} '_CoOccur_' testNames{i} '.csv'], 'w');
            fprintf(fID, '.');
            for d = 1:length(distances)
                fprintf(fID, '\t%s', distances{d});
            end
            fprintf(fID, '\n');
            for di = 1:length(distances)
                fprintf(fID, '%s', distances{di});
                for di2 = 1:length(distances)
                    fprintf(fID, '\t%d', distCoOccur(di, di2, c, i));
                end
                fprintf(fID, '\n');
            end
            fclose(fID);
        end
    end
end