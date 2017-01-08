% reads in file as out<angleID>_nt_Model.csv (obtained from geant4  simulations)
% this needs some fixing for usage with condor (something like isnan(i), see documentation on manchester condor website for using matlab scripts with condor)

function WEPL(i)
% alter loop according to number of projections
R = zeros(0,0);
%K = 0.170; %MeV/cm
K = 0.0170; %MeV/mm %%%%%%%%% THIS HAS BEEN FIXED NOW~~~~~~~~~~
%syms beta;
m_e = 0.510998910; %MeV
%I = 75*10^(-6); %MeV
I = 78*10^(-6); %MeV
m_p = 938.272046; %MeV
beta = @(E_k) (sqrt(1 - (m_p./(E_k + m_p)).^2));

% stopping power
Sinv = @(E_k) 1./((K./beta(E_k).^2) .* (log(2.*m_e.*beta(E_k).^2/(I.*(1-beta(E_k).^2))) - beta(E_k).^2));

%for i = 0:179
    
    
    tmp = csvread(strcat('out', num2str(i), '_nt_Model.csv'), 24, 0);

    % check if we have a straggler at the end
    while (tmp(end,8) == 0)
        tmp(end,:) = [];
    end
    
    %removal of ones that didn't make it to the end
    a = diff(tmp);
    t = find(a(:,8) == 0);
    tmp(t,:) = [];
    leftChamber = tmp(tmp(:,8) == 0, :);
    rightChamber = tmp(tmp(:,8) == 1, :);
    
    projection = zeros(0,1);
    
    for j = 1:size(leftChamber,1)
        projection(j,1) = integral(Sinv, rightChamber(j,1), leftChamber(j, 1));
    end
    
    totalChamber = [leftChamber rightChamber projection];
    %csvwrite(strcat('/home/billy/geant/SlitModelx2/proj/','proj', num2str(i), '.csv'), totalChamber);
    csvwrite(strcat('proj',num2str(i),'.csv'), totalChamber);
%end

end
