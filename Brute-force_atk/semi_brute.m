%% 

clear; clc;
% close all;
load('unified_features_train.mat');
load('unified_features_test.mat');

%% Main loop
rng('default');
len = size(unified_train, 2);
perc = 320;
perc = perc/100;

pd = makedist('Uniform','lower',1,'upper',300);
r = random(pd, [perc*len, 1]);
len_r = perc*len;
r = floor(r);
for i=1:len_r
    if r(i,1)==0
        r(i,1) = 1;
    end
end
r


pd2 = makedist('Normal');
% pd2 = makedist('Normal','lower',0,'upper',1000);
r2 = random(pd, [perc*len, 1]);
r2
% figure(9);
% plot(unified_test(1, :));
%% Change values

for i=1:100
    pd2 = makedist('Normal');
    r2 = random(pd, [perc*len, 1]);
    for j=1:len_r
        unified_test(i, r(j, 1)) = r2(j,1)/20;
    end
end
figure(9);
 hold on
 plot(unified_test(1,:));

get_decidability;

