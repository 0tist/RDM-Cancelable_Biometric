% clear;
n_sub = 100;
n_s = 1;
load('unified_features_train.mat'); 
load('unified_features_test.mat');
%%
k = 1;
l = 1;
%%
m=1;
n=1;
%% 
for i = 1:n_sub
    for j = 1:n_sub
        if i==j
            for s = 0:n_s-1
                g(1,k) = max(cs(unified_train(n_s*i-s,:)...
                    ,unified_test(j,:)),0);
            
                k = k+1;
            end

            m=m+1;
        else 
            for s = 0:n_s-1
                im(1,l) = max(cs(unified_train(n_s*i-s,:)...
                    ,unified_test(j,:)),0);
                l = l+1;
            end
%             
            n = n+1;
        end
    end
end

%%
var_g=var(g); % variance
var_im=var(im);
avg_g=mean(g); %mean
avg_im=mean(im);
d=(avg_g-avg_im)/sqrt((var_g+var_im)/2); %decidability
fprintf('old d = %d\n',d);

%% FREQ DISTRIBUTION
figure(5);
[count, bins] = hist(g);
[count1, bins1] = hist(im);
count = smooth(count,'lowess');
count1 = smooth(count1,'lowess');  %lowess
plot(bins,count/60*100,'-s',bins1,count1/3540*100,'-d','linewidth',2);
xlabel('Score');
ylabel('Frequency');
legend('Genuine','Imposter');

%% Score normalisation

% Tanh
muG = mean(g);
sigG = std(g);

for j=1:size(g,2)
    g(1,j) = (0.5*((tanh (0.01*((g(1,j)- muG)/sigG)))+1));
end

for j=1:size(im,2)
    im(1,j) = (0.5*((tanh (0.01*((im(1,j)- muG)/sigG)))+1));
end

% R_anchor
R_avg = mean(im);
R_std = std(im);
R_anchor = R_avg + R_std;
Th = R_anchor;
mn = min(im);
mx = max(im);

for j=1:size(g,2)
    
            if(g(1,j)<=Th)
                g(1,j) = (g(1,j)-mn) / (2*(Th - mn));
            else
                g(1,j) = 0.5 + (g(1,j) - Th)/(mx - Th);
            end
end

for j=1:size(im,2)
            if(im(1,j)<=Th)
                im(1,j) = (im(1,j)-mn) / (2*(Th - mn));
            else
                im(1,j) = 0.5 + (im(1,j) - Th)/(mx - Th);
            end
end

%% new decidability
var_g=var(g); % variance
var_im=var(im);
avg_g=mean(g); %mean
avg_im=mean(im);
d=(avg_g-avg_im)/sqrt((var_g+var_im)/2); %decidability
fprintf('new d = %i\n',d);
%%
% hold on;
[count, bins] = hist(g,17);
[count1, bins1] = hist(im,17);
count = smooth(count,'lowess');
count1 = smooth(count1,'lowess');  %lowess
plot(bins./5, count,'-', bins1./5,count1/107,'--','linewidth',4);
xlabel('Normalised Score');
ylabel('Normalised Frequency');
legend('Genuine','Imposter');

% legend('Genuine','Imposter');

% unified_train = rand()

%%
% hold on;
% figure(6);
hold on;
[count, bins] = hist(g,17);
[count1, bins1] = hist(im,17);
count = smooth(count,'lowess');
count1 = smooth(count1,'lowess');  %lowess
plot(bins1./5,count1/115,':','linewidth',4);


%% 
% hold on;
figure(5);
[count, bins] = hist(g);
[count1, bins1] = hist(im);
count = smooth(count,'lowess');
count1 = smooth(count1,'lowess');  %lowess
plot(bins1./7,count1/9900,'-d','linewidth',2);
xlabel('Normalised Score');
ylabel('Normalised Frequency');
legend('Impostor', 'Brute Force Genuine', 'Brute Force Impostor');
%%
eer;
fprintf('EER      = %d\n',EER);
fprintf('EER plot = %d\n',EERplot);
save('/Users/hi/Downloads/Brute Force/Brute Force\g.mat','g');
save('/Users/hi/Downloads/Brute Force/Brute Force\im.mat','im');

