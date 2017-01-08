angles = (0:1:179)*pi/180;
load('coefficients.mat');
%addpath('~/matlab/Inpaint_nans/')
tic;

%%%%%%%%%%% for effective length calculation %%%%%%%%%%
l = 1; % this is in mm (1 mm pixel size)
s = 0.5; % this is in mm (0.5 mm step size!)

%bp = zeros(260);
%bpStack = zeros(260, 260, length(angles));
bp = zeros(460);
count = zeros(460);
%effLength = zeros(520);
%chords = zeros(520);
%bpStack = zeros(520, 520, length(angles));
parfor i=1:length(angles)
    i
    protons = csvread(strcat('/home/billy/geant/EdgeModelx2/proj/proj', int2str(i-1),'.csv'), 0, 0);
    
    entryAngle = atan(protons(:,5)./protons(:,7));
    exitAngle = atan(protons(:,13)./protons(:,15));
    entryPos = protons(:,2)*1000; % change metres to mm
    exitPos = protons(:,10)*1000;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%%filtering of the protons based on std! is this right?? i think so?
    
    protonIn(1,i) = size(protons,1);
    relAngle = (exitAngle-entryAngle); %relative angle
    relPos = (exitPos-entryPos); %relative lateral deflection
    stdRelAngle = std(relAngle);
    stdRelPos = std(relPos);
    meanRelAngle = mean(relAngle);
    meanRelPos = mean(relPos);
    diffAngle = abs(relAngle - meanRelAngle);
    [rRelAngle, cRelAngle] = find(diffAngle > 3*stdRelAngle);
    diffPos = abs(relPos - meanRelPos);
    [rRelPos, cRelPos] = find(diffPos > 3*stdRelPos);
    if(isnan(stdRelPos))
        stdRelPos
    elseif (isnan(stdRelAngle))
        stdRelAngle
    end
    
    rmv = [rRelAngle; rRelPos];
    rmv = unique(rmv); % remove duplicates
    
    % remove elements that have been filtered
    protons(rmv, :) = [];
    entryAngle(rmv, :) = [];
    exitAngle(rmv, :) = [];
    entryPos(rmv, :) = [];
    exitPos(rmv, :) = [];

    protonOut(1,i) = size(protons,1);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
   
    % loop through protons and remove those that have deviated more than 1
    % mm
%     size(protons)
%     filtering = true;
%     
%     if (filtering)
%         indx = find(abs(exitPos - entryPos) > 1); % find all with deviation more than 1 mm
%         and then remove them...
%         protons(indx,:) = [];
%     end
%     size(protons)
    
    
    %product = zeros(260);
    %count = zeros(260);
    %chords = zeros(260);
    product = zeros(460);
    effLength = zeros(460);
    count = zeros(460);
    chords = zeros(460);
    for index=1:size(protons, 1)
        x = [];
        y = [];
        xNew = [];
        yNew = [];
        proj = protons(index, 17);
        %proj = protons(index, 18);
        y0 = [entryPos(index); entryAngle(index)];
        y2 = [exitPos(index); exitAngle(index)];
        
        % get position of intersection of ray with ellipse if it was a
        % straight line (all in mm!)
        A = [-230 y0(1)];
        B = [230 y2(1)];
        %A = y0';
        %B = y2';
        inter = lineEllipse(A,entryAngle(index),B,exitAngle(index),70,80, angles(i));
        
        % if intersection from exit is before intersection from entrance,
        % then its not an intersection (could happen when tracing back
        % lines!)
        if(inter.cut)
            if(inter.x2 <= inter.x1) 
                inter.cut = false;
            end
        end

        % check for intersection. if no intersection, straight line!
        if (inter.cut) 
            dirVecA = [cos(entryAngle(index)) sin(entryAngle(index))]; %direction from A to ellipse
            dirVecB = [cos(exitAngle(index)) sin(exitAngle(index))];  % direction from B to ellipse
            y0 = [inter.y1; entryAngle(index)]; % entry position for most likely path
            y2 = [inter.y2; exitAngle(index)]; % exit position for most likel path (ie [t; theta])
            
            % do a bit of playing around with u0, u1, u2 so that we avoid
            % problems...
            u0 = inter.x1;
            u2 = inter.x2;
            %%%% can play around with number of knots
            stepSize = (u2-u0)/5;
            u1 = [u0:stepSize:u2];
            % compute the most likely path
            %ymlp = mlp2(y0, y2, u0, u1, u2, stepSize, p);
            a = CubicSpline(inter.x1, inter.x2, y0(1), y2(1), sin(entryAngle(index))/cos(entryAngle(index)), ...
                sin(exitAngle(index))/cos(exitAngle(index)));
            f = @(x) a(1)*x.^3 + a(2)*x.^2 + a(3)*x + a(4);
            ymlp = f(u1);

            x = [A(1) u1 B(1)];
            
            y = [A(2) ymlp B(2)];
            % rotate the coordinates of the mlp ANTICLOCKWISE
            xNew = x * cos(angles(i)) - y * sin(angles(i));
            yNew = x * sin(angles(i)) + y * cos(angles(i));
            
            ind = myradonBig(xNew, yNew);
            %break;
        else
            x = [A(1) B(1)];
            y = [A(2) B(2)];
            
            % rotate the coordinates of the mlp ANTICLOCKWISE
            xNew = x * cos(angles(i)) - y * sin(angles(i));
            yNew = x * sin(angles(i)) + y * cos(angles(i));
            %Anew = [cos(angles(i)) -sin(angles(i)); sin(angles(i)) cos(angles(i))]*A';
            %Bnew = [cos(angles(i)) -sin(angles(i)); sin(angles(i)) cos(angles(i))]*B';
            
            [ind] = myradonBig(xNew, yNew);
        end
        
        % check to see whether the path passes through the reconstruction
        % region
        if (isempty(ind)) 
            continue;
        end
        indCell = num2cell(ind(:,1:3), 1);
        indx = sub2ind(size(product), indCell{:,1:2});
        effLength(indx) = ind(:,3);
        product(indx) = product(indx) + effLength(indx)*proj;
        chords(indx) = chords(indx) + effLength(indx);
        %figure; imagesc(product);
        %drawnow;
        %index
        %imagesc(product);
        %pause(0.1)
    end

    product = product ./ chords;
    product(isnan(product)) = 0;
    %bpStack(:,:,i) = product;
    indzero = find(product == 0); % find all zeros
    product(indzero) = 0.4930; % simple fix...
    bp = bp + (pi/180)*product;
    
    
end
toc;

protonCount = [protonIn' protonOut'];
save(mfilename, 'bp','protonCount');